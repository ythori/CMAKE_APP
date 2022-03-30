

#include "_particleFilterTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "ParticleFilter.h"
#include "ParticleFilterShared.h"
#include "ParticleFilterPDF.h"
#include "../Shared/matplotlibCpp.h"


void particleFilterTest() {
	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
    std::string misc = DA.getMiscFolder() + "Misc_samples/";
    //std::string toy_path = misc + "data_1dim_pf.txt";
    //std::string toy_path = misc + "data_1dim_pf_tempo.txt";
    std::string toy_path = misc + "data_1dim_pf2.txt";
    //std::string toy_path = DA.getMusicDB() + "crestmuse/PEDBv2.2/bac-inv001-schif-g/"
    DA.setSaveFolder("test_pf");     // result_folder/designed_folder/


    //Eigen::MatrixXd mat(2, 3);
    //mat << 1, 2, 3,
    //    4, 5, 6;
    //echo(mat);

    //std::string pf_type = "self_organized";     // or not
    //std::string pf_model = "second_order_difference";   // or not
    //int dim = 2;
    //auto F = setF(pf_type, pf_model, dim);
    //echo(F);
    //auto G = setG(pf_type, pf_model, dim, F.cols(), true);
    //echo(G);
    //auto H = setH(dim, F.cols());
    //echo(H);
    //testttt();

    //echo(toy_path);
    auto toy = readVectorTxt(toy_path);
    //echo(toy);
    int dim = 1;
    std::string save_folder = DA.getSaveFolderPath();
    std::shared_ptr<ParticleFilter> PF = std::make_shared<ParticleFilter>(dim, save_folder);
    PF->setInitParams(toy[0] - 0.1, toy[0] + 0.1, log(0.1), 0, log(2.0), 0);

    echo("pf");
    echo(toy);
    PF->fit(toy);

    auto smoothed_eigen = PF->getSmoothed();
    std::vector<double> smoothed;
    for (auto v : smoothed_eigen) smoothed.emplace_back(v(0));
    echo("smoothed");
    echo_v1(smoothed);

    //auto exp_x = PF->getExpLatent();
    //auto std_vec_toy = convertEVec2SVec(toy);
    ////echo_v1(std_vec_toy);

    pyInitialize();
    bool is_data = true;
    bool is_filter = true;
    bool is_smooth = true;
    PF->showSequence(is_data, is_filter, is_smooth);
    pyFinalize();

}
