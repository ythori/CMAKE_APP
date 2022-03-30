

#include "_scoreFollowingTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../Shared/passPyCpp.h"
#include "ScoreFollowing.h"


void scoreFollowingTest() {
    pyInitialize();

    DataAddress DA;
    DA.resetSaveFolder();		// set result folder (just in case)
    std::string DBname = "PEDBv2.2";
    std::string target = "bac-inv001-schif-g";
    std::string score_xml = "bac-inv001-wiener-p012-013.xml";

    std::shared_ptr<ScoreFollowing> SF = std::make_shared<ScoreFollowing>(DBname, target, score_xml);
    
    // MIDI and XML analysis
    //SF->getMidiToMidiAlignment();

    int fps = 20;   // fps20 = 50msec/frame,   fps50 = 20msec/frame

    // Full DTW
    //SF->getChroma(fps);
    //SF->getCQT(fps);
    //SF->getFullDTWPath(fps);
    //SF->evalFullDTWPath(fps);

    // Onset DTW
    //SF->onsetDetectingAll(fps);
    //SF->_getOnsetDTWPath(fps);
    //SF->evalOnsetDTWPath(fps);

    // All
    bool is_onset = true;   // Only for DTW (onset detecting is executed anytime)
    std::string att = "CQT";
    bool is_midi_analysis = true;
    auto path_dic = SF->getResultPath(fps, att);

    //SF->getCQT(fps);
    //SF->getOnsetSpectrum(fps);

    //std::vector<std::vector<double>> tmat;
    //std::vector<double> temp = { 1, 2, 3, 4, 5 };
    //tmat.emplace_back(temp);
    //temp = { 10, 11, 12, 13, 14 };
    //tmat.emplace_back(temp);
    //auto ret = SF->normalizeCQT(tmat, "standardization");   // dev_max, normalization, standardization
    //echo_v2(ret);

    //SF->getDTWPath(fps, is_onset, att, is_midi_analysis);

    //SF->makePathDetailData(path_dic["midi_result"], path_dic["onset_path"],
    //                       path_dic["score_onset"], path_dic["ex_onset"]);

    //SF->evalPath(path_dic["midi_result"], path_dic["onset_detail_path"], fps);
    //SF->evalPath(path_dic["midi_result"], path_dic["onset_detail_path"], fps);


    //SF->


    //std::string eval_path = DA.getSavePath("test_score_following/eval_onset_dtw_fps_20.txt");
    //echo(eval_path);
    //SF->showEvalPianoroll(eval_path);

}
