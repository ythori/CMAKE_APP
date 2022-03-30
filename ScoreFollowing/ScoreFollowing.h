
/* ================================
Created by Hori on 2021/6/17

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
	Folder structure
		[PEDBvx.x]
			|- [music_name]--<music_name>.mid		Original midi
			|			   |-<mod_music_name>.xml	score xml (note number)
			.			   |-deviation.xml			deviation xml (tempo, attack, extra-note, miss-note)
			.			   |-score.mid				Score midi that is the basis of the original midi
			.			   |-ex.wav					Wave-converted original midi
						   --score_wav				Wave-converted score midi


	Make accurate data

	Score following
		1. DTW
		2. Onset detecting and DTW
		3.a Onset detecting, DTW, and HSMM
		3.b Onset detecting, DTW, and GP-HSMM

	Evaluation
		Input: Analyzing data of midi and xml
			   alignment matching
		Output: Recall, Precision, and F-value

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Set

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef SCORE_FOLLOWING_H
#define SCORE_FOLLOWING_H

#include "pch.h"

class ScoreFollowing {
public:
	ScoreFollowing();
	ScoreFollowing(const std::string db_fname, const std::string music_fname, const std::string score_xml_fname);

	// For MIDI analysis
	void getMidiToMidiAlignment();	// Analyze midi and xml

	// For DTW
	void getChroma(const int fps);
	void getCQT(const int fps);
	void getSpectrum(const int fps, const std::string att = "CQT");

	void onsetDetectingForWAVE(const int fps);
	void onsetDetectingForScore(const int fps, const bool is_midi_analysis = true);
	void onsetDetectingAll(const int fps, const bool is_midi_analysis = true);

	void getOnsetSpectrum(const int fps, const std::string att = "CQT", const bool is_midi_analysis = true);
	std::vector<std::vector<double>> normalizeCQT(const std::vector<std::vector<double>> mat, const std::string method);

	void getDTWPath(const int fps, const bool is_onset = true,
					const std::string att = "CQT", const bool is_midi_analysis = false);

	void makePathDetailData(const std::string score_info_path, const std::string onset_dtw_path,
							const std::string score_onset_path, const std::string ex_onset_path);

	// For tempo estimation (particle filter)
	void tempoEstimation(const std::string music_folder, const std::string onset_path_path);
	void addTempoInfo(const std::string music_folder, const std::string onset_path_path,
					  const std::string tempo_result_path, const bool is_show);

	// For coarse alignment (HSMM)
	void musicHSMM(const std::string music_folder, const std::string mod_onset_path_path, const bool is_show);


	// For evaluation
	void evalPath(const std::unordered_map<std::string, std::string> path_dic, const int fps, const std::string eval_att);
	void showEvalPianoroll(const std::string eval_path);


	// Misc
	std::unordered_map<std::string, std::string> getResultPath(const int fps, const std::string att) const;
	void testp();

private:
	bool is;
	std::string save_folder_path;
	std::string music_folder_path, music_db_folder_name, music_folder_name, score_xml_file_name, est_alignment_txt_path;
	std::string ex_midi_path, score_midi_path, ex_wave_path, score_wave_path;
	std::vector<std::vector<double>> ex_spectrum, score_spectrum, ex_onset_spectrum, score_onset_spectrum;
	//std::vector<int> ex_idx_list, score_idx_list;
};



#endif //SCORE_FOLLOWING_H