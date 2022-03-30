#include "pch.h"
#include "_moduleTest.h"


void sharedTest() {
    //opTest();
    //std::cout << std::endl;
    //DPTest();
    //std::cout << std::endl;
    //ConstantsTest();
    //std::cout << std::endl;
    //timerTest();
    //std::cout << std::endl;
    //vecTest();
    //std::cout << std::endl;
    //saveTest();
    //std::cout << std::endl;
    EigenTest();
    //std::cout << std::endl;
    //boostTest();
    //std::cout << std::endl;
    //MPTest();
    //std::cout << std::endl;

    //Py_Initialize();
    //pyTest(true, false);
    //std::cout << std::endl;
    //pyNumpyTest(true, false);
    //std::cout << std::endl;
    //matplotlibTest(true, false);
    //Py_Finalize();
}

void opTest() {
    /* execution test */
    std::cout << "Hello World!\n";
}

void DPTest() {
    echo("DataProc test");
    echo(1);
    echoes(1, 2, 3);
}

void ConstantsTest() {
    /* constants */
    echo("Constants test");
    DataAddress DA;
    echo(DA.getResultFolder());
    echo(DA.getSaveFolderPath());
    //DA.setSaveFolder("test");
    echo(DA.getSaveFolderPath());
}

void timerTest() {
    echo("Timer test");
    Timer timer;
    timer.elapsed();
    timer.show();
    timer.restart();
    timer.elapsed();
    timer.show();
}

void vecTest() {
    /* vec_op test */
    echo("Vec_op test");
    std::vector<double> vec1 = { 1, 2, 3 };
    std::vector<double> vec2 = { 3, 4, 5 };
    auto vec = vec1 + vec2;

    /* echo test */
    for (auto x : vec) std::cout << x << std::endl;
    echo_v1(vec1);

    // Sort test
    std::unordered_map<int, std::vector<std::tuple<int, Eigen::MatrixXd, int, int>>> ret;
    std::vector<std::tuple<int, Eigen::MatrixXd, int, int>> temp1;
    temp1.emplace_back(std::make_tuple(0, Eigen::MatrixXd::Constant(2, 2, 0), 5, 9));
    temp1.emplace_back(std::make_tuple(1, Eigen::MatrixXd::Constant(2, 2, 1), 13, 15));
    temp1.emplace_back(std::make_tuple(2, Eigen::MatrixXd::Constant(2, 2, 2), 0, 4));
    temp1.emplace_back(std::make_tuple(3, Eigen::MatrixXd::Constant(2, 2, 3), 10, 12));
    ret[0] = temp1;
    std::vector<std::tuple<int, Eigen::MatrixXd, int, int>> temp2;
    temp2.emplace_back(std::make_tuple(0, Eigen::MatrixXd::Constant(2, 2, 10), 10, 13));
    temp2.emplace_back(std::make_tuple(1, Eigen::MatrixXd::Constant(2, 2, 11), 0, 5));
    temp2.emplace_back(std::make_tuple(2, Eigen::MatrixXd::Constant(2, 2, 12), 14, 20));
    temp2.emplace_back(std::make_tuple(3, Eigen::MatrixXd::Constant(2, 2, 13), 6, 9));
    ret[1] = temp2;

    echo("pre_sort");
    for (int i = 0; i < ret.size(); ++i) {
        echoes("idx_dataset:", i);
        for (auto v : ret.at(i)) {
            echoes("class:", std::get<0>(v), "start:", std::get<2>(v), "end:", std::get<3>(v));
        }
        echo("\n");
    }

    for (int i = 0; i < ret.size(); ++i) {
        sort(ret[i].begin(), ret[i].end(),
             [](auto const& lhs, auto const& rhs) {
                 return std::get<2>(lhs) < std::get<2>(rhs);
             });
    }


    echo("aft_sort");
    for (int i = 0; i < ret.size(); ++i) {
        echoes("idx_dataset:", i);
        for (auto v : ret.at(i)) {
            echoes("class:", std::get<0>(v), "start:", std::get<2>(v), "end:", std::get<3>(v));
        }
        echo("\n");
    }


}

