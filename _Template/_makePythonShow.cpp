
/* ================================
	Template for make image using python
================================ */

/*



#include "pch.h"
#include "../Shared/matplotlibCpp.h"


void _cls::show_(const bool is_,
				 const std::unordered_map<std::string, std::string> params) {

	std::vector<double> vec1, vec2;
	std::vector<std::vector<double>> mat1, mat2;

	// Gen instance and pointer
	std::shared_ptr<PlotData> PD = std::make_shared<PlotData>();
	std::vector<std::shared_ptr<PlotData>> data_vec;

	// Set line and axis data
	std::string style_data1 = "";	// "r-", "b--", ...
	std::string legend_data1 = "";
	std::string style_data2 = "";
	std::string legend_data2 = "";
	std::string axis_x = "";
	std::string axis_y = "";
	if (containsKey(params, "style_data")) style_data1 = params.at("style_data1");
	if (containsKey(params, "legend_data")) legend_data1 = params.at("legend_data1");
	if (containsKey(params, "style_data")) style_data2 = params.at("style_data2");
	if (containsKey(params, "legend_data")) legend_data2 = params.at("legend_data2");
	if (containsKey(params, "axis_x")) axis_x = params.at("axis_x");
	if (containsKey(params, "axis_y")) axis_y = params.at("axis_y");

	// Set data
	if (dim == 1) {
		std::string plot_type = "line_val_only";
		std::vector<double> obs_vec;
		for (int i = 0; i < obs.size(); ++i) {
			obs_vec.emplace_back(obs[i](0));
		}
		if (is_data1 == true) {
			PD->setData(obs_vec, plot_type, style_data, legend_data);
		}
		if (is_data2 == true) {
			PD->setData(filter_vec, plot_type, style_filter, legend_filter);
		}
		//PD.showDataInformation();

		if (axis_x != "") PD->setXLabel(axis_x);
		if (axis_y != "") PD->setYLabel(axis_y);

		data_vec.emplace_back(PD);
	}
	else {
		std::string plot_type = "line_with_pos";
		std::vector<std::vector<double>> obs_mat;
		for (int i = 0; i < obs.size(); ++i) {
			std::vector<double> temp = { obs[i](0), obs[i](1) };
			obs_mat.emplace_back(temp);
		}
		if (is_data == true) PD->setData(obs_mat, plot_type);
		//PD.showDataInformation();
		data_vec.emplace_back(PD);
	}

	// Gen matplotlib instance
	std::shared_ptr<MatplotlibCpp> PLT = std::make_shared<MatplotlibCpp>(data_vec);
	PLT->setSaveFolderPath(result_folder);
	PLT->setSaveFileName("test.png");
	PLT->setShowFlag(true);
	PLT->showInterface();
	PLT->execute();
}



*/
