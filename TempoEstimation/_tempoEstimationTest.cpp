

#include "_tempoEstimationTest.h"
#include "TempoEstimation.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../Shared/passPyCpp.h"

void tempoEstimationTest() {
    pyInitialize();

    DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
    std::string crest = DA.getMusicDB() + "crestmuse/";
    std::string mididata_folder = crest + "PEDBv2.2/bac-inv001-schif-g/";

    //std::string path = DA.getResultFolder() + "test_score_following/path_onset_bac-inv001-schif-g_fps_20.txt";
    //std::string path = mididata_folder + "path_onset_bac-inv001-schif-g_fps_20.txt";
    //std::string midi = mididata_folder + "score_following.txt";
    //std::string score = mididata_folder + "score_onset_fps_20.txt";
    //std::string ex = mididata_folder + "ex_onset_fps_20.txt";
    std::string onset_path_path = mididata_folder + "onset_path.txt";

    std::shared_ptr<TempoEstimation> TE = std::make_shared<TempoEstimation>(mididata_folder, onset_path_path);

    //auto _ = TE->setInput();
    //saveTxt2DVec(_, mididata_folder + "_.txt");

    //TE->execute();
    TE->showResult();

    pyFinalize();

}

