

#include "ProbabilityFunctions.h"
#include "pch.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"
#include "../Shared/vec_op.h"

double digam(const double x) {
    return boost::math::digamma(x);
}

/* ================================
Random number
================================ */
double genStNorm() {
	// If want to set seed, rng_stnorm(seed)

	static boost::lagged_fibonacci1279 rng_stnorm;
	boost::normal_distribution<> snd(0.0, 1.0);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::normal_distribution<> > var_snor(rng_stnorm, snd);

	return var_snor();
}

double genNorm(const double mean, const double var) {
	// boost required not var but standard deviation (thus, required sqrt(var))
	//	: Above not accurate ?
	static boost::lagged_fibonacci1279 rng_norm;
	//boost::normal_distribution<> nd(mean, sqrt(var));
	boost::normal_distribution<> nd(mean, var);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::normal_distribution<> > var_nor(rng_norm, nd);

	return var_nor();
}

// For callback
double genNorm(const std::vector<double> mean_var) {
	double mean = mean_var[0];
	double var = mean_var[1];

	return genNorm(mean, var);
}


Eigen::VectorXd genMultiNorm(const Eigen::VectorXd mean, const Eigen::MatrixXd cov) {
	Eigen::VectorXd stnormvec = Eigen::VectorXd::Zero(mean.size());
	Eigen::LLT<Eigen::MatrixXd> chol(cov);
	Eigen::MatrixXd L;
	L = chol.matrixL();
	for (int i = 0; i < mean.size(); ++i) {
		stnormvec(i) = genStNorm();
	}

	return (L * stnormvec).col(0) + mean;
}


double genUniform(const double minval, const double maxval) {
	static boost::lagged_fibonacci1279 rng_uni;
	boost::uniform_real<> ud(minval, maxval);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::uniform_real<> > var_udr(rng_uni, ud);

	return var_udr();
}


double genBeta(const double alpha, const double beta) {
	static boost::lagged_fibonacci1279 rng_beta;
	boost::random::beta_distribution<> bd(alpha, beta);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::random::beta_distribution<> > var_betar(rng_beta, bd);
	return var_betar();
}

// For callback
double genBeta(const std::vector<double> alpha_beta) {
	double alpha = alpha_beta[0];
	double beta = alpha_beta[1];

	return genBeta(alpha, beta);
}


double genGamma(const double shape, const double scale) {
	static boost::lagged_fibonacci1279 rng_ga;
	boost::gamma_distribution<> gd(shape, scale);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::gamma_distribution<> > var_gar(rng_ga, gd);

	return var_gar();
}

// For callback
double genGamma(const std::vector<double> shape_scale) {
	double shape = shape_scale[0];
	double scale = shape_scale[1];

	return genGamma(shape, scale);
}


double genInvGamma(const double shape, const double scale) {
	static boost::lagged_fibonacci1279 rng_ga;
	boost::gamma_distribution<> gd(shape, scale);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::gamma_distribution<> > var_gar(rng_ga, gd);

	return 1.0 / var_gar();
}

Eigen::MatrixXd genWishart(const int dof, const Eigen::MatrixXd cov) {
	Eigen::MatrixXd temp = Eigen::MatrixXd::Zero(cov.rows(), dof);
	Eigen::VectorXd zerovec = Eigen::VectorXd::Zero(cov.rows());
	Eigen::MatrixXd output = Eigen::MatrixXd::Zero(cov.rows(), cov.cols());
	for (int i = 0; i < dof; ++i) {
		temp.col(i) = genMultiNorm(zerovec, cov);
		//output += (temp.col(i) * temp.transpose().row(i));
	}
	//return output;
	return temp * temp.transpose();
}


Eigen::VectorXi genMulti(const Eigen::VectorXd vec, const int N) {
	Eigen::VectorXi output = Eigen::VectorXi::Zero(vec.size());

	// Ensure memory
	unsigned int rng_mul = xor128();
	gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
	gsl_rng_set(r, rng_mul);
	std::vector<double> params(vec.size());
	//Eigen::Map<Eigen::VectorXd>(&params[0], vec.size()) = vec;
	std::vector<unsigned int> multi(vec.size());
	gsl_ran_multinomial(r, vec.size(), N, &vec[0], &multi[0]);
	for (int i = 0; i < vec.size(); ++i) {
		output(i) = multi[i];
	}

	// Release memory
	gsl_rng_free(r);

	return output;

}

double genCauchy(const double mode, const double scale) {
	static boost::lagged_fibonacci1279 rng_cauchy;
	boost::cauchy_distribution<> nd(mode, scale);
	boost::variate_generator<boost::lagged_fibonacci1279&,
		boost::cauchy_distribution<> > var_car(rng_cauchy, nd);

	return var_car();
}

