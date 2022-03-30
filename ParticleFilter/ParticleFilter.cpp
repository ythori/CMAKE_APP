

#include "ParticleFilter.h"
#include "ParticleFilterShared.h"
#include "ParticleFilterPDF.h"
#include "pch.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"
#include "../ProbabilityFunctions/ProbabilityFunctions.h"
#include "../ProbabilityFunctions/GenRandomNumber.h"
#include "../Shared/matplotlibCpp.h"


ParticleFilter::ParticleFilter() {}

ParticleFilter::ParticleFilter(const int input_dim, const std::string r_folder_path) {
	is_test = false;
	dim = input_dim;
	if (is_test == true) n_particles = 3;
	else n_particles = 10000;
	resampling_th = 0.9;	// th = n_sample / resampling_th, if resampling_th is 1, sure to resample
	threshold = resampling_th * n_particles;
	lag = 5;	// For smoothing
	weights = Eigen::VectorXd::Constant(n_particles, 1. / n_particles);
	x_t_hist.clear();
	x_t_hist.reserve(lag);

	// Init params
	latent_t0_min = -0.462713 -0.1;
	latent_t0_max = -0.462713+0.1;
	log_tau2_t0_mean = log(0.1);	// log(0.1) < log(0.5) < log(1) -> -2.302 < -0.693 < 0
	tau2_t0_var = 0.5;
	tau2_t0_var = 0;
	log_sigma2_t0_mean = log(2.0);
	sigma2_t0_var = 0.5;
	sigma2_t0_var = 0;

	// Fix hyper-hyper parameters
	hyper_hyper_tau = 0.01;		// pow(x, 2), x shows standard deviation of log(tau^2)
	hyper_hyper_sigma = 0.01;	// pow(x, 2), x shows standard deviation of log(sigma^2)

	exp_x.clear();

	result_folder = r_folder_path;
	result_pred = r_folder_path + "pred.txt";
	result_filter = r_folder_path + "filter.txt";
	result_smooth = r_folder_path + "smooth.txt";
	pf_type = "self_organized";
	pf_model = "second_order_difference";
	noise_distribution = "normal";	// or cauchy
	is_cal_prenoise = false;
}

void ParticleFilter::setInitParams(const double x0_min, const double x0_max,
								   const double tau0_mean, const double tau0_var,
								   const double sigma0_mean, const double sigma0_var) {
	/* ================================
		For tempo (IOI) estiamtion
			latent_t0_min = 60.0;	// Infimum of tempo
			latent_t0_max = 90.0;	// Supremum of tempo

			log_tau_t0_mean = log(5);	// 1.60943	-> log(0) < log(tau^2) < log(10) -> 1 < 1.60943 < 2.3025
			tau2_t0_var = 1;

			sigma2_t0_mean = log(0.1);	// -2.3025 -> log(0.01) < log(sigma^2) < log(1) -> -4.6051 < -2.3025 < 0
			sigma2_t0_var = 4;
	================================ */

	latent_t0_min = x0_min;
	latent_t0_max = x0_max;
	log_tau2_t0_mean = tau0_mean;
	tau2_t0_var = tau0_var;
	log_sigma2_t0_mean = sigma0_mean;
	sigma2_t0_var = sigma0_var;
}


