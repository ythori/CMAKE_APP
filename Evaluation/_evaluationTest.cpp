

#include "_evaluationTest.h"
#include "EvaluationSystem.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"

void evaluationTest() {
    //matchingTest();
    matchingLabelTest();
}

void matchingTest() {
	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)

    std::string data_folder_path = DA.getResultFolder() + "test_midi/";
    std::string data1_path = data_folder_path + "test_score_onset.txt";
    std::string data2_path = data_folder_path + "est_score_onset.txt";
    auto data1 = readStdVectorTxt(data1_path);
    auto data2 = readStdVectorTxt(data2_path);

    //echo_v1(data1);
    //echo_v1(data2);
    double tolerance = 10;

    evaluationMatching(data1, data2, tolerance);


    data1_path = data_folder_path + "test_ex_onset.txt";
    data2_path = data_folder_path + "est_ex_onset.txt";
    data1 = readStdVectorTxt(data1_path);
    data2 = readStdVectorTxt(data2_path);
    for (int i = 0; i < data2.size(); ++i) data2[i] *= 10;

    evaluationMatching(data1, data2, tolerance);
}

void matchingLabelTest() {
    DataAddress DA;
    DA.resetSaveFolder();		// set result folder (just in case)

    std::string data_folder_path = DA.getMiscFolder() + "Matching_samples/";
    std::string data_gt_path = data_folder_path + "gt_label.txt";
    std::string data_est_path = data_folder_path + "est_label.txt";
    std::string data_match_path = data_folder_path + "matching_path.txt";
    auto data_gt = readStdVector2DiTxt(data_gt_path);
    auto data_est = readStdVector2DiTxt(data_est_path);
    auto t_match_path = readStdVector2DiTxt(data_match_path);
    std::vector<std::pair<int, int>> match_path;
    for (auto path : t_match_path) {
        auto p = std::pair<int, int>(path[0], path[1]);
        match_path.emplace_back(p);
    }

    double tolerance = 2. / 3;
    evaluationMatchingWithLabel(data_gt, data_est, match_path, tolerance);

}