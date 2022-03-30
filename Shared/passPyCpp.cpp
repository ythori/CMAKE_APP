
#include "passPyCpp.h"

//namespace py = boost::python;
//namespace np = boost::python::numpy;


void pyInitialize(){ Py_Initialize(); }
void pyFinalize(){ Py_Finalize(); }

int extractInt(boost::python::api::object pyobj) {
    return boost::python::extract<int>(pyobj);
}

double extractDouble(boost::python::api::object pyobj) {
    return boost::python::extract<double>(pyobj);
}

std::vector<int> extractVectorIntFromPyobj(PyObject* ret_from_func) {
    std::vector<int> ret_vec;
    long one_data;
    if (ret_from_func && PyList_Check(ret_from_func)) {
        Py_ssize_t length = PyList_Size(ret_from_func);
        ret_vec.resize(length);
        for (int i = 0; i < length; ++i) {
            PyObject* c = PyList_GetItem(ret_from_func, i);
            PyArg_Parse(c, "i", &one_data);
            ret_vec[i] = (int)one_data;
        }
        return ret_vec;
    }
    else {
        ret_vec = { 0 };
        return ret_vec;
    }
}

std::vector<double> extractVectorDoubleFromPyobj(PyObject* ret_from_func) {
    std::vector<double> ret_vec;
    float one_data;
    if (ret_from_func && PyList_Check(ret_from_func)) {
        Py_ssize_t length = PyList_Size(ret_from_func);
        ret_vec.resize(length);
        for (int i = 0; i < length; ++i) {
            PyObject* c = PyList_GetItem(ret_from_func, i);
            PyArg_Parse(c, "f", &one_data);
            ret_vec[i] = (double)one_data;
        }
        return ret_vec;
    }
    else {
        ret_vec = { 0 };
        return ret_vec;
    }
}

std::vector<std::string> extractVectorStrFromPyobj(PyObject* ret_from_func) {
    std::vector<std::string> ret_vec;
    char* one_data;
    if (ret_from_func && PyList_Check(ret_from_func)) {
        Py_ssize_t length = PyList_Size(ret_from_func);
        ret_vec.resize(length);
        for (int i = 0; i < length; ++i) {
            PyObject* c = PyList_GetItem(ret_from_func, i);
            PyArg_Parse(c, "s", &one_data);
            std::string strs(one_data);
            ret_vec[i] = strs;
        }
        return ret_vec;
    }
    else {
        ret_vec = { 0 };
        return ret_vec;
    }
}



std::vector<std::vector<double>> extractMatrixDoubleFromPyobj(PyObject* ret_from_func) {
    std::vector<std::vector<double>> ret_mat;
    float one_data;
    if (ret_from_func && PyList_Check(ret_from_func)) {
        Py_ssize_t rows = PyList_Size(ret_from_func);
        ret_mat.resize(rows);
        for (int i = 0; i < rows; ++i) {
            PyObject* vec = PyList_GetItem(ret_from_func, i);
            Py_ssize_t cols = PyList_Size(vec);
            std::vector<double> t_vec(cols);
            for (int j = 0; j < cols; ++j) {
                PyObject* c = PyList_GetItem(vec, j);
                PyArg_Parse(c, "f", &one_data);
                t_vec[j] = (double)one_data;
            }
            ret_mat[i] = t_vec;
        }
        return ret_mat;
    }
    else {
        std::vector<double> t_vec = { 0 };
        ret_mat.emplace_back(t_vec);
        return ret_mat;
    }
}

std::vector<int> extractNumpyIntVec(boost::python::api::object& ret_from_func) {
    std::vector<int> ret_mat;

    boost::python::numpy::initialize();

    auto result_array = boost::python::extract<boost::python::numpy::ndarray>(ret_from_func);
    const boost::python::numpy::ndarray& ret = result_array();

    if ((ret.get_nd() == 1) && (ret.get_dtype() == boost::python::numpy::dtype::get_builtin<int>())) {
        auto shape = ret.get_shape();
        auto strides = ret.get_strides();
        ret_mat.resize(shape[0]);
        for (int i = 0; i < shape[0]; ++i) ret_mat[i] = *reinterpret_cast<int*>(ret.get_data() + i * strides[0]);
    }
    else {
        ret_mat.emplace_back(0);
    }
    return ret_mat;
}

