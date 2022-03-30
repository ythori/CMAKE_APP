#include "pch.h"
#include "DynamicTimeWarping.h"
#include "matplotlibCpp.h"
#include "../Shared/DataProc.h"

/* ================================
	Distance functions
================================ */
double _difference(const std::vector<double> one_data1, const std::vector<double> one_data2, const int expo) {
	double diff = one_data1[0] - one_data2[0];
	if (expo == 1) return std::fabs(diff);
	else return std::pow(std::fabs(diff), expo);
}

double _lpnorm(const std::vector<double> data1, const std::vector<double> data2, const int expo) {
	// ||x||_p = (|x1|^p + |x2|^p + ... + |xn|^p)^(1/p)
	double sum_val = 0;
	for (int i = 0; i < data1.size(); ++i) {
		sum_val += std::pow(std::fabs(data1[i] - data2[i]), expo);
	}
	return std::pow(sum_val, 1. / expo);
}


/* ================================
	Misc functions
================================ */
std::vector<std::vector<double>> reduce_by_half(const std::vector<std::vector<double>>& data) {
	int n_max = data.size() - (data.size() % 2);
	int n_dim = data[0].size();
	std::vector<std::vector<double>> output;
	for (int i = 0; i < n_max; i += 2) output.emplace_back((data[i] + data[i + 1]) / 2.);
	return output;
}

std::vector<std::pair<int, int>> expand_window(std::vector<std::pair<int, int>> path, const int len_x, const int len_y, const int radius) {
	auto _path = path;
	for (auto pos : path) {
		for (int a = -radius; a < radius + 1; ++a) {
			for (int b = -radius; b < radius + 1; ++b) {
				auto t_pair = std::make_pair(pos.first + a, pos.second + b);
				if (idxFinder(_path, t_pair)) continue;
				_path.emplace_back(t_pair);
			}
		}
	}
	std::vector<std::pair<int, int>> _window;
	for (auto pos : _path) {
		auto t_pair = std::make_pair((int)(pos.first * 2), (int)(pos.second * 2));
		if (!idxFinder(_window, t_pair)) _window.emplace_back(t_pair);

		auto t_pair_jp1 = std::make_pair((int)(pos.first * 2), (int)(pos.second * 2 + 1));
		if (!idxFinder(_window, t_pair_jp1)) _window.emplace_back(t_pair_jp1);

		auto t_pair_ip1 = std::make_pair((int)(pos.first * 2 + 1), (int)(pos.second * 2));
		if (!idxFinder(_window, t_pair_ip1)) _window.emplace_back(t_pair_ip1);

		auto t_pair_ijp1 = std::make_pair((int)(pos.first * 2 + 1), (int)(pos.second * 2 + 1));
		if (!idxFinder(_window, t_pair_ijp1)) _window.emplace_back(t_pair_ijp1);
	}

	std::vector<std::pair<int, int>> window;
	int start_j = 0;
	for (int i = 0; i < len_x; ++i) {
		int new_start_j = 0;
		bool is_new_start_j = false;
		for (int j = start_j; j < len_y; ++j) {
			auto t_pair = std::make_pair(i, j);
			if (idxFinder(_window, t_pair)) {
				window.emplace_back(t_pair);
				if (is_new_start_j == false) {
					new_start_j = j;
					is_new_start_j = true;
				}
			}
			else if (is_new_start_j == true) break;
		}
		start_j = new_start_j;
	}
	return window;
}


/* ================================
	Main
================================ */
DynamicTimeWarping::DynamicTimeWarping() {
	radius = 1;
	distfunc_att = "1";		// L1-norm
	distfunc_att_num = 1;	// int(distfunc_att)
	dist_att_num = 1;	// 1: Manha
	path.clear();
	distance = 0;
	is_show = true;
	is_save = false;
	save_folder_path = "NULL";
	save_file_name = "NULL";
	save_path = "NULL";
}


void DynamicTimeWarping::setInput(const std::vector<double> x, const std::vector<double> y) {
	data1.clear();
	data2.clear();
	for (int i = 0; i < x.size(); ++i) {
		std::vector<double> temp_x = { x[i] };
		data1.emplace_back(temp_x);
	}
	for (int i = 0; i < y.size(); ++i) {
		std::vector<double> temp_y = { y[i] };
		data2.emplace_back(temp_y);
	}
	bool is_digit_num = checkInt(distfunc_att);
	if (is_digit_num == true) dist_att_num = std::stoi(distfunc_att);
	else dist_att_num = -1;
	distFunc = _lpnorm;
}

