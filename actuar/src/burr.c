/*  ===== actuar: an R package for Actuarial Science =====
 *
 *  Functions to compute density, cumulative distribution and quantile
 *  functions, raw and limited moments and to simulate random variates
 *  for the Burr distribution. See ../R/burr.R for details.
 *
 *  AUTHORS: Mathieu Pigeon and Vincent Goulet <vincent.goulet@act.ulaval.ca>
 */

#include <R.h>
#include <Rmath.h>
#include "locale.h"
#include "dpq.h"

double dburr(double x, double shape1, double shape2, double scale,
	     int give_log)
{
    /*  We work with the density expressed as
     *
     *  shape1 * shape2 * u * (1 - u) / x
     *
     *  with u = v/(1 + v), v = (x/scale)^shape2.
     */

    double tmp, logu, log1mu;

    if (!R_FINITE(shape1) ||
	!R_FINITE(shape2) ||
	!R_FINITE(scale)  ||
	shape1 <= 0.0 ||
	shape2 <= 0.0 ||
	scale  <= 0.0)
	return R_NaN;

    if (!R_FINITE(x) || x < 0.0)
	return R_D_d0;

    tmp = shape2 * (log(x) - log(scale));
    log1mu = - log1p(exp(tmp));
    logu = tmp + log1mu;

    return R_D_exp(log(shape1) + log(shape2) + logu + log1mu - log(x));
}

double pburr(double q, double shape1, double shape2, double scale,
	     int lower_tail, int log_p)
{
    double u, tmp;

    if (!R_FINITE(shape1) ||
	!R_FINITE(shape2) ||
	!R_FINITE(scale)  ||
	shape1 <= 0.0 ||
	shape2 <= 0.0 ||
	scale  <= 0.0)
	return R_NaN;

    if (q <= 0)
	return R_DT_0;

    tmp = shape2 * (log(q) - log(scale));
    u = exp(tmp - log1p(exp(tmp)));

    return R_DT_Cval(R_pow(1.0 + tmp, -shape1));
}

double qburr(double p, double shape1, double shape2, double scale,
	     int lower_tail, int log_p)
{
    if (!R_FINITE(shape1) ||
	!R_FINITE(shape2) ||
	!R_FINITE(scale)  ||
	shape1 <= 0.0 ||
	shape2 <= 0.0 ||
	scale  <= 0.0)
	return R_NaN;

    R_Q_P01_boundaries(p, 0, R_PosInf );
    p =  R_D_qIv(p);

    return scale * R_pow(R_pow(R_D_Cval(p), -1.0/shape1) - 1.0, 1.0/shape2);
}

double rburr(double shape1, double shape2, double scale)
{
    if (!R_FINITE(shape1) ||
	!R_FINITE(shape2) ||
	!R_FINITE(scale)  ||
	shape1 <= 0.0 ||
	shape2 <= 0.0 ||
	scale  <= 0.0)
	return R_NaN;

    return scale * R_pow(R_pow(unif_rand(), -1.0/shape1) - 1.0, 1.0/shape2);
}

double mburr(double order, double shape1, double shape2, double scale,
	     int give_log)
{
    double tmp;

    if (!R_FINITE(shape1) ||
	!R_FINITE(shape2) ||
	!R_FINITE(scale)  ||
	!R_FINITE(order) ||
	shape1 <= 0.0 ||
	shape2 <= 0.0 ||
	scale  <= 0.0 ||
	order  <= -shape2 ||
	order  >= shape1 * shape2)
	return R_NaN;

    tmp = order / shape2;

    return R_pow(scale, order) * gammafn(1.0 + tmp) * gammafn(shape1 - tmp)
	/ gammafn(shape1);
}

double levburr(double limit, double shape1, double shape2, double scale,
	       double order, int give_log)
{
    double u, tmp1, tmp2, tmp3;

    if (!R_FINITE(shape1) ||
	!R_FINITE(shape2) ||
	!R_FINITE(scale)  ||
	!R_FINITE(order) ||
	shape1 <= 0.0 ||
	shape2 <= 0.0 ||
	scale  <= 0.0 ||
	order  <= -shape2)
	return R_NaN;

    if (limit <= 0.0)
	return 0;

    if (!R_FINITE(limit))
	return mburr(order, shape1, shape2, scale, 0);

    tmp1 = order / shape2;
    tmp2 = 1.0 + tmp1;
    tmp3 = shape1 - tmp1;

    u = 1.0 / (1.0 + exp(shape2 * (log(limit) - log(scale))));

    return R_pow(scale, order) * gammafn(tmp2) * gammafn(tmp3)
	* pbeta(0.5 - u + 0.5, tmp2, tmp3, 1, 0) / gammafn(shape1)
	+ R_pow(limit, order) * R_pow(u, shape1);
}
