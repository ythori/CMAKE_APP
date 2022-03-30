

#include "MusicPreciseAnalysis.h"
#include "pch.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../ScoreFollowing/ScoreFollowing.h"


MusicPreciseAnalysis::MusicPreciseAnalysis() {
	is_midi_analysis = false;
	is_exe_dtw = false;
	is_out_dtw_result = false;
	is_exe_tempo_estimation = false;
	is_out_te_result = false;
}

void MusicPreciseAnalysis::exeScoreFollowing(const std::string db_fname, const std::string music_fname, const std::string score_xml_fname,
											 const int fps, const bool is_onset, const std::string att) {
	/* ================================
		is_onset: DTW method (onset or full) -> onset
		att: Spectrogram (Chroma or CQT) -> CQT

	================================ */
	std::shared_ptr<ScoreFollowing> SF = std::make_shared<ScoreFollowing>(db_fname, music_fname, score_xml_fname);
	auto path_dic = SF->getResultPath(fps, att);

	// DTW
	if (is_exe_dtw == false) {
		echo("Start: DTW");
		SF->getDTWPath(fps, is_onset, att, is_midi_analysis);
		echo("Finish: DTW");
	}
	else {
		echo("DTW: Already finished");
	}

	// DTW - result
	if (is_out_dtw_result == false) {
		SF->makePathDetailData(path_dic["midi_result"], path_dic["onset_path"],
							   path_dic["score_onset"], path_dic["ex_onset"]);

		std::unordered_map<std::string, std::string> eval_required_txt_path_dic;
		std::string eval_att = "onset_path";
		eval_required_txt_path_dic[eval_att] = path_dic["onset_detail_path"];
		eval_required_txt_path_dic["score"] = path_dic["midi_result"];
		SF->evalPath(eval_required_txt_path_dic, fps, eval_att);
	}


	// Tempo estimation
	if (is_exe_tempo_estimation == false) {
		echo("Start: Tempo estimation");
		SF->tempoEstimation(path_dic["music_folder"], path_dic["onset_detail_path"]);
		echo("Finish: Tempo estimation");
	}
	else {
		echo("Tempo estimation: Already finished");
	}

	// Tempo estimation - result
	if (is_out_te_result == false) {
		bool is_show = true;
		SF->addTempoInfo(path_dic["music_folder"], path_dic["onset_detail_path"], path_dic["tempo_estimation_path"], is_show);
	}



}






void MusicPreciseAnalysis::execute(const std::string db_fname, const std::string music_fname, const std::string score_xml_fname,
								   const int fps, const bool is_onset, const std::string att) {
	echo("Start: Score following");
	exeScoreFollowing(db_fname, music_fname, score_xml_fname, fps, is_onset, att);
	echo("Finish: Score following");
	
}

//void HSMM
