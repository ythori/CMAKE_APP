
/* ================================
Created by Hori on 2021//

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Music precise analysis (integration)

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Procedure
		(TD = target_music_directory)

		1. /ScoreFollowing/ScoreFollowing.cpp

			1.1. DTW
				1.1.1. MIDI and XML analysis (using CrestMuseProcessor())
						: GET: TD/score_following.txt
							(midi_note_number, onset_msec_score, velocity, tempo, onset_msec_est_ex (from xml),
							 onset_msec_ex (match midi and xml), chord_idx, belief (0:match, 1:miss, 2:extra, 3:not_match))
				1.1.2. Get CQT
						: TD/score_cqt_fps_<20>.txt	(Standardized)
						: TD/ex_cqt_fps_<20>.txt	(Standardized)
				1.1.3. Onset detecting and get onset CQT
						: TD/score_onset_fps_<20>.txt
						: TD/score_onset_cqt_fps_<20>.txt	(Normalized)
						: TD/ex_onset_fps_<20>.txt
						: TD/ex_onset_cqt_fps_<20>.txt		(Normalized)
				1.1.4. DTW
						: TD/path_onset_<bac-inv001-schif-g_fps_20>.txt
							(path between onset indexes)
				1.1.5. Make detailed path data (makePathDetailData())
						: TD/onset_path.txt
							(score_onset_idx, ex_onset_idx, score_idx, ex_idx, score_msec, ex_msec, score_tempo, first_onset_flag)
				(1.1.6. Evaluation)


			1.2. Tempo estimation
				1.2.1. Tempo estimation
						: TD/mod_onset_path.txt
							(score_onset_idx, ex_onset_idx, score_idx, ex_idx, score_msec, ex_msec, mnn, score_tempo, first_onset_flag,
							 local_ex_tempo, ma_local_ex_tempo, estimated_tempo)




	Usage
		1. Set

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef MUSIC_PRECISE_ANALYSIS_C_H
#define MUSIC_PRECISE_ANALYSIS_C_H

#include "pch.h"


class MusicPreciseAnalysis {
public:
	MusicPreciseAnalysis();

	void execute(const std::string db_fname, const std::string music_fname, const std::string score_xml_fname,
				 const int fps = 20, const bool is_onset = true, const std::string att = "CQT");


	void setIsMidiAnalysis(const bool is_exe) { is_midi_analysis = is_exe; }
	void setIsDTW(const bool is_exe) { is_exe_dtw = is_exe; }
	void setIsDTWResult(const bool is_exe) { is_out_dtw_result = is_exe; }
	void setIsTempoEstimation(const bool is_exe) { is_exe_tempo_estimation = is_exe; }
	void setIsTempoEstimationResult(const bool is_exe) { is_out_te_result = is_exe; }



private:
	/* ================================
	* Score following results
		// Shared
		ret["score_result_folder"] = save_folder_path;	// ../Results/test_score_following
		ret["music_folder"] = music_folder_path;		// ../PEDBv2.2/bac-inv001-schif-g
		ret["db_version"] = music_db_folder_name;		// PEDBv2.2
		ret["music_name"] = music_folder_name;			// bac-inv001-schif-g
		ret["ex_wave"] = ex_wave_path;

		// MIDI analysis
		ret["midi_result"] = est_alignment_txt_path;	// ../bac../score_following.txt (mnn, score_on, est_ex_on, ex_on, chord_idx, belief)

		// DTW
		ret["score_onset"] = music_folder_path + "score_onset_fps_" + std::to_string(fps) + ".txt";
		ret["score_cqt"] = music_folder_path + "score_cqt_fps_" + std::to_string(fps) + ".txt";
		ret["ex_onset"] = music_folder_path + "ex_onset_fps_" + std::to_string(fps) + ".txt";
		ret["ex_cqt"] = music_folder_path + "ex_cqt_fps_" + std::to_string(fps) + ".txt";
		ret["onset_path"] = music_folder_path + "path_onset_" + music_folder_name + "_fps_" + std::to_string(fps) + ".txt";
		ret["onset_detail_path"] = music_folder_path + "onset_path.txt";

		// Tempo estimation
		ret["mod_onset_path"] = music_folder_path + "mod_onset_path.txt";


	================================ */


	void exeScoreFollowing(const std::string db_fname, const std::string music_fname, const std::string score_xml_fname,
						   const int fps = 20, const bool is_onset = true, const std::string att = "CQT");


	bool is_midi_analysis, is_exe_dtw, is_out_dtw_result, is_exe_tempo_estimation, is_out_te_result;
	std::unordered_map<std::string, std::string> score_following_results, tempo_estimation_results;

};



#endif //MUSIC_PRECISE_ANALYSIS_C_H