void saveTest() {
    /* save txt test */
    echo("Save test");
    std::vector<std::vector<double>> vvec = { {1, 2, 3}, {3, 4, 6} };
    DataAddress DA;
    DA.resetSaveFolder();
    DA.setSaveFolder("test_vector_write");
    //std::string save_folder = "C:/Users/brume/Documents/Project/DB/_test/";
    //std::string fname = save_folder + "testtxt.txt";
    auto fname = DA.getSavePath("testtxt_2_3.txt");
    saveTxt2DVec(vvec, fname);
}

void EigenTest() {
    /* Eigen test */
    echo("Eigen test");
    int n_data = 5;
    int n_class = 3;
    Eigen::MatrixXd mat1(n_data, n_class), mat2(n_class, n_class);
    mat1 << 1, 2, 3,
        4, 5, 6,
        7, 8, 9,
        10, 11, 12,
        13, 14, 15;
    mat2 << 10, 100, 1000,
        -10, -100, -1000,
        0.1, 0.5, -0.1;
    //echo(mat1);

    // c = 1: mat2 = [100, -100, 0.5] -> +mat1 = [101, -98, 3.5], [104, -95, 6.5], ...
    //echo(mat1.rowwise().sum()); // return [6, 15, 24, 33, 42]^t
    //Eigen::VectorXd temp = mat2.col(0);
    //auto t = mat1.colwise().sum();
    //echo(t);
    //echo(mat1.rowwise() + mat2.col(1).transpose());
    //mat1.rowwise() += mat2.col(1).transpose();
    //echo(mat1);

    //Eigen::VectorXd vec(n_class);
    //vec << 1, 10, 100;
    //echo(vec);
    //for (int c = 0; c < mat1.cols(); ++c) mat1.col(c).array() *= vec(c);
    //echo(mat1);

    //echo(mat1);
    Eigen::MatrixXd trans_mat = mat1.transpose();
    Eigen::Map<const Eigen::VectorXd>mat3(trans_mat.transpose().data(), trans_mat.size());
    echo(mat3);





}

void boostTest() {
    /* boost test */
    echo("boost test");
    std::cout << "boostバージョン:" << BOOST_VERSION << std::endl;
    std::cout << "boostライブラリバージョン:" << BOOST_LIB_VERSION << std::endl;

    /* gsl test */
    double x = 5.0;
    double y = gsl_sf_bessel_J0(x);
    echoes("J0(%g) = %.18e\n", x, y);
}

void MPTest() {
    /* mathproc test */
    echo("MathProc test");
    /* random */
    echoes(frand(), irand(5, 10), decimalToHexadecimal(100), hexadecimalToDecimal("1E240"));

    /* logsumexp */
    std::vector<double> lvec = { 1.09861229, 1.60943791, 2.30258509 };  // ln({3, 5, 10})
    auto elvec = convertSVec2EVec(lvec);
    echoes(logsumexp(lvec), logsumexpEigenSum(elvec));
    echo(logsumexpEigen(elvec));    // ln({3/18, 5/18, 10/18}) = {-1.79175947, -1.28093385, -0.58778666}

    /* normalize */
    Eigen::VectorXd nvec = Eigen::VectorXd::Zero(3);
    nvec << 3, 5, 10;
    echo(normalizeEigen(nvec));
    Eigen::MatrixXd nmat(2, 3);
    nmat << 1, 2, 3,
        4, 5, 6;
    echo(normalizeMatEigen(nmat, 0));
    echo(normalizeMatEigen(nmat, 1));

    /* cov, inv */
    Eigen::MatrixXd cmat(7, 2);
    cmat << 10, 10,
        5, 2,
        2, 8,
        4, 3,
        9, 7,
        3, 4,
        2, 1;
    auto cov_mat = calCov(cmat);    // 9.14286, 5.71429, 5.71429, 9.71429
    echo(cov_mat);
    echo(invMat(cov_mat));  // 0.172965, -0.101744, -0.101744, 0.162791
}

