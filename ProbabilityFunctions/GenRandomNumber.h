
/* ================================
Created by Hori on 2021/7/16

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Generate Random value class

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	A. Metropolis hastings algorithm
		MetropolisHastingsAlgorithm.cpp includes one-step function (after acceptable rate r calculation)

	B. Metropolis hastings algorithm
		1. Basic MH algorithm
			1.1. Independent MH
			1.2. Random walk MH
		2. Hamiltonian Monte Carlo
			HamiltonianMonteCarlo.cpp
		3. No-U-Turn Sampler
			NoUTurnSampler.cpp




================================ */

/* ================================

================================ */


#ifndef GEN_RANDOM_NUMBER_H
#define GEN_RANDOM_NUMBER_H

#include "pch.h"

class GenRandom {
public:
	GenRandom();

	void genNorm1D(double mu, double sigma2, int n_num);
	void genNorm2D(Eigen::VectorXd mu, Eigen::MatrixXd cov, int n_num);
	void genUni(double infimum, double supremum, int n_num);
	void genInt(const int infimum, const int supremum, const int n_num);
	void genCauchy1D(double mode, double var, int n_num);
	void genCauchy2D(Eigen::VectorXd mu, Eigen::MatrixXd cov, int n_num);	// Independent
	void genGa(double a, double b, int n_num);
	void genMultinomial(const Eigen::VectorXd prob, const int n_num);	// Like histgram
	void genMultinomialChoice(const Eigen::VectorXd prob, const int n_num);		// Get index

	/* =============================================================
	 Note: If you want to use a callback function, need to use "static_cast<double(*)(std::vector<double>)>(function_name)"
		ex) GR.exeRandomWalkMH(static_cast<double(*)(std::vector<double>)>(pdfLnGamma), gamma_params, 50, false);
			GR.exeRandomWalkMH(static_cast<double(*)(std::vector<std::vector<double>>)>(pdfLnMultiNorm), multi_norm_params, 50);
	 ============================================================= */


	// // MH methods
	//void exeIndependentMH(const std::function<double(const std::vector<double>)>& callback_post_pdf,
	//					  const std::function<double(const std::vector<double>)>& callback_prop_rvf,
	//					  const std::function<double(const std::vector<double>)>& callback_prop_pdf,
	//					  const std::vector<double>& params_post, const std::vector<double>& params_prop,
	//					  const int n_num, const bool candidate_negative = true);
	//void exeRandomWalkMH(const std::function<double(const std::vector<double>)>& callback_post_pdf,
	//					 const std::vector<double>& params_post, const int n_num, const bool candidate_negative = true);
	//void exeRandomWalkMH(const std::function<double(const std::vector<std::vector<double>>)>& callback_post_pdf,
	//					 const std::vector<std::vector<double>>& params_post, const int n_num, const bool candidate_negative = true);
	//void exeHMC(const std::function<double(const std::vector<double>)>& callback_post_pdf,
	//			const std::function<double(const std::vector<double>)>& callback_diff_post_pdf,
	//			const std::vector<double>& params_post, const int n_num, const bool candidate_negative = true);
	//void exeHMC(const std::function<double(const std::vector<std::vector<double>>)>& callback_post_pdf,
	//			const std::function<std::vector<double>(const std::vector<std::vector<double>>)>& callback_diff_post_pdf,
	//			const std::vector<std::vector<double>>& params_post, const int n_num, const bool candidate_negative = true);

	//void exeNUTS(const std::function<double(const std::vector<double>)>& callback_post_pdf,
	//			 const std::function<double(const std::vector<double>)>& callback_diff_post_pdf,
	//			 const std::vector<double>& params_post, const int n_num, const bool candidate_negative = true);


	/* =======================================
	VectorXi:
		Int
	VectorXd:
		Normal-1D(n_num), Uni, Cauchy-1D, Ga
	MatrixXd:
		Normal-multi(n_num, dim), Cauchy-2D
	======================================= */
	Eigen::VectorXi getRandomVectorInt() const { return e_vec_i; }
	Eigen::VectorXd getRandomVectorDouble() const { return e_vec_d; }
	Eigen::MatrixXd getRandomMatrixDouble() const { return e_mat_d; }

	/* =======================================
	Scalar:
		1D-dimensional-output pdf
	Vec:
		Multi-dimensional-output pdf
	Mat: (now not implemented)
		For Wishart
	======================================= */
	double getMeanScalar() const { return mean_scalar; }	// Average of random values
	double getExpectScalar() const { return expect_scalar; }	// Expectation based on a probabilistic density function
	Eigen::VectorXd getMeanVec() const { return mean_vec; }	// Average of random matrices
	Eigen::VectorXd getExpectVec() const { return expect_vec; }		// Expectation based on a probabilistic density function
	Eigen::MatrixXd getMeanMat() const { return mean_mat; }	// Average of random matrices
	Eigen::MatrixXd getExpectMat() const { return expect_mat; }		// Expectation based on a probabilistic density function

	double getAcceptRate() const { return accept_rate; }


private:

	int n_all_sampling, n_all_accept;
	double init_val, accept_rate;

	//std::shared_ptr<MetropolisHastingsAlgorithm> MH;
	//std::shared_ptr<HamiltonianMonteCarlo> HMC;
	//std::shared_ptr<NoUTurnSampler> NUTS;

	Eigen::VectorXi e_vec_i;
	Eigen::VectorXd e_vec_d;
	Eigen::MatrixXd e_mat_d;

	double mean_scalar;	// Average of random values
	double expect_scalar;	// Expectation based on a probabilistic density function
	Eigen::VectorXd mean_vec;	// Average of random matrices
	Eigen::VectorXd expect_vec;		// Expectation based on a probabilistic density function
	Eigen::MatrixXd mean_mat;	// Average of random matrices
	Eigen::MatrixXd expect_mat;		// Expectation based on a probabilistic density function


	std::string temp;


};





#endif //GEN_RANDOM_NUMBER_H