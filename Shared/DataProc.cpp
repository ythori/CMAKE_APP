
#include "pch.h"
#include "DataProc.h"
#include <regex>
#include <boost/lexical_cast.hpp>

#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>

bool createDirectory(const std::string folder_name) {
	//std::cout << std::experimental::filesystem::current_path() << std::endl;
	bool is_sf = std::experimental::filesystem::create_directories(folder_name);
	return is_sf;
}

void createFileUsingStr(const std::string save_dir, const std::string file_name, const std::string contents) {
	std::string save_path;
	if (save_dir.substr(save_dir.length() - 1) == "/") save_path = save_dir + file_name;
	else save_path = save_dir + "/" + file_name;

	std::ofstream writing_file;
	std::cout << save_path << std::endl;
	writing_file.open(save_path, std::ios::out);
	writing_file << contents << std::endl;
	writing_file.close();
}

std::vector<std::string> readFileStr(const std::string file_path) {
	std::vector<std::string> ret;
	std::string ttxt = "";

	std::ifstream ifs(file_path);

	while (getline(ifs, ttxt)) {
		//std::cout << ttxt << std::endl;
		ret.emplace_back(ttxt);
	}
	ifs.close();
	return ret;
}


int nearIdxFinder(const std::vector<int> vec, const int val) {
	int back = std::lower_bound(vec.begin() + 1, vec.end() - 1, val) - vec.begin();
	int front = back - 1;

	float x = vec[front] - val;
	float y = vec[back] - val;

	if (x * x < y * y) return front;
	else return back;
}


void recursiveComb(std::vector<int> indexes, int s, int rest, std::vector<std::vector<int>>& ret) {
	if (rest == 0) {
		ret.emplace_back(indexes);
	}
	else {
		if (s < 0) return;
		recursiveComb(indexes, s - 1, rest, ret);
		indexes[rest - 1] = s;
		recursiveComb(indexes, s - 1, rest - 1, ret);
	}
}

std::vector<std::vector<int>> getCombIdx(int n, int k) {
	std::vector<std::vector<int>> ret;
	std::vector<int> indexes(k);
	recursiveComb(indexes, n - 1, k, ret);
	return ret;
}



bool checkInt(const std::string str) {
	if (std::all_of(str.cbegin(), str.cend(), isdigit)) return true;
	else return false;
}

bool isNumber(std::string token) {
	//return std::regex_match(token, std::regex(("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
	try {
		double x = boost::lexical_cast<double>(token);
	}
	catch(...) {
		return false;
	}
	return true;

}

bool isIntDouble(const double val) {
	double eps = 0.0001;
	if (val == int(val)) return true;
	else {
		if (fabs(val - round(val)) > eps) return false;
		else return true;
	}
}

/* ================================
Read std::string
================================ */
std::string trim(const std::string& row_str) {
	char* trimCharacterList = " \t\v\r\n";
	std::string result;

	// Trimming from left
	std::string::size_type left = row_str.find_first_not_of(trimCharacterList);

	if (left != std::string::npos) {
		// Trimming from right if chara is found.
		std::string::size_type right = row_str.find_last_not_of(trimCharacterList);
		result = row_str.substr(left, right - left + 1);
	}
	return result;
}

std::vector<std::string> readTxt(const std::string adrs) {
	std::vector<std::string> result;
	std::ifstream ifs(adrs);
	std::string str;
	while (getline(ifs, str)) {   // read at every line
		auto trim_str = trim(str);
		result.emplace_back(trim_str);
	}
	return result;
}


/* ================================
Read Eigen or vector
================================ */
std::vector<std::string> split(const std::string& s, // input
							   const std::string& delim){ // delimiter
	// split string by delimiter, and store to result
	std::vector<std::string> result;
	std::string::size_type pos = 0;

	while (pos != std::string::npos) {
		std::string::size_type p = s.find(delim, pos);   // find pos including delim from pos

		if (p == std::string::npos) {   // if not find, return npos
			result.push_back(s.substr(pos));    // push back from pos
			break;
		}
		else {
			result.push_back(s.substr(pos, p - pos));   // push back from pos to the number of (p-pos)
		}
		pos = p + delim.size();
	}
	return result;
}

std::vector<double> readStdVectorTxt(const std::string adrs) {
	std::vector<double> vec;
	std::ifstream ifs(adrs);
	std::string str;
	std::vector<std::string> vstr;
	int nrow = 0;
	while (getline(ifs, str)) {   // read at every line
		if (str[0] != '#') {
			vec.push_back(std::stod(str)); // store data as a vector
		}
	}
	return vec;
}

