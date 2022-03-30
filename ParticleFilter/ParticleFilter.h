
/* ================================
Created by Hori on 2021/7/16

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Particle filter
		Self-organizing state space model

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	x_t = Fx_{t-1} + Gv
	y_t = Hx_t + w
	x = (x, Q, R)
	v = N(0, Q)
	w = N(0, R)
	Q = N(0, e1)
	R = N(0, e2)

	(example)
		x_t = Fx_{t-1} + Gv
		y_t = Hx_t + w
		v: N(0, tau^2)
		w: Cauthy(0, sigma^2)

			  x_{t}        =       F            x_{t-1}        +     G         v_{t}

		[     x_{t}      ] = [2, -1, 0, 0][     x_{t-1}      ] + [1, 0, 0][   v_{x_t}   ], v_x{t} ~ N(0, tau^2)
		[    x_{t-1}     ]   [1,  0, 0, 0][     x_{t-2}      ]   [0, 0, 0][ v_{tau^2_t} ], v_{tau^2_t} ~ N(0, hhtau)
		[ log(tau^2_{t}) ]   [0,  0, 1, 0][ log(tau^2_{t-1}) ]   [0, 1, 0][v_{sigma^2_t}], v_{sigma^2_t} ~ N(0, hhsigma)
		[log(sigma^2_{t})]   [0,  0, 0, 1][log(sigma^2_{t-1})]   [0, 0, 1]


		y_{t} =      H           x_{t}        + w_{t}

		y_{t} = [1 0, 0, 0][     x_{t}      ] + w_{y_t}, w_{y_t} ~ Cauthy(0, sigma^2)
						   [    x_{t-1}     ]
						   [ log(tau^2_{t}) ]
						   [log(sigma^2_{t})]

	1. Initialize (parameters, parent_index(using uniform))	<- parent_index: (n_lag, n_particles)
	2. Generate v_{t} = [v_{x_t}, v_{tau}, v_{sigma}]
	3. Calculate x_{t} = Fx_{t-1} + Gv_{t}
	4. Calculate likelihood w = p(y_{t}|x_{t})	<- Cauthy, mu = x_{t}
	5. Normalize w = w/Sigma_w, and to be Multinomial distribution
	6. Resampling using multinomial pdf
	7. Indexes of resmapling set to parent_index (0, n_particles) and exclude (n_lag - 1, n_particles)
	8. Smoothing
		8.1. smoothing_index = parent_index[j+1,:][smoothing_index]
		8.2. x_t_given_t[t-j] = x_t_given_t[t-j][smoothing_index]


	Usage
		1. Set

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef PARTICLE_FILTER_H
#define PARTICLE_FILTER_H

#include "pch.h"


class ParticleFilter {
public:
	ParticleFilter();
	ParticleFilter(const int input_dim, const std::string r_folder_path);
	
	void fit(const Eigen::VectorXd data);
	void setInitParams(const double x0_min, const double x0_max,
					   const double tau0_mean, const double tau0_var,
					   const double sigma0_mean, const double sigma0_var);

	void initialize();
	void observedDouble(const double obs_data);
	void observedVector(const Eigen::VectorXd obs_data);

	std::vector<Eigen::VectorXd> getSmoothed() const { return smoothed; }

	void showSequence(const bool is_data, const bool is_filter, const bool is_smooth);
	void showSequence(const bool is_data, const bool is_filter, const bool is_smooth,
					  const std::unordered_map<std::string, std::string> params);

	//std::vector<double> getExpLatent();


private:
	bool is_test;
	int dim, n_particles, lag;	// th = n_sample / resampling_th, if resampling_th is 1, sure to resample
	double resampling_th, threshold;

	// For initialization
	double latent_t0_min, latent_t0_max, log_tau2_t0_mean, tau2_t0_var, log_sigma2_t0_mean, sigma2_t0_var;

	// Hyper hyper parameter for tau^2_t and sigma^2_t
	double hyper_hyper_tau;		// pow(x, 2), x shows standard deviation of log(tau^2)
	double hyper_hyper_sigma;	// pow(x, 2), x shows standard deviation of log(sigma^2)

	std::string result_folder, result_pred, result_filter, result_smooth;
	std::string pf_type, pf_model, noise_distribution;
	bool is_cal_prenoise;

	Eigen::VectorXd weights;
	std::vector<Eigen::VectorXi> parent_index;	// For smoothing (lag+1, n_particles)

	std::vector<Eigen::VectorXd> obs;			// data y_t
	std::vector<Eigen::VectorXd> filtered, smoothed;
	std::vector<Eigen::MatrixXd> filter_samples;	// hidden x_{ t | t }
	std::vector<Eigen::MatrixXd> x_t_hist;

	std::vector<double> exp_x;

	// Hyper parameters
	Eigen::MatrixXd F, G, H;


	void onePeriodPredictiveDistribution(const int t, const int full_length);
	Eigen::MatrixXd systemModel(const Eigen::MatrixXd x_t_minus_1, const Eigen::MatrixXd v_t);
	Eigen::MatrixXd observationModel(const Eigen::MatrixXd x_t_given_t_minus_1);
	Eigen::VectorXd filteringPDF(const Eigen::VectorXd obs, const Eigen::MatrixXd y_t_given_x_t,
								 const Eigen::VectorXd log_sigma2, const std::string pdf);
	Eigen::VectorXd estimateOneData(const Eigen::MatrixXd x_t_given_t_minus_1);
	Eigen::VectorXi resampling(const Eigen::VectorXd prob);

};



#endif //PARTICLE_FILTER_H