void ParticleFilter::initialize() {
	std::shared_ptr<GenRandom> GR = std::make_shared<GenRandom>();

	F = setF(pf_type, pf_model, dim);
	G = setG(pf_type, pf_model, dim, F.cols(), is_cal_prenoise);
	H = setH(dim, F.cols());

	//echo(F);	// ex) [[2, -1, 0, 0], [1, 0, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]
	//echo(G);	// ex) [[1, 0, 0], [0, 0, 0], [0, 1, 0], [0, 0, 1]
	//echo(H);	// ex) [1, 0, 0, 0]

	// Latent vector x's initializing
	Eigen::MatrixXd init_filter = Eigen::MatrixXd::Zero(F.cols(), n_particles);		// x_0|0

	// Set X = [x_0, x_{-1}, log(tau^2_{0}), log(sigma^2_{0})
	for (int i = 0; i < F.cols(); ++i) {
		if (i < F.cols() - 2) {	// i = 0, 1
			// Initialize latent variables {x}_{t-1|t-1}
			echoes("init_x", latent_t0_min, latent_t0_max);
			GR->genUni(latent_t0_min, latent_t0_max, n_particles);
			init_filter.row(i) = GR->getRandomVectorDouble();
			if (is_test == true) init_filter.row(i) = Eigen::VectorXd::Constant(n_particles, -0.462713);
		}
		else if (i == F.cols() - 2) {	// i = 2 = log(tau^2)
			// Initialize tau^2
			if (tau2_t0_var != 0) {
				GR->genNorm1D(log_tau2_t0_mean, tau2_t0_var, n_particles);	// N(0, 25)
				init_filter.row(i) = GR->getRandomVectorDouble();
			}
			else {
				echoes("lot_tau2_constant", log_tau2_t0_mean);
				Eigen::VectorXd t_vec = Eigen::VectorXd::Constant(n_particles, log_tau2_t0_mean);
				init_filter.row(i) = t_vec;
			}
		}

		else {	// log(sigma^2)
			// Initialize sigma^2
			if (sigma2_t0_var != 0) {
				GR->genNorm1D(log_sigma2_t0_mean, sigma2_t0_var, n_particles);	// N(0, 0.25)
				init_filter.row(i) = GR->getRandomVectorDouble();
			}
			else {
				echoes("lot_sigma2_constant", log_sigma2_t0_mean);
				Eigen::VectorXd t_vec = Eigen::VectorXd::Constant(n_particles, log_sigma2_t0_mean);
				init_filter.row(i) = t_vec;
			}
		}
		
	}
	filter_samples.emplace_back(init_filter);

	//for (int i = 0; i < F.cols(); ++i) {
	//	if (i < F.cols() - 2) echo(init_filter.row(i));
	//	else echo(init_filter.row(i).array().exp());
	//}

	//// Set initial parent_index
	//for (int i = 0; i < lag + 1; ++i) {
	//	GR->genInt(0, n_particles - 1, n_particles);
	//	parent_index.emplace_back(GR->getRandomVectorInt());
	//	//echo(parent_index[i]);
	//}

}

void ParticleFilter::fit(const Eigen::VectorXd data) {
	// In case that all observed date is obtained
	initialize();
	//echo(F);
	//echo(G);
	//echo(H);
	if (is_test == true) echoes("Init pars\n", filter_samples.back());


	
	for (int t = 0; t < data.size(); ++t) {
		if (t % 100 == 0) echoes(t + 1, "/", data.size() + 1);
		if (is_test == true) {
			if (t == 10) break;
			if (t == 30) break;
		}
		// Filtering
		//	Gen v_t, culculate x_{t|t-1}, likelihood(=weight, p(y_t|x_{t|t-1})
	
		if (dim == 1) observedDouble(data(t));
		else observedVector(data.row(t));
		onePeriodPredictiveDistribution(t, data.size());
		//Eigen::MatrixXd v_t = Eigen::MatrixXd::Zero(G.cols(), n_particles);
	}
	echoes(obs.size(), filtered.size(), smoothed.size());
	//for (auto x : filtered) echo(x);
}

//
//std::vector<double> ParticleFilter::getExpLatent() {
//	exp_x.clear();
//	for (int t = 1; t < filter_samples.size(); ++t) {
//		//auto exp_x_t = x_t.mean();
//		auto w_t = filteringPDF(obs[t - 1], filter_samples[t].row(0), filter_samples[t].row(F.cols() - 1), "cauthy");
//		//echo(w_t);
//		//echo(filter_samples[t].row(0));
//		//echo(12345);
//		//echo(filter_samples[t].row(0) * w_t);
//		//echo(123456);
//		auto exp_x_t = filter_samples[t].row(0) * w_t;
//		//echo(exp_x_t);
//		exp_x.emplace_back(exp_x_t);
//
//		if (is_test == true) {
//			echo(t);
//			auto x_t = filter_samples[t].row(0);
//			auto log_tau2 = filter_samples[t].row(F.cols() - 2);
//			auto log_sigma2 = filter_samples[t].row(F.cols() - 1);
//			auto tau2 = log_tau2.array().exp();
//			auto sigma2 = log_tau2.array().exp();
//			echo(x_t);
//			echo(w_t.transpose());
//			echo(exp_x_t);
//			echo(tau2);
//			echo(sigma2);
//		}
//
//	}
//	return exp_x;
//}