Eigen::VectorXd genMultiCauchy(const Eigen::VectorXd mode, const Eigen::MatrixXd scalemat) {
	Eigen::VectorXd temp = Eigen::VectorXd::Zero(mode.size());

	for (int i = 0; i < mode.size(); ++i) {
		temp(i) = genCauchy(mode(i), scalemat(i, i));
	}

	return temp;
}




/* ================================
PDF value
================================ */
double pdfLnExp(const double obs, const double lambda) {
	// l exp(-lx) -> log(l) - lx
	return log(lambda) - (lambda * obs);
}

double pdfLnPoisson(const int obs, const double lambda) {
	// (l^k)exp(-l)/k! -> k log(l) - l - log(k!), log(k!)=log(k)+log(k-1)+...+log(1)
	double log_x_f = 0;
	for (int i = 1; i < obs + 1; ++i) log_x_f += log(i);
	double log_poisson = obs * log(lambda) - lambda - log_x_f;
	return log_poisson;
}

double pdfLnNorm(const double obs, const double mean, const double var) {
	// (1/sqrt(2*pi*s^2)) exp(-(x-m)^2 / 2s^2) -> -0.5log(2*pi*s^2) - ((x-m)^2 / 2s^2)
	double temp = -(0.5 * log(2.0 * M_PI * var));
	return temp - (0.5 * pow((obs - mean), 2) / var);
}

// Overload
double pdfLnNorm(const std::vector<double> obs_mean_var) {
	return pdfLnNorm(obs_mean_var[0], obs_mean_var[1], obs_mean_var[2]);
}


double pdfLnMultiNorm(const Eigen::VectorXd& obs, const Eigen::VectorXd& mean, Eigen::MatrixXd cov) {
	// (1 / ((sqrt(2pi))^d * sqrt(|S|))) exp(-0.5(x-m)^t S^(-1) (x-m)
	//	-> (-0.5 * d log(2pi) - 0.5|S|) - 0.5(x-m)^t S^(-1) (x-m)
	//	-> -0.5(d log(2pi) * |S|) - 0.5(x-m)^t S^(-1) (x-m)
	int dim = obs.size();
	double det = cov.determinant();

	// If rank reduction
	if (det < 1e-200) {
		for (int i = 0; i < obs.size(); ++i) {
			cov(i, i) += genUniform(0.001, 0.01);
		}
		det = cov.determinant();
	}

	double temp2 = -0.5 * ((dim * log(2.0 * M_PI)) + log(det));
	//Eigen::MatrixXd invcov;
	//invMat(cov, invcov);
	auto invcov = invMat(cov);

	Eigen::MatrixXd diff = Eigen::MatrixXd::Zero(1, obs.size());
	diff.row(0) = obs - mean;

	return temp2 - (0.5 * (diff * invcov * diff.transpose())(0, 0));
}

// Overload 1
double pdfLnMultiNorm(const std::vector<std::vector<double>> obs_mean_var) {
	std::vector<double> tvec = obs_mean_var[0];
	//Eigen::MatrixXd tobs = convertSVec2EMat1D(tvec);
	//echo(typeid(tobs).name());
	//auto obs = convertEMat1D2Evec(tobs);
	auto obs = convertSVec2EVec(tvec);

	tvec = obs_mean_var[1];
	//auto mean = convertSVec2EMat1D(tvec);
	auto mean = convertSVec2EVec(tvec);
	int n_dim = obs_mean_var[0].size();
	std::vector<std::vector<double>> tcov(n_dim, std::vector<double>(n_dim, 0));
	for (int i = 0; i < n_dim; ++i) {
		tcov[i] = obs_mean_var[2 + i];
	}
	auto cov = convertSVec2EMat(tcov);
	return pdfLnMultiNorm(obs, mean, cov);
}

// Overload 2
double pdfLnMultiNorm(const std::vector<std::vector<std::vector<double>>> obs_mean_var) {
	/* ===========================================================
		obs_mean_var = { {data_{1:N}}, {{mean}}, {cov_{d1:D} }
					 = {  { {data_1}, {data_2}, ..., {data_N} }, { {mean} }, { {cov_d1}, {cov_d2}, ..., {cov_dD} }  }  }
	 =========================================================== */

	 // Convert all parameters formatted std::vector_2d to Eigen::MatrixXd
	auto data = convertSVec2EMat(obs_mean_var[0]);	// (idx_data, dim)
	auto mean = convertSVec2EMat(obs_mean_var[1]);	// (1, dim)
	auto cov = convertSVec2EMat(obs_mean_var[2]);	// (dim, dim)
	int n_dim = cov.rows();
	int n_data = data.rows();
	double cov_det = cov.determinant();

	// Normalizing constant
	// -(D/2)log(2pi) - (1/2)log|S| = -(1/2) (D log(2 pi) + log|S|)
	double normalizing_constant = -0.5 * ((n_dim * log(2.0 * M_PI)) + log(cov_det));

	// Kernel
	//Eigen::MatrixXd invcov;
	//invMat(cov, invcov);	// Including rank judge
	auto invcov = invMat(cov);
	auto data_m_mean = data.rowwise() - mean.row(0);	// data_m_mean: (idx_data, dim)	
	auto diff_sigma_diff = data_m_mean * invcov * data_m_mean.transpose();
	//double kernel = -0.5 * diff_sigma_diff.diagonal().sum();
	double kernel = -0.5 * diff_sigma_diff.trace();

	return n_data * normalizing_constant + kernel;
}




