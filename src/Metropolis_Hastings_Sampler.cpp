// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::depends(BH)]]

#include <RcppArmadillo.h>
#include <boost/random.hpp>
#include <boost/random/uniform_real_distribution.hpp>


#include <boost/config/no_tr1/cmath.hpp>
#include <istream>
#include <iosfwd>
#include <boost/assert.hpp>
#include <boost/limits.hpp>
#include <boost/static_assert.hpp>
#include <boost/random/detail/config.hpp>
#include <boost/random/detail/operators.hpp>
#include <boost/random/uniform_01.hpp>


using namespace Rcpp;

namespace mjd {

    namespace random {
	// Distributed under the Boost Software License, Version 1.0.
	//    (See http://www.boost.org/LICENSE_1_0.txt)
    // Coppied from the Boost libraries because they use an assert statement
    // which will not pass r cmd check.
    // http://www.boost.org/doc/libs/1_53_0/boost/random/normal_distribution.hpp
    // deterministic Box-Muller method, uses trigonometric functions

    /**
    * Instantiations of class template normal_distribution model a
    * \random_distribution. Such a distribution produces random numbers
    * @c x distributed with probability density function
    * \f$\displaystyle p(x) =
    *   \frac{1}{\sqrt{2\pi\sigma}} e^{-\frac{(x-\mu)^2}{2\sigma^2}}
    * \f$,
    * where mean and sigma are the parameters of the distribution.
    */
    template<class RealType = double>
    class normal_distribution
    {
    public:
      typedef RealType input_type;
      typedef RealType result_type;

      class param_type {
      public:
        typedef normal_distribution distribution_type;

        /**
        * Constructs a @c param_type with a given mean and
        * standard deviation.
        *
        * Requires: sigma >= 0
        */
        explicit param_type(RealType mean_arg = RealType(0.0),
                            RealType sigma_arg = RealType(1.0))
          : _mean(mean_arg),
            _sigma(sigma_arg)
            {}

        /** Returns the mean of the distribution. */
        RealType mean() const { return _mean; }

        /** Returns the standand deviation of the distribution. */
        RealType sigma() const { return _sigma; }

        /** Writes a @c param_type to a @c std::ostream. */
        BOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, param_type, parm)
        { os << parm._mean << " " << parm._sigma ; return os; }

        /** Reads a @c param_type from a @c std::istream. */
        BOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, param_type, parm)
        { is >> parm._mean >> std::ws >> parm._sigma; return is; }

        /** Returns true if the two sets of parameters are the same. */
        BOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(param_type, lhs, rhs)
        { return lhs._mean == rhs._mean && lhs._sigma == rhs._sigma; }

        /** Returns true if the two sets of parameters are the different. */
        BOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(param_type)

      private:
        RealType _mean;
        RealType _sigma;
      };

      /**
      * Constructs a @c normal_distribution object. @c mean and @c sigma are
      * the parameters for the distribution.
      *
      * Requires: sigma >= 0
      */
      explicit normal_distribution(const RealType& mean_arg = RealType(0.0),
                                   const RealType& sigma_arg = RealType(1.0))
        : _mean(mean_arg), _sigma(sigma_arg),
          _r1(0), _r2(0), _cached_rho(0), _valid(false)
          {}

      /**
      * Constructs a @c normal_distribution object from its parameters.
      */
      explicit normal_distribution(const param_type& parm)
        : _mean(parm.mean()), _sigma(parm.sigma()),
          _r1(0), _r2(0), _cached_rho(0), _valid(false)
          {}

      /**  Returns the mean of the distribution. */
      RealType mean() const { return _mean; }
      /** Returns the standard deviation of the distribution. */
      RealType sigma() const { return _sigma; }

      /** Returns the smallest value that the distribution can produce. */
      RealType min BOOST_PREVENT_MACRO_SUBSTITUTION () const
      { return -std::numeric_limits<RealType>::infinity(); }
      /** Returns the largest value that the distribution can produce. */
      RealType max BOOST_PREVENT_MACRO_SUBSTITUTION () const
      { return std::numeric_limits<RealType>::infinity(); }

      /** Returns the parameters of the distribution. */
      param_type param() const { return param_type(_mean, _sigma); }
      /** Sets the parameters of the distribution. */
      void param(const param_type& parm)
      {
        _mean = parm.mean();
        _sigma = parm.sigma();
        _valid = false;
      }