std::vector<std::vector<double>> readStdVector2DTxt(const std::string adrs, std::string delim) {
	std::vector<std::vector<double>> mat;
	std::ifstream ifs(adrs);
	std::string str;
	std::vector<double> vec;
	std::vector<std::string> vstr;
	int nrow = 0;
	while (getline(ifs, str)) {   // read at every line
		if (str[0] != '#') {
			auto vstr = split(str, delim);    // split line(str) by delimiter and store to vector(vstr)

			for (auto s : vstr) {
				vec.emplace_back(std::stod(s)); // store data as a vector
			}
			mat.emplace_back(vec);
			vec.clear();
		}
	}
	return mat;
}

std::vector<std::vector<int>> readStdVector2DiTxt(const std::string adrs, std::string delim) {
	std::vector<std::vector<int>> mat;
	std::ifstream ifs(adrs);
	std::string str;
	std::vector<int> vec;
	std::vector<std::string> vstr;
	int nrow = 0;
	while (getline(ifs, str)) {   // read at every line
		if (str[0] != '#') {
			auto vstr = split(str, delim);    // split line(str) by delimiter and store to vector(vstr)

			for (auto s : vstr) {
				vec.emplace_back(std::stoi(s)); // store data as a vector
			}
			mat.emplace_back(vec);
			vec.clear();
		}
	}
	return mat;
}

Eigen::VectorXd readVectorTxt(const std::string adrs) {
	Eigen::VectorXd vec;
	auto vvec = readStdVectorTxt(adrs);
	int nrow = vvec.size();

	vec.resize(nrow);

	for (int i = 0; i < nrow; ++i) {
		vec(i) = vvec[i];
	}
	return vec;
}

std::vector<Eigen::VectorXd> readStdMatrixTxt(const std::string adrs,   // file address (input:2D txt data)
											  std::string delim) { // delimiter
	std::vector<Eigen::VectorXd> mat;
	std::ifstream ifs(adrs);
	std::string str;
	std::vector<double> vec;
	Eigen::VectorXd temp;
	int flag = 0;

	while (getline(ifs, str)) {   // read at every line
		if (str[0] != '#') {
			auto vstr = split(str, delim);    // split line(str) by delimiter and store to vector(vstr)

			for (auto s : vstr) {
				vec.push_back(std::stod(s)); // store data as a vector
			}

			if (flag == 0) {
				temp.resize(vec.size());
				flag = 1;
			}
			temp = Eigen::Map<Eigen::VectorXd>(&vec[0], vec.size());
			mat.push_back(temp);
			vec.clear();
		}
	}
	return mat;
}

Eigen::MatrixXd readMatrixTxt(const std::string adrs,   // file address (input:2D txt data)
							  std::string delim) { // delimiter
	Eigen::MatrixXd mat;
	auto tmat = readStdMatrixTxt(adrs, delim);

	//echo(adrs);
	int nrow = tmat.size();
	int ncol = tmat[0].size();

	mat.resize(nrow, ncol);

	for (int i = 0; i < nrow; ++i) {
		for (int j = 0; j < ncol; ++j) {
			mat(i, j) = tmat[i](j);
			//mat(i, j) = vec[i*ncol + j];
		}
	}
	//    mat = Map<MatrixXd>(&vec[0], nrow, ncol);   // transform 1d vector to matrix, miss??
	return mat;
}


/* ================================
Save Eigen or vector
================================ */
void saveTxt1D(Eigen::VectorXd& vec, // save vector
			   std::string filename) {  // output name
	std::ofstream writing_file;

	writing_file.open(filename, std::ios::out);
	for (int i = 0; i < vec.size(); ++i) {
		writing_file << std::to_string(vec(i)) << std::endl;
	}
	writing_file.close();
}

void saveTxt1Di(Eigen::VectorXi& vec, // save vector
	std::string filename) {  // output name
	std::ofstream writing_file;

	writing_file.open(filename, std::ios::out);
	for (int i = 0; i < vec.size(); ++i) {
		writing_file << std::to_string(vec(i)) << std::endl;
	}
	writing_file.close();
}


void saveTxt2D(Eigen::MatrixXd& mat, // save matrix
	std::string filename) {  // output name
	int i, j;
	std::ofstream writing_file;
	std::string strs;

	writing_file.open(filename, std::ios::out);
	for (i = 0; i < mat.rows(); ++i) {
		strs = std::to_string(mat(i, 0));
		for (j = 1; j < mat.cols(); ++j) {
			strs += " ";
			strs += std::to_string(mat(i, j));
		}
		writing_file << strs << std::endl;
	}
	writing_file.close();
}


void saveTxt2Di(Eigen::MatrixXi& mat, // save matrix
	std::string filename) {  // output name
	int i, j;
	std::ofstream writing_file;
	std::string strs;

	writing_file.open(filename, std::ios::out);
	for (i = 0; i < mat.rows(); ++i) {
		strs = std::to_string(mat(i, 0));
		for (j = 1; j < mat.cols(); ++j) {
			strs += " ";
			strs += std::to_string(mat(i, j));
		}
		writing_file << strs << std::endl;
	}
	writing_file.close();
}

