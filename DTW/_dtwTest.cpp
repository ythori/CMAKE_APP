

#include "_dtwTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "DynamicTimeWarping.h"
#include "../Shared/passPyCpp.h"

void dtwTest() {
	pyInitialize();

	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
    std::string dtw_sample_folder = DA.getMiscFolder() + "DTW_samples/";
	std::string data1 = dtw_sample_folder + "dtw_5d_toy1.txt";
	std::string data2 = dtw_sample_folder + "dtw_5d_toy2.txt";
	DA.setSaveFolder("test_dtw");

	auto data1_mat = readStdVector2DTxt(data1);
	auto data2_mat = readStdVector2DTxt(data2);
	//echo_v2(data1_mat);
	//echo_v2(data2_mat);

	DynamicTimeWarping DTW;
	DTW.setInput(data1_mat, data2_mat);
	DTW.fastDTW();

	auto path = DTW.getPath();
	auto distance = DTW.getDistance();
	echo(distance);
	for (auto p : path) echoes(p.first, p.second);
	DTW.setSavePath(DA.getSaveFolderPath(), "dtw_path.txt");
	DTW.saveDTWPath();


	//DTW.setSavePath(DA.getSaveFolderPath(), "dtw_test.png");
	//DTW.showSaveParams();
	//DTW.showDTW();

	pyFinalize();
}