double pdfLnGMM(const Eigen::VectorXd& obs, const std::vector<double>& mix, const std::vector<Eigen::VectorXd>& mean,
				std::vector<Eigen::MatrixXd> cov) {
	Eigen::VectorXd temp = Eigen::VectorXd::Zero(mix.size());
	for (int m = 0; m < mix.size(); ++m) {
		temp(m) = log(mix[m]) + pdfLnMultiNorm(obs, mean[m], cov[m]);
	}
	double log_prob = logsumexpEigenSum(temp);
	return log_prob;
}

double pdfLnGamma(const double obs, const double shape, const double scale) {
	return (shape - 1) * log(obs) - (obs / scale) - log(boost::math::tgamma(shape)) - (shape * log(scale));
}

// For callback
double pdfLnGamma(const std::vector<double> obs_shape_scale) {
	return pdfLnGamma(obs_shape_scale[0], obs_shape_scale[1], obs_shape_scale[2]);
}



double pdfLnCauchy(const double obs, const double mode, const double scale) {
	return log(scale) - log(M_PI) - log(pow(obs - mode, 2) + pow(scale, 2));
}


double pdfLnMultiCauchy(const Eigen::VectorXd& obs, const Eigen::VectorXd& mode, const Eigen::MatrixXd& scalemat) {
	double dens = 0;
	for (int i = 0; i < obs.size(); ++i) {
		dens += pdfLnCauchy(obs(i), mode(i), scalemat(i, i));
	}
	return dens;
}


/* ==================================================
Diff PDF value ((log p(x|A)) -> (d/dx)(log p(x|A)) )
================================================== */

double pdfDiffLnNorm(const std::vector<double> obs_mean_var) {
	// -(x-m)^2 / 2s^2   ->   -(x-m)/s^2
	double denom = obs_mean_var[2];
	if (denom < 0.000001) denom = 0.00001;
	return -(obs_mean_var[0] - obs_mean_var[1]) / denom;
}

double pdfDiffLnGamma(const std::vector<double> obs_shape_scale) {
	// (a-1)log(x)-(x/b)   ->   (a-1)/x - (1/b)
	double obs = obs_shape_scale[0];
	if (obs < 0.000001) obs = 0.00001;
	double invobs = 1. / obs;
	double scale = obs_shape_scale[2];
	if (scale < 0.000001) scale = 0.00001;
	double invscale = 1. / scale;
	return (obs_shape_scale[1] - 1) * invobs - invscale;
}

std::vector<double> pdfDiffLnMultiNorm(const std::vector<std::vector<double>> obs_mean_var) {
	// -(X-M)^t S^(-1) (X-M) / 2   ->   -S^(-1)(X-M)
	auto tvec = obs_mean_var[0] - obs_mean_var[1];
	auto obs_minus_m = convertSVec2EVec(tvec);	// (X-M)

	int n_dim = obs_mean_var[0].size();
	std::vector<std::vector<double>> tcov(n_dim, std::vector<double>(n_dim, 0));
	for (int i = 0; i < n_dim; ++i) {
		tcov[i] = obs_mean_var[2 + i];
	}
	auto cov = convertSVec2EMat(tcov);

	// invMat() includes rank judge
	//Eigen::MatrixXd invcov;
	//invMat(cov, invcov);	// S^(-1)
	auto invcov = invMat(cov);	// S^(-1)

	auto diff_vec = invcov * obs_minus_m;	// S^(-1) (X-M)
	std::vector<double> output(n_dim, 0);
	for (int i = 0; i < n_dim; ++i) output[i] = -diff_vec(i);

	return output;
}


/* ================================
Expectation
================================ */
void getExpN1d(const double mean, const double var, double& E_x, double& E_x2) {
	E_x = mean;
	E_x2 = pow(mean, 2) + var;
}

