

#include "createTestTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"

void createTestTest() {
	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
    std::string crest = DA.getMusicDB() + "crestmuse/";
    std::string mididata_folder = crest + "dataset/bac-inv001-schif-g/";
    //std::string mididata = mididata_folder + "ex.mid";
    std::string mididata = mididata_folder + "score.mid";
    DA.resetSaveFolder();   // Result folder
    DA.setSaveFolder("test_midi");     // result_folder/designed_folder/
    auto save_file_path = DA.getSavePath("test_score_onset.txt");


}

