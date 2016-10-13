/*  ===== actuar: An R Package for Actuarial Science =====
 *
 *  Functions to calculate raw and limited moments for the inverse gaussian
 *  distribution. See ../R/InvGaussSupp.R for details.
 *
 *  AUTHORS: Christophe Dutang and Vincent Goulet <vincent.goulet@act.ulaval.ca>
 */

#include <R.h>
#include <Rmath.h>
#include "locale.h"
#include "dpq.h"

double dinvgauss(double x, double mu, double phi, int give_log)
{
    /*  We work with the density expressed as
     *
     *  (2 pi phi x^3)^(-1/2) exp(- u^2/(2 phi x))
     *
     *  with u = (x - mu)/mu.
     */

#ifdef IEEE_754
    if (ISNAN(x) || ISNAN(mu) || ISNAN(phi))
	return x + mu + phi;
#endif
    if (mu <= 0.0 || phi <= 0.0)
        return R_NaN;

    /* limiting case phi = Inf */
    if (!R_FINITE(phi))
	return (x == 0) ? R_PosInf : ACT_D__0;

    if (!R_FINITE(x) || x <= 0.0)
	return ACT_D__0;

    /* limiting case mu = Inf */
    if (!R_FINITE(mu))
	return ACT_D_exp(-(log(phi) + 3 * log(x) + 1/phi/x)/2 - M_LN_SQRT_2PI);

    /* standard cases */
    double xm = x/mu;
    double phim = phi * mu;

    return ACT_D_exp(-(log(phim) + 3 * log(xm) + R_pow_di(xm - 1, 2)/phim/xm)/2
		     - M_LN_SQRT_2PI - log(mu));
}

double pinvgauss(double q, double mu, double phi, int lower_tail, int log_p)
{
#ifdef IEEE_754
    if (ISNAN(q) || ISNAN(mu) || ISNAN(phi))
	return q + mu + phi;
#endif
    if (mu <= 0.0 || phi <= 0.0)
        return R_NaN;

    /* limiting case phi = Inf */
    if (!R_FINITE(phi))
	return ACT_DT_1;

    if (q <= 0)
        return ACT_DT_0;

    /* limiting case mu = Inf */
    if (!R_FINITE(mu))
	return pchisq(1/q/phi, 1, !lower_tail, log_p);

    /* standard cases */
    double qm = q/mu;
    double phim = phi * mu;

    /* approximation for (survival) probabilities in the far right tail */
    if (!lower_tail && qm > 1e6)
    {
	double r = qm/2/phim;
	if (r > 5e5)
	    return ACT_D_exp(1/phim - M_LN_SQRT_PI - log(2*phim) - 1.5 * log1p(r) - r);
    }

    /* all other probabilities */
    double a, b, r = sqrt(q * phi);

    a = pnorm((qm - 1)/r, 0, 1, lower_tail, /* log_p */1);
    b = 2/phim + pnorm(-(qm + 1)/r, 0, 1, /* l._t. */1, /* log_p */1);

    return ACT_D_exp(a + (lower_tail ? log1p(exp(b - a)) : ACT_Log1_Exp(b - a)));
}

/* Needed by qinvgauss() for Newton-Raphson iterations. */
double nrstep(double x, double p, double logp, double phi)
{
    double logF, dlogp;

    logF = pinvgauss(x, 1, phi, /*l._t.*/1, /*log.p*/1);
    dlogp = logp - logF;

    return ((fabs(dlogp) < 1e-5) ? dlogp * exp(logp - log1p(-dlogp/2)) :
	    p - exp(logF)) / dinvgauss(x, 1, phi, 0);
}

