/* =======================================================================================

Created by thori on 2021/4/8.
	rev. 

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Data processing functions

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


Print:
	void echo(x):
		std::cout<<x<<std::endl;
	void echo_v1(1d_vector)
		Print std::vector
	void echo_v2(2d_vector)
		Print std::vector<std::vector<>>
	void echoes(a, b, ...):
		Print (a, b, ...)

Check
	bool checkInt(const std::string)
	bool isIntDouble(const double val);		if val == int(val) or not

ReadTxt:
	std::string trim(const std::string& row_str);
	std::vector<std::string> split(const std::string& s, const std::string& delim)

	std::vector<std::string> readTxt(const std::string adrs)
	Eigen::VectorXd readVectorTxt(const std::string adrs)
	Eigen::MatrixXd readMatrixTxt(const std::string adrs, std::string delim = " ")
	std::vector<double> readStdVectorTxt(const std::string adrs)
	std::vector<Eigen::VectorXd> readStdMatrixTxt(const std::string adrs, std::string delim = " ")
	std::vector<std::vector<double>> readStdVector2DTxt(const std::string adrs, std::string delim = " ");
	std::vector<std::vector<int>> readStdVector2DiTxt(const std::string adrs, std::string delim = " ");

SaveTxt:
	void saveTxt2DVec(2d_vector, std::string filename)
	void saveTxt1D(Eigen::VectorXd& vec, std::string filename)
	void saveTxt1Di(Eigen::VectorXi& vec, std::string filename)
	void saveTxt2D(Eigen::MatrixXd& mat, std::string filename)
	void saveTxt2Di(Eigen::MatrixXi& mat, std::string filename)


Convert between std::vector and Eigen:
	std::vector<std::vector<T>> convertEMat2SVec(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& mat)
	std::vector<T> convertEVec2SVec(Eigen::Matrix<T, Eigen::Dynamic, 1>& eigen_1dmat)
	Eigen::Matrix<typename Vector::value_type, Eigen::Dynamic, 1> convertSVec2EVec(Vector& vector)
	void matrix_to_array1d(Matrix& matrix, T* array1d)
	Eigen::Matrix<ValueType, Eigen::Dynamic, Eigen::Dynamic> convertSVec2EMat(Matrix& matrix)


Find idx
	bool idxFinder(const class, const class)
	int nearIdxFinder(const std::vector<int>, const int val)

Sort
	argSort(const input)
	getComb(const std::vector<T>, const int)
		: return nCk combination


unordered_map<pair, _>, unordered_map<tuple, _>
	struct HashPair
	struct HashTuple

	Usage:
		1. Declaration: std::unordered_map<std::pair<long, long>, std::string, HashPair> key_sig;
		2. Usage: key_sig[std::make_pair(0, 0)] = "C-Maj";


======================================================================================= */


#ifndef C_DATAPROC_H
#define C_DATAPROC_H

#include "pch.h"

//#define echo(x) std::cout<<x<<std::endl;
//#define _T std::string("test")
//#define _E std::string("echo")

template <typename T>
void echo(T obj) {
	std::cout << obj << std::endl;
}

template <typename T>
void _echo(std::string type, T obj) {
	if (type == "test" || type == "echo") std::cout << obj << std::endl;
}


// output 1-dimensional container
template <class T>
void echo_v1(T obj) {
	for (auto it = obj.begin(); it != obj.end(); ++it) std::cout << *it << " ";
	std::cout << std::endl;
}

template <class T>
void _echo_v1(std::string type, T obj) {
	if (type == "test" || type == "echo") {
		for (auto it = obj.begin(); it != obj.end(); ++it) std::cout << *it << " ";
		std::cout << std::endl;
	}
}


// output 2-dimensional vector
template <typename T>
void echo_v2(T obj) {
	for (auto v : obj) {
		for (auto vv : v) {
			std::cout << vv << " ";
		}
		std::cout << std::endl;
	}
}

template <typename T>
void _echo_v2(std::string type, T obj) {
	if (type == "test" || type == "echo") {
		for (auto v : obj) {
			for (auto vv : v) {
				std::cout << vv << " ";
			}
			std::cout << std::endl;
		}
	}
}