void getExpNxd(const Eigen::VectorXd mean, const Eigen::MatrixXd cov,
			   Eigen::VectorXd& E_x, Eigen::MatrixXd& E_xxt, double& E_xtx) {
	E_x = mean;
	E_xxt = mean * mean.transpose() + cov;
	E_xtx = mean.dot(mean) + cov.trace();

}

void getExpGa(const double a, const double b, double& E_t, double& E_lt) {
	// Ga(τ|a,b) = (1 / Γ(a)・b^(a))・τ^(a-1) exp(-τ/b)
	E_t = a * b;
	E_lt = digam(a) + log(b);
}

void getExpIGa(const double a, const double b, double& E_t, double& E_invt, double& E_lt) {
	// Ga(τ|a,b)
	// = (1 / Γ(a)・b^(a))・τ^(-a-1) exp(-1/τb)
	if (a < 1.0) E_t = 1.0 / (a * b);
	else E_t = 1.0 / ((a - 1) * b);
	E_invt = a * b;
	E_lt = -log(b) - digam(a);
}

void getExpWis(const int nu, const Eigen::MatrixXd& S,
			   Eigen::MatrixXd& E_lam, double& E_idetlam) {
	// W(Λ|ν,S)
	// 1 dimension: W(x|ν,s) = Ga(x|ν/2,2s) (= χ(ν);s=1)
	int D = S.rows();
	E_lam = nu * S.array();

	double temp = 0;
	for (int i = 1; i < D + 1; ++i) temp += digam((nu + 1.0 - i) / 2.0);
	E_idetlam = temp + (D * log(2)) + log(S.determinant());
}

void getExpBeta(const double alpha, const double beta, double& E_x, double& E_lx, double& E_mx, double& E_lmx) {
	// Beta(x|α,β)
	double ab = alpha + beta;
	E_x = alpha / ab;
	E_lx = digam(alpha) - digam(ab);
	E_mx = beta / (ab);
	E_lmx = digam(beta) - digam(ab);
}

void getExpDir(const Eigen::VectorXd& alpha, Eigen::VectorXd& E_x, Eigen::VectorXd& E_lx) {
	// Dir(x1,...,xk|α1,...,αk)
	double alpha_sum = alpha.sum();
	E_x = alpha.array() / alpha_sum;
	E_lx.resize(alpha.size());
	for (int k = 0; k < alpha.size(); ++k) E_lx(k) = digam(alpha(k)) - digam(alpha_sum);
}

void getExpGIG(const double a, const double b, const double p, double& E_x, double& E_invx, double& E_lx) {
	// GIG(x|a,b,p)
	// T1 = (a/b)^{p/2}
	// K_p(y) = modified_B(p, y)
	// T2 = 2 * K_p(√(ab))
	// T3 = (-1/2) * (ax + b/x)
	// f(x) = (T1/T2) * x^(p-1) * exp(T3)
	// if in case expectation, we can use kve(). (Kve can avoid an inf value)
	double Kp = modBessel("kve", p, sqrt(a * b));
	if (isnan(Kp) || isinf(Kp)) std::cout << a << " " << b << " " << p << std::endl;
	double Kpp1 = modBessel("kve", p + 1, sqrt(a * b));
	if (isnan(Kpp1) || isinf(Kpp1)) std::cout << a << " " << b << " " << p << std::endl;
	E_x = (sqrt(b) * Kpp1) / (sqrt(a) * Kp);
	E_invx = (sqrt(a) * Kpp1) / (sqrt(b) * Kp) - (2.0 * p / b);
	//    cout << a << " " << b << " " << p << " " << E_x << endl;

	double px = partialGIG(a, b, p);
	E_lx = log(sqrt(b) / sqrt(a)) + px;
}

/* ================================
Others
================================ */
double modBessel(const std::string kv, double v, double z) {
	if (kv == "kv") {
		return boost::math::cyl_bessel_k(v, z);
	}
	else if (kv == "kve") {
		return boost::math::cyl_bessel_k(v, z) * exp(z);
	}
	else {
		std::cout << "Mod Bessel error" << std::endl;
		return 0;
	}
}

double partialGIG(double a, double b, double p) {
	// 5-point formula
	double h = 0.00001;
	double tKp1 = modBessel("kve", p - (2 * h), sqrt(a * b));
	double tKp2 = -8.0 * modBessel("kve", p - h, sqrt(a * b));
	double tKp3 = 8.0 * modBessel("kve", p + h, sqrt(a * b));
	double tKp4 = -1.0 * modBessel("kve", p + (2 * h), sqrt(a * b));
	double tKp5 = modBessel("kve", p, sqrt(a * b));

	// d log K_p(x) = K'_p(x)/K_p(x)
	return ((tKp1 + tKp2 + tKp3 + tKp4) / (12 * h)) / tKp5;
}