      /**
      * Effects: Subsequent uses of the distribution do not depend
      * on values produced by any engine prior to invoking reset.
      */
      void reset() { _valid = false; }

      /**  Returns a normal variate. */
      template<class Engine>
      result_type operator()(Engine& eng)
      {
        using std::sqrt;
        using std::log;
        using std::sin;
        using std::cos;

        if(!_valid) {
          _r1 = boost::uniform_01<RealType>()(eng);
          _r2 = boost::uniform_01<RealType>()(eng);
          _cached_rho = sqrt(-result_type(2) * log(result_type(1)-_r2));
          _valid = true;
        } else {
          _valid = false;
        }
        // Can we have a boost::mathconst please?
        const result_type pi = result_type(3.14159265358979323846);

        return _cached_rho * (_valid ?
                              cos(result_type(2)*pi*_r1) :
                                sin(result_type(2)*pi*_r1))
          * _sigma + _mean;
      }

      /** Returns a normal variate with parameters specified by @c param. */
      template<class URNG>
      result_type operator()(URNG& urng, const param_type& parm)
      {
        return normal_distribution(parm)(urng);
      }

      /** Writes a @c normal_distribution to a @c std::ostream. */
      BOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, normal_distribution, nd)
      {
        os << nd._mean << " " << nd._sigma << " "
           << nd._valid << " " << nd._cached_rho << " " << nd._r1;
        return os;
      }

      /** Reads a @c normal_distribution from a @c std::istream. */
      BOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, normal_distribution, nd)
      {
        is >> std::ws >> nd._mean >> std::ws >> nd._sigma
           >> std::ws >> nd._valid >> std::ws >> nd._cached_rho
           >> std::ws >> nd._r1;
           return is;
      }

      /**
      * Returns true if the two instances of @c normal_distribution will
      * return identical sequences of values given equal generators.
      */
      BOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(normal_distribution, lhs, rhs)
      {
        return lhs._mean == rhs._mean && lhs._sigma == rhs._sigma
        && lhs._valid == rhs._valid
        && (!lhs._valid || (lhs._r1 == rhs._r1 && lhs._r2 == rhs._r2));
      }

      /**
      * Returns true if the two instances of @c normal_distribution will
      * return different sequences of values given equal generators.
      */
      BOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(normal_distribution)

    private:
      RealType _mean, _sigma;
      RealType _r1, _r2, _cached_rho;
      bool _valid;

    };

    } // namespace random

    using random::normal_distribution;
    using std::pow;
    using std::exp;
    using std::sqrt;

    // Returns the erf() of a value (not super precice, but ok)
    double erf(double x)
    {
     double y = 1.0 / ( 1.0 + 0.3275911 * x);
     return 1 - (((((
            + 1.061405429  * y
            - 1.453152027) * y
            + 1.421413741) * y
            - 0.284496736) * y
            + 0.254829592) * y)
            * exp (-x * x);
    }

    // Returns the probability of x, given the distribution described by mu and sigma.
    double pdf(double x, double mu, double sigma)
    {
      //Constants
      static const double pi = 3.14159265;
      return exp( -1 * (x - mu) * (x - mu) / (2 * sigma * sigma)) / (sigma * sqrt(2.0 * pi));
    }

    // Returns the probability of [-inf,x] of a gaussian distribution
    double cdf(double x, double mu, double sigma)
    {
        return 0.5 * (1 + mjd::erf((x - mu) / (sigma * sqrt(2.0))));
    }


  // Function to calculate the number of out 2-stars
  double Out2Star(arma::mat net,
                  arma::mat triples,
                  double alpha,
                  int together) {

    int number_of_triples = triples.n_rows;
    double st1 = 0;
    double st2 = 0;
    double st3 = 0;

    for (int i = 0; i < number_of_triples; ++i) {
      st1 += net(triples(i, 0), triples(i, 1)) * net(triples(i, 0),
          triples(i, 2));
      st2 += net(triples(i, 1), triples(i, 0)) * net(triples(i, 1),
          triples(i, 2));
      st3 += net(triples(i, 2), triples(i, 0)) * net(triples(i, 2),
          triples(i, 1));
    }

    double to_return = 0;
    if (together == 1) {
      to_return = pow((st1 + st2 + st3), alpha);
    } else {
      to_return = pow(st1, alpha) + pow(st2, alpha) + pow(st3, alpha);
    }
    return to_return;
  };

  // Function to calculate the number of in 2-stars
  double In2Star(arma::mat net,
                 arma::mat triples,
                 double alpha,
                 int together) {

    int number_of_triples = triples.n_rows;
    double st1 = 0;
    double st2 = 0;
    double st3 = 0;

    for (int i = 0; i < number_of_triples; ++i) {
      st1 += net(triples(i, 2), triples(i, 0)) * net(triples(i, 1),
          triples(i, 0));
      st2 += net(triples(i, 2), triples(i, 1)) * net(triples(i, 0),
          triples(i, 1));
      st3 += net(triples(i, 0), triples(i, 2)) * net(triples(i, 1),
          triples(i, 2));
    }

    double to_return = 0;
    if (together == 1) {
      to_return = pow((st1 + st2 + st3), alpha);
    } else {
      to_return = pow(st1, alpha) + pow(st2, alpha) + pow(st3, alpha);
    }
    return to_return;
  };

  // Function to calculate the number of transitive triads
  double TTriads(arma::mat net,
                 arma::mat triples,
                 double alpha,
                 int together) {

    int number_of_triples = triples.n_rows;
    double st1 = 0;
    double st2 = 0;
    double st3 = 0;
    double st4 = 0;
    double st5 = 0;
    double st6 = 0;

    for (int i = 0; i < number_of_triples; ++i) {
      st1 += net(triples(i, 0), triples(i, 1)) * net(triples(i, 1),
          triples(i, 2)) * net(triples(i, 0), triples(i, 2));
      st2 += net(triples(i, 0), triples(i, 1)) * net(triples(i, 2),
          triples(i, 1)) * net(triples(i, 2), triples(i, 0));
      st3 += net(triples(i, 0), triples(i, 1)) * net(triples(i, 2),
          triples(i, 1)) * net(triples(i, 0), triples(i, 2));
      st4 += net(triples(i, 1), triples(i, 0)) * net(triples(i, 1),
          triples(i, 2)) * net(triples(i, 2), triples(i, 0));
      st5 += net(triples(i, 1), triples(i, 0)) * net(triples(i, 1),
          triples(i, 2)) * net(triples(i, 0), triples(i, 2));
      st6 += net(triples(i, 1), triples(i, 0)) * net(triples(i, 2),
          triples(i, 1)) * net(triples(i, 2), triples(i, 0));
    }

    double to_return = 0;
    if (together == 1) {
      to_return = pow((st1 + st2 + st3 + st4 + st5 + st6), alpha);
    } else {
      to_return = pow(st1, alpha) + pow(st2, alpha) + pow(st3, alpha)
          + pow(st4, alpha) + pow(st5, alpha) + pow(st6, alpha);
    }
    return to_return;
  };

  // Function to calculate the number of closed triads
  double CTriads(arma::mat net,
                 arma::mat triples,
                 double alpha,
                 int together){

    int number_of_triples = triples.n_rows;
    double st1 = 0;
    double st2 = 0;

    for (int i = 0; i < number_of_triples; ++i) {
      st1 += net(triples(i, 0), triples(i, 1)) * net(triples(i, 1),
          triples(i, 2)) * net(triples(i, 2), triples(i, 0));
      st2 += net(triples(i, 1), triples(i, 0)) * net(triples(i, 2),
          triples(i,1)) * net(triples(i, 0), triples(i, 2));
    }

    double to_return = 0;
    if (together == 1) {
      to_return = pow((st1 + st2), alpha);
    } else {
      to_return = pow(st1, alpha) + pow(st2, alpha);
    }
    return to_return;
  };

  // Function to calculate the number of reciprocated edges
  double Recip(arma::mat net,
               arma::mat pairs,
               double alpha,
               int together) {

    int number_of_pairs = pairs.n_rows;
    double st1 = 0;
    for (int i = 0; i < number_of_pairs; ++i) {
        st1 += net(pairs(i, 0), pairs(i, 1)) * net(pairs(i, 1), pairs(i, 0));
    }

    double to_return = pow(st1, alpha);
    return to_return;
  };

  // Function to calculate the density of the network
  double EdgeDensity(arma::mat net,
                     arma::mat pairs,
                     double alpha,
                     int together) {

    int number_of_pairs = pairs.n_rows;
    double st1 = 0;
    double st2 = 0;
    double to_return = 0;

    for (int i = 0; i < number_of_pairs; ++i) {
      st1 += net(pairs(i, 0), pairs(i, 1));
      st1 += net(pairs(i, 1), pairs(i, 0));
      st2 += pow(net(pairs(i, 0), pairs(i, 1)), alpha);
      st2 += pow(net(pairs(i, 1), pairs(i, 0)), alpha);
    }

    if (together == 1) {
      to_return = pow(st1, alpha);
    } else {
      to_return = st2;
    }
    return to_return;
  };

  // Function that will calculate h statistics
  double CalculateNetworkStatistics(arma::mat current_network,
                                    arma::vec statistics_to_use,
                                    arma::vec thetas,
                                    arma::mat triples,
                                    arma::mat pairs,
                                    arma::vec alphas,
                                    int together) {

    double to_return = 0;

    if (statistics_to_use[0] == 1) {
        to_return += thetas[0] * Out2Star(current_network, triples, alphas[0],
            together);
    }
    if (statistics_to_use[1] == 1) {
        to_return += thetas[1] * In2Star(current_network, triples, alphas[1],
            together);
    }
    if (statistics_to_use[2] == 1) {
        to_return += thetas[2] * CTriads(current_network, triples, alphas[2],
            together);
    }
    if (statistics_to_use[3] == 1) {
        to_return += thetas[3] * Recip(current_network, pairs, alphas[3],
            together);
    }
    if (statistics_to_use[4] == 1) {
        to_return += thetas[4] * TTriads(current_network, triples, alphas[4],
            together);
    }
    if (statistics_to_use[5] == 1) {
        to_return += thetas[5] * EdgeDensity(current_network, pairs,
            alphas[5], together);
    }
    return to_return;
  };

  // Function that will calculate and save all of the h statistics for a network
  arma::vec save_network_statistics(arma::mat current_network,
                                    arma::mat triples,
                                    arma::mat pairs,
                                    arma::vec alphas,
                                    int together) {

    arma::vec to_return = arma::zeros(6);
    to_return[0] = Out2Star(current_network, triples, alphas[0], together);
    to_return[1] = In2Star(current_network, triples, alphas[1], together);
    to_return[2] = CTriads(current_network, triples, alphas[2], together);
    to_return[3] = Recip(current_network, pairs, alphas[3], together);
    to_return[4] = TTriads(current_network, triples, alphas[4], together);
    to_return[5] = EdgeDensity(current_network, pairs, alphas[5], together);
    return to_return;
  };

} //end of mjd namespace

