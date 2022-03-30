/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Fast dynamic time warping

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. DynamicTimeWarping DTW;
		2. DTW.fastDTW(data1, data2)

		Optional:
			setRadius(int)		Default = 1
			setDist(str)		Default = "1"
				dist: Number string("1", "2", ...) shows Lp-norm
					  Euc, KL, etc.

================================ */


#ifndef DYNAMIC_TIME_WARPING_H
#define DYNAMIC_TIME_WARPING_H

#include "pch.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"
#include "../Shared/vec_op.h"

/* ================================
	Distance functions
================================ */
double _difference(const std::vector<double> one_data1, const std::vector<double> one_data2, const int expo);
double _lpnorm(const std::vector<double> one_data1, const std::vector<double> one_data2, const int expo);

/* ================================
	Misc functions
================================ */
std::vector<std::vector<double>> reduce_by_half(const std::vector<std::vector<double>>& data);
std::vector<std::pair<int, int>> expand_window(std::vector<std::pair<int, int>> path, const int len_x, const int len_y, const int radius);


/* ================================
	Main
================================ */
class DynamicTimeWarping{
public:
	DynamicTimeWarping();

	void setRadius(const int rad) { radius = rad; }
	void setDistFunc(const std::string att) { distfunc_att = att; }
	void setDist(const int num) { dist_att_num = num; }
	void setInput(const std::vector<double> x, const std::vector<double> y);
	void setInput(const std::vector<std::vector<double>> x, const std::vector<std::vector<double>> y);

	void calcDistMat(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y,
					 std::unordered_map<std::pair<int, int>, std::tuple<double, int, int>, HashPair>& D,
					 const std::vector<std::pair<int, int>>& window);


	void DTW();		// For default DTW
	void DTW(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y);		// For fastDTW
	void _DTW(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y,
			    std::vector<std::pair<int, int>>& window, const bool is_window);

	void fastDTW();
	void _fastDTW(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y);

	std::vector<std::pair<int, int>> getPath() const { return path; }
	double getDistance() const { return distance; }

	// Save path
	void saveDTWPath() const;

	// For show and save
	void setIsShow(const bool show) { is_show = show; }
	void setIsSave(const bool save);
	void setSavePath(const std::string folder, const std::string file) { is_save = true; save_folder_path = folder; save_file_name = file; save_path = folder + file; }
	void showSaveParams() const;
	void showDTW() const;


private:
	int radius, distfunc_att_num, dist_att_num;
	std::string distfunc_att, save_folder_path, save_file_name, save_path;
	std::function<double(const std::vector<double>, const std::vector<double>, const int)> distFunc;
	std::vector<std::vector<double>> data1, data2;
	std::vector<std::pair<int, int>> path;
	bool is_show, is_save;
	double distance;

};

#endif // DYNAMIC_TIME_WARPING_H

