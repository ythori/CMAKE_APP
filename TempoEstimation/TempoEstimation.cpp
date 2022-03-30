

#include "TempoEstimation.h"
#include "pch.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../ParticleFilter/ParticleFilter.h"
#include "../Shared/matplotlibCpp.h"


TempoEstimation::TempoEstimation() {
	is = false;
}

TempoEstimation::TempoEstimation(const std::string spath, const std::string path) {
	save_folder_path = spath;
	path_path = path;
	save_pf_result_path = spath + "tempo_estimation.txt";
	save_mod_onset_path = spath + "mod_onset_path.txt";

	step = 4;
	dim = 1;

	onset_arr.clear();

}


std::vector<std::vector<double>> TempoEstimation::setInput() {
	// path_vec:
	// [mnn, score_midi_onset, vel, tempo, ex_est_onset, ex_midi_onset, chord_idx, belief(0:match, 1:miss, 2:extra, 3:not_match),
	//	path_onset_idx_score, path_onset_idx_ex, path_idx_score, path_idx_ex, msec_score, msec_ex, add_on_score, first_onset_flag]
	auto path_vec = readStdVector2DiTxt(path_path);

	// Extract first onset matching path
	onset_arr.clear();
	int pre_msec = -1;
	for (int i = 0; i < path_vec.size(); ++i) {
		auto path_info = path_vec[i];
		//if (path_info[8] == 1) {
		if (pre_msec < path_info[12]) {
			std::vector<int> temp = { path_info[12], path_info[13], path_info[3] };	// [path_score_msec, path_ex_msec, tempo_score] 
			//std::vector<int> temp = { path_info[4], path_info[5], path_info[7] };	// [path_score_msec, path_ex_msec, tempo_score] 
			onset_arr.emplace_back(temp);
			pre_msec = path_info[12];
		}
	}
	//echo_v2(onset_arr);

	// Consider skip size (moving average)
	// ex_local_tempo = (ioi_score / ioi_ex) * score_tempo
	//		ex) ioi_score = 100, score_tempo = 60 -> ioi_ex = 50 (= more fast): (100 / 50) * 60 = 120
	std::vector<std::vector<double>> ex_tempo;
	for (int i = 0; i < onset_arr.size(); ++i) {
		double local_ex_tempo = 0;
		double ma_local_ex_tempo = 0;
		if (i < 2) {	// i = 0 or 1
			local_ex_tempo = (double(onset_arr[1][0]) - double(onset_arr[0][0])) / (double(onset_arr[1][1]) - double(onset_arr[0][1])) * double(onset_arr[0][2]);
			ma_local_ex_tempo = local_ex_tempo;
		}
		else {
			local_ex_tempo = (double(onset_arr[i][0]) - double(onset_arr[i - 1][0])) / (double(onset_arr[i][1]) - double(onset_arr[i - 1][1])) * double(onset_arr[i][2]);
			ma_local_ex_tempo = local_ex_tempo;

			if (i < step) {	// i = 2, 3 (in case of step = 4)
				for (auto pre_tempo : ex_tempo) {
					ma_local_ex_tempo += pre_tempo[0];
				}
				double s = (ex_tempo.size() + 1.0);
				ma_local_ex_tempo /= s;
			}
			else {	// 4 - (SMA_t = SMA_{t-1} - P_{t-1}/n + P_t/n)
				auto most_old_lt = ex_tempo[ex_tempo.size() - step][0];		// ex) i=3 -> .size()=4 ={0,1,2,3} -> exclude {0}
				auto pre_ma = ex_tempo.back()[1];
				ma_local_ex_tempo = pre_ma - (most_old_lt / step) + (local_ex_tempo / step);
			}
		}
		std::vector<double> temp = { local_ex_tempo, ma_local_ex_tempo };
		ex_tempo.emplace_back(temp);
	}
	//echoes(onset_arr.size(), ex_tempo.size());
	return ex_tempo;
}

void TempoEstimation::execute() {

	auto ex_tempo = setInput();
	Eigen::VectorXd ex_tempo_eigen = convertSVec2EMat(ex_tempo).col(1);

	echo(ex_tempo_eigen);

	std::shared_ptr<ParticleFilter> PF = std::make_shared<ParticleFilter>(dim, save_folder_path);

	PF->setInitParams(ex_tempo_eigen[0] - 0.1, ex_tempo_eigen[0] + 0.1, log(0.1), 0, log(2.0), 0);
	PF->fit(ex_tempo_eigen);

	auto smoothed_eigen = PF->getSmoothed();
	std::vector<double> smoothed;
	for (auto v : smoothed_eigen) smoothed.emplace_back(v(0));
	//echo_v1(smoothed);

	std::vector<std::vector<double>> ret(smoothed_eigen.size(), std::vector<double>(3));

	//echoes(ex_tempo.size(), ex_tempo[0].size(), smoothed_eigen.size());
	for (int i = 0; i < ex_tempo.size(); ++i) {
		ret[i][0] = ex_tempo[i][0];
		ret[i][1] = ex_tempo[i][1];
		ret[i][2] = smoothed_eigen[i](0);
	}

	saveTxt2DVec(ret, save_pf_result_path);

}


void TempoEstimation::showResult() {
	auto results = readMatrixTxt(save_mod_onset_path);
	std::vector<double> local_ex_tempo, ma_local_ex_tempo, smoothed_ex_tempo;

	int pre_msec = -1;
	for (int r = 0; r < results.rows(); ++r) {
		//if (results(r, 8) == 1) {
		if (pre_msec < results(r, 12)) {
			//local_ex_tempo.emplace_back(results(r, 9));
			//ma_local_ex_tempo.emplace_back(results(r, 10));
			//smoothed_ex_tempo.emplace_back(results(r, 11));
			local_ex_tempo.emplace_back(results(r, 16));
			ma_local_ex_tempo.emplace_back(results(r, 17));
			smoothed_ex_tempo.emplace_back(results(r, 18));
			pre_msec = results(r, 12);
		}
	}

	// Gen instance and pointer
	std::shared_ptr<PlotData> PD = std::make_shared<PlotData>();
	std::vector<std::shared_ptr<PlotData>> data_vec;

	// Set line and axis data
	std::string style_local = "k-";	// "r-", "b--", ...
	std::string legend_local = "Local tempo";
	std::string style_ma = "g-";
	std::string legend_ma = "Moving-average tempo";
	std::string style_smoothed = "r-";
	std::string legend_smoothed = "Estimated tempo";
	std::string axis_x = "Onset index";
	std::string axis_y = "Tempo";

	// Set data
	std::string plot_type = "line_val_only";

	PD->setData(local_ex_tempo, plot_type, style_local, legend_local);
	PD->setData(ma_local_ex_tempo, plot_type, style_ma, legend_ma);
	PD->setData(smoothed_ex_tempo, plot_type, style_smoothed, legend_smoothed);

	PD->setXLabel(axis_x);
	PD->setYLabel(axis_y);

	data_vec.emplace_back(PD);

	// Gen matplotlib instance
	std::shared_ptr<MatplotlibCpp> PLT = std::make_shared<MatplotlibCpp>(data_vec);
	PLT->setSaveFolderPath(save_folder_path);
	PLT->setSaveFileName("tempo.png");
	PLT->setShowFlag(true);
	PLT->showInterface();
	PLT->execute();
}