void ParticleFilter::observedDouble(double obs_data) {
	Eigen::VectorXd temp = Eigen::VectorXd::Zero(1);
	temp(0) = obs_data;
	obs.push_back(temp);
}


void ParticleFilter::observedVector(Eigen::VectorXd obs_data) {
	obs.push_back(obs_data);
}


Eigen::MatrixXd ParticleFilter::systemModel(const Eigen::MatrixXd x_t_minus_1, const Eigen::MatrixXd v_t) {
	// F * x_{t-1} + G * v_{t}
	// Return (F.cols(), n_particles)

	//echo(F);
	//echo(x_t_minus_1);
	//echo(F * x_t_minus_1);
	//echo(G);
	//echo(v_t);
	//echo(G * v_t);
	//echo(F * x_t_minus_1 + G * v_t);
	return F * x_t_minus_1 + G * v_t;
}


Eigen::MatrixXd ParticleFilter::observationModel(const Eigen::MatrixXd x_t_given_t_minus_1) {
	// H * x_{t}
	// Return (H.cols(), n_particles)
	// Observation noise is used in next step which is calculated likelihood

	//echoes(H.rows(), H.cols(), x_t_given_t_minus_1.rows(), x_t_given_t_minus_1.cols());
	return H * x_t_given_t_minus_1;
}


Eigen::VectorXd ParticleFilter::filteringPDF(const Eigen::VectorXd obs, const Eigen::MatrixXd y_t_given_x_t,
											 const Eigen::VectorXd log_sigma2, const std::string pdf) {
	// Calculate log_likelihood
	// Return log value

	Eigen::VectorXd log_w_t = Eigen::VectorXd::Zero(n_particles);
	//Eigen::VectorXd w_t = Eigen::VectorXd::Zero(n_particles);
	if (pdf == "cauthy") {
		//echoes(pdf, obs);
		//echo(y_t_given_x_t);
		//echo(log_sigma2);
		auto sigma = log_sigma2.array().exp();
		for (int n = 0; n < n_particles; ++n) {
			for (int d = 0; d < dim; ++d) {
				log_w_t(n) += pdfLnCauchy(obs(d), y_t_given_x_t(d, n), sigma(n));
			}
		}
		//echo(log_w_t);
		//w_t = logsumexpEigen(log_w_t);
		//echo("");
		//echo(w_t);
		//echo("");
		//w_t = w_t.array().exp();
	}

	else if (pdf == "normal") {
		auto sigma = log_sigma2.array().exp();
		for (int n = 0; n < n_particles; ++n) {
			for (int d = 0; d < dim; ++d) {
				log_w_t(n) += pdfLnNorm(obs(d), y_t_given_x_t(d, n), sigma(n));
			}
		}
		//w_t = logsumexpEigen(log_w_t);
		//w_t = w_t.array().exp();
	}

	else if (pdf == "ioi") {
		//  pdfIOI(const double obs_ioi, const double est_tempo, const double score_ioi, const double score_tempo, const double var)
		echo(1);
	}
	return log_w_t;
}

Eigen::VectorXd ParticleFilter::estimateOneData(const Eigen::MatrixXd x_t_given_t_minus_1) {
	auto observation = observationModel(x_t_given_t_minus_1);	// H * x_{t|t}
	Eigen::VectorXd ret = Eigen::VectorXd::Zero(dim);
	for (int n = 0; n < n_particles; ++n) {
		for (int d = 0; d < dim; ++d) {
			ret(d) += observation(d, n) * weights(n);
		}
	}
	return ret;
}


Eigen::VectorXi ParticleFilter::resampling(const Eigen::VectorXd prob) {
	// Using multinomial distribution
	//std::shared_ptr<GenRandom> GR = std::make_shared<GenRandom>();
	//GR->genMultinomialChoice(prob, n_particles);
	//return GR->getRandomVectorInt();

	// Using layered sampling
	Eigen::VectorXi idx = Eigen::VectorXi::Zero(n_particles);
	//double u = genUniform(0, 1.0 / n_particles);
	auto steps = cumsum(prob);

	std::vector<double> randuni;
	for (int i = 0; i < n_particles; ++i) {
		randuni.push_back(frand());
	}
	std::sort(randuni.begin(), randuni.end());
	if (is_test == true) randuni = { 0.17164558, 0.26607704, 0.83571776 };
	int start = 0;
	for (int i = 0; i < n_particles; ++i) {
		for (int j = start; j < n_particles; ++j) {
			if (randuni[i] <= steps(j)) {
				idx(i) = j;
				start = j;
				break;
			}
		}
	}
	//echo("rs");
	//echo(prob);
	//echoes("idx", idx);
	return idx;
}



