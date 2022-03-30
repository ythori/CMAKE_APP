/* ================================
Created by Hori on 2020/4/20

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Cpp program with python

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Set data to PlotData()
			PlotData PD;
			PD.setData(data);
			PD.setPlotType("line");

		2. Set MatplotlibCpp using PlotData() with Python.h
			MatplotlibCpp PLT(std::make_shared<PlotData>(PD));
			PLT.setSaveFolderPath(savefolder_path);
			PLT.setSaveFileName("test3.png");

		3. Execute
			PLT.execute()


================================ */

#ifndef MATPLOTLIB_CPP_H
#define MATPLOTLIB_CPP_H

#include "pch.h"
#include "passPyCpp.h"
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

struct PlotData {
	// Contain only one figure data
	//data_att["data"] = "double_vec";
	//data_att["x_ticks"] = "NULL";
	//data_att["y_ticks"] = "NULL";
	//data_att["x_labels"] = "NULL";
	//data_att["y_labels"] = "NULL";
	//data_att["x_lim_range"] = "NULL";
	//data_att["y_lim_range"] = "NULL";

public:
	PlotData();
	void setData(const std::vector<int> vec, const std::string ptype,
				 const std::string sty="", const std::string leg="") {
		vec_int.emplace_back(vec);
		data_type_set.emplace_back("vec_int");
		plot_type.emplace_back(ptype);
		style.emplace_back(sty);
		legend.emplace_back(leg);
		if (leg != "") is_legend = true;
		++n_data;
	}
	void setData(const std::vector<double> vec, const std::string ptype,
				 const std::string sty = "", const std::string leg = "") {
		vec_double.emplace_back(vec);
		data_type_set.emplace_back("vec_double");
		plot_type.emplace_back(ptype);
		style.emplace_back(sty);
		legend.emplace_back(leg);
		if (leg != "") is_legend = true;
		++n_data;
	}
	//void setData(const std::vector<std::vector<int>> mat, const std::string ptype,
	//			 const std::string sty = "", const std::string leg = "") {
	//	mat_int.emplace_back(mat);
	//	data_type_set.emplace_back("mat_int");
	//	plot_type.emplace_back(ptype);
	//	style.emplace_back(sty);
	//	legend.emplace_back(leg);
	//	if (leg != "") is_legend = true;
	//	++n_data;
	//}
	void setData(const std::vector<std::vector<double>> mat, const std::string ptype,
				 const std::string sty = "", const std::string leg = "") {
		mat_double.emplace_back(mat);
		data_type_set.emplace_back("mat_double");
		plot_type.emplace_back(ptype);
		style.emplace_back(sty);
		legend.emplace_back(leg);
		if (leg != "") is_legend = true;
		++n_data;
	}

	void setStyle(const std::vector<std::string> sty) {
		if (sty.size() == n_data) style = sty;
	}

	void setLegend(const std::vector<std::string> leg) {
		if (leg.size() != n_data) {
			is_legend = false;
		}
		else {
			legend = leg;
			is_legend = true;
		}
	}


	void setTitle(const std::string t) { title = t; }
	void setXLabel(const std::string xl) { x_label = xl; }
	void setYLabel(const std::string yl) { y_label = yl; }
	//void setLegend(const bool is) { legend = is; }

	void setXTicks(const std::vector<int> ticks) { x_ticks = ticks; data_att["x_ticks"] = "int"; }
	void setYTicks(const std::vector<int> ticks) { y_ticks = ticks; data_att["y_ticks"] = "int"; }

	void setXLabels(const std::vector<int> labels) { x_labels_int = labels; data_att["x_labels"] = "int"; }
	void setXLabels(const std::vector<double> labels) { x_labels_double = labels; data_att["x_labels"] = "double"; }
	void setXLabels(const std::vector<std::string> labels) { x_labels_str = labels; data_att["x_labels"] = "str"; }

	void setYLabels(const std::vector<int> labels) { y_labels_int = labels; data_att["y_labels"] = "int"; }
	void setYLabels(const std::vector<double> labels) { y_labels_double = labels; data_att["y_labels"] = "double"; }
	void setYLabels(const std::vector<std::string> labels) { y_labels_str = labels; data_att["y_labels"] = "str"; }

