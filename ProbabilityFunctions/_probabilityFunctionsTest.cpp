

#include "_probabilityFunctionsTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "ProbabilityFunctions.h"
#include "GenRandomNumber.h"
#include "../Shared/passPyCpp.h"
#include "../Shared/matplotlibCpp.h"



void probabilityFunctionsTest() {
    echo("Misc Probability functions test");
    //miscProbabilityFunctionsTest();

    echo("Generate random number test");
    //genRandomNumberTest();

    echo("Show shape test");
    showProbShape();

}

void miscProbabilityFunctionsTest() {
    echo(digam(10));
    echo(genNorm(0, 2));
    echo(genCauchy(0, 5));
    echo(pdfLnNorm(1, 0, 1));   // -1.41894
    echo(pdfLnCauchy(1, 0, 1)); // -1.83788

}

void genRandomNumberTest() {
    auto GR = std::make_shared<GenRandom>();
    GR->genNorm1D(0, 0.1, 100);
    auto random_evec = GR->getRandomVectorDouble();
    echo(random_evec);

    //GR->genUni(2, 10, 5);
    //random_evec = GR->getRandomVectorDouble();
    //echo(random_evec);
}

void showProbShape() {
	DataAddress DA;
	DA.resetSaveFolder();   // Result folder
	DA.setSaveFolder("test_pdf");
	auto result_folder = DA.getSaveFolderPath();


	pyInitialize();

	int n_mix = 3;
	std::vector<int> cand_list = { 3, 8, 15 };
	int exp_cand = 8;
	std::vector<std::vector<double>> input_mat, input_mat_poi, input_mat_gmm;
	for (int i = 1; i < 20; ++i) {
		Eigen::VectorXd obs = Eigen::VectorXd::Constant(1, i);

		// Poisson
		double log_lik_poi = pdfLnPoisson(i, exp_cand);

		// GMM
		Eigen::MatrixXd cov = Eigen::MatrixXd::Constant(1, 1, 0.01);
		std::vector<double> mix;
		std::vector<Eigen::VectorXd> means;
		std::vector<Eigen::MatrixXd> covs;
		double mix_val = 1.0 / n_mix;
		for (auto cand : cand_list) {
			means.emplace_back(Eigen::VectorXd::Constant(1, cand));
			mix.emplace_back(mix_val);
			covs.emplace_back(cov);
		}
		double log_lik_gmm = pdfLnGMM(obs, mix, means, covs);

		double log_lik = log_lik_poi + log_lik_gmm;

		//std::vector<double> t_poi = { double(i), log_lik_poi };
		//std::vector<double> t_gmm = { double(i), log_lik_gmm };
		//std::vector<double> temp = { double(i), log_lik };
		std::vector<double> t_poi = { double(i), exp(log_lik_poi) };
		std::vector<double> t_gmm = { double(i), exp(log_lik_gmm) };
		std::vector<double> temp = { double(i), exp(log_lik) };
		input_mat_poi.emplace_back(t_poi);
		input_mat_gmm.emplace_back(t_gmm);
		input_mat.emplace_back(temp);
		//echoes(obs, exp(log_lik_gmm));
	}

	// Gen instance and pointer
	std::vector<std::shared_ptr<PlotData>> data_vec;
	std::shared_ptr<PlotData> PD = std::make_shared<PlotData>();

	std::string plot_type = "line_with_pos";
	std::string style_data = "";	// "r-", "b--", ...
	std::string legend_poi = "Poisson";
	std::string legend_gmm = "GMM";
	std::string legend_data = "Poisson-GMM";
	std::string axis_x = "x";
	std::string axis_y = "p";

	PD->setData(input_mat_poi, plot_type, style_data, legend_poi);
	PD->setData(input_mat_gmm, plot_type, style_data, legend_gmm);
	PD->setData(input_mat, plot_type, style_data, legend_data);
	PD->setXLabel(axis_x);
	PD->setYLabel(axis_y);
	
	data_vec.emplace_back(PD);

	// Gen matplotlib instance
	MatplotlibCpp PLT(data_vec);
	PLT.setSaveFolderPath(result_folder);
	PLT.setSaveFileName("test.png");
	PLT.setShowFlag(true);
	PLT.showInterface();
	PLT.execute();
	pyFinalize();
}