void DynamicTimeWarping::setInput(const std::vector<std::vector<double>> x, const std::vector<std::vector<double>> y) {
	if (x[0].size() != y[0].size()) {
		echo("Data dimension error");
	}
	else {
		data1 = x;
		data2 = y;
	}
	bool is_digit_num = checkInt(distfunc_att);
	if (is_digit_num == true) dist_att_num = std::stoi(distfunc_att);
	else dist_att_num = -1;
	distFunc = _lpnorm;
}


void DynamicTimeWarping::calcDistMat(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y,
									 std::unordered_map<std::pair<int, int>, std::tuple<double, int, int>, HashPair>& D,
									 const std::vector<std::pair<int, int>>& window) {
	// Set D[pair<i, i>] = tuple<d, i, i>
	//echo(11111111111111);
	if (dist_att_num == 1) {	// Manhattan distance
		for (auto pos : window) {
			auto x_m1_vec = x[pos.first - 1];
			auto y_m1_vec = y[pos.second - 1];
			auto dt = distFunc(x_m1_vec, y_m1_vec, dist_att_num);
			//echoes("dt", dt);

			double dist_i_m1 = 1e+100;
			double dist_j_m1 = 1e+100;
			double dist_ij_m1 = 1e+100;
			std::vector<std::pair<int, int>> pos_list;
			pos_list.emplace_back(std::make_pair(pos.first - 1, pos.second));
			if (containsKey(D, pos_list[pos_list.size() - 1])) {
				dist_i_m1 = std::get<0>(D[std::make_pair(pos.first - 1, pos.second)]) + dt;
			}
			pos_list.emplace_back(std::make_pair(pos.first, pos.second - 1));
			if (containsKey(D, pos_list[pos_list.size() - 1])) {
				dist_j_m1 = std::get<0>(D[std::make_pair(pos.first, pos.second - 1)]) + dt;
			}
			pos_list.emplace_back(std::make_pair(pos.first - 1, pos.second - 1));
			if (containsKey(D, pos_list[pos_list.size() - 1])) {
				dist_ij_m1 = std::get<0>(D[std::make_pair(pos.first - 1, pos.second - 1)]) + dt;
			}
			//double dist_ij_m1 = std::get<0>(D[std::make_pair(pos.first - 1, pos.second - 1)]) + dt;
			//echoes(pos.first - 1, pos.second - 1, dist_i_m1, dist_j_m1, dist_ij_m1);

			// Get index of min value position
			std::vector<double> val_list = { dist_i_m1, dist_j_m1, dist_ij_m1 };
			int idx_min = 0;
			double min_val = val_list[0];
			for (int i = 1; i < val_list.size(); ++i) {
				if (min_val > val_list[i]) {
					idx_min = i;
					min_val = val_list[i];
				}
			}

			D[std::make_pair(pos.first, pos.second)] = std::make_tuple(val_list[idx_min], pos_list[idx_min].first, pos_list[idx_min].second);
			//echoes(pos.first, pos.second, "pos:(", pos_list[idx_min].first, pos_list[idx_min].second, ")", val_list[idx_min]);
		}
	}
	else if (dist_att_num == 2) {	// constraint (1/2 to 2)
		echo(2);
	}
}


void DynamicTimeWarping::DTW() {
	std::vector<std::pair<int, int>> window;
	_DTW(data1, data2, window, false);
}

void DynamicTimeWarping::DTW(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y) {
	std::vector<std::pair<int, int>> window;
	_DTW(x, y, window, false);
}