void ParticleFilter::onePeriodPredictiveDistribution(const int t, const int full_length) {
	std::string st = "";
	if (is_test == true) st = "test";
	else st = "";

	std::shared_ptr<GenRandom> GR = std::make_shared<GenRandom>();

	/* ================================
		Generate noise v_t
			v_t = [   v_{x_t}   ]
			      [   v_{y_t}   ]	// In case of dim = 2
				  [ v_{tau^2_t} ]	// In case of self_organization
				  [v_{sigma^2_t}]	// In case of self_organization
	================================ */
	Eigen::MatrixXd v_t = Eigen::MatrixXd::Zero(G.cols(), n_particles);

	// v_{x_t} ~ N(0, tau^2)
	int n_order = 1;
	if (pf_model == "second_order_difference") n_order += 1;	// x_t, x_t-1

	// v_{x_t}, v_{y_t}
	//echo(111);
	for (int d = 0; d < dim; ++d) {		// x_t, (x_t-1), y_t, (y_t-1)
		for (int n = 0; n < n_particles; ++n) {
			double tau2_mean, tau2_var;
			if (pf_type == "self_organized") {
				//double log_2_var = filter_samples.back().row(F.cols() - 2)[n];
				double log_2_var = filter_samples.back()(F.cols() - 2, n);
				tau2_mean = 0;
				//tau2_var = log_tau2_t0_mean;
				tau2_var = exp(log_2_var);
				//tau2_var = 0.1;
				//if (t >= 7) echo(log_2_var);
			}
			else {
				tau2_mean = 0;
				tau2_var = log_tau2_t0_mean;
			}
			//if (t >= 7) echoes(d, n, tau2_mean, tau2_var);
			v_t(d, n) = genNorm(tau2_mean, tau2_var);	// genNorm() requires parameter var
			//echoes(d, n, tau2_mean, tau2_var, v_t(d, n));
		}
	}
	//echo(v_t);
	// v_{tau^2_t} ~ N(0, hht)
	// v_{sigma^2_t} ~ N(0, hhs)
	//echo(222);
	if (pf_type == "self_organized") {
		GR->genNorm1D(0, hyper_hyper_tau, n_particles);
		v_t.row(G.cols() - 2) = GR->getRandomVectorDouble();	// v_t: x_t, x_t-1, sys_noise, obs_noise

		// v_{sigma^2_t} ~ N(0, hhs)
		GR->genNorm1D(0, hyper_hyper_sigma, n_particles);
		v_t.row(G.cols() - 1) = GR->getRandomVectorDouble();
	}

	if (is_test == true) {
		v_t << -0.02822862, -0.28050154, 0.16787399,
			-0.01674407, 0.00413569, 0.01817219,
			0.00732506, -0.00509825, -0.00483902;
	}
	//echo(v_t);
	//echo("=====");

	//Eigen::VectorXd tv = v_t.row(2);
	//echo(tv);

	/* ================================
		Process state space model
	================================ */

	/* ================================
		System model (update)
			: x_{t|t-1} = F * x_{t-1} + G * v_{t}
	================================ */
	//echo("system");
	_echoes(st, "Before pars\n", filter_samples.back());
	auto x_t_given_t_minus_1 = systemModel(filter_samples.back(), v_t);
	_echoes(st, "After pars\n", x_t_given_t_minus_1);
	//echo(x_t_given_t_minus_1.row(3));
	//filter_samples.emplace_back(x_t_given_t_minus_1);

	/* ================================
		Observation model (extract)
			: y_{t|t} = H * x_{t|t-1} + w_{t}
				: Actually, calculate only H * x_{t|t-1}
					-> In next step, calculate log_likelihood N_or_C(H * x_{t|t}, w_{t})
	================================ */
	//echo("obs");
	auto y_t_given_x_t = observationModel(x_t_given_t_minus_1);
	//_echo(st, "pars_obs");
	//_echo(st, y_t_given_x_t);

	/* ================================
		Likelihood
			: log_likelihood N_or_C(H * x_{t|t}, w_{t})
				: x_t_given_t_minus_1: [x_t, x_t-1, log(tau^2), log(sigma^2)]	<- bottom
					-> Observation noise: x_t_given_t_minus_1.row(F.cols() - 1)
			w_t: log value
	================================ */
	auto observation_noise = x_t_given_t_minus_1.row(F.cols() - 1);	// Call by reference ?
	//observation_noise = observation_noise.array().exp();	// Failed !
	//auto t_observation_noise = observation_noise.array().exp();
	auto t_observation_noise = observation_noise;
	//Eigen::VectorXd observation_noise = Eigen::VectorXd::Constant(n_particles, 2);
	//echo(observation_noise);
	auto log_w_t = filteringPDF(obs.back(), y_t_given_x_t, t_observation_noise, "cauthy");
	_echoes(st, "loglikelihood", log_w_t);

	weights = (weights.array().log() + log_w_t.array()).array().exp();
	//auto t_weights = weights.array().log() + log_w_t.array();
	//weights = t_weights.array().exp();
	weights = weights.array() / weights.sum();
	_echoes(st, "weights", weights);

	filtered.emplace_back(estimateOneData(x_t_given_t_minus_1));	// sum(x_{t|t} * weights)

	_echoes(st, "filtered", filtered.back());


	/* ================================
		Smoothing
			: smoothed[i] uses (i+lag) weights
	================================ */
	//if (t == 0) smoothed.emplace_back(obs[0]);	// Need ?
	if (t < lag) {
		auto it = x_t_hist.begin();
		x_t_hist.insert(it, x_t_given_t_minus_1);	// [7, 6, 5, 4] -> [8, 7, 6, 5, 4]
		//x_t_hist.emplace_back(x_t_given_t_minus_1);
	}
	else {
		// Insert to the head ([1] -> [2, 1] -> [3, 2, 1], ...)
		//echo(1);
		//for (auto xx : x_t_hist) echoes(xx, "\n");
		x_t_hist.pop_back();	// [7, 6, 5, 4, 3] -> [7, 6, 5, 4]
		//echo(2);
		//for (auto xx : x_t_hist) echoes(xx, "\n");
		auto it = x_t_hist.begin();
		x_t_hist.insert(it, x_t_given_t_minus_1);	// [7, 6, 5, 4] -> [8, 7, 6, 5, 4]
		//echo(3);
		//for (auto xx : x_t_hist) echoes(xx, "\n");
		smoothed.emplace_back(estimateOneData(x_t_hist.back()));

		if (t == full_length - 1) {
			for (int i = 0; i < lag; ++i) {
				int idx = (i + 1) % 5;	// 1 -> 2 -> 3 -> 4 -> 0
				smoothed.emplace_back(estimateOneData(x_t_hist[idx]));
			}
		}
	}
	if (is_test == true) {
		echo("hist");
		for (int i = 0; i < x_t_hist.size(); ++i) echoes(i, "\n", x_t_hist[i]);
		echo("hist_fin");

	}

	/* ================================
		Resampling
	================================ */
	auto ess = 1 / (weights.array() * weights.array()).sum();
	_echoes(st, "ess", ess, threshold);
	if (ess < threshold) {
		_echo(st, "resample");
		auto idx = resampling(weights);
		_echoes(st, "idx", idx);

		_echoes(st, "before pars\n", x_t_given_t_minus_1);
		_echoes(st, "before weights\n", weights);
		if (is_test == true) {
			for (int i = 0; i < x_t_hist.size(); ++i) echoes("before hist", i, "\n", x_t_hist[i]);
		}
		auto temp_x_t = extractPartMat(x_t_given_t_minus_1, idx);
		//echo(temp_x_t);
		filter_samples.emplace_back(temp_x_t);

		Eigen::MatrixXd t_weights_mat = Eigen::MatrixXd::Zero(1, weights.size());
		t_weights_mat.row(0) = weights;
		auto t_weights = extractPartMat(t_weights_mat, idx);

		for (int i = 0; i < MIN2(t + 1, lag); ++i) {
			x_t_hist[i] = extractPartMat(x_t_hist[i], idx);
		}

		weights = t_weights.row(0);
		weights = weights.array() / weights.sum();

		_echoes(st, "after pars\n", filter_samples.back());
		_echoes(st, "after weights\n", weights);
		if (is_test == true) {
			for (int i = 0; i < x_t_hist.size(); ++i) echoes("after_hist", i, "\n", x_t_hist[i]);
			echo("hist_fin");
		}

	}

	else filter_samples.emplace_back(x_t_given_t_minus_1);

}


