

#include "GenRandomNumber.h"
#include "pch.h"
#include "../Shared/MathProc.h"
#include "ProbabilityFunctions.h"

GenRandom::GenRandom() {
	// Initialize
	e_vec_i = Eigen::VectorXi::Zero(1);
	e_vec_d = Eigen::VectorXd::Zero(1);
	e_mat_d = Eigen::MatrixXd::Zero(1, 1);

	mean_scalar = 0;
	expect_scalar = 0;
	mean_vec = Eigen::VectorXd::Zero(1);
	expect_vec = Eigen::VectorXd::Zero(1);
	mean_mat = Eigen::MatrixXd::Zero(1, 1);
	expect_mat = Eigen::MatrixXd::Zero(1, 1);

	init_val = 0.1;

	//MH = std::make_shared<MetropolisHastingsAlgorithm>();
	//HMC = std::make_shared<HamiltonianMonteCarlo>();
	//NUTS = std::make_shared<NoUTurnSampler>();
}


void GenRandom::genNorm1D(double mu, double sigma2, int n_num) {
	e_vec_d.resize(n_num);
	mean_scalar = 0;
	for (int i = 0; i < n_num; ++i) {
		e_vec_d(i) = genNorm(mu, sigma2);
		mean_scalar += e_vec_d(i);
	}
	mean_scalar /= n_num;
	expect_scalar = mu;
}


void GenRandom::genNorm2D(Eigen::VectorXd mu, Eigen::MatrixXd cov, int n_num) {
	//e_mat_d.resize(mu.size(), n_num);
	e_mat_d.resize(n_num, mu.size());
	mean_vec = Eigen::VectorXd::Zero(mu.size());
	for (int i = 0; i < n_num; ++i) {
		e_mat_d.row(i) = genMultiNorm(mu, cov);
		mean_vec += e_mat_d.row(i);
	}
	mean_vec = mean_vec.array() / n_num;
	expect_vec = mu;
}


void GenRandom::genUni(double infimum, double supremum, int n_num) {
	e_vec_d.resize(n_num);
	mean_scalar = 0;
	for (int i = 0; i < n_num; ++i) {
		e_vec_d(i) = genUniform(infimum, supremum);
		mean_scalar += e_vec_d(i);
	}
	mean_scalar /= n_num;
	expect_scalar = (infimum + supremum) / 2.0;
}


void GenRandom::genInt(const int infimum, const int supremum, const int n_num) {
	e_vec_i.resize(n_num);
	mean_scalar = 0;
	for (int i = 0; i < n_num; ++i) {
		e_vec_i(i) = irand(infimum, supremum);
		mean_scalar += (double)e_vec_i(i);
	}
	mean_scalar /= n_num;
	expect_scalar = (double)(infimum + supremum) / 2.0;
}


void GenRandom::genCauchy1D(double mode, double var, int n_num) {
	e_vec_d.resize(n_num);
	mean_scalar = 0;
	for (int i = 0; i < n_num; ++i) {
		e_vec_d(i) = genCauchy(mode, var);
		mean_scalar += e_vec_d(i);
	}
	mean_scalar /= n_num;
	expect_scalar = mode;	// The expectation value is not defined in Cauchy distribution
}


void GenRandom::genCauchy2D(Eigen::VectorXd mode, Eigen::MatrixXd cov, int n_num) {
	e_mat_d.resize(n_num, mode.size());
	mean_vec = Eigen::VectorXd::Zero(mode.size());
	for (int i = 0; i < n_num; ++i) {
		e_mat_d.row(i) = genMultiCauchy(mode, cov);
		mean_vec += e_mat_d.row(i);
	}
	mean_vec = mean_vec.array() / n_num;
	expect_vec = mode;
}


void GenRandom::genGa(double a, double b, int n_num) {
	e_vec_d.resize(n_num);
	mean_scalar = 0;
	for (int i = 0; i < n_num; ++i) {
		e_vec_d(i) = genGamma(a, b);
		mean_scalar += e_vec_d(i);
	}
	mean_scalar /= n_num;
	expect_scalar = a * b;
}


void GenRandom::genMultinomial(Eigen::VectorXd prob, int n_num) {
	// ex) prob = [0.1, 0.1, 0.1, 0.6, 0.1]
	//	   return [0, 0, 1, 7, 2] <- like histgram, then 0+0+1+7+2 = n_num
	e_vec_i = genMulti(prob, n_num);
}


