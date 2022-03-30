
#include "matplotlibCpp.h"
#include "DataProc.h"



PlotData::PlotData() {
	title = "NULL";
	x_label = "NULL";
	y_label = "NULL";
	is_non_axis = false;
	color_map = "NULL";
	n_color_div = 0;
	vec_int.clear();
	vec_double.clear();
	mat_double.clear();

	data_type_set.clear();

	data_att["x_ticks"] = "NULL";
	data_att["y_ticks"] = "NULL";
	data_att["x_labels"] = "NULL";
	data_att["y_labels"] = "NULL";
	data_att["x_lim_range"] = "NULL";
	data_att["y_lim_range"] = "NULL";

	n_data = 0;
}


void PlotData::showDataInformation() const {
	std::cout << "Number of data: " << data_type_set.size() << std::endl;
	std::cout << "data type and plot type: ";
	for (int i = 0; i < n_data; ++i) {
		echoes("No.", i + 1, ":", data_type_set[i], plot_type[i], style[i], legend[i]);
		//std::cout << "No. " << i + 1 << ": " << data_type_set[i] << " " << plot_type[i] << " " << style << " " << legend[i] std::endl;
	}
	std::cout << "Title: " << title << std::endl;;
	std::cout << "X label name: " << x_label << std::endl;
	std::cout << "Y label name: " << y_label << std::endl;
	std::cout << "Legend: ";
	//if (legend != true) std::cout << "True" << std::endl;
	//else std::cout << "None" << std::endl;
	std::cout << "X ticks: " << data_att.at("x_ticks") << std::endl;
	std::cout << "Y ticks: " << data_att.at("y_ticks") << std::endl;
	std::cout << "X labels list: " << data_att.at("x_labels") << std::endl;
	std::cout << "Y labels list: " << data_att.at("y_labels") << std::endl;
	std::cout << "X limit range: " << data_att.at("x_lim_range") << std::endl;
	std::cout << "Y limit range: " << data_att.at("y_lim_range") << std::endl;
}


MatplotlibCpp::MatplotlibCpp() {
	save_folder_path = "NULL";
	save_file_name = "NULL";
	save_path = "NULL";
	save_flag = false;
	show_flag = true;
	py_module_folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/Shared/";
	py_module_file_name = "matplotlibPython";
	//py_module_class_name = "MatplotlibCpp";
	n_figure = 0;
};

//MatplotlibCpp::MatplotlibCpp(const std::shared_ptr<PlotData> PDinst) {
//	PD.emplace_back(PDinst); 
//	save_folder_path = "NULL";
//	save_file_name = "NULL";
//	save_path = "NULL";
//	save_flag = false;
//	show_flag = true;
//	py_module_folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/Shared/";
//	py_module_file_name = "matplotlibPython";
//	//py_module_class_name = "MatplotlibCpp";
//};

MatplotlibCpp::MatplotlibCpp(const std::vector<std::shared_ptr<PlotData>> PDinst) : PD(PDinst) {
	save_folder_path = "NULL";
	save_file_name = "NULL";
	save_path = "NULL";
	save_flag = false;
	show_flag = true;
	py_module_folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/Shared/";
	py_module_file_name = "matplotlibPython";
	//py_module_class_name = "MatplotlibCpp";
	n_figure = PDinst.size();
};

void MatplotlibCpp::setSaveFileName(const std::string name){
	if (save_folder_path == "NULL") {
		save_flag = false;
	}
	else {
		save_file_name = name;
		save_path = save_folder_path + name;
		save_flag = true;
	}
}

void MatplotlibCpp::showInterface() const {
	if (show_flag == true) std::cout << "Show: TRUE" << std::endl;
	else std::cout << "Show: FALSE" << std::endl;

	if (save_flag == true) std::cout << "Save: TRUE, path: " << save_path << std::endl;
	else std::cout << "Save: FALSE" << std::endl;
}

