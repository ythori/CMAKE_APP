
#include "pch.h"
#include "MathProc.h"
#include <math.h>


int xor128() {
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	unsigned long t;
	t = (x ^ (x << 11));
	x = y; y = z; z = w;
	return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

double frand() {
	unsigned int x = xor128();
	//    cout << "x: " << x << endl;
	double y = x % 1000000 / static_cast<double>(1000000);
	//    cout << "y: " << y << endl;
	return y;
}

int irand(const int minval, const int maxval) {
	std::random_device rnd;
	static std::mt19937 mt(rnd());
	std::uniform_int_distribution<> randint(minval, maxval);
	return randint(mt);
}

std::string decimalToHexadecimal(const int num) {
	std::stringstream ss;
	ss << std::hex << num;
	return "0x" + ss.str();
}

int hexadecimalToDecimal(const std::string numstr) {
	int num;
	std::istringstream(numstr) >> std::hex >> num;
	return num;
}


double calPartial(const std::function<double(const std::vector<double>)>& callback_func,
				  const std::vector<double> variables, const double value, const int variable_num) {
	// 5th point appropriation
	double h = 0.00001;
	std::vector<double> mod_variables(variables.size(), 0);
	for (size_t st = 0; st < variables.size(); ++st) mod_variables[st] = variables[st];
	mod_variables[variable_num] = value + h;
	double y1 = callback_func(mod_variables);
	mod_variables[variable_num] = value - h;
	double y2 = callback_func(mod_variables);
	mod_variables[variable_num] = value + 2 * h;
	double y3 = callback_func(mod_variables);
	mod_variables[variable_num] = value - 2 * h;
	double y4 = callback_func(mod_variables);

	return (y4 - 8 * y2 + 8 * y1 - y3) / (12 * h);
}

// Overload
double calcPartial(const std::function<double(const std::vector<std::vector<std::vector<double>>>)>& callback_func,
				   const std::vector<std::vector<std::vector<double>>>& variables, const std::tuple<int, int, int>& variable_position) {

	double h = 0.00001;
	std::vector<std::vector<std::vector<double>>> mod_variables = variables;
	int pos_param = std::get<0>(variable_position);
	int pos_data_idx = std::get<1>(variable_position);
	int pos_data_dim = std::get<2>(variable_position);
	double value_before = variables[pos_param][pos_data_idx][pos_data_dim];

	mod_variables[pos_param][pos_data_idx][pos_data_dim] = value_before + h;
	double y1 = callback_func(mod_variables);
	mod_variables[pos_param][pos_data_idx][pos_data_dim] = value_before - h;
	double y2 = callback_func(mod_variables);
	mod_variables[pos_param][pos_data_idx][pos_data_dim] = value_before + 2 * h;
	double y3 = callback_func(mod_variables);
	mod_variables[pos_param][pos_data_idx][pos_data_dim] = value_before - 2 * h;
	double y4 = callback_func(mod_variables);

	return (y4 - 8 * y2 + 8 * y1 - y3) / (12 * h);

}



Eigen::MatrixXd sortForColEigenXd(const Eigen::MatrixXd& objmat, const int col) {
	/* ================================
	1. Generate query
		(case positive value, if you want to sort for negative, change 0 to -100000...)
	2. Generate identity matrix
	3. Sort identity matrix
	4. Multiply transposed above matrix to objective matrix
	================================ */
	Eigen::VectorXd distance = objmat.col(col).array() - 0;
	Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> perm_row(objmat.rows());
	perm_row.setIdentity();
	std::sort(perm_row.indices().data(),
			  perm_row.indices().data() + perm_row.indices().size(),
			  [&distance](size_t i1, size_t i2) {return distance(i1) < distance(i2); });
	return perm_row.transpose() * objmat;
}

Eigen::MatrixXi sortForColEigenXi(const Eigen::MatrixXi& objmat, const int col) {
	Eigen::VectorXi distance = objmat.col(col).array() - 0;
	Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> perm_row(objmat.rows());
	perm_row.setIdentity();
	std::sort(perm_row.indices().data(),
			  perm_row.indices().data() + perm_row.indices().size(),
			  [&distance](size_t i1, size_t i2) {return distance(i1) < distance(i2); });
	return perm_row.transpose() * objmat;
}



double logsumexp(const std::vector<double>& lps) {
	if (lps.size() == 0) return -1e300;
	double mx = lps[0];
	for (size_t i = 0; i < lps.size(); i++) {
		if (lps[i] > mx) mx = lps[i];
	}
	double sum = 0;
	for (size_t i = 0; i < lps.size(); i++) {
		sum += exp(lps[i] - mx);
	}
	return mx + log(sum);
}

Eigen::VectorXd normalizeEigen(const Eigen::VectorXd& vec) {
	return vec.array() / vec.sum();
}


Eigen::MatrixXd normalizeMatEigen(const Eigen::MatrixXd& mat, const int axis) {
	Eigen::MatrixXd resmat = Eigen::MatrixXd::Zero(mat.rows(), mat.cols());
	if (axis == 0) {
		// Equal .colwise().sum()	<- Extract a column vector, sum the vector, extract next column.
		//std::cout << "Create normalized column vectors" << std::endl;
		Eigen::VectorXd tcol;
		for (int c = 0; c < mat.cols(); ++c) {
			tcol = mat.col(c);
			auto t_vec = normalizeEigen(tcol);
			resmat.col(c) = t_vec;
		}
	}
	else {
		// Equal .rowwise().sum()	<- Extract a row vector, sum the vector, extract next row.
		//std::cout << "Create normalized row vectors" << std::endl;
		Eigen::VectorXd trow;
		for (int r = 0; r < mat.rows(); ++r) {
			trow = mat.row(r);
			auto t_vec = normalizeEigen(trow);
			resmat.row(r) = t_vec;
		}
	}
	return resmat;
}


double logsumexpEigenSum(const Eigen::VectorXd& vec) {
	double mx = vec(0);
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec(i) > mx) mx = vec(i);
	}
	double sum = 0;
	for (size_t i = 0; i < vec.size(); i++) {
		sum += exp(vec(i) - mx);
	}
	return mx + log(sum);
}