	void setXLimRange(const std::pair<double, double> lim) { x_lim_range = lim; data_att["x_lim_range"] = "double"; }
	void setYLimRange(const std::pair<double, double> lim) { y_lim_range = lim; data_att["y_lim_range"] = "double"; }

	// For DTW, spectrogram
	void setNonLabels(const bool axis) { is_non_axis = axis; }
	void setColor(const std::string color, const int n_div) { color_map = color; n_color_div = n_div; }


	std::vector<std::string> getDataTypeSet() const { return data_type_set; }
	std::vector<std::string> getPlotType() const { return plot_type; }
	std::unordered_map<std::string, std::string> getDataAtt() const { return data_att; }

	std::vector<std::vector<int>> getVecInt() const { return vec_int; }
	std::vector<std::vector<double>> getVecDouble() const { return vec_double; }
	std::vector<std::vector<std::vector<int>>> getMatInt() const { return mat_int; }
	std::vector<std::vector<std::vector<double>>> getMatDouble() const { return mat_double; }

	std::string getTitle() const { return title; }
	std::string getXLabel() const { return x_label; }
	std::string getYLabel() const { return y_label; }
	std::vector<std::string> getStyle() const { return style; }
	bool getIsLegend() const { return is_legend; }
	std::vector<std::string> getLegend() const { return legend; }

	std::vector<int> getXTicks() const { return x_ticks; }
	std::vector<int> getYTicks() const { return y_ticks; }
	std::vector<int> getXLabelsInt() const { return x_labels_int; }
	std::vector<int> getYLabelsInt() const { return y_labels_int; }
	std::vector<double> getXLabelsDouble() const { return x_labels_double; }
	std::vector<double> getYLabelsDouble() const { return y_labels_double; }
	std::vector<std::string> getXLabelsStr() const { return x_labels_str; }
	std::vector<std::string> getYLabelsStr() const { return y_labels_str; }

	std::pair<double, double> getXLimRange() const { return x_lim_range; }
	std::pair<double, double> getYLimRange() const { return y_lim_range; }

	bool getIsAxis() const { return is_non_axis; }
	std::string getColorMap() const { return color_map; }
	int getNColorDiv() const { return n_color_div; }

	void showDataInformation() const;

private:
	std::vector<std::vector<int>> vec_int;
	std::vector<std::vector<double>> vec_double;
	std::vector<std::vector<std::vector<int>>> mat_int;
	std::vector<std::vector<std::vector<double>>> mat_double;

	std::vector<std::string> data_type_set, plot_type, style, legend;
	std::string title, x_label, y_label, color_map;
	bool is_legend, is_non_axis;
	//bool is_non_axis;
	std::unordered_map<std::string, std::string> data_att;

	std::vector<int> x_ticks, y_ticks;
	std::vector<int> x_labels_int, y_labels_int;
	std::vector<double> x_labels_double, y_labels_double;
	std::vector<std::string> x_labels_str, y_labels_str;

	std::pair<double, double> x_lim_range, y_lim_range;

	int n_data, n_color_div;

};

class MatplotlibCpp {
public:
	MatplotlibCpp();
	//MatplotlibCpp(const std::shared_ptr<PlotData> PDinst);
	MatplotlibCpp(const std::vector<std::shared_ptr<PlotData>> PDinst);

	void setPyModuleFolderPath(const std::string path) { py_module_folder_path = path; }
	void setPyModuleFileName(const std::string name) { py_module_file_name = name; }
	void setPyModuleClassName(const std::string name) { py_module_class_name = name; }

	void setSaveFolderPath(const std::string path) { save_folder_path = path; }
	void setSaveFileName(const std::string name);
	void setShowFlag(const bool flag) { show_flag = flag; }

	void setPyInitialize() { pyInitialize(); }
	void setPyFinalize() { pyFinalize(); }

	void execute();

	void showInterface() const;

private:
	std::string py_module_folder_path, py_module_file_name, py_module_class_name;
	std::string save_folder_path, save_file_name, save_path;
	bool show_flag, save_flag;
	int n_figure;
	std::vector<std::shared_ptr<PlotData>> PD;
};

/* ================================
	Get 1d-vector from PyObject*
================================ */


#endif // MATPLOTLIB_CPP_H