void pyTest(const bool py_initialize, const bool py_finalize) {

    if (py_initialize == true) pyInitialize();
    //Py_Initialize();

    std::string folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/Shared/";
    std::string file_name = "python_code_sample";
    std::string func_name1 = "sample_test";
    std::string func_name2 = "sample_test2";
    std::string func_name3 = "sample_test3";
    std::string func_name_actual = "sample_test_actual";

    echo("int_vector");
    std::vector<int> int_vec1 = { 0, 2, 4, 6 };
    std::vector<int> int_vec2 = { 10, 12, 14, 16 };

    std::vector<PyObject*> args;
    auto p_int_list1_obj = setVectorFromCppInt(int_vec1);
    auto p_int_list2_obj = setVectorFromCppInt(int_vec2);
    args.emplace_back(p_int_list1_obj);
    args.emplace_back(p_int_list2_obj);
    auto pArgs = setArgs(args);

    auto result_pyobj = setFunc(folder_path, file_name, func_name2, pArgs);
    auto result_vec_int = extractVectorIntFromPyobj(result_pyobj);
    echo_v1(result_vec_int);

    std::cout << std::endl;
    echo("double_vector");
    args.clear();
    std::vector<double> double_vec = { 0, 0.5, 1.5 };
    auto p_double_list_obj = setVectorFromCppDouble(double_vec);
    args.emplace_back(p_double_list_obj);
    pArgs = setArgs(args);
    result_pyobj = setFunc(folder_path, file_name, func_name2, pArgs);
    auto result_vec_double = extractVectorDoubleFromPyobj(result_pyobj);
    echo_v1(result_vec_double);

    std::cout << std::endl;
    echo("string_vector");
    args.clear();
    std::vector<std::string> str_vec = { "a", "b", "c" };
    auto p_str_list_obj = setVectorFromCppStr(str_vec);
    args.emplace_back(p_str_list_obj);
    pArgs = setArgs(args);
    result_pyobj = setFunc(folder_path, file_name, func_name3, pArgs);
    auto result_vec_str = extractVectorStrFromPyobj(result_pyobj);
    echo_v1(result_vec_str);

    std::cout << std::endl;
    echo("actual");
    args.clear();
    std::vector<std::vector<double>> mat;
    std::vector<double> t_vec = { 0, 1.1, 2.2 };
    for (int i = 0; i < 5; ++i) {
        mat.emplace_back(t_vec * (double)i);
    }
    auto p_list_obj = setMatrixFromCppDouble(mat);
    args.emplace_back(p_list_obj);  // Assume spectrogram
    args.emplace_back(p_double_list_obj);   // Assume ticks
    args.emplace_back(PyLong_FromLong(1));  // Assume property
    args.emplace_back(PyUnicode_FromString("wav_file_address"));  // Assume property
    pArgs = setArgs(args);
    result_pyobj = setFunc(folder_path, file_name, func_name_actual, pArgs);
    result_vec_double = extractVectorDoubleFromPyobj(result_pyobj);
    echo_v1(result_vec_double);

    //Py_XDECREF(p_list_obj);
    //Py_XDECREF(p_int_list1_obj);
    //Py_XDECREF(p_int_list2_obj);
    //Py_XDECREF(p_double_list_obj);
    //Py_XDECREF(p_str_list_obj);
    //Py_XDECREF(result_pyobj);
    //for (auto arg: args) Py_XDECREF(arg);

    if (py_finalize == true) pyFinalize();
    //Py_Finalize();
}