std::vector<double> extractNumpyVec(PyObject* ret_from_func) {
    std::vector<double> ret_mat;

    boost::python::numpy::initialize();

    auto result_array = boost::python::extract<boost::python::numpy::ndarray>(ret_from_func);
    const boost::python::numpy::ndarray& ret = result_array();

    if ((ret.get_nd() == 1) && (ret.get_dtype() == boost::python::numpy::dtype::get_builtin<double>())) {
        auto shape = ret.get_shape();
        auto strides = ret.get_strides();
        ret_mat.resize(shape[0]);
        for (int i = 0; i < shape[0]; ++i) ret_mat[i] = *reinterpret_cast<double*>(ret.get_data() + i * strides[0]);
    }
    else {
        ret_mat.emplace_back(0);
    }
    return ret_mat;
}

std::vector<double> extractNumpyVec(boost::python::api::object& ret_from_func) {
    std::vector<double> ret_mat;

    boost::python::numpy::initialize();

    auto result_array = boost::python::extract<boost::python::numpy::ndarray>(ret_from_func);
    const boost::python::numpy::ndarray& ret = result_array();

    if ((ret.get_nd() == 1) && (ret.get_dtype() == boost::python::numpy::dtype::get_builtin<double>())) {
        auto shape = ret.get_shape();
        auto strides = ret.get_strides();
        ret_mat.resize(shape[0]);
        for (int i = 0; i < shape[0]; ++i) ret_mat[i] = *reinterpret_cast<double*>(ret.get_data() + i * strides[0]);
    }
    else {
        ret_mat.emplace_back(0);
    }
    return ret_mat;
}

std::vector<std::vector<double>> extractNumpyMat(PyObject* ret_from_func) {
    std::vector<std::vector<double>> ret_mat;

    boost::python::numpy::initialize();

    auto result_array = boost::python::extract<boost::python::numpy::ndarray>(ret_from_func);
    const boost::python::numpy::ndarray& ret = result_array();

    if ((ret.get_nd() == 2) && (ret.get_dtype() == boost::python::numpy::dtype::get_builtin<double>())) {
        auto shape = ret.get_shape();
        auto strides = ret.get_strides();
        ret_mat.resize(shape[0], std::vector<double>(shape[1]));
        for (int i = 0; i < shape[0]; ++i) {
            for (int j = 0; j < shape[1]; ++j) {
                ret_mat[i][j] = *reinterpret_cast<double*>(ret.get_data() + i * strides[0] + j * strides[1]);
            }
        }
    }
    else {
        std::vector<double> t_vec = { 0 };
        ret_mat.emplace_back(t_vec);
    }
    return ret_mat;
}


std::vector<std::vector<double>> extractNumpyMat(boost::python::api::object& ret_from_func) {
    std::vector<std::vector<double>> ret_mat;

    boost::python::numpy::initialize();

    auto result_array = boost::python::extract<boost::python::numpy::ndarray>(ret_from_func);
    const boost::python::numpy::ndarray& ret = result_array();

    if ((ret.get_nd() == 2) && (ret.get_dtype() == boost::python::numpy::dtype::get_builtin<double>())) {
        auto shape = ret.get_shape();
        auto strides = ret.get_strides();
        ret_mat.resize(shape[0], std::vector<double>(shape[1]));
        for (int i = 0; i < shape[0]; ++i) {
            for (int j = 0; j < shape[1]; ++j) {
                ret_mat[i][j] = *reinterpret_cast<double*>(ret.get_data() + i * strides[0] + j * strides[1]);
            }
        }
    }
    else {
        std::vector<double> t_vec = { 0 };
        ret_mat.emplace_back(t_vec);
    }
    return ret_mat;
}

PyObject* setVectorFromCppInt(const std::vector<int>& vec) {
    PyObject* vec_list = PyList_New(vec.size());
    for (int i = 0; i < vec.size(); ++i) {
        PyObject* temp = PyLong_FromLong((long)vec[i]);
        PyList_SetItem(vec_list, i, temp);
    }
    return vec_list;
}

