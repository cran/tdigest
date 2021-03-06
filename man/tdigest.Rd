% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/create.R
\name{tdigest}
\alias{tdigest}
\alias{print.tdigest}
\title{Create a new t-Digest histogram from a vector}
\usage{
tdigest(vec, compression = 100)

\method{print}{tdigest}(x, ...)
}
\arguments{
\item{vec}{vector (will be converted to \code{double} if not already double).
NOTE that this is ALTREP-aware and will not materialize the passed-in
object in order to add the values to the t-Digest.}

\item{compression}{the input compression value; should be >= 1.0; this
will control how aggressively the t-Digest compresses data together.
The original t-Digest paper suggests using a value of 100 for a good
balance between precision and efficiency. It will land at very small
(think like 1e-6 percentile points) errors at extreme points in the
distribution, and compression ratios of around 500 for large data sets
(~1 million datapoints). Defaults to 100.}

\item{x}{\code{tdigest} object}

\item{...}{unused}
}
\value{
a \code{tdigest} object
}
\description{
The t-Digest construction algorithm, by Dunning et al., uses a variant of 1-dimensional
k-means clustering to produce a very compact data structure that allows
accurate estimation of quantiles. This t-Digest data structure can be used
to estimate quantiles, compute other rank statistics or even to estimate
related measures like trimmed means. The advantage of the t-Digest over
previous digests for this purpose is that the t-Digest handles data with
full floating point resolution. The accuracy of quantile estimates produced
by t-Digests can be orders of magnitude more accurate than those produced
by previous digest algorithms. Methods are provided to create and update
t-Digests and retrieve quantiles from the accumulated distributions.
}
\examples{
set.seed(1492)
x <- sample(0:100, 1000000, replace = TRUE)
td <- tdigest(x, 1000)
tquantile(td, c(0, 0.01, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.99, 1))
quantile(td)
}
\references{
\href{https://arxiv.org/abs/1902.04023}{Computing Extremely Accurate Quantiles Using t-Digests}
}
\keyword{internal}
