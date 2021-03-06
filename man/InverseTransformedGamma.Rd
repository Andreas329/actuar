\name{InverseTransformedGamma}
\alias{InverseTransformedGamma}
\alias{dinvtrgamma}
\alias{pinvtrgamma}
\alias{qinvtrgamma}
\alias{rinvtrgamma}
\alias{minvtrgamma}
\alias{levinvtrgamma}
\title{The Inverse Transformed Gamma Distribution}
\description{
  Density function, distribution function, quantile function, random generation,
  raw moments, and limited moments for the Inverse Transformed Gamma
  distribution with parameters \code{shape1}, \code{shape2} and
  \code{scale}.
}
\usage{
dinvtrgamma(x, shape1, shape2, rate = 1, scale = 1/rate,
            log = FALSE)
pinvtrgamma(q, shape1, shape2, rate = 1, scale = 1/rate,
            lower.tail = TRUE, log.p = FALSE)
qinvtrgamma(p, shape1, shape2, rate = 1, scale = 1/rate,
            lower.tail = TRUE, log.p = FALSE)
rinvtrgamma(n, shape1, shape2, rate = 1, scale = 1/rate)
minvtrgamma(order, shape1, shape2, rate = 1, scale = 1/rate)
levinvtrgamma(limit, shape1, shape2, rate = 1, scale = 1/rate,
              order = 1)
}
\arguments{
  \item{x, q}{vector of quantiles.}
  \item{p}{vector of probabilities.}
  \item{n}{number of observations. If \code{length(n) > 1}, the length is
    taken to be the number required.}
  \item{shape1, shape2, scale}{parameters. Must be strictly positive.}
  \item{rate}{an alternative way to specify the scale.}
  \item{log, log.p}{logical; if \code{TRUE}, probabilities/densities
    \eqn{p} are returned as \eqn{\log(p)}{log(p)}.}
  \item{lower.tail}{logical; if \code{TRUE} (default), probabilities are
    \eqn{P[X \le x]}{P[X <= x]}, otherwise, \eqn{P[X > x]}.}
  \item{order}{order of the moment.}
  \item{limit}{limit of the loss variable.}
}
\details{
  The inverse transformed gamma distribution with parameters
  \code{shape1} \eqn{= \alpha}{= a}, \code{shape2} \eqn{= \tau}{= b} and
  \code{scale} \eqn{= \theta}{= s}, has density:
  \deqn{f(x) = \frac{\tau u^\alpha e^{-u}}{x \Gamma(\alpha)}, %
      \quad u = (\theta/x)^\tau}{%
    f(x) = b u^a exp(-u) / (x Gamma(a)), u = (s/x)^b}
  for \eqn{x > 0}, \eqn{\alpha > 0}{a > 0}, \eqn{\tau > 0}{b > 0}
  and \eqn{\theta > 0}{s > 0}.
  (Here \eqn{\Gamma(\alpha)}{Gamma(a)} is the function implemented
  by \R's \code{\link{gamma}()} and defined in its help.)

  The inverse transformed gamma is the distribution of the random
  variable
  \eqn{\theta X^{-1/\tau},}{s X^(-1/b),}
  where \eqn{X} has a gamma distribution with shape parameter
  \eqn{\alpha}{a} and scale parameter \eqn{1} or, equivalently, of the
  random variable
  \eqn{Y^{-1/\tau}}{Y^(-1/b)}
  with \eqn{Y} a gamma distribution with shape parameter \eqn{\alpha}{a}
  and scale parameter \eqn{\theta^{-\tau}}{s^(-b)}.

  The inverse transformed gamma distribution defines a family of
  distributions with the following special cases:
  \itemize{
    \item An \link[=dinvgamma]{Inverse Gamma} distribution when
    \code{shape2 == 1};
    \item An \link[=dinvweibull]{Inverse Weibull} distribution when
    \code{shape1 == 1};
    \item An \link[=dinvexp]{Inverse Exponential} distribution when
    \code{shape1 == shape2 == 1};
  }

  The \eqn{k}th raw moment of the random variable \eqn{X} is
  \eqn{E[X^k]}{E[X^k]}, \eqn{k < \alpha\tau}{k < shape1 * shape2}, and
  the \eqn{k}th limited moment at some limit \eqn{d} is \eqn{E[\min(X,
  d)^k]}{E[min(X, d)^k]} for all \eqn{k}.
}
\value{
  \code{dinvtrgamma} gives the density,
  \code{pinvtrgamma} gives the distribution function,
  \code{qinvtrgamma} gives the quantile function,
  \code{rinvtrgamma} generates random deviates,
  \code{minvtrgamma} gives the \eqn{k}th raw moment, and
  \code{levinvtrgamma} gives the \eqn{k}th moment of the limited loss
  variable.

  Invalid arguments will result in return value \code{NaN}, with a warning.
}
\note{
  \code{levinvtrgamma} computes the limited expected value using
  \code{gammainc} from package \pkg{expint}.

  Distribution also known as the Inverse Generalized Gamma. See also Kleiber and
  Kotz (2003) for alternative names and parametrizations.
}
\references{
  Kleiber, C. and Kotz, S. (2003), \emph{Statistical Size Distributions
  in Economics and Actuarial Sciences}, Wiley.

  Klugman, S. A., Panjer, H. H. and Willmot, G. E. (2012),
  \emph{Loss Models, From Data to Decisions, Fourth Edition}, Wiley.
}
\author{
  Vincent Goulet \email{vincent.goulet@act.ulaval.ca} and
  Mathieu Pigeon
}
\examples{
exp(dinvtrgamma(2, 3, 4, 5, log = TRUE))
p <- (1:10)/10
pinvtrgamma(qinvtrgamma(p, 2, 3, 4), 2, 3, 4)
minvtrgamma(2, 3, 4, 5)
levinvtrgamma(200, 3, 4, 5, order = 2)
}
\keyword{distribution}