void DynamicTimeWarping::_DTW(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y,
							  std::vector<std::pair<int, int>>& window, const bool is_window) {
	// Set path and distance (member vaiables)
	if (is_window == false) {
		window.clear();
		for (int i = 0; i < x.size(); ++i) {
			for (int j = 0; j < y.size(); ++j) window.emplace_back(std::make_pair(i, j));
		}
	}
	for (int i = 0; i < window.size(); ++i) {
		window[i].first += 1;
		window[i].second += 1;
	}

	std::unordered_map<std::pair<int, int>, std::tuple<double, int, int>, HashPair> D;	// [distance, i, j]
	D[std::make_pair(0, 0)] = std::make_tuple(0, 0, 0);

	calcDistMat(x, y, D, window);

	path.clear();
	int i = x.size();
	int j = y.size();

	while (true) {
		if (i == 0 && j == 0) break;
		//echoes("pre", i, j);
		path.emplace_back(std::make_pair(i - 1, j - 1));
		int next_i = std::get<1>(D[std::make_pair(i, j)]);
		int next_j = std::get<2>(D[std::make_pair(i, j)]);
		i = next_i;
		j = next_j;
		//echoes("aft", i, j);
	}
	std::reverse(begin(path), end(path));
	distance = std::get<0>(D[std::make_pair(x.size(), y.size())]);
	echoes("Distance:", distance);
	echoes(path.size(), "/ data1:", data1.size(), "data2:", data2.size());
}


void DynamicTimeWarping::fastDTW() {
	// data1, data2: Original
	_fastDTW(data1, data2);
}


void DynamicTimeWarping::_fastDTW(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y) {
	// x, y: Modified original data for recursion
	int min_time_size = radius + 2;
	if ((x.size() < min_time_size) || (y.size() < min_time_size)) {
		DTW(x, y);		// First path is created
	}
	else {
		auto x_shrinked = reduce_by_half(x);
		auto y_shrinked = reduce_by_half(y);
		_fastDTW(x_shrinked, y_shrinked);
		auto window = expand_window(path, x.size(), y.size(), radius);
		_DTW(x, y, window, true);
	}
}


/* ================================
	Save path to txt
================================ */
void DynamicTimeWarping::saveDTWPath() const {
	std::vector<std::vector<int>> t_path;
	for (auto p : path) {
		std::vector<int> temp = { p.first, p.second };
		t_path.emplace_back(temp);
	}
	saveTxt2DVec(t_path, save_path);
}

/* ================================
	For show and save
================================ */
void DynamicTimeWarping::setIsSave(const bool save) {
	if (save_path == "NULL") is_save = false;
	else is_save = save;
}

void DynamicTimeWarping::showSaveParams() const {
	if (is_show == true) std::cout << "Show: TRUE" << std::endl;
	else std::cout << "Show: FALSE" << std::endl;

	if (is_save == true) {
		std::cout << "Save: TRUE" << std::endl;
		std::cout << "Folder path: "  << save_folder_path << ", Filename: " << save_file_name << std::endl;
		std::cout << "(Save path: " << save_path << ")" << std::endl;
	}
	else std::cout << "Save: FALSE" << std::endl;
}

void DynamicTimeWarping::showDTW() const {

	PlotData PD1, PD2, PD3;
	std::vector<std::shared_ptr<PlotData>> data_vec;

	PD1.setData(data1, "heatmap");
	PD1.setNonLabels(true);
	PD1.setColor("Oranges", 0);

	double mod_rate = double(data1.size()) / data2.size();
	std::vector<std::vector<double>> match_line;
	for (auto match_pos : path) {
		double pos_data1 = match_pos.first + 0.5;
		double pos_data2 = match_pos.second * mod_rate + 0.5;
		std::vector<double> one_match = { pos_data2, 0 };
		match_line.emplace_back(one_match);
		one_match = { pos_data1, 1 };
		match_line.emplace_back(one_match);
		PD2.setData(match_line, "line_with_pos");
		match_line.clear();
	}
	PD2.setNonLabels(true);
	PD2.setXLimRange(std::pair<double, double>(0, data1.size()));
	PD2.setColor("jet", 5);

	PD3.setData(data2, "heatmap");
	PD3.setNonLabels(true);
	PD3.setColor("Oranges", 0);

	data_vec.emplace_back(std::make_shared<PlotData>(PD1));
	data_vec.emplace_back(std::make_shared<PlotData>(PD2));
	data_vec.emplace_back(std::make_shared<PlotData>(PD3));

	MatplotlibCpp PLT(data_vec);
	PLT.setSaveFolderPath(save_folder_path);
	PLT.setSaveFileName(save_file_name);
	PLT.setShowFlag(true);
	PLT.showInterface();
	PLT.execute();

}







