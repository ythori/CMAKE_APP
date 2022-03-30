/* ================================
Created by Hori on 2021/4/9.

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Math functions

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Random values
		int xor128();
			Generate random series pattern

		double frand();
			Generate uniform value in [0, 1]

		int irand(const int minval, const int maxval);
			Generate int value in [minval, maxval]


	Convert number
		std::string decimalToHexadecimal(const int num)
			Convert decimal_number to hexadecimal_number_str


	double calcPartial(): Numerical differentiation
		double calcPartial(const std::function<double(const std::vector<double>)>& callback_func,
						   const std::vector<double> variables, const double value, const int variable_num = 0);
			-> For gamma pdf: std::function<double(std::vector<double>)>: return double, input std::vector<double>
				input: {obs, shape, scale}

		double calcPartial(const std::function<double(const std::vector<std::vector<std::vector<double>>>)>& callback_func,
						   const std::vector<std::vector<std::vector<double>>>& variables, const std::tuple<int, int, int>& variable_position);
			-> For LnMultiNorm pdf
				input: {{obs[n, dim]}, {mean_ndim[0, dim]}, {cov_ndim[dim, dim]}}


	Sort matrix data
		void sortForColEigenXd(const Eigen::MatrixXd& objmat, Eigen::MatrixXd& mat, const int col)
			Sort Eigen_double_matrix by ordering nearby objmat[col]

		void sortForColEigenXi(const Eigen::MatrixXi& objmat, Eigen::MatrixXi& mat, const int col);


	logsumexp
		Only summation
			double logsumexp(const std::vector<double>& lps);
				Calculate exp(log x1 + log x2 + ...)

			double logsumexpEigenSum(const Eigen::VectorXd & vec);

		Add normalization
			Eigen::VectorXd logsumexpEigen(const Eigen::VectorXd& vec);
				Transform to normalized vector consisted of logarithm value



	Matrix operator
		Normalize
			Eigen::VectorXd normalizeEigen(const Eigen::VectorXd& vec);
				Transform to normalized vector

			Eigen::MatrixXd normalizeMatEigen(const Eigen::MatrixXd& mat, const int axis);
				Normalize following to all columns or all rows

		Cov, Inv
			Eigen::MatrixXd calCov(const Eigen::MatrixXd& mat);
				Calculate covariance

			Eigen::MatrixXd invMat(const Eigen::MatrixXd& input_mat);
				Calculate inverse matrix

		Eigen::VectorXd cumsum(const Eigen::VectorXd& input_vec);
			For particle filter, calculate cumulative summation (x1, x1+x2, x1+x2+x3, ...)

		Eigen::MatrixXd householder(const Eigen::MatrixXd& input_mat, std::vector<Eigen::MatrixXd>& H);
			For local stationary AR model, calculate householder translation
				H includes householder matrix and producted householder matrix (can extract by H.back())


	Unwrap phase
		double constrainAngle(double x);
		double angleConv(double angle);
		double angleDiff(double a, double b);
		double unwrap(double previousAngle, double newAngle);
		double mmod(double a, double m);	// equal to python np.mod(a, m)
		std::vector<std::vector<double>> unwrapNumpy(std::vector<std::vector<double>> mat);		// mat[time][freq]


================================ */


#ifndef MATHPROC_H
#define MATHPROC_H

#include "pch.h"

//#define M_PI (3.1415926535897932384626)
#define SWAP(type, a, b) do {type tmp = a; a = b; b = tmp;} while (0)
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define MAX3(a, b, c) ((a) > (MAX2(b, c)) ? (a) : (MAX2(b, c)))
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MIN3(a, b, c) ((a) < (MIN2(b, c)) ? (a) : (MIN2(b, c)))


int xor128();
double frand();
int irand(const int minval, const int maxval);

std::string decimalToHexadecimal(const int num);
int hexadecimalToDecimal(const std::string numstr);

double calcPartial(const std::function<double(const std::vector<double>)>& callback_func,
				   const std::vector<double> variables, const double value, const int variable_num = 0);
double calcPartial(const std::function<double(const std::vector<std::vector<std::vector<double>>>)>& callback_func,
				   const std::vector<std::vector<std::vector<double>>>& variables, const std::tuple<int, int, int>& variable_position);

Eigen::MatrixXd sortForColEigenXd(const Eigen::MatrixXd& objmat, const int col);
Eigen::MatrixXi sortForColEigenXi(const Eigen::MatrixXi& objmat, const int col);

double logsumexp(const std::vector<double>& lps);
double logsumexpEigenSum(const Eigen::VectorXd& vec);
Eigen::VectorXd logsumexpEigen(const Eigen::VectorXd& vec);

Eigen::VectorXd normalizeEigen(const Eigen::VectorXd& vec);
Eigen::MatrixXd normalizeMatEigen(const Eigen::MatrixXd& mat, const int axis);
Eigen::MatrixXd calCov(const Eigen::MatrixXd& mat);
Eigen::MatrixXd invMat(const Eigen::MatrixXd& input_mat);
Eigen::VectorXd cumsum(const Eigen::VectorXd& input_vec);
Eigen::MatrixXd householder(const Eigen::MatrixXd& input_mat, std::vector<Eigen::MatrixXd>& H);

/* =================
 * Unwrap function
 ================= */
double constrainAngle(double x);
double angleConv(double angle);
double angleDiff(double a, double b);
double unwrap(double previousAngle, double newAngle);
double mmod(double a, double m);	// equal to python np.mod(a, m)
std::vector<std::vector<double>> unwrapNumpy(std::vector<std::vector<double>>& mat);		// mat[time][freq]



#endif //MATHPROC_H