// echo for optional argument length
template <typename... Args>
void echoes(Args... args) {
	using swallow = std::initializer_list<int>;
	(void)swallow {
		(void(std::cout << args << " "), 0)...
	};
	std::cout << std::endl;
}

template <typename... Args>
void _echoes(std::string type, Args... args) {
	if (type == "test" || type == "echo") {
		using swallow = std::initializer_list<int>;
		(void)swallow {
			(void(std::cout << args << " "), 0)...
		};
		std::cout << std::endl;
	}
}


/* ======================================
 * Create directory, file, Read file
 ====================================== */
bool createDirectory(const std::string folder_name);
void createFileUsingStr(const std::string save_dir, const std::string file_name, const std::string contents);
std::vector<std::string> readFileStr(const std::string file_path);


/* ======================================
 * Sort 2-dimesional vector
 ====================================== */
template <typename T>
std::vector<std::vector<T>> sortTwoDimension(const std::vector<std::vector<T>> vec, const int num) {
	std::vector<std::vector<T>> cvec(vec);
	std::sort(cvec.begin(), cvec.end(), [num](const std::vector<T>& alpha, const std::vector<T>& beta) {return alpha[num] < beta[num]; });
	return cvec;
}


/* ======================================
 * Convert Eigen::Matrix to std::vector
 ====================================== */

template <typename T>
std::vector<std::vector<T>> convertEMat2SVec(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& mat) {
	int row = mat.rows();
	int col = mat.cols();
	std::vector<std::vector<T>> vec(row, std::vector<T>(col, 0));
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			vec[i][j] = mat(i, j);
		}
	}
	return vec;
}


template <typename T>
std::vector<T> convertEVec2SVec(Eigen::Matrix<T, Eigen::Dynamic, 1>& eigen_1dmat) {
	std::vector<T> std_vec(eigen_1dmat.data(), eigen_1dmat.data() + eigen_1dmat.size());
	return std_vec;
}


/* ======================================
 * Convert std::vector to Eigen::Matrix
 ====================================== */
template <class Vector>
Eigen::Matrix<typename Vector::value_type, Eigen::Dynamic, 1> convertSVec2EVec(Vector& vector) {
	auto ret = vector;
	typedef typename Vector::value_type value_type;
	//return Eigen::Map<Eigen::Matrix<value_type, Eigen::Dynamic, 1> >(&vector[0], vector.size(), 1);
	return Eigen::Map<Eigen::Matrix<value_type, Eigen::Dynamic, 1> >(&ret[0], ret.size(), 1);
}

template <class Matrix, class T>
void matrix_to_array1d(Matrix& matrix, T* array1d) {
	const int cols = matrix.size();
	const int rows = matrix[0].size();

	for (int i = 0; i < cols; ++i) {
		std::copy(matrix[i].begin(), matrix[i].end(), array1d);
		array1d += rows;
	}
}

template <class ValueType = double, class Matrix>
Eigen::Matrix<ValueType, Eigen::Dynamic, Eigen::Dynamic> convertSVec2EMat(Matrix& matrix) {
	const int cols = matrix.size();
	const int rows = matrix[0].size();

	// copy to 1d array
	std::unique_ptr<ValueType[]> array1d(new ValueType[rows * cols]);
	matrix_to_array1d(matrix, array1d.get());

	// eigen matrix from 1darray
	return (Eigen::Map<Eigen::Matrix<ValueType, Eigen::Dynamic, Eigen::Dynamic> >(array1d.get(), rows, cols)).transpose();
}


template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> extractPartMat(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& mat,
																const Eigen::VectorXi& idx) {
	int row = mat.rows();
	int col = mat.cols();
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> ret;
	ret.resize(row, col);
	for (int c = 0; c < col; ++c) {
		//std::cout << c << " " << idx(c) << " " << mat.col(idx(c)) << std::endl;
		ret.col(c) = mat.col(idx(c));
	}
	return ret;
}



/* ================================
Check strings whether int or not
================================ */
bool checkInt(const std::string str);
bool isNumber(std::string token);
bool isIntDouble(const double val);

