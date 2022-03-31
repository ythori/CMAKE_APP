

#include "_musicPreciseAnalysisTest.h"
#include "MusicPreciseAnalysis.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../Shared/passPyCpp.h"
#include "../Shared/ConstantParams.h"


void musicPreciseAnalysisTest() {
    echo("Precise analysis");
    pyInitialize();


    // Data
    DataAddress DA;
    DA.resetSaveFolder();		// set result folder (just in case)
    auto CP = std::make_shared<CParams>();
    auto cm = CP->getCrestmusePath();
    //echo_v1(cm);
    //std::string DBname = "PEDBv2.2";
    //std::string target = "bac-inv001-schif-g";
    //std::string score_xml = "bac-inv001-wiener-p012-013.xml";
    std::string DBname = cm[0];
    std::string target = cm[1];
    std::string score_xml = cm[2];

    // Parameters for score following (score, ex analysis and DTW)
    // Followings are set as default parameters
    int fps = 20;   // fps20 = 50msec/frame,   fps50 = 20msec/frame
    bool is_onset = true;   // Only for DTW (onset detecting is executed anytime)
    std::string att = "CQT";

    std::shared_ptr<MusicPreciseAnalysis> MPA = std::make_shared<MusicPreciseAnalysis>();

    // is_process: true = already finished
    //MPA->setIsMidiAnalysis(true);
    //MPA->setIsDTW(true);
    //MPA->setIsDTWResult(true);

    MPA->execute(DBname, target, score_xml);


    pyFinalize();

}