double qinvgauss(double p, double mu, double phi, int lower_tail, int log_p,
		 double tol, int maxit, int echo)
{
#ifdef IEEE_754
    if (ISNAN(p) || ISNAN(mu) || ISNAN(phi))
	return p + mu + phi;
#endif
    if (mu <= 0.0 || phi <= 0.0)
        return R_NaN;

    /* limiting case phi = Inf */
    if (!R_FINITE(phi))
	return 1.0;

    /* must be able to do at least one iteration */
    if (maxit < 1)
	error(_("maximum number of iterations must be at least 1"));

    ACT_Q_P01_boundaries(p, 0, R_PosInf);

    int i = 1;
    double logp, kappa, mode, x, dx, s;

    /* same as ACT_DT_qIv macro, but to set both p and logp */
    if (log_p)
    {
	if (lower_tail)
	{
	    logp = p;
	    p = exp(p);
	}
	else
	{
	    p = -expm1(p);
	    logp = log(p);
	}
    }
    else
    {
	p = ACT_D_Lval(p);
	logp = log(p);
    }

    /* convert to mean = 1 */
    phi *= mu;

    /* mode */
    kappa = 1.5 * phi;
    if (kappa <= 1e3)
	mode = sqrt(1 + kappa * kappa) - kappa;
    else			/* Taylor series correction */
    {
	double k = 1/2/kappa;
	mode = k * (1 - k * k);
    }

    /* starting value */
    if (logp < -11.51)		/* small left tail probability */
	x = 1/phi/R_pow_di(qnorm(logp, 0, 1, /*l._t.*/1, /*log.p*/1), 2);
    else if (logp > -1e-5)	/* small right tail probability */
	x = qgamma(logp, 1/phi, phi, /*l._t.*/1, /*log.p*/1);
    else			/* use the mode otherwise */
	x = mode;

    /* if echoing iterations, start by printing the header and the
     * first value */
    if (echo)
        Rprintf("iter\tadjustment\tquantile\n%d\t   ----   \t%.8g\n",
                0, x);

    /* first Newton-Raphson outside the loop to retain the sign of
     * the adjustment */
    dx = nrstep(x, p, logp, phi);
    s = sign(dx);
    x += dx;

    if (echo)
	Rprintf("%d\t%-14.8g\t%.8g\n", i, dx, x);

    /* now do the iterations */
    do
    {
	i++;
	if (i > maxit)
	{
	    warning(_("maximum number of iterations exceeded"));
	    break;
	}

	dx = nrstep(x, p, logp, phi);

	/* change of sign indicates that machine precision has been overstepped */
	if (dx * s < 0)
	    dx = 0;
	else
	    x += dx;

	if (echo)
	    Rprintf("%d\t%-14.8g\t%.8g\n", i, dx, x);

    } while (fabs(dx) > tol);

    return x * mu;
}

double rinvgauss(double mu, double phi)
{
    if (mu <= 0.0 || phi <= 0.0)
        return R_NaN;

    double y, x;

    /* convert to mean = 1 */
    phi *= mu;

    y = R_pow_di(rnorm(0, 1), 2);
    x = 1 + phi/2 * (y - sqrt(4 * y/phi + R_pow_di(y, 2)));

    return mu * ((unif_rand() <= 1/(1 + x)) ? x : 1/x);
}

double minvGauss(double order, double nu, double lambda, int give_log)
{
#ifdef IEEE_754
    if (ISNAN(order) || ISNAN(nu) || ISNAN(lambda))
	return order + nu + lambda;
#endif
    if (!R_FINITE(nu) ||
        !R_FINITE(lambda) ||
        !R_FINITE(order) ||
        nu <= 0.0 ||
        lambda <= 0.0 ||
        (int) order != order)
        return R_NaN;

    /* Trivial case */
    if (order == 0.0)
        return 0.0;

    int i, n = order;
    double z = 0.0;

    for (i = 0; i < n; i++)
        z += R_pow_di(nu, n) * gammafn(n + i) *
            R_pow_di(2.0 * lambda/nu, -i) /
            (gammafn(i + 1) * gammafn(n - i));
    return z;
}

double levinvGauss(double limit, double nu, double lambda, double order,
                   int give_log)
{
#ifdef IEEE_754
    if (ISNAN(limit) || ISNAN(nu) || ISNAN(lambda) || ISNAN(order))
	return limit + nu + lambda + order;
#endif
    if (!R_FINITE(nu)     ||
        !R_FINITE(lambda) ||
        !R_FINITE(order)  ||
        nu <= 0.0    ||
        lambda < 0.0 ||
        order != 1.0)
        return R_NaN;

    if (limit <= 0.0)
        return 0.0;

    /* From R, order == 1 */
    double tmp, y, z;

    tmp = sqrt(lambda/limit);
    y = (limit + nu)/nu;
    z = (limit - nu)/nu;

    return limit - nu * z * pnorm(z * tmp, 0.0, 1.0, 1, 0)
        - nu * y * exp(2.0 * lambda/nu) * pnorm(-y * tmp, 0.0, 1.0, 1, 0);
}

double mgfinvGauss(double x, double nu, double lambda, int give_log)
{
#ifdef IEEE_754
    if (ISNAN(x) || ISNAN(nu) || ISNAN(lambda))
	return x + nu + lambda;
#endif
    if (!R_FINITE(nu) ||
        !R_FINITE(lambda) ||
        nu <= 0.0 ||
        lambda < 0.0 ||
        x > lambda/(2.0 * nu * nu))
        return R_NaN;

    if (x == 0.0)
        return ACT_D_exp(0.0);

    return ACT_D_exp(lambda / nu * (1.0 - sqrt(1.0 - 2.0 * nu * nu * x/lambda)));
}