void GenRandom::genMultinomialChoice(Eigen::VectorXd prob, int n_num) {
	// ex) prob = [0.1, 0.1, 0.1, 0.6, 0.1]
	//		-> temp = [0, 0, 1, 7, 2] <- like histgram, then 0+0+1+7+2 = 10
	//		return [3]	<- Index of max value (list)
	e_vec_i.resize(n_num);
	int n_sample = 1;
	Eigen::VectorXf::Index idx;
	for (int i = 0; i < n_num; ++i) {
		auto histgram = genMulti(prob, n_sample);
		histgram.maxCoeff(&idx);
		e_vec_i(i) = idx;
	}
}

/* ===============================
 * Metropolis Hastings Algorithm
 =============================== */
//
//void GenRandom::exeIndependentMH(const std::function<double(const std::vector<double>)>& callback_post_pdf,
//								 const std::function<double(const std::vector<double>)>& callback_prop_rvf,
//								 const std::function<double(const std::vector<double>)>& callback_prop_pdf,
//								 const std::vector<double>& params_post, const std::vector<double>& params_prop,
//								 const int n_num, const bool candidate_negative) {
//
//	e_vec_d = Eigen::VectorXd(n_num);
//	Eigen::VectorXd t_e_vec_d = Eigen::VectorXd(n_num * MH->getMHSkip());
//	int n_accept = 0;
//	int n_act_accept = 0;
//	double current_val = init_val;
//	std::vector<double> params_q(params_prop.size() + 1, 0);
//	std::vector<double> params_f(params_post.size() + 1, 0);
//	for (size_t p = 0; p < params_prop.size(); ++p) params_q[p + 1] = params_prop[p];
//	for (size_t p = 0; p < params_post.size(); ++p) params_f[p + 1] = params_post[p];
//	n_all_sampling = 0;
//	n_all_accept = 0;
//
//	while (n_accept < n_num * MH->getMHSkip()) {
//
//		/* =========
//		 * 1 step
//		========= */
//
//		// a gen q(a|const_prop_params)
//		double candidate_a = callback_prop_rvf(params_prop);
//		if (candidate_a <= 0 && candidate_negative == false) continue;
//
//		// log q(theta) = log q(theta|const_prop_params)
//		params_q[0] = current_val;
//		double log_q_theta = callback_prop_pdf(params_q);
//
//		// log f(a|x) = log f(a|const_post_params)
//		params_f[0] = candidate_a;
//		double log_f_a = callback_post_pdf(params_f);
//
//		// log q(a) = log q(a|const_prop_params)
//		params_q[0] = candidate_a;
//		double log_q_a = callback_prop_pdf(params_q);
//
//		// log f(a|x) = log f(a|const_post_params)
//		params_f[0] = current_val;
//		double log_f_theta = callback_post_pdf(params_f);
//
//		double r = log_q_theta + log_f_a - log_q_a - log_f_theta;
//
//		// Judge
//		int burn_count = MH->getNBurnIn();
//		//bool accept = this->MetropoliceHasting(r, current_val, candidate_a, t_e_vec_d,
//		//	n_accept, e_vec_d, n_act_accept, burn_count);
//		bool accept = MH->oneStepMH(r, current_val, candidate_a, t_e_vec_d,
//									n_accept, e_vec_d, n_act_accept, burn_count, n_all_sampling, n_all_accept);
//	}
//	mean_scalar = t_e_vec_d.mean();
//	accept_rate = (double)n_all_accept / n_all_sampling;
//	//echo(t_e_vec_d);
//	//echo("===");
//	//echo(e_vec_d);
//	//echo("===");
//}
//
//// 1D sampling
//void GenRandom::exeRandomWalkMH(const std::function<double(const std::vector<double>)>& callback_post_pdf,
//								const std::vector<double>& params_post, const int n_num, const bool candidate_negative) {
//
//	e_vec_d = Eigen::VectorXd(n_num);
//	Eigen::VectorXd t_e_vec_d = Eigen::VectorXd(n_num * MH->getMHSkip());
//	int n_accept = 0;
//	int n_act_accept = 0;
//	double current_val = init_val;
//	std::vector<double> params_f(params_post.size() + 1, 0);
//	for (size_t p = 0; p < params_post.size(); ++p) params_f[p + 1] = params_post[p];
//
//	int burn_count = 0;
//
//	n_all_sampling = 0;
//	n_all_accept = 0;
//
//	while (n_accept < n_num * MH->getMHSkip()) {
//
//		/* =========
//		 * 1 step
//		========= */
//
//		// a gen q(a|const_prop_params)
//		double candidate_a = current_val + genStNorm();
//		if (candidate_a <= 0 && candidate_negative == false) continue;
//
//		// log f(a|x) = log f(a|const_post_params)
//		params_f[0] = candidate_a;
//		double log_f_a = callback_post_pdf(params_f);
//
//		// log f(a|x) = log f(a|const_post_params)
//		params_f[0] = current_val;
//		double log_f_theta = callback_post_pdf(params_f);
//
//		double r = log_f_a - log_f_theta;
//
//		// Judge
//		bool accept = MH->oneStepMH(r, current_val, candidate_a, t_e_vec_d,
//									n_accept, e_vec_d, n_act_accept, burn_count, n_all_sampling, n_all_accept);
//
//	}
//	mean_scalar = t_e_vec_d.mean();
//	accept_rate = (double)n_all_accept / n_all_sampling;
//
//	//echo(e_vec_d);
//	//echo("====");
//	//echo(t_e_vec_d.mean());
//}
//
//// Multi-dimensional sampling (overload)
//void GenRandom::exeRandomWalkMH(const std::function<double(const std::vector<std::vector<double>>)>& callback_post_pdf,
//								const std::vector<std::vector<double>>& params_post, const int n_num, const bool candidate_negative) {
//
//	int n_dim = params_post[0].size();
//	e_mat_d = Eigen::MatrixXd(n_num, n_dim);
//	Eigen::MatrixXd t_e_mat_d = Eigen::MatrixXd(n_num * MH->getMHSkip(), n_dim);
//	int n_accept = 0;
//	int n_act_accept = 0;
//	Eigen::VectorXd current_val = Eigen::VectorXd::Zero(n_dim).array() + init_val;
//	std::vector<std::vector<double>> params_f(params_post.size() + 1, std::vector<double>(params_post[0].size(), 0));
//	for (size_t p = 0; p < params_post.size(); ++p) params_f[p + 1] = params_post[p];
//	Eigen::VectorXd mean_for_random_walk = Eigen::VectorXd::Zero(n_dim);
//	Eigen::MatrixXd cov_for_random_walk = Eigen::MatrixXd::Identity(n_dim, n_dim);
//
//	int burn_count = 0;
//	n_all_sampling = 0;
//	n_all_accept = 0;
//
//	while (n_accept < n_num * MH->getMHSkip()) {
//
//		/* =========
//		 * 1 step
//		========= */
//
//		// a gen q(a|const_prop_params)
//		Eigen::VectorXd candidate_a = current_val.array() + genMultiNorm(mean_for_random_walk, cov_for_random_walk).array();
//		if (candidate_negative == false) {
//			bool cont_flag = false;
//			for (int i = 0; i < n_dim; ++i) {
//				if (candidate_a(i) < 0) cont_flag = true;
//			}
//			if (cont_flag == true) continue;
//		}
//		// log f(a|x) = log f(a|const_post_params)
//		params_f[0] = convertEVec2SVec(candidate_a);
//		double log_f_a = callback_post_pdf(params_f);
//
//		// log f(a|x) = log f(a|const_post_params)
//		params_f[0] = convertEVec2SVec(current_val);
//		double log_f_theta = callback_post_pdf(params_f);
//
//		double r = log_f_a - log_f_theta;
//
//		// Judge
//		bool accept = MH->oneStepMH(r, current_val, candidate_a, t_e_mat_d,
//									n_accept, e_mat_d, n_act_accept, burn_count, n_all_sampling, n_all_accept);
//
//	}
//	mean_vec = t_e_mat_d.colwise().mean();
//	accept_rate = (double)n_all_accept / n_all_sampling;
//
//	//echo(e_mat_d);
//	//echo("====");
//	//echo(t_e_mat_d.colwise().mean());
//	//echo("====");
//
//}
//
//
//// 1D sampling
//void GenRandom::exeHMC(const std::function<double(const std::vector<double>)>& callback_post_pdf,
//					   const std::function<double(const std::vector<double>)>& callback_diff_post_pdf,
//					   const std::vector<double>& params_post, const int n_num, const bool candidate_negative) {
//
//	e_vec_d = HMC->exeHMC(callback_post_pdf, callback_diff_post_pdf, params_post, n_num, candidate_negative);
//	mean_scalar = e_vec_d.mean();
//	accept_rate = HMC->getAcceptRate();
//
//}
//
//// Multi-dimensional sampling
//void GenRandom::exeHMC(const std::function<double(const std::vector<std::vector<double>>)>& callback_post_pdf,
//					   const std::function<std::vector<double>(const std::vector<std::vector<double>>)>& callback_diff_post_pdf,
//					   const std::vector<std::vector<double>>& params_post, const int n_num, const bool candidate_negative) {
//
//	e_mat_d = HMC->exeHMC(callback_post_pdf, callback_diff_post_pdf, params_post, n_num, candidate_negative);
//	mean_vec = e_mat_d.colwise().mean();
//	accept_rate = HMC->getAcceptRate();
//
//}