void MatplotlibCpp::execute() {
	/* ================================
		data_dic:      data_dic["figure[n]"] = { "data[n]" : data }
		params_dic:    params_dic["figure[n]"] = { "att" : val }
			: att: {type[n]}, x_ticks, y_ticks, x_labels, y_labels, x_lim_range, y_lim_range, title, legend
		interface_dic: interface_dic["show", "save", "path"]
	================================ */
	boost::python::dict pydict_data_all, pydict_params_all, pydict_interface;
	std::unordered_map<std::string, int> show_save_dic;

	/* ================================
		pydict_interface
	================================ */
	if (show_flag == true) setDictBoost(pydict_interface, "show", 1);
	else setDictBoost(pydict_interface, "show", 0);
	if (save_flag == true) {
		setDictBoost(pydict_interface, "save", 1);
		setDictBoost(pydict_interface, "path", save_path);
	}
	else {
		setDictBoost(pydict_interface, "save", 0);
		setDictBoost(pydict_interface, "path", "NULL");
	}

	for (int i = 0; i < n_figure; ++i) {
		auto att_dic = PD[i]->getDataAtt();		// dic: x_ticks, y_ticks, x_labels, y_labels, x_lim_range, y_lim_range
		auto data_type = PD[i]->getDataTypeSet();	// str_vec: vec_int, vec_double, mat_double
		auto plot_type = PD[i]->getPlotType();	// str_vec: line_val_only, line_with_pos, scatter, heatmap, ...
		auto title = PD[i]->getTitle();
		auto style = PD[i]->getStyle();
		auto legend = PD[i]->getLegend();
		auto is_legend = PD[i]->getIsLegend();
		auto xlabel = PD[i]->getXLabel();
		auto ylabel = PD[i]->getYLabel();
		auto is_non_axis = PD[i]->getIsAxis();	// For DTW
		auto color_map = PD[i]->getColorMap();
		auto n_color_div = PD[i]->getNColorDiv();

		std::string fig_key = "figure" + std::to_string(i + 1);

		/* ================================
			pydict_data
		================================ */
		int n_data_vi = 0;
		int n_data_vd = 0;
		int n_data_mi = 0;
		int n_data_md = 0;
		std::vector<boost::python::numpy::ndarray> pydict_data;
		for (auto dtype : data_type) {
			if (dtype == "vec_int") {
				auto arr_data = PD[i]->getVecInt()[n_data_vi];
				++n_data_vi;
				//echo_v1(arr_data);
				auto data_np = setNumpyFromVector(arr_data);
				pydict_data.emplace_back(data_np);
			}
			else if (dtype == "vec_double") {
				auto arr_data = PD[i]->getVecDouble()[n_data_vd];
				++n_data_vd;
				//echo_v1(arr_data);
				auto data_np = setNumpyFromVector(arr_data);
				pydict_data.emplace_back(data_np);
			}
			//else if (dtype == "mat_int") {
			//	auto arr_data = PD[i]->getMatInt()[n_data_mi];
			//	++n_data_mi;
			//	//echo_v2(arr_data);
			//	auto data_np = setNumpyFromVector(arr_data);
			//	pydict_data.emplace_back(data_np);
			//}
			else if (dtype == "mat_double") {
				auto arr_data = PD[i]->getMatDouble()[n_data_md];
				++n_data_md;
				//echo_v2(arr_data);
				auto data_np = setNumpyFromVector(arr_data);
				pydict_data.emplace_back(data_np);
			}
		}
		auto pydict_data_of_a_figure = setDictBoost(pydict_data);
		setDictBoost(pydict_data_all, fig_key, pydict_data_of_a_figure);

		/* ================================
			pydict_params
				type, title, legend, xlabel, ylabel, is_axis, color_map, n_color_div
				atts: xy_ticks, xy_labels, xy_lim_range
		================================ */
		auto pylist = setListBoost(plot_type);
		boost::python::dict pydict_params;
		std::string key = "type";
		setDictBoost(pydict_params, key, pylist);

		pylist = setListBoost(style);
		key = "style";
		setDictBoost(pydict_params, key, pylist);

		pylist = setListBoost(legend);
		key = "legend";
		setDictBoost(pydict_params, key, pylist);

		key = "title";
		if (title != "NULL") setDictBoost(pydict_params, key, title);
		key = "is_legend";
		if (is_legend == true)	setDictBoost(pydict_params, key, 1);
		else setDictBoost(pydict_params, key, 0);
		//if (legend != "") setDictBoost(pydict_params, key, legend);
		key = "xlabel";
		if (xlabel != "NULL") setDictBoost(pydict_params, key, xlabel);
		key = "ylabel";
		if (ylabel != "NULL") setDictBoost(pydict_params, key, ylabel);
		key = "is_axis";
		if (is_non_axis == true) setDictBoost(pydict_params, key, 1);
		key = "color";
		if (color_map != "NULL") setDictBoost(pydict_params, key, color_map);
		key = "n_color";
		if (n_color_div > 0) setDictBoost(pydict_params, key, n_color_div);

		for (auto att : att_dic) {
			if (att.first == "x_ticks") {
				if (att.second == "int") {
					auto x_ticks = PD[i]->getXTicks();	// std::vector<int>
				}
			}
			else if (att.first == "y_ticks") {
				if (att.second == "int") {
					auto y_ticks = PD[i]->getXTicks();	// std::vector<int>
				}
			}
			else if (att.first == "x_labels") {
				if (att.second == "int") {
					auto x_labels = PD[i]->getXLabelsInt();
				}
				else if (att.second == "double") {
					auto x_labels = PD[i]->getXLabelsDouble();
				}
				if (att.second == "str") {
					auto x_labels = PD[i]->getXLabelsStr();
				}
			}
			else if (att.first == "y_labels") {
				if (att.second != "NONE") {
					if (att.second == "int") {
						auto y_labels = PD[i]->getYLabelsInt();
					}
					else if (att.second == "double") {
						auto y_labels = PD[i]->getYLabelsDouble();
					}
					if (att.second == "str") {
						auto y_labels = PD[i]->getYLabelsStr();
					}
				}
			}
			else if (att.first == "x_lim_range") {
				if (att.second == "double") {
					auto x_lim_range = PD[i]->getXLimRange();
					std::vector<double> x_lim_range_vec = { x_lim_range.first, x_lim_range.second };
					auto x_lim_range_list = setListBoost(x_lim_range_vec);
					key = "x_lim_range";
					setDictBoost(pydict_params, key, x_lim_range_list);
				}
			}
			else if (att.first == "y_lim_range") {
				if (att.second == "double") {
					auto y_lim_range = PD[i]->getYLimRange();
				}
			}
			else if (att.first == "axis") {
				if (att.second == "Nothing") {

				}
			}
		}
		setDictBoost(pydict_params_all, fig_key, pydict_params);
	}
	std::string func_name = "matplotlibPlot";
	boost::python::dict pydict_args;
	std::string key = "dataset";
	setDictBoost(pydict_args, key, pydict_data_all);
	key = "params";
	setDictBoost(pydict_args, key, pydict_params_all);
	key = "interface";
	setDictBoost(pydict_args, key, pydict_interface);

	auto pyresult = setFunc(py_module_folder_path, py_module_file_name, func_name, pydict_args);

}