void pyNumpyTest(const bool py_initialize, const bool py_finalize){
    echo("numpy");

    if (py_initialize == true) pyInitialize();
    //Py_Initialize();

    std::string folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/Shared/";
    std::string file_name = "python_code_sample";
    std::string func_name = "sample_test_numpy1";

    std::unordered_map<std::string, int> mpi;
    std::unordered_map<std::string, double> mpd;
    std::unordered_map<std::string, std::string> mps;
    mpi["a"] = 1;
    mpi["b"] = 2;
    //mpd["a"] = 1.5;
    //mpd["b"] = 2.2;
    mps["a"] = "aaa";
    mps["b"] = "bbb";

    /* ================================
        Using PyList object
    ================================ */
    echo("Using PyList");
    std::vector<PyObject*> args;
    args.clear();
    std::vector<std::vector<double>> mat;
    std::vector<double> t_vec = {0, 1.1, 2.2};
    for (int i = 0; i < 5; ++i) {
        mat.emplace_back(t_vec * (double)i);
    }
    auto p_list_obj = setMatrixFromCppDouble(mat);
    args.emplace_back(p_list_obj);  // Assume spectrogram
    args.emplace_back(PyLong_FromLong(1));  // Assume property
    args.emplace_back(PyUnicode_FromString("wav_file_address"));  // Assume property

    std::vector<PyObject*> params;
    auto dic_i = setDictStrInt(mpi);
    auto dic_d = setDictStrDouble(mpd);
    auto dic_s = setDictStrStr(mps);
    params = { dic_i, dic_d, dic_s };
    auto arg_params = setDict(params);
    args.emplace_back(arg_params);

    auto pArgs = setArgs(args);
    auto result_pyobj = setFunc(folder_path, file_name, func_name, pArgs);

    // If return list(2d)
    auto result_mat_double = extractNumpyMat(result_pyobj);
    echo_v2(result_mat_double);

    std::cout << std::endl;


    /* ================================
        Using Numpy object
    ================================ */
    echo("Using Numpy");
    func_name = "sample_test_numpy2";
    int n_row = 20;
    int n_col = 30;
    std::vector<std::vector<double>> data1(n_row, std::vector<double>(n_col));
    std::vector<std::vector<double>> data2(n_row, std::vector<double>(n_col));
    for (int i = 0; i < n_row; ++i) {
        for (int j = 0; j < n_col; ++j) {
            data1[i][j] = i + j;
            data2[i][j] = i - j;
        }
    }
    echo(data1[int(n_row/2)][int(n_col/2)]);
    auto data1_np = setNumpyFromVector(data1);
    auto data2_np = setNumpyFromVector(data1);
    std::vector<boost::python::numpy::ndarray> pydict_data = { data1_np, data2_np };
    auto pydict_numpy = setDictBoost(pydict_data);
    //auto pydict_numpy = setDictBoost(mpd);

    auto pydict_int = setDictBoost(mpi);
    auto pydict_double = setDictBoost(mpd);
    auto pydict_str = setDictBoost(mps);

    std::vector<boost::python::dict> pydict_nids = { pydict_numpy, pydict_int, pydict_double, pydict_str };
    auto pydict_args = setDictBoost(pydict_nids);
    auto pyresult = setFunc(folder_path, file_name, func_name, pydict_args);

    //result_npobj.
    //auto result_vec_double = extractNumpyVecFromPyobj(result_pyobj);
    //echo_v1(result_vec_double);
    auto result_mat_double_boost = extractNumpyMat(pyresult);
    echo_v2(result_mat_double_boost);


    if (py_finalize == true) pyFinalize();
    //Py_Finalize();
}

void matplotlibTest(const bool py_initialize, const bool py_finalize) {
    /* matplotlibcpp test */
    echo("Matplotlib test");

    if (py_initialize == true) pyInitialize();

    DataAddress DA;
    DA.resetSaveFolder();
    DA.setSaveFolder("test_matplotlib_write");
    auto savefolder_path = DA.getSaveFolderPath();

    std::vector<double> toy11, toy12, toy21;
    for (int i = 0; i < 50; ++i) {
        toy11.emplace_back(i + 0.5);
        toy12.emplace_back(i - 0.5);
        toy21.emplace_back(i * 0.5);
    }
    std::string plot_type = "line";

    // Set one figure data (not one plot data)
    PlotData PD1;
    std::vector<std::shared_ptr<PlotData>> data_vec;
    PD1.setData(toy11, plot_type);
    PD1.setData(toy12, plot_type);
    //PD1.showDataInformation();
    data_vec.emplace_back(std::make_shared<PlotData>(PD1));

    PlotData PD2;
    PD2.setData(toy21, plot_type);
    data_vec.emplace_back(std::make_shared<PlotData>(PD2));

    MatplotlibCpp PLT(data_vec);
    PLT.setSaveFolderPath(savefolder_path);
    PLT.setSaveFileName("test3.png");
    PLT.setShowFlag(true);
    PLT.showInterface();
    PLT.execute();





    //PyObject* pyInstance = setClass(folder_path, file_name, class_name);
    //std::string func_name = 
//
//    //pyResult = PyObject_CallMethodObjArgs(pyInstance, );

    //Py_Finalize();
    if (py_finalize == true) pyFinalize();


}
