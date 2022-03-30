/* ================================
Created by Hori on 2020/4/20

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Cpp program with python

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Py_Initialize();
		2. Set path and file names
			std::string folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/Shared/";
			std::string file_name = "python_code_sample";
			std::string func_name = "sample_test";
		3. Set args
			std::vector<int> int_vec = { 0, 2, 4, 6 };
			std::vector<PyObject*> args;
			auto p_int_list_obj = setVectorFromCppInt(int_vec);
			args.emplace_back(p_int_list_obj);
			auto pArgs = setArgs(args);
		4. Get results
			auto result_pyobj = setFunc(folder_path, file_name, func_name, pArgs);
		5. Convert PyObject* to std::vector
			auto result_vec_int = extractVectorIntFromPyobj(result_pyobj);



================================ */
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

//#define BOOST_PYTHON_STATIC_LIB

#ifndef PASS_PY_CPP_H
#define PASS_PY_CPP_H

#include "pch.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

void pyInitialize();
void pyFinalize();

/* ================================
	Get int from pyobject
================================ */
int extractInt(boost::python::api::object pyobj);
double extractDouble(boost::python::api::object pyobj);

/* ================================
	Get 1d-vector from PyObject*
================================ */
std::vector<int> extractVectorIntFromPyobj(PyObject* ret_from_func);	// PyObject cannot set const
std::vector<double> extractVectorDoubleFromPyobj(PyObject* ret_from_func);
std::vector<std::string> extractVectorStrFromPyobj(PyObject* ret_from_func);

/* ================================
	Get 2d-vector from PyObject*
		Not implemented: int and std::string
================================ */
//std::vector<std::vector<int>> extractMatrixIntFromPyobj(PyObject* ret_from_func);
std::vector<std::vector<double>> extractMatrixDoubleFromPyobj(PyObject* ret_from_func);
//std::vector<std::vector<std::string>> extractMatrixStrFromPyobj(PyObject* ret_from_func);

/* ================================
	Get numpy from PyObject* or boost::python::api::object
================================ */
std::vector<int> extractNumpyIntVec(boost::python::api::object& ret_from_func);
std::vector<double> extractNumpyVec(PyObject* ret_from_func);
std::vector<double> extractNumpyVec(boost::python::api::object& ret_from_func);		// Overload
std::vector<std::vector<double>> extractNumpyMat(PyObject* ret_from_func);
std::vector<std::vector<double>> extractNumpyMat(boost::python::api::object& ret_from_func);	// Overload


/* ================================
	Set PyObject* using 1d-vector
================================ */
PyObject* setVectorFromCppInt(const std::vector<int>& vec);
PyObject* setVectorFromCppDouble(const std::vector<double>& vec);
PyObject* setVectorFromCppStr(const std::vector<std::string>& vec);

/* ================================
	Set PyObject* using 2d-vector
		Not implemented: int and std::string
================================ */
//PyObject* setMatrixFromCppInt(const std::vector<std::vector<int>> mat);
PyObject* setMatrixFromCppDouble(const std::vector<std::vector<double>>& mat);
//PyObject* setMatrixFromCppStr(const std::vector<std::vector<std::string>> mat);

/* ================================
	Set PyObject* using unordered_map
================================ */
PyObject* setDictStrInt(const std::unordered_map<std::string, int> kwards);
PyObject* setDictStrDouble(const std::unordered_map<std::string, double> kwards);
PyObject* setDictStrStr(const std::unordered_map<std::string, std::string> kwards);
PyObject* setDict(const std::vector<PyObject*> kwards_int_double_str);

/* ================================
	Set boost::list object
================================ */
boost::python::list setListBoost(const std::vector<double> vec);
boost::python::list setListBoost(const std::vector<std::string> vec);

/* ================================
	Set boost::dict object
================================ */
boost::python::dict setDictBoost(const std::unordered_map<std::string, int> kwards);
boost::python::dict setDictBoost(const std::unordered_map<std::string, double> kwards);	// Overload
boost::python::dict setDictBoost(const std::unordered_map<std::string, std::string> kwards);	// Overload
boost::python::dict setDictBoost(const std::vector<boost::python::numpy::ndarray>& dataset);	// Overload
boost::python::dict setDictBoost(const std::vector<boost::python::dict>& dataset_nids);	// Overload
void setDictBoost(boost::python::dict& pydict, const std::string key, const int val);	// Overload
void setDictBoost(boost::python::dict& pydict, const std::string key, const std::string val);	// Overload
void setDictBoost(boost::python::dict& pydict, const std::string key, const boost::python::dict& data_dict);	// Overload
void setDictBoost(boost::python::dict& pydict, const std::string key, const boost::python::list& data_list);	// Overload

/* ================================
	Set numpy object using vector
================================ */
boost::python::numpy::ndarray setNumpyFromVector(const std::vector<int>& vec);
boost::python::numpy::ndarray setNumpyFromVector(const std::vector<double>& vec);
//boost::python::numpy::ndarray setNumpyFromVector(const std::vector<std::vector<int>>& mat);	// Overload
boost::python::numpy::ndarray setNumpyFromVector(const std::vector<std::vector<double>>& mat);	// Overload


/* ================================
	Set args
	Execute function
================================ */
PyObject* setArgs(const std::vector<PyObject*> args);
PyObject* setFunc(const std::string folder_path, const std::string py_file_name,
				  const std::string py_func_name, PyObject* args);
PyObject* setFunc(const std::string folder_path, const std::string py_file_name,
				  const std::string py_func_name, PyObject* args);
//PyObject* setFunc(const std::string folder_path, const std::string py_file_name,
//				  const std::string py_func_name, const boost::python::dict pydict);	// Overload
boost::python::api::object setFunc(const std::string folder_path, const std::string py_file_name,
								   const std::string py_func_name, const boost::python::dict& pydict);	// Overload
PyObject* setClass(const std::string folder_path, const std::string py_file_name,
				   const std::string py_class_name, PyObject* args);
boost::python::api::object setClass(const std::string folder_path, const std::string py_file_name,
								    const std::string py_class_name, const boost::python::dict& pydict);	// Overload
boost::python::api::object setMethod(const boost::python::api::object& class_obj, const::std::string method_name,
									 const boost::python::dict& pydict);




#endif // PASS_PY_CPP_H