Eigen::VectorXd logsumexpEigen(const Eigen::VectorXd& vec) {
	double mx = logsumexpEigenSum(vec);
	return vec.array() - mx;
}


Eigen::MatrixXd calCov(const Eigen::MatrixXd& input_mat) {
	// rows: one data
	Eigen::VectorXd mean = input_mat.colwise().mean();
	Eigen::MatrixXd temp = Eigen::MatrixXd::Zero(input_mat.rows(), input_mat.cols());
	for (int i = 0; i < input_mat.cols(); ++i) {
		temp.col(i) = input_mat.col(i).array() - mean(i);
	}

	return temp.transpose() * temp / input_mat.rows();
}

Eigen::MatrixXd invMat(const Eigen::MatrixXd& input_mat) {
	// AX = I -> X=LU(A).solve(I)
	Eigen::FullPivLU<Eigen::MatrixXd> lu(input_mat);
	Eigen::MatrixXd I = Eigen::MatrixXd::Identity(input_mat.rows(), input_mat.cols());

	// If rank reduction
	if (lu.rank() != input_mat.rows()) {
		Eigen::MatrixXd temp(input_mat);
		for (int i = 0; i < input_mat.rows(); ++i) {
			temp(i, i) += (frand() * 0.01);
		}
		Eigen::FullPivLU<Eigen::MatrixXd> t_lu(temp);
		lu = t_lu;
	}

	return lu.solve(I);
}


Eigen::VectorXd cumsum(const Eigen::VectorXd& input_vec) {
	Eigen::VectorXd output_vec = Eigen::VectorXd::Zero(input_vec.size());
	output_vec(0) = input_vec(0);
	for (int i = 1; i < input_vec.size(); ++i) {
		output_vec(i) = output_vec(i - 1) + input_vec(i);
	}
	return output_vec;
}