//void MatplotlibCpp::execute(const int show_flag){
//	/* ================================
//	Set data
//		def addData(self, data, att, idx=-1):
//		def setData(self, data):
//
//	Set save folder
//		def setSaveFolder(self, save_folder):
//		def setSaveFileName(self, save_fname):
//
//	================================ */
//
//	PyObject* pyInstance = setClass(py_module_folder_path, py_module_file_name, py_module_class_name);
//	PyObject* pArgs = NULL;
//	PyObject* pyResult = NULL;
//	std::vector<PyObject*> args;
//
//	// Set save folder path
//	pyResult = PyObject_CallMethod(pyInstance, "setSaveFolder", "(s)", save_folder_path.c_str());
//
//	// Set save file name
//	pyResult = PyObject_CallMethod(pyInstance, "setSaveFileName", "(s)", save_file_name.c_str());
//
//	// Set data
//	for (auto plot_data : PD) {
//		args.clear();
//		auto data_type_set = plot_data->getDataTypeSet();
//		PyObject* py_data_obj = NULL;
//
//		// First data
//		auto data_type = data_type_set[0];
//		int n_vec_int = 0;
//		int n_vec_double = 0;
//		int n_mat_double = 0;
//
//		if (data_type == "vec_int") {
//			auto data = plot_data->getVecInt();
//			py_data_obj = setVectorFromCppInt(data[0]);
//			++n_vec_int;
//		}
//		else if (data_type == "vec_double") {
//			auto data = plot_data->getVecDouble();
//			//std::cout << "data" << std::endl;
//			//for (auto d : data[0]) {
//			//	std::cout << d << " " << std::endl;
//			//}
//			py_data_obj = setVectorFromCppDouble(data[0]);
//			//std::cout << "size: " << PyList_Size(py_data_obj) << std::endl;
//			++n_vec_double;
//		}
//		else if (data_type == "mat_double") {
//			auto data = plot_data->getMatDouble();
//			py_data_obj = setMatrixFromCppDouble(data[0]);
//			++n_mat_double;
//		}
//		//std::cout << "size2: " << PyList_Size(py_data_obj) << std::endl;
//		args.emplace_back(py_data_obj);
//		args.emplace_back(PyUnicode_FromString(plot_data->getPlotType()[0].c_str()));
//		pArgs = setArgs(args);
//		pyResult = PyObject_CallMethod(pyInstance, "setData", "(O)", pArgs);
//
//		if (data_type_set.size() > 1) {
//			for (int i = 1; i < data_type_set.size(); ++i) {
//				args.clear();
//				data_type = data_type_set[i];
//
//				if (data_type == "vec_int") {
//					auto data = plot_data->getVecInt();
//					py_data_obj = setVectorFromCppInt(data[n_vec_int]);
//					++n_vec_int;
//				}
//				else if (data_type == "vec_double") {
//					auto data = plot_data->getVecDouble();
//					py_data_obj = setVectorFromCppDouble(data[n_vec_double]);
//					++n_vec_double;
//				}
//				else if (data_type == "mat_double") {
//					auto data = plot_data->getMatDouble();
//					py_data_obj = setMatrixFromCppDouble(data[n_mat_double]);
//					++n_mat_double;
//				}
//				args.emplace_back(py_data_obj);
//				args.emplace_back(PyUnicode_FromString(plot_data->getPlotType()[i].c_str()));
//				args.emplace_back(PyLong_FromLong(-1));
//				pArgs = setArgs(args);
//				pyResult = PyObject_CallMethod(pyInstance, "addData", "(O)", pArgs);
//			}
//		}
//		
//	}
//
//	args.clear();
//	args.emplace_back(PyBool_FromLong(1));
//	pArgs = setArgs(args);
//	pyResult = PyObject_CallMethod(pyInstance, "execute", "(i)", show_flag);
//
//
//	//std::cout << "===" << std::endl;
//
//	//setFunc(py_module_folder_path, py_module_file_name, "mattest", NULL);
//
//
//	//PyObject* sys = PyImport_ImportModule("sys");       // import sys
//	//PyObject* path = PyObject_GetAttrString(sys, "path");   // get sys.path
//	//PyList_Append(path, PyUnicode_FromString("C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python37_86/Lib/site-packages"));
//	//PyObject* matplotlibname = PyUnicode_FromString("matplotlib");
//	//PyObject* pyplotname = PyUnicode_FromString("matplotlib.pyplot");
//	//PyObject* matplotlib = PyImport_Import(matplotlibname);
//	//Py_DECREF(matplotlibname);
//	//if (!matplotlib) std::cout << "error" << std::endl;
//	//else std::cout << "success" << std::endl;
//	//PyObject* pFunc = PyObject_GetAttrString(matplotlib, "figure");
//	//auto fig = PyObject_CallObject(pFunc, NULL);
//	//if (!fig) std::cout << "error" << std::endl;
//	//else std::cout << "success" << std::endl;
//
//
//
//
//
//	//std::vector<std::vector<double>> mat;
//	//std::vector<double> t_vec = { 0, 1.1, 2.2 };
//	//for (int i = 0; i < 5; ++i) {
//	//	mat.emplace_back(t_vec * (double)i);
//	//}
//	//auto p_list_obj = setMatrixFromCppDouble(mat);
//	//args.emplace_back(p_list_obj);  // Assume spectrogram
//	//args.emplace_back(p_double_list_obj);   // Assume ticks
//	//args.emplace_back(PyLong_FromLong(1));  // Assume property
//	//args.emplace_back(PyUnicode_FromString("wav_file_address"));  // Assume property
//	//pArgs = setArgs(args);
//
//
//	//auto _ = PyObject_CallMethodObjArg(pyInstance, )
//	//PyObject* pyResult = PyObject_CallMethodObjArgs(pyInstance, );
//}



