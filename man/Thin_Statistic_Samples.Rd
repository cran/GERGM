% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Thin_Statistic_Samples.R
\name{Thin_Statistic_Samples}
\alias{Thin_Statistic_Samples}
\title{A function to thin out simulated GOF statistics so that they have an
MCMC autocorrelation of less tahn 0.01.}
\usage{
Thin_Statistic_Samples(statistics)
}
\arguments{
\item{statistics}{A data.frame stored in the
`@simulated_statistics_for_GOF` field of the GERGM object.}
}
\value{
A data.frame that has been thinned to reduce autocorrelation.
}
\description{
Takes as input the `@simulated_statistics_for_GOF` field of the
GERGM object (a data.frame) and returns the same data frame but now thinned
to reduce autocorrelation in the samples. Useful for exactly replicating the
statistics used in the GOF() function.
}