Eigen::MatrixXd householder(const Eigen::MatrixXd& input_mat, std::vector<Eigen::MatrixXd>& H) {
	int dim_row = input_mat.rows();
	int dim_col = input_mat.cols();
	Eigen::MatrixXd output(input_mat);
	Eigen::MatrixXd prodH = Eigen::MatrixXd::Ones(dim_row, dim_row);

	for (int j = 0; j < dim_col; ++j) {
		if (j == dim_row - 1) break;
		Eigen::MatrixXd Hmat = Eigen::MatrixXd::Identity(dim_row, dim_row);
		Eigen::MatrixXd I = Eigen::MatrixXd::Identity(dim_row - j, dim_row - j);
		Eigen::MatrixXd tmat = output.block(j, j, dim_row - j, 1);
		tmat = tmat + (tmat.norm() * I.col(0));
		I = I - (tmat * tmat.transpose() * (2.0 / tmat.squaredNorm()));
		Hmat.block(j, j, dim_row - j, dim_row - j) = I;
		H.push_back((Hmat));
		output = Hmat * output;
		if (j == 0) prodH = Hmat;
		else prodH = Hmat * prodH;
	}
	H.push_back(prodH);
	return output;
}


double constrainAngle(double x) {
	x = fmod(x + M_PI, 2 * M_PI);
	if (x < 0)
		x += (2 * M_PI);
	return x - M_PI;
}
double angleConv(double angle) {
	return fmod(constrainAngle(angle), 2 * M_PI);
}
double angleDiff(double a, double b) {
	double dif = fmod(b - a + M_PI, 2 * M_PI);
	if (dif < 0)
		dif += (2 * M_PI);
	return dif - M_PI;
}
double unwrap(double previousAngle, double newAngle) {
	return previousAngle - angleDiff(newAngle, angleConv(previousAngle));
}

double mmod(double a, double m) {
	if (m < 0) { m = -m; }
	if (a < 0) {
		a = m - (std::fmod(-a, m));
	}
	else {
		a = std::fmod(a, m);
	}
	return a;
}

std::vector<std::vector<double>> unwrapNumpy(std::vector<std::vector<double>>& mat) {
	int n_time = mat.size();
	int n_freq = mat[0].size();
	double M2PI = 2 * M_PI;
	std::cout << n_time << " " << n_freq << " " << M2PI << std::endl;
	std::vector<std::vector<double>> dd(n_time, std::vector<double>(n_freq - 1));
	std::vector<std::vector<double>> dd_mod(n_time, std::vector<double>(n_freq - 1));
	std::vector<std::vector<double>> ph_correct(n_time, std::vector<double>(n_freq - 1));
	std::vector<std::vector<double>> ph_cumsum(n_time, std::vector<double>(n_freq - 1));
	std::vector<std::vector<double>> up(n_time, std::vector<double>(n_freq));

	for (int t = 0; t < n_time; ++t) {
		up[t][0] = mat[t][0];
		for (int k = 0; k < n_freq - 1; ++k) {
			dd[t][k] = mat[t][k + 1] - mat[t][k];
			dd_mod[t][k] = mmod((dd[t][k] + M_PI), M2PI) - M_PI;
			if ((dd_mod[t][k] == -M_PI) && (dd[t][k] > 0)) dd_mod[t][k] = M_PI;
			ph_correct[t][k] = dd_mod[t][k] - dd[t][k];
			if (abs(dd[t][k]) < M_PI) ph_correct[t][k] = 0;
			if (k == 0) ph_cumsum[t][k] = ph_correct[t][k];
			else {
				ph_cumsum[t][k] = ph_correct[t][k] + ph_cumsum[t][k - 1];
				up[t][k] = mat[t][k] + ph_cumsum[t][k - 1];
			}
		}
		up[t][n_freq - 1] = mat[t][n_freq - 1] + ph_cumsum[t][n_freq - 2];
	}
	return up;
}
