
/* ================================
Created by Hori on 2021/7/16

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Probability Functions

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Set

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef PROBABILITY_FUNCTIONS_H
#define PROBABILITY_FUNCTIONS_H

#include "pch.h"

double digam(const double x);

/* ================================
Random number
================================ */
double genStNorm();
double genNorm(const double mean, const double var);
double genNorm(const std::vector<double> mean_var);		// Overload
Eigen::VectorXd genMultiNorm(const Eigen::VectorXd mean, const Eigen::MatrixXd cov);
double genUniform(const double minval, const double maxval);
double genBeta(const double alpha, const double beta);	// Beta(x|a,b)
double genBeta(const std::vector<double> alpha_beta);	// Overload
double genGamma(const double shape, const double scale);	// Ga(t|a,b)=(1/((a)Eb^(a)))E^(a-1) exp(-/b)
double genGamma(const std::vector<double> shape_scale);		// Overload
double genInvGamma(const double shape, const double scale);	// IGa(t|a,b)=(1/((a)Eb^(a)))E^(-a-1) exp(-1/b)
Eigen::MatrixXd genWishart(const int dof, const Eigen::MatrixXd cov);
Eigen::VectorXi genMulti(const Eigen::VectorXd vec, const int N);
double genCauchy(const double mode, const double scale);
Eigen::VectorXd genMultiCauchy(const Eigen::VectorXd mode, const Eigen::MatrixXd scalemat);


/* ================================
PDF value
return log value
================================ */
double pdfLnExp(const double obs, const double lambda);
double pdfLnPoisson(const int obs, const double lambda);
double pdfLnNorm(const double obs, const double mean, const double var);
double pdfLnNorm(const std::vector<double> obs_mean_var);	// Overload
double pdfLnMultiNorm(const Eigen::VectorXd& obs, const Eigen::VectorXd& mean, Eigen::MatrixXd cov);
double pdfLnMultiNorm(const std::vector<std::vector<double>> obs_mean_var);	// Overload
double pdfLnMultiNorm(const std::vector<std::vector<std::vector<double>>> obs_mean_var);	// Overload
double pdfLnGMM(const Eigen::VectorXd& obs, const std::vector<double>& mix, const std::vector<Eigen::VectorXd>& mean,
				std::vector<Eigen::MatrixXd> cov);
double pdfLnGamma(const double obs, const double shape, const double scale);
double pdfLnGamma(const std::vector<double> obs_shape_scale);	// Overload
double pdfLnCauchy(const double obs, const double mode, const double scale);
double pdfLnMultiCauchy(const Eigen::VectorXd& obs, const Eigen::VectorXd& mode, const Eigen::MatrixXd& scalemat);


/* ==================================================
Diff PDF value ((log p(x|A)) -> (d/dx)(log p(x|A)) )
================================================== */
double pdfDiffLnNorm(const std::vector<double> obs_mean_var);
double pdfDiffLnGamma(const std::vector<double> obs_shape_scale);
std::vector<double> pdfDiffLnMultiNorm(const std::vector<std::vector<double>> obs_mean_var);



/* ================================
Expectation
================================ */
void getExpN1d(const double mean, const double var, double& E_x, double& E_x2);
void getExpNxd(const Eigen::VectorXd mean, const Eigen::MatrixXd cov,
			   Eigen::VectorXd& E_x, Eigen::MatrixXd& E_xxt, double& E_xtx);
void getExpGa(const double a, const double b, double& E_t, double& E_lt);
void getExpIGa(const double a, const double b, double& E_t, double& E_invt, double& E_lt);
void getExpWis(const int nu, const Eigen::MatrixXd& S,
			   Eigen::MatrixXd& E_lam, double& E_idetlam);
void getExpBeta(const double alpha, const double beta,
				double& E_x, double& E_lx, double& E_mx, double& E_lmx);
void getExpDir(const Eigen::VectorXd& alpha, Eigen::VectorXd& E_x, Eigen::VectorXd& E_lx);
void getExpGIG(const double a, const double b, const double p,
			   double& E_x, double& E_invx, double& E_lx);

/* ================================
Others
================================ */
double modBessel(const std::string kv, double v, double z);
double partialGIG(double a, double b, double p);




#endif //PROBABILITY_FUNCTIONS_H