PyObject* setVectorFromCppDouble(const std::vector<double>& vec) {
    PyObject* vec_list = PyList_New(vec.size());
    for (int i = 0; i < vec.size(); ++i) {
        //std::cout << vec[i] << std::endl;
        PyObject* temp = PyFloat_FromDouble(vec[i]);
        PyList_SetItem(vec_list, i, temp);
    }
    return vec_list;
}


PyObject* setVectorFromCppStr(const std::vector<std::string>& vec) {
    PyObject* vec_list = PyList_New(vec.size());
    for (int i = 0; i < vec.size(); ++i) {
        PyObject* temp = PyUnicode_FromString(vec[i].c_str());
        PyList_SetItem(vec_list, i, temp);
    }
    return vec_list;
}


PyObject* setMatrixFromCppDouble(const std::vector<std::vector<double>>& mat) {
    PyObject* mat_list = PyList_New(mat.size());
    for (int i = 0; i < mat.size(); ++i) {
        //if (i%100==0) std::cout << i << std::endl;
        std::vector<double> t_vec = mat[i];
        PyList_SetItem(mat_list, i, setVectorFromCppDouble(t_vec));
    }
    return mat_list;
}


PyObject* setDictStrInt(const std::unordered_map<std::string, int> kwards) {
    PyObject* dic = PyDict_New();
    for (auto itr = kwards.begin(); itr != kwards.end(); ++itr) {
        std::string key = itr->first;
        int val = itr->second;
        //std::cout << key << " " << val << std::endl;
        PyDict_SetItemString(dic, key.c_str(), PyLong_FromLong(val));
    }
    //std::cout << PyDict_Size(dic) << std::endl;
    return dic;
}

PyObject* setDictStrDouble(const std::unordered_map<std::string, double> kwards) {
    PyObject* dic = PyDict_New();
    for (auto itr = kwards.begin(); itr != kwards.end(); ++itr) {
        std::string key = itr->first;
        double val = itr->second;
        PyDict_SetItemString(dic, key.c_str(), PyFloat_FromDouble(val));
    }
    return dic;
}

PyObject* setDictStrStr(const std::unordered_map<std::string, std::string> kwards) {
    PyObject* dic = PyDict_New();
    for (auto itr = kwards.begin(); itr != kwards.end(); ++itr) {
        std::string key = itr->first;
        std::string val = itr->second;
        PyDict_SetItemString(dic, key.c_str(), PyUnicode_FromString(val.c_str()));
    }
    return dic;
}

PyObject* setDict(const std::vector<PyObject*> kwards_int_double_str) {
    PyObject* dic = PyDict_New();
    if (PyDict_Size(kwards_int_double_str[0]) != 0) PyDict_SetItemString(dic, "params_i", kwards_int_double_str[0]);
    if (PyDict_Size(kwards_int_double_str[1]) != 0) PyDict_SetItemString(dic, "params_d", kwards_int_double_str[1]);
    if (PyDict_Size(kwards_int_double_str[2]) != 0) PyDict_SetItemString(dic, "params_s", kwards_int_double_str[2]);
    return dic;
}


boost::python::list setListBoost(const std::vector<double> vec) {
    boost::python::list pylist;
    for (auto s : vec) pylist.append(s);
    return pylist;
}

boost::python::list setListBoost(const std::vector<std::string> vec) {
    boost::python::list pylist;
    for (auto s : vec) pylist.append(s);
    return pylist;
}


boost::python::dict setDictBoost(const std::unordered_map<std::string, int> kwards) {
    boost::python::dict pydict;
    std::unordered_map<std::string, int>::const_iterator it;
    for (it = kwards.begin(); it != kwards.end(); it++)
        pydict[it->first] = it->second;
    return pydict;
}

boost::python::dict setDictBoost(const std::unordered_map<std::string, double> kwards) {
    boost::python::dict pydict;
    std::unordered_map<std::string, double>::const_iterator it;
    for (it = kwards.begin(); it != kwards.end(); it++)
        pydict[it->first] = it->second;
    return pydict;
}

boost::python::dict setDictBoost(const std::unordered_map<std::string, std::string> kwards) {
    boost::python::dict pydict;
    std::unordered_map<std::string, std::string>::const_iterator it;
    for (it = kwards.begin(); it != kwards.end(); it++)
        pydict[it->first] = it->second;
    return pydict;
}

boost::python::dict setDictBoost(const std::vector<boost::python::numpy::ndarray>& dataset) {
    boost::python::dict pydict;
    int n_data = dataset.size();
    for (int i = 0; i < n_data; ++i) {
        pydict["data" + std::to_string(i + 1)] = dataset[i];
    }
    return pydict;
}

boost::python::dict setDictBoost(const std::vector<boost::python::dict>& dataset_nids) {
    boost::python::dict pydict;
    //for (auto data: dataset_nids) std::cout << boost::python::len(data) << std::endl;
    if (boost::python::len(dataset_nids[0]) != 0) pydict["data"] = dataset_nids[0];
    if (boost::python::len(dataset_nids[1]) != 0) pydict["params_i"] = dataset_nids[1];
    if (boost::python::len(dataset_nids[2]) != 0) pydict["params_d"] = dataset_nids[2];
    if (boost::python::len(dataset_nids[3]) != 0) pydict["params_s"] = dataset_nids[3];
    return pydict;
}

void setDictBoost(boost::python::dict& pydict, const std::string key, const int val) {
    pydict[key.c_str()] = val;
}

void setDictBoost(boost::python::dict& pydict, const std::string key, const std::string val) {
    pydict[key.c_str()] = val.c_str();
}

void setDictBoost(boost::python::dict& pydict, const std::string key, const boost::python::dict& data_dict) {
    pydict[key.c_str()] = data_dict;
}

void setDictBoost(boost::python::dict& pydict, const std::string key, const boost::python::list& data_list) {
    pydict[key.c_str()] = data_list;
}


boost::python::numpy::ndarray setNumpyFromVector(const std::vector<int>& vec) {
//void setNumpyFromVector(const std::vector<std::vector<double>>& mat) {
    boost::python::numpy::initialize();
    boost::python::tuple shapeA = boost::python::make_tuple(vec.size());
    boost::python::numpy::ndarray arr = boost::python::numpy::zeros(shapeA, boost::python::numpy::dtype::get_builtin<int>());
    for (int i = 0; i != vec.size(); ++i) arr[i] = vec[i];
    return arr;
}

boost::python::numpy::ndarray setNumpyFromVector(const std::vector<double>& vec) {
    //void setNumpyFromVector(const std::vector<std::vector<double>>& mat) {
    boost::python::numpy::initialize();
    boost::python::tuple shapeA = boost::python::make_tuple(vec.size());
    boost::python::numpy::ndarray arr = boost::python::numpy::zeros(shapeA, boost::python::numpy::dtype::get_builtin<double>());
    for (int i = 0; i != vec.size(); ++i) arr[i] = vec[i];
    return arr;
}

//boost::python::numpy::ndarray setNumpyFromVector(const std::vector<std::vector<int>>& mat) {
//    //void setNumpyFromVector(const std::vector<std::vector<double>>& mat) {
//    boost::python::numpy::initialize();
//    int n_row = mat.size();
//    int n_col = mat[0].size();
//    //std::cout << n_row << " " << n_col << std::endl;
//    boost::python::tuple shapeA = boost::python::make_tuple(n_row, n_col);
//    boost::python::numpy::ndarray arr = boost::python::numpy::zeros(shapeA, boost::python::numpy::dtype::get_builtin<int>());
//    for (int i = 0; i != n_row; ++i) {
//        //if (i % 100 == 0) std::cout << i << std::endl;
//        for (int j = 0; j != n_col; ++j) {
//            arr[i][j] = mat[i][j];
//        }
//    }
//    return arr;
//}

boost::python::numpy::ndarray setNumpyFromVector(const std::vector<std::vector<double>>& mat) {
    //void setNumpyFromVector(const std::vector<std::vector<double>>& mat) {
    boost::python::numpy::initialize();
    int n_row = mat.size();
    int n_col = mat[0].size();
    //std::cout << n_row << " " << n_col << std::endl;
    boost::python::tuple shapeA = boost::python::make_tuple(n_row, n_col);
    boost::python::numpy::ndarray arr = boost::python::numpy::zeros(shapeA, boost::python::numpy::dtype::get_builtin<double>());
    for (int i = 0; i != n_row; ++i) {
        //if (i % 100 == 0) std::cout << i << std::endl;
        for (int j = 0; j != n_col; ++j) {
            arr[i][j] = mat[i][j];
        }
    }
    return arr;
}


PyObject* setArgs(const std::vector<PyObject*> args) {
    PyObject* pArgs = PyTuple_New(args.size());
    for (int i = 0; i < args.size(); ++i) PyTuple_SetItem(pArgs, i, args[i]);
    return pArgs;
}


PyObject* setFunc(const std::string folder_path, const std::string py_file_name,
                  const std::string py_func_name, PyObject* args) {
    PyObject* sys = PyImport_ImportModule("sys");       // import sys
    PyObject* path = PyObject_GetAttrString(sys, "path");   // get sys.path
    PyList_Append(path, PyUnicode_FromString(folder_path.c_str()));

    PyObject* pythonName = PyUnicode_FromString(py_file_name.c_str());    // Python can deal with only unicode
    PyObject* pythonModule = PyImport_Import(pythonName);   // import file
    Py_DECREF(pythonName);

    PyObject* pFunc = PyObject_GetAttrString(pythonModule, py_func_name.c_str());
    //Py_XDECREF(sys);
    //Py_XDECREF(path);
    //Py_XDECREF(pythonModule);

    return PyObject_CallObject(pFunc, args);
}


boost::python::api::object setFunc(const std::string folder_path, const std::string py_file_name,
                                   const std::string py_func_name, const boost::python::dict& pydict) {
    boost::python::numpy::initialize();
    auto main_ns = boost::python::import("__main__").attr("__dict__");      // Name space

    std::ifstream ifs(folder_path + py_file_name + ".py");
    std::string script((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());    // Full text of .py

    //std::cout << script << std::endl;
    boost::python::exec(script.c_str(), main_ns);
    //std::cout << py_func_name << std::endl;
    auto func = main_ns[py_func_name];
    auto pyresult = func(pydict);

    return pyresult;
}

PyObject* setClass(const std::string folder_path, const std::string py_file_name,
                   const std::string py_class_name, PyObject* args) {
    // Init args = NULL
    PyObject* sys = PyImport_ImportModule("sys");       // import sys
    PyObject* path = PyObject_GetAttrString(sys, "path");   // get sys.path
    PyList_Append(path, PyUnicode_FromString(folder_path.c_str()));

    PyObject* pyModuleName = PyUnicode_FromString(py_file_name.c_str());
    PyObject* pyModule = PyImport_Import(pyModuleName);
    Py_DECREF(pyModuleName);

    PyObject* pyDict = PyModule_GetDict(pyModule);
    PyObject* pyClass = PyDict_GetItemString(pyDict, py_class_name.c_str());
    return PyObject_CallObject(pyClass, args);
}

boost::python::api::object setClass(const std::string folder_path, const std::string py_file_name,
                                    const std::string py_class_name, const boost::python::dict& pydict) {
    boost::python::numpy::initialize();
    auto main_ns = boost::python::import("__main__").attr("__dict__");      // Name space

    std::ifstream ifs(folder_path + py_file_name + ".py");
    std::string script((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());    // Full text of .py

    //std::cout << script << std::endl;
    //std::cout << py_class_name << std::endl;
    boost::python::exec(script.c_str(), main_ns);
    auto func = main_ns[py_class_name];
    auto pyresult = func(pydict);

    return pyresult;
}

boost::python::api::object setMethod(const boost::python::api::object& class_obj, const::std::string method_name,
                                     const boost::python::dict& pydict) {
    auto pymethod = class_obj.attr(method_name.c_str());
    auto pyresult = pymethod(pydict);
    return pyresult;
}