/* ================================
Misc
	idxFinder(vec, number): Search same index number from vector
================================ */
template<class VEC, class VALUE>
bool idxFinder(VEC m, VALUE v) {
	auto itr = std::find(m.begin(), m.end(), v);
	size_t index = std::distance(m.begin(), itr);
	if (index != m.size()) return true;
	else return false;
}

int nearIdxFinder(const std::vector<int> vec, const int val);


/* ================================
Misc
	argSort
================================ */
template<class VEC>
std::vector<int> argSort(VEC v) {
	std::vector<int> index(v.size());
	std::iota(index.begin(), index.end(), 0);
	//echo_v1(index);
	std::sort(index.begin(), index.end(),
			  [&](int x, int y) { return v[x] < v[y]; }
	);
	return index;
}


void recursiveComb(std::vector<int> indexes, int s, int rest, std::vector<std::vector<int>>& ret);
std::vector<std::vector<int>> getCombIdx(int n, int k);

template <typename T>
std::vector<std::vector<T>> getComb(const std::vector<T>& target, const int k) {
	int n = target.size();
	auto comb_idx = getCombIdx(n, k);
	int rows = comb_idx.size();
	int cols = comb_idx[0].size();
	std::vector<std::vector<T>> ret(rows, std::vector<T>(cols));
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			ret[i][j] = target[comb_idx[i][j]];
		}
	}
	return ret;
}


/* ================================
Save Eigen or vector
================================ */
template <typename T>
void saveTxt1DVec(T obj, std::string filename) {
	std::ofstream outfile(filename, std::ios_base::out);
	for (auto data : obj) {
		outfile << data << std::endl;;
	}
}

template <typename T>
void saveTxt2DVec(T obj, std::string filename) {
	std::ofstream outfile(filename, std::ios_base::out);
	int i;
	for (auto v : obj) {
		i = 0;
		for (auto vv : v) {
			if (i == 0) outfile << vv;
			else outfile << " " << vv;
			++i;
		}
		outfile << std::endl;
	}
}
void saveTxt1D(Eigen::VectorXd& vec, std::string filename);
void saveTxt1Di(Eigen::VectorXi& vec, std::string filename);
void saveTxt2D(Eigen::MatrixXd& mat, std::string filename);
void saveTxt2Di(Eigen::MatrixXi& mat, std::string filename);


/* ================================
Read std::string
================================ */
std::string trim(const std::string& row_str);
std::vector<std::string> readTxt(const std::string adrs);

/* ================================
Read Eigen or vector
================================ */
std::vector<std::string> split(const std::string& s, // input
							   const std::string& delim); // delimiter
Eigen::VectorXd readVectorTxt(const std::string adrs);
Eigen::MatrixXd readMatrixTxt(const std::string adrs, std::string delim = " ");
std::vector<double> readStdVectorTxt(const std::string adrs);
std::vector<std::vector<double>> readStdVector2DTxt(const std::string adrs, std::string delim = " ");
std::vector<std::vector<int>> readStdVector2DiTxt(const std::string adrs, std::string delim = " ");
std::vector<Eigen::VectorXd> readStdMatrixTxt(const std::string adrs, std::string delim = " ");


/* ================================
For unordered_map<pair, _>
================================ */

struct HashPair {
	template<class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p) const {

		// Hash first
		auto hash1 = std::hash<T1>{}(p.first);

		// Hash second
		auto hash2 = std::hash<T2>{}(p.second);

		// Exclude multiple
		size_t seed = 0;
		seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

struct HashTuple {
	template<class T1, class T2, class T3>
	size_t operator()(const std::tuple<T1, T2, T3>& p) const {

		// Hash first
		auto hash1 = std::hash<T1>{}(std::get<0>(p));

		// Hash second
		auto hash2 = std::hash<T2>{}(std::get<1>(p));

		// Hash third
		auto hash3 = std::hash<T3>{}(std::get<2>(p));

		// Exclude multiple
		size_t seed = 0;
		seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

template<class MAP, class VALUE>
static bool containsKey(MAP m, VALUE v){
	return m.find(v) != m.end();
}



#endif //C_DATAPROC_H