using std::log;

// [[Rcpp::export]]
List Metropolis_Hastings_Sampler (int number_of_iterations,
          double shape_parameter,
          int number_of_nodes,
          arma::vec statistics_to_use,
          arma::mat initial_network,
          int take_sample_every,
          arma::vec thetas,
          arma::mat triples,
          arma::mat pairs,
          arma::vec alphas,
          int together,
          int seed,
          int number_of_samples_to_store) {

  // Allocate variables and data structures
  double variance = shape_parameter;
  int list_length = 4;
  List to_return(list_length);
  //int number_of_samples_to_store = ceil (number_of_iterations /
  //    take_sample_every);
  int number_of_thetas = statistics_to_use.n_elem;
  int MH_Counter = 0;
  int Storage_Counter = 0;
  arma::vec Accept_or_Reject = arma::zeros (number_of_iterations);
  arma::cube Network_Samples = arma::zeros (number_of_nodes, number_of_nodes,
      number_of_samples_to_store);
  arma::vec Mean_Edge_Weights = arma::zeros (number_of_samples_to_store);
  arma::mat Save_H_Statistics = arma::zeros (number_of_samples_to_store,
      number_of_thetas);
  arma::mat current_edge_weights = initial_network;

  // Set RNG and define uniform distribution
  boost::mt19937 generator(seed);
  //boost::random::uniform_real_distribution<double>  uniform_distribution(0.0,1.0);
  boost::uniform_01<double> uniform_distribution;

  // Outer loop over the number of samples
  for (int n = 0; n < number_of_iterations; ++n) {
    double log_prob_accept = 0;
    arma::mat proposed_edge_weights = current_edge_weights;
    // Run loop to sample new edge weights
    for (int i = 0; i < number_of_nodes; ++i) {
      for (int j = 0; j < number_of_nodes; ++j) {
        if (i != j) {

            double log_probability_of_current_under_new = 0;
            double log_probability_of_new_under_current = 0;
            //draw a new edge value centered at the old edge value
            double current_edge_value = current_edge_weights(i,j);
            //draw from a truncated normal
            mjd::normal_distribution<double> proposal(current_edge_value,variance);
            int in_zero_one = 0;
            //NumericVector new_edge_value = 0.5;
            double new_edge_value = 0.5;
            while(in_zero_one == 0){
				        //NumericVector temp = rnorm(1, current_edge_value, variance);
			          //new_edge_value = temp[0];
                new_edge_value = proposal(generator);
                if(new_edge_value > 0 & new_edge_value < 1){
                    in_zero_one = 1;
                }
            }
            if (new_edge_value > 0.999) {
                new_edge_value = 0.999;
            }
            if (new_edge_value < 0.001) {
                new_edge_value= 0.001;
            }
            //report(new_edge_value);

            // calculate the probability of the new edge under current beta dist
            double lower_bound = mjd::cdf(0,current_edge_value,variance);
            double upper_bound = mjd::cdf(1,current_edge_value,variance);
            double raw_prob = mjd::pdf(new_edge_value,current_edge_value,variance);
            double prob_new_edge_under_old = (raw_prob/(upper_bound - lower_bound));

            // calculate the probability of the current edge under new beta dist
            lower_bound = mjd::cdf(0,new_edge_value,variance);
            upper_bound = mjd::cdf(1,new_edge_value,variance);
            raw_prob = mjd::pdf(current_edge_value,new_edge_value,variance);
            double prob_old_edge_under_new = (raw_prob/(upper_bound - lower_bound));

            //save everything
            proposed_edge_weights(i,j) = new_edge_value;
            log_probability_of_new_under_current = log(prob_new_edge_under_old);
            log_probability_of_current_under_new = log(prob_old_edge_under_new);

            // Calculate acceptance probability
            log_prob_accept += (log_probability_of_current_under_new
              - log_probability_of_new_under_current);

        }
      }
    }


    double proposed_addition = mjd::CalculateNetworkStatistics(
              proposed_edge_weights, statistics_to_use, thetas, triples, pairs,
              alphas, together);
    double current_addition = mjd::CalculateNetworkStatistics(
              current_edge_weights, statistics_to_use, thetas, triples, pairs,
              alphas, together);

    log_prob_accept += (proposed_addition - current_addition);

    //double rand_num = uniform_distribution(generator);
    double rand_num = uniform_distribution(generator);
    double lud = 0;
    lud = log(rand_num);

    double accept_proportion = 0;
    // Accept or reject the new proposed positions
    if (log_prob_accept < lud) {
      accept_proportion +=0;
    } else {
      accept_proportion +=1;
      for (int i = 0; i < number_of_nodes; ++i) {
          for (int j = 0; j < number_of_nodes; ++j) {
              if (i != j) {
                  double temp = proposed_edge_weights(i, j);
                  current_edge_weights(i, j) = temp;
              }
          }
      }
    }

    Accept_or_Reject[n] = accept_proportion;
    Storage_Counter += 1;

    // Save network statistics
    if (Storage_Counter == take_sample_every) {

      arma::vec save_stats = mjd::save_network_statistics(current_edge_weights,
          triples, pairs, alphas, together);

      for (int m = 0; m < 6; ++m) {
        Save_H_Statistics(MH_Counter, m) = save_stats[m];
      }

      double mew = 0;

      for (int i = 0; i < number_of_nodes; ++i) {
        for (int j = 0; j < number_of_nodes; ++j) {
          if (i != j) {

            //we use this trick to break the referencing
            double temp = current_edge_weights(i, j);
            Network_Samples(i, j, MH_Counter) = temp;
            mew += temp;
          }
        }
      }

      mew = mew / double(number_of_nodes * (number_of_nodes - 1));
      Mean_Edge_Weights[MH_Counter] = mew;
      Storage_Counter = 0;
      MH_Counter += 1;
    }
  }

  // Save the data and then return
  to_return[0] = Accept_or_Reject;
  to_return[1] = Network_Samples;
  to_return[2] = Save_H_Statistics;
  to_return[3] = Mean_Edge_Weights;
  return to_return;
}
