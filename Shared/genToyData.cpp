
#include "pch.h"
#include "genToyData.h"
#include "DataProc.h"
#include "Constants.h"
#include "MathProc.h"

void genMultiDimSeq() {
    DataAddress DA;
    DA.resetSaveFolder();		// set result folder (just in case)
    std::string misc = DA.getMiscFolder() + "Misc_samples/";
    std::string save_folder = misc;
    std::string save_file_path_data_1 = misc + "data_mdim_1.txt";
    std::string save_file_path_dc_1 = misc + "data_mdim_dc_1.txt";
    std::string save_file_path_data_2 = misc + "data_mdim_2.txt";
    std::string save_file_path_dc_2 = misc + "data_mdim_dc_2.txt";


    std::vector<std::pair<int, int>> dur_class1, dur_class2;
    dur_class1.emplace_back(std::make_pair(0, 11));
    dur_class1.emplace_back(std::make_pair(1, 9));
    dur_class1.emplace_back(std::make_pair(2, 12));
    dur_class1.emplace_back(std::make_pair(3, 8));
    dur_class1.emplace_back(std::make_pair(4, 10));

    int n_length2 = 0;
    for (auto dc1 : dur_class1) {
        int t_dur = dc1.second + irand(-2, 2);
        dur_class2.emplace_back(std::make_pair(dc1.first, t_dur));
        n_length2 += t_dur;
    }

    std::unordered_map<int, std::vector<int>> available_val_dic;    // {class: idx_ones}
    available_val_dic[0] = { 9, 19, 29 };
    available_val_dic[1] = { 19, 29, 39 };
    available_val_dic[2] = { 39, 49, 59 };
    available_val_dic[3] = { 59, 69, 79 };
    available_val_dic[4] = { 79, 9, 19 };

    bool is_deviation = false;

    Eigen::MatrixXd mat1 = Eigen::MatrixXd::Zero(50, 88);   // [time, dim]
    Eigen::MatrixXd mat2 = Eigen::MatrixXd::Zero(n_length2, 88);
    std::vector<std::vector<int>> dur_class_data1, dur_class_data2; // [dur, class, idx_val1, idx_val2, idx_val3]

    int start_t_data1 = 0;
    int start_t_data2 = 0;
    for (int c = 0; c < dur_class1.size(); ++c) {
        int data1_class = c;    // [0, 1, 2, 3, 4]
        //int data2_class = (c + 2) % 5;  // [2, 3, 4, 0, 1]
        int data2_class = c;  // [0, 1, 2, 3, 4]

        int data1_duration = 10;
        int data2_duration = 10;
        if (is_deviation == false) {
            data1_duration = dur_class1[data1_class].second;
            data2_duration = dur_class2[data2_class].second;
        }

        auto val_idx_1 = available_val_dic.at(data1_class);
        auto val_idx_2 = available_val_dic.at(data2_class);

        for (auto idx : val_idx_1) mat1.block(start_t_data1, idx, data1_duration, 1).array() = 1;
        for (auto idx : val_idx_2) mat2.block(start_t_data2, idx, data2_duration, 1).array() = 1;

        start_t_data1 += data1_duration;
        start_t_data2 += data2_duration;

        std::vector<int> temp;
        temp = { data1_duration, data1_class };
        for (auto mask : available_val_dic.at(data1_class)) temp.emplace_back(mask);
        temp.emplace_back(-10);
        temp.emplace_back(-data1_duration);
        temp.emplace_back(-data2_duration);
        dur_class_data1.emplace_back(temp);
        temp = { data2_duration, data2_class };
        for (auto mask : available_val_dic.at(data2_class)) temp.emplace_back(mask);
        temp.emplace_back(-10);
        temp.emplace_back(-data1_duration);
        temp.emplace_back(-data2_duration);
        dur_class_data2.emplace_back(temp);

    }
    saveTxt2D(mat1, save_file_path_data_1);
    saveTxt2DVec(dur_class_data1, save_file_path_dc_1);
    saveTxt2D(mat2, save_file_path_data_2);
    saveTxt2DVec(dur_class_data2, save_file_path_dc_2);

}
