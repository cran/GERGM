test_that("That gergm with exponential downweighting works", {
  skip_on_cran()
  skip("For time")
  ########################### 1. No Covariates #############################
  # Preparing an unbounded network without covariates for gergm estimation #

  set.seed(12345)
  net <- matrix(rnorm(100,0,20),10,10)
  colnames(net) <- rownames(net) <- letters[1:10]


  # three parameter model
  formula <- net ~  edges + mutual(0.95) +  ttriads(0.95)

  test <- gergm(formula,
                number_of_networks_to_simulate = 40000,
                thin = 1/40,
                proposal_variance = 0.5,
                downweight_statistics_together = TRUE,
                MCMC_burnin = 10000,
                seed = 456,
                convergence_tolerance = 0.5)

  check_against <- c(2.347, -0.323)
  expect_equal(round(as.numeric(test@theta.coef[1,]),3), check_against)

})
test_that("undirected with exponential downweighting works", {
  skip_on_cran()
  skip("For time")
  #check that code works for undirected network
  #
  set.seed(12345)
  net <- matrix(rnorm(100,0,20),10,10)
  colnames(net) <- rownames(net) <- letters[1:10]

  formula <- net ~  edges + ttriads(alpha = 0.9) + twostars(0.9)

  test <- gergm(formula,
                normalization_type = "division",
                network_is_directed = FALSE,
                use_MPLE_only = FALSE,
                estimation_method = "Metropolis",
                number_of_networks_to_simulate = 40000,
                thin = 1/40,
                proposal_variance = 0.5,
                downweight_statistics_together = TRUE,
                MCMC_burnin = 10000,
                seed = 456,
                convergence_tolerance = 0.5)

  check_against <- c(0.276, -0.587)
  expect_equal(round(as.numeric(test@theta.coef[1,]),3), check_against)

})
test_that("covariates with exponential downweighting works", {
  skip_on_cran()
  skip("For time")

  set.seed(12345)
  net <- matrix(runif(100,0,1),10,10)
  colnames(net) <- rownames(net) <- letters[1:10]
  node_level_covariates <- data.frame(Age = c(25,30,34,27,36,39,27,28,35,40),
                                      Height = c(70,70,67,58,65,67,64,74,76,80),
                                      Type = c("A","B","B","A","A","A","B","B","C","C"))
  rownames(node_level_covariates) <- letters[1:10]
  network_covariate <- net + matrix(rnorm(100,0,.5),10,10)
  formula <- net ~ edges + mutual(0.95) + ttriads(0.95) + sender("Age") +
    netcov("network_covariate") + nodemix("Type",base = "A")

  test <- gergm(formula,
                covariate_data = node_level_covariates,
                number_of_networks_to_simulate = 100000,
                thin = 1/100,
                proposal_variance = 0.1,
                downweight_statistics_together = TRUE,
                MCMC_burnin = 50000,
                seed = 456,
                convergence_tolerance = 0.5)

    check_against <- c(0.901, -0.090, -0.016, -0.025, -0.024, -0.056, -0.056,
                       -0.035, 0.002, -0.040, -0.050,  3.065,  0.129, -1.931)
    check <- c(round(as.numeric(test@theta.coef[1,]),3),round(as.numeric(test@lambda.coef[1,]),3))
    expect_equal(check, check_against)

})



test_that("That weighted MPLE works", {
  skip_on_cran()
  skip("For time")
  ########################### 1. No Covariates #############################
  # Preparing an unbounded network without covariates for gergm estimation #

  set.seed(12345)
  net <- matrix(rnorm(100,0,20),10,10)
  colnames(net) <- rownames(net) <- letters[1:10]


  # three parameter model
  formula <- net ~  edges + mutual(0.8) +  ttriads(0.8)

  # MPLE ONLY
  test <- gergm(formula,
                normalization_type = "division",
                network_is_directed = TRUE,
                use_MPLE_only = TRUE,
                estimation_method = "Metropolis",
                number_of_networks_to_simulate = 40000,
                thin = 1/40,
                proposal_variance = 0.5,
                downweight_statistics_together = TRUE,
                MCMC_burnin = 10000,
                seed = 456,
                convergence_tolerance = 0.5,
                MPLE_gain_factor = 0.05,
                force_x_theta_updates = 1,
                weighted_MPLE = TRUE)

  check_against <- c(-9.440,  1.428)
  expect_equal(round(as.numeric(test@theta.coef[1,]),3), check_against)

})