void ParticleFilter::showSequence(const bool is_data, const bool is_filter, const bool is_smooth) {
	std::unordered_map<std::string, std::string> params;
	showSequence(is_data, is_filter, is_smooth, params);
}

void ParticleFilter::showSequence(const bool is_data, const bool is_filter, const bool is_smooth,
								  const std::unordered_map<std::string, std::string> params) {
	std::vector<double> obs_vec, filter_vec, smoothed_vec;
	std::vector<std::vector<double>> obs_mat, filter_mat, smoothed_mat;

	std::shared_ptr<PlotData> PD = std::make_shared<PlotData>();
	std::vector<std::shared_ptr<PlotData>> data_vec;

	//echoes(obs.size(), filtered.size(), smoothed.size());
	std::string style_data = "";
	std::string style_filter = "";
	std::string style_smooth = "";
	std::string legend_data = "";
	std::string legend_filter = "";
	std::string legend_smooth = "";
	std::string axis_x = "";
	std::string axis_y = "";
	if (containsKey(params, "style_data")) style_data = params.at("style_data");
	if (containsKey(params, "style_filter")) style_filter = params.at("style_filter");
	if (containsKey(params, "style_smooth")) style_smooth = params.at("style_smooth");
	if (containsKey(params, "legend_data")) legend_data = params.at("legend_data");
	if (containsKey(params, "legend_filter")) legend_filter = params.at("legend_filter");
	if (containsKey(params, "legend_smooth")) legend_smooth = params.at("legend_smooth");
	if (containsKey(params, "axis_x")) axis_x = params.at("axis_x");
	if (containsKey(params, "axis_y")) axis_y = params.at("axis_y");

	if (dim == 1) {
		std::string plot_type = "line_val_only";
		for (int i = 0; i < obs.size(); ++i) {
			obs_vec.emplace_back(obs[i](0));
			filter_vec.emplace_back(filtered[i](0));
			smoothed_vec.emplace_back(smoothed[i](0));
		}
		if (is_data == true) {
			PD->setData(obs_vec, plot_type, style_data, legend_data);
		}
		if (is_filter == true) {
			PD->setData(filter_vec, plot_type, style_filter, legend_filter);
		}
		if (is_smooth == true) {
			PD->setData(smoothed_vec, plot_type, style_smooth, legend_smooth);
		}
		//PD.showDataInformation();

		if (axis_x != "") PD->setXLabel(axis_x);
		if (axis_y != "") PD->setYLabel(axis_y);

		data_vec.emplace_back(PD);
	}
	else {
		std::string plot_type = "line_with_pos";
		for (int i = 0; i < obs.size(); ++i) {
			std::vector<double> temp = { obs[i](0), obs[i](1) };
			obs_mat.emplace_back(temp);
			temp = { filtered[i](0), filtered[i](1) };
			filter_mat.emplace_back(temp);
			temp = { smoothed[i](0), smoothed[i](1) };
			smoothed_mat.emplace_back(temp);
		}
		if (is_data == true) PD->setData(obs_mat, plot_type);
		if (is_filter == true) PD->setData(filter_mat, plot_type);
		if (is_smooth == true) PD->setData(smoothed_mat, plot_type);
		//PD.showDataInformation();
		data_vec.emplace_back(PD);
	}
	std::shared_ptr<MatplotlibCpp> PLT = std::make_shared<MatplotlibCpp>(data_vec);
	PLT->setSaveFolderPath(result_folder);
	PLT->setSaveFileName("test.png");
	PLT->setShowFlag(true);
	PLT->showInterface();
	PLT->execute();

}


//
//void ParticleFilter::onePeriodPredictiveDistribution(const int t) {
//	std::string st = "";
//	if (is_test == true) st = "test";
//	else st = "";
//
//	std::shared_ptr<GenRandom> GR = std::make_shared<GenRandom>();
//
//	/* ================================
//		Generate noise v_t
//	================================ */
//	Eigen::MatrixXd v_t = Eigen::MatrixXd::Zero(G.cols(), n_particles);
//
//	/* ================================
//		Generate noise v_t
//			self-organized model
//	================================ */
//	//// v_{tau^2_t} ~ N(0, hht)
//	//GR->genNorm1D(0, hyper_hyper_tau, n_particles);
//	//v_t.row(G.cols() - 2) = GR->getRandomVectorDouble();
//
//	//// v_{sigma^2_t} ~ N(0, hhs)
//	//GR->genNorm1D(0, hyper_hyper_sigma, n_particles);
//	//v_t.row(G.cols() - 1) = GR->getRandomVectorDouble();
//
//	//// log(tau^2): Fx + Gv = log(tau^2_{t-1}) + v_{tau^2_t}
//	//Eigen::VectorXd log_tau_2 = Eigen::VectorXd::Zero(n_particles);
//	//log_tau_2 = filter_samples.back().row(G.cols() - 2) + v_t.row(G.cols() - 2);
//
//	//echo(filter_samples.back().row(G.cols() - 2));
//	//echo(v_t);
//	//echo(log_tau_2);
//
//	// v_{x_t} ~ N(0, tau^2)
//	for (int d = 0; d < dim; ++d) {
//		for (int n = 0; n < n_particles; ++n) {
//			//double tau2 = exp(log_tau_2(n));		// exp(log(tau^2))
//			double tau2 = 0.025;
//			v_t(d, n) = genNorm(0, tau2);	// genNorm() requires parameter var
//		}
//	}
//
//
//	/* ================================
//		Process state space model
//	================================ */
//	// x_{t|t-1} = F * x_{t-1} + G * v_{t}
//	auto x_t_given_t_minus_1 = systemModel(filter_samples.back(), v_t);
//	//echo(x_t_given_t_minus_1);
//
//	// Calculate weights ( = likelihood)
//	auto y_t_given_x_t = observationModel(x_t_given_t_minus_1);
//	//echo(y_t_given_x_t);
//	auto w_t = filteringPDF(obs.back(), y_t_given_x_t, x_t_given_t_minus_1.row(F.cols() - 1), "cauthy");
//
//	_echo(st, "w_t");
//	_echo(st, w_t.transpose());
//
//	if (t > 0) {
//		auto exp_x_t = filter_samples[t].row(0) * w_t;
//		exp_x.emplace_back(exp_x_t);
//	}
//
//	/* ================================
//		Resampling
//	================================ */
//	//GR->genMultinomialChoice(w_t, n_particles);
//	//auto current_index = GR->getRandomVectorInt();
//	auto current_index = resampling(w_t);
//	//echo(current_index.transpose());
//	//for (auto p : parent_index)	echo(p.transpose());
//	parent_index.insert(parent_index.begin(), current_index);
//	parent_index.pop_back();
//
//	_echo(st, "parent_index");
//	for (auto p : parent_index)	_echo(st, p.transpose());
//
//
//	/* ================================
//		Filtering
//	================================ */
//	auto smoothing_index = parent_index[0];		// = current_index
//	//echo(x_t_given_t_minus_1);
//	//echo(smoothing_index);
//	auto temp_x_t = extractPartMat(x_t_given_t_minus_1, smoothing_index);
//	//echo(temp_x_t);
//	filter_samples.emplace_back(temp_x_t);
//
//
//	///* ================================
//	//	Smoothing
//	//================================ */
//	//int actual_lag = 0;
//	//if (t < lag) actual_lag = t;
//	//else actual_lag = lag;
//
//	//for (int tt = 0; tt < actual_lag; ++tt) {	// t = 0, not done
//	//	Eigen::MatrixXi temp = Eigen::MatrixXi::Zero(1, n_particles);
//	//	temp.row(0) = parent_index[tt + 1];
//	//	_echoes(st, t, tt);
//	//	//echoes(temp.rows(), temp.cols());
//	//	_echo(st, smoothing_index.transpose());
//	//	auto t_smoothing_index = extractPartMat(temp, smoothing_index);
//	//	smoothing_index = t_smoothing_index.row(0);
//	//	_echo(st, smoothing_index.transpose());
//	//	filter_samples[t - tt] = extractPartMat(filter_samples[t - tt], smoothing_index);
//	//}
//
//
//}