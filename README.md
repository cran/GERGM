# GERGM
An R package to estimate Generalized Exponential Random Graph Models

NOTE: **This package is still under development and is very much a work in progress. Please do not use in any publication without express consent of the authors. PLEASE REPORT ANY BUGS OR ERRORS TO <mzd5530@psu.edu>**. 

## Model Overview 

An R package which implements the Generalized Exponential Random Graph Model (GERGM) with an extension to estimation via Metropolis Hastings. The relevant papers detailing the model can be found at the links below:

* Bruce A. Desmarais, and Skyler J. Cranmer,  (2012). "Statistical inference for valued-edge networks: the generalized exponential random graph model". PloS One. [[Available Here](http://dx.plos.org/10.1371/journal.pone.0030136)]
* James D. Wilson, Matthew J. Denny, Shankar Bhamidi, Skyler Cranmer, and Bruce Desmarais (2015). "Stochastic Weighted Graphs: Flexible Model Specification and Simulation". [[Available Here](http://arxiv.org/abs/1505.04015)]

## Installation

### Requirements for using C++ code with R

See the **Requirements for using C++ code with R** section in the following tutorial: [Using C++ and R code Together with Rcpp](http://www.mjdenny.com/Rcpp_Intro.html).

### Installing The Package
  
To install this package from Github, you will need to Hadley Wickham's devtools package installed.

    install.packages("devtools")
    library("devtools")
    
Now we can install from Github using the following line:

    devtools::install_github("matthewjdenny/GERGM")

I have  had success installing with R 3.2.0+ installed but if you do not have the latest version of R installed, or run into some install errors (please email if you do), it should work as long as you install the dependencies first with the following block of code:

    install.packages( pkgs = c("BH","RcppArmadillo","ggplot2","methods"), dependencies = TRUE)

Once the `GERGM` package is installed, you may access its functionality as you would any other package by calling:

    library(GERGM)

If all went well, check out the `?GERGM` help file to see a full working example with info on how the data should look. 

## Basic Useage

We are currently in the process of completing adding functionality for using node level covariates in the model and should have this functionality included in the package by the beginning of July. The model is fully functioning  and tested for specification that do not include node leve covariates. **Note that if you are not using covariates, the network you supply to the `gergm()` function must have all edge values on the [0,1] interval.** The easiest way to do this is to use the included `Prepare_Network_and_Covariates()` function to transform the network onto the [0,1] interval. If you wish to include node level covariates in your specification, then you may specify additional parameters in the `Prepare_Network_and_Covariates()` function. A working example is provided in the following section.

## Examples

Here are two simple working examples using the `gergm( )` function: 

    library(GERGM)
    ########################### 1. No Covariates #############################
    # Preparing an unbounded network without covariates for gergm estimation #
    net <- matrix(rnorm(100,0,20),10,10)
    colnames(net) <- rownames(net) <- letters[1:10]
    formula <- net ~ recip + edges  
      
    test <- gergm(formula,
                  normalization_type = "division",
                  network_is_directed = TRUE,
                  use_MPLE_only = FALSE,
                  estimation_method = "Metropolis",
                  maximum_number_of_lambda_updates = 1,
                  maximum_number_of_theta_updates = 5,
                  number_of_networks_to_simulate = 40000,
                  thin = 1/10,
                  proposal_variance = 0.5,
                  downweight_statistics_together = TRUE,
                  MCMC_burnin = 10000,
                  seed = 456,
                  convergence_tolerance = 0.01,
                  MPLE_gain_factor = 0,
                  force_x_theta_update = 4)  
                  
    ########################### 2. Covariates #############################
    # Preparing an unbounded network with covariates for gergm estimation #
    net <- matrix(runif(100,0,1),10,10)
    colnames(net) <- rownames(net) <- letters[1:10]
    node_level_covariates <- data.frame(Age = c(25,30,34,27,36,39,27,28,35,40),
                                        Height = c(70,70,67,58,65,67,64,74,76,80))
    rownames(node_level_covariates) <- letters[1:10]
    network_covariate <- net + matrix(rnorm(100,0,.5),10,10)
    formula <- net ~ recip + edges + sender("Age") + nodecov("Height") + netcov("network_covariate")
         
    test <- gergm(formula,
                  covariate_data = node_level_covariates,
                  network_is_directed = TRUE,
                  use_MPLE_only = FALSE,
                  estimation_method = "Metropolis",
                  maximum_number_of_lambda_updates = 5,
                  maximum_number_of_theta_updates = 5,
                  number_of_networks_to_simulate = 500000,
                  thin = 1/10,
                  proposal_variance = 0.5,
                  downweight_statistics_together = TRUE,
                  MCMC_burnin = 200000,
                  seed = 456,
                  convergence_tolerance = 0.01,
                  MPLE_gain_factor = 0,
                  force_x_theta_update = 2)

Finally you will want to check the `output_directory` which will contain a number of .pdf's which can aide in assesing model fit and in determining the statistical significance of theta parameter estimates. 

### Output

If `output_name` is specified in the `gergm()` function, then five files will be automatically generated and saved to the `output_directory`. The example file names provided below are for `output_name = "Test"`:

* **"Test_GOF.pdf"**  -- Normalized, standardized Goodness Of Fit plot comparing statistics of networks simulated from final parameter estimates to the observed network.
* **"Test_Parameter_Estimates.pdf"** -- Theta parameter estimates with 90 and 90% confidence intervals.
* **"Test_GERGM_Object.Rdata"** -- The GERGM object returned by the `gergm()` functionafter estimation.
* **"Test_Estimation_Log.txt"** -- A log of all console output generated by the `gergm()` function.
* **"Test_Trace_Plot.pdf"** -- Trace plot from last round of network simulations used to generate GOF plot, useful for diagnosing an acceotance rate that is too low.


## Testing
            
So far, this package has been tested successfully on OSX 10.9.5. Please email me at <mzd5530@psu.edu> if you have success on another OS or run into any problems.
