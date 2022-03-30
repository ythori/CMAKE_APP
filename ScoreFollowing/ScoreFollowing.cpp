

#include "ScoreFollowing.h"
#include "pch.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../CrestMuseProcessor/CrestMuse.h"
#include "../PythonMadmom/Madmom.h"
#include "../DTW/DynamicTimeWarping.h"
#include "../Shared/matplotlibCpp.h"
#include "../Shared/vec_op.h"
#include "../TempoEstimation/TempoEstimation.h"


ScoreFollowing::ScoreFollowing() {}


ScoreFollowing::ScoreFollowing(const std::string db_fname, const std::string music_fname, const std::string score_xml_fname) {
	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
	DA.setSaveFolder("test_score_following");
	save_folder_path = DA.getSaveFolderPath();

	std::string crestmuse_db_folder = DA.getMusicDB() + "crestmuse/";
	music_folder_path = crestmuse_db_folder + db_fname + "/" + music_fname + "/";

	music_db_folder_name = db_fname;
	music_folder_name = music_fname;
	score_xml_file_name = score_xml_fname;
	est_alignment_txt_path = music_folder_path + "score_following.txt";

	ex_midi_path = music_folder_path + music_fname + ".mid";
	score_midi_path = music_folder_path + "score.mid";
	ex_wave_path = music_folder_path + "ex.wav";
	score_wave_path = music_folder_path + "score.wav";

}


void ScoreFollowing::getMidiToMidiAlignment() {
	std::shared_ptr<CrestMuseProcessor> CMP = std::make_shared<CrestMuseProcessor>();
	// setMusicDataPath() -> analyzeAllMidi() -> analyzeXml() -> setEstExOnsetMsecToScoreMidiUsingXml()
	echo("MIDI and XML analysis");
	CMP->scoreFollowingWithMidiAndXml(music_db_folder_name, music_folder_name, score_xml_file_name, "deviation.xml");
	CMP->writeScoreFollowing();		// Write score [mnn, score_ontime, est_ontime, ex_ontime, chord_idx, flag], flag=-1: miss-note
}


void ScoreFollowing::onsetDetectingForWAVE(const int fps) {
	echo("Onset detecting: ex_WAVE");

	std::string ex_onset_save_path = music_folder_path + "ex_onset_fps_" + std::to_string(fps) + ".txt";

	std::shared_ptr<MadmomCall> MC = std::make_shared<MadmomCall>();
	MC->setWavePath(ex_wave_path);
	std::vector<std::vector<int>> ex_onset;
	auto t_ex_onset = MC->onsetDetecting(fps);
	//echo_v1(ex_onset);
	int msec_per_frame = int(1000 / fps);	// ex) 1000 / 50 = 20
	int n = 0;
	for (auto msec : t_ex_onset) {
		int idx = int(msec / msec_per_frame);	// ex) 120 = int(120/20) = 6 (= From 120 to 140),   250 = int(250/20) = 12 (= From 240 to 260)
		//if (msec % msec_per_frame != 0) idx += 1;
		std::vector<int> temp = { n, idx, msec };
		ex_onset.emplace_back(temp);
		++n;
	}
	saveTxt2DVec(ex_onset, ex_onset_save_path);
}

void ScoreFollowing::onsetDetectingForScore(const int fps, const bool is_midi_analysis) {
	if (is_midi_analysis == false) {
		echo("MIDI analysis");
		getMidiToMidiAlignment();
	}

	echo("Onset detecting: score_music");

	std::string score_onset_save_path = music_folder_path + "score_onset_fps_" + std::to_string(fps) + ".txt";

	std::vector<std::vector<int>> score_onset, add_onset;

	// 1st: Estimate using MIDI data
	auto midi_vec = readStdVector2DiTxt(est_alignment_txt_path);	// [mnn, score_ontime, est_ontime, ex_ontime, chord_idx, flag], flag=-1: miss-note
	int msec_per_frame = int(1000 / fps);	// ex) 1000 / 50 = 20
	int pre_msec = -1;
	for (auto midi_one_vec : midi_vec) {
		if (pre_msec < midi_one_vec[1]) {
			int msec = midi_one_vec[1];
			int idx = int(msec / msec_per_frame);
			std::vector<int> temp = { idx, msec, 0 };
			score_onset.emplace_back(temp);
			pre_msec = msec;
			//echo_v1(temp);
		}
	}

	// 2nd: Add estimation using onset detecting
	std::shared_ptr<MadmomCall> MC = std::make_shared<MadmomCall>();
	MC->setWavePath(score_wave_path);
	auto t_score_onset = MC->onsetDetecting(fps);
	int start_idx = 0;
	for (auto msec : t_score_onset) {
		int idx = int(msec / msec_per_frame);
		//echoes("cqt_idx", idx);
		for (int i = start_idx; i < score_onset.size(); ++i) {	// Dupli check
			//echoes("start_idx", start_idx);
			int idx_onset_from_midi = score_onset[i][0];
			//echoes("idx_midi", idx_onset_from_midi);
			if ((idx - 2 < idx_onset_from_midi) && (idx_onset_from_midi < idx + 2)) {	// Duplicate onset
				start_idx = i;
				//echoes(idx_onset_from_midi, idx);
				break;
			}
			else if (idx + 1 < idx_onset_from_midi) {
				std::vector<int> temp = { idx, msec, 1 };
				add_onset.emplace_back(temp);
				start_idx = i;
				//echoes(temp);
				//echo(idx);
				break;
			}
		}
	}

	score_onset.insert(score_onset.end(), add_onset.begin(), add_onset.end());
	score_onset = sortTwoDimension(score_onset, 1);	// [idx, msec], sort based on msec

	for (int i = 0; i < score_onset.size(); ++i) {
		score_onset[i].insert(score_onset[i].begin(), i);
	}

	saveTxt2DVec(score_onset, score_onset_save_path);

}

void ScoreFollowing::onsetDetectingAll(const int fps, const bool is_midi_analysis) {

	if (is_midi_analysis == false) {
		echo("MIDI analysis");
		getMidiToMidiAlignment();
	}

	echo("Onset detecting");

	onsetDetectingForWAVE(fps);
	onsetDetectingForScore(fps);

}


void ScoreFollowing::getChroma(const int fps) {
	std::shared_ptr<MadmomCall> MC = std::make_shared<MadmomCall>();
	//int fps = 50;

	MC->setWavePath(ex_wave_path);
	MC->analyzeChroma(fps);
	ex_spectrum = MC->getChroma();

	MC->setWavePath(score_wave_path);
	MC->analyzeChroma(fps);
	score_spectrum = MC->getChroma();

	echoes("Chroma arr size", ex_spectrum.size(), ex_spectrum[0].size(), score_spectrum.size(), score_spectrum[0].size());

	// Save full chroma
	std::string score_chroma_save_path = music_folder_path + "score_chroma_fps_" + std::to_string(fps) + ".txt";
	std::string ex_chroma_save_path = music_folder_path + "ex_chroma_fps_" + std::to_string(fps) + ".txt";
	saveTxt2DVec(score_spectrum, score_chroma_save_path);
	saveTxt2DVec(ex_spectrum, ex_chroma_save_path);

}


void ScoreFollowing::getCQT(const int fps) {
	std::shared_ptr<MadmomCall> MC = std::make_shared<MadmomCall>();
	//int fps = 50;

	MC->setWavePath(ex_wave_path);
	MC->analyzeCQT(fps);
	ex_spectrum = MC->getCQT();

	MC->setWavePath(score_wave_path);
	MC->analyzeCQT(fps);
	score_spectrum = MC->getCQT();

	//echoes("CQT arr size", ex_spectrum.size(), ex_spectrum[0].size(), score_spectrum.size(), score_spectrum[0].size());

	// Normalize (Standardization using all elements)
	std::string normalize_method = "standardization";
	auto t_score_spectrum = normalizeCQT(score_spectrum, normalize_method);
	auto t_ex_spectrum = normalizeCQT(ex_spectrum, normalize_method);

	// Save full cqt
	std::string score_cqt_save_path = music_folder_path + "score_cqt_fps_" + std::to_string(fps) + ".txt";
	std::string ex_cqt_save_path = music_folder_path + "ex_cqt_fps_" + std::to_string(fps) + ".txt";
	saveTxt2DVec(t_score_spectrum, score_cqt_save_path);
	saveTxt2DVec(t_ex_spectrum, ex_cqt_save_path);

}

void ScoreFollowing::getSpectrum(const int fps, const std::string att) {
	if (att == "Chroma") {
		echoes("Chroma analysis: fps", fps);
		getChroma(fps);
	}
	else {
		echoes("CQT analysis: fps", fps);
		getCQT(fps);
	}
}

std::vector<std::vector<double>> ScoreFollowing::normalizeCQT(const std::vector<std::vector<double>> mat, const std::string method) {
	int rows = mat.size();
	int cols = mat[0].size();
	std::vector<std::vector<double>> ret(rows, std::vector<double>(cols));
	if (method == "dev_max") {
		double max_val = -10000000;
		for (auto vec : mat) {
			double t_max = vec_op::maxv(vec);
			if (t_max > max_val) max_val = t_max;
		}
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				ret[r][c] = mat[r][c] / max_val;
			}
		}
	}
	else if (method == "normalization") {
		for (int r = 0; r < rows; ++r) {
			double t_sum = vec_op::sum(mat[r]);
			for (int c = 0; c < cols; ++c) {
				ret[r][c] = mat[r][c] / t_sum;
			}
		}
	}
	else if (method == "standardization") {
		double sum_x = 0;
		double sum_x2 = 0;
		double n_elements = rows * cols;
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				sum_x += mat[r][c];
				sum_x2 += pow(mat[r][c], 2);
			}
		}
		double mean = sum_x / n_elements;
		double sd = sqrt(sum_x2 / n_elements - pow(mean, 2));

		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				ret[r][c] = (mat[r][c] - mean) / sd;
			}
		}
	}
	return ret;
}


void ScoreFollowing::getOnsetSpectrum(const int fps, const std::string att, const bool is_midi_analysis) {
	// 1: Get spectrum (getSpectrum(): include save process)
	// 2: Onset detecting (onsetDetectingAll(): include save process)
	// 3: Get onset spectrum

	if (is_midi_analysis == false) {
		echo("MIDI analysis");
		getMidiToMidiAlignment();
	}

	// Get spectrum (default(att): CQT)
	// Standardization
	echo(att + " analysis");
	getSpectrum(fps, att);

	// Onset detecting
	onsetDetectingAll(fps);

	// Get onset spectrum
	std::string score_onset_save_path = music_folder_path + "score_onset_fps_" + std::to_string(fps) + ".txt";
	std::string ex_onset_save_path = music_folder_path + "ex_onset_fps_" + std::to_string(fps) + ".txt";
	auto score_onset = readStdVector2DiTxt(score_onset_save_path);
	auto ex_onset = readStdVector2DiTxt(ex_onset_save_path);

	//std::vector<std::vector<double>> ex_onset_spectrum, score_onset_spectrum;
	//std::vector<int> ex_idx_list, score_idx_list;
	ex_onset_spectrum.clear();
	score_onset_spectrum.clear();
	//ex_idx_list.clear();
	//score_idx_list.clear();
	for (auto s : score_onset) {	// (idx, msec)
		//if (s[0] > 0) {
		//	score_onset_chroma.emplace_back(score_chroma[s[0] - 1]);
		//	score_idx_list.emplace_back(s[0] - 1);
		//}
		//int pre_idx, cur_idx, next_idx;
		//if (s[0] == 0) pre_idx = 0;
		//else pre_idx = s[0];
		//int cur_idx = s[0];
		int cur_idx = s[1];		// s[0]:onset_idx, s[1]:idx, s[2]:msec, s[3]:add_flag
		//if ((s[0] + 1) == score_onset.size()) next_idx = s[0];
		//else next_idx = s[0] + 1;

		//score_onset_spectrum.emplace_back(score_spectrum[pre_idx]);
		score_onset_spectrum.emplace_back(score_spectrum[cur_idx]);
		//score_onset_spectrum.emplace_back(score_spectrum[next_idx]);

		//score_onset_spectrum.emplace_back(score_spectrum[s[0]]);
		//if ((s[0] + 1) == score_onset.size()) score_onset_spectrum.emplace_back(score_spectrum[s[0]]);
		//else score_onset_spectrum.emplace_back(score_spectrum[s[0] + 1]);

		//score_idx_list.emplace_back(pre_idx);
		//score_idx_list.emplace_back(cur_idx);
		//score_idx_list.emplace_back(next_idx);

		//score_idx_list.emplace_back(s[0]);
		//if (s[0] < score_chroma.size() - 1) {
		//	score_onset_chroma.emplace_back(score_chroma[s[0] + 1]);
		//	score_idx_list.emplace_back(s[0] + 1);
		//}
	}
	for (auto e : ex_onset) {
		//if (e[0] > 0) {
		//	ex_onset_chroma.emplace_back(ex_chroma[e[0] - 1]);
		//	ex_idx_list.emplace_back(e[0] - 1);
		//}
		//echo(e[0]);
		//ex_onset_spectrum.emplace_back(ex_spectrum[e[0]]);

		int pre_idx, cur_idx, next_idx;
		//if (e[0] == 0) pre_idx = 0;
		//else pre_idx = e[0];
		//cur_idx = e[0];
		cur_idx = e[1];		// e[0]:onset_idx, e[1]:idx, e[2]:msec
		//if ((e[0] + 1) == ex_onset.size()) next_idx = e[0];
		//else next_idx = e[0] + 1;

		//ex_onset_spectrum.emplace_back(ex_spectrum[pre_idx]);
		ex_onset_spectrum.emplace_back(ex_spectrum[cur_idx]);
		//ex_onset_spectrum.emplace_back(ex_spectrum[next_idx]);

		//ex_onset_spectrum.emplace_back(ex_spectrum[e[0]]);
		//if ((e[0] + 1) == ex_onset.size()) ex_onset_spectrum.emplace_back(ex_spectrum[e[0]]);
		//else ex_onset_spectrum.emplace_back(ex_spectrum[e[0] + 1]);

		//ex_idx_list.emplace_back(pre_idx);
		//ex_idx_list.emplace_back(cur_idx);
		//ex_idx_list.emplace_back(next_idx);


		//ex_idx_list.emplace_back(e[0]);
		//echo_v1(ex_chroma[e[0]]);
		//if (e[0] < ex_chroma.size() - 1) {
		//	ex_onset_chroma.emplace_back(ex_chroma[e[0] + 1]);
		//	ex_idx_list.emplace_back(e[0] + 1);
		//}
	}

	// Normalize (Normalization along with every frame)
	//std::string normalize_method = "dev_max";
	std::string normalize_method = "normalization";
	//std::string normalize_method = "standardization";
	score_onset_spectrum = normalizeCQT(score_onset_spectrum, normalize_method);
	ex_onset_spectrum = normalizeCQT(ex_onset_spectrum, normalize_method);


	// Save onset spectrum
	std::string score_onset_spectrum_save_path = music_folder_path + "score_onset_" + att + "_fps_" + std::to_string(fps) + ".txt";
	std::string ex_onset_spectrum_save_path = music_folder_path + "ex_onset_" + att + "_fps_" + std::to_string(fps) + ".txt";
	saveTxt2DVec(score_onset_spectrum, score_onset_spectrum_save_path);
	saveTxt2DVec(ex_onset_spectrum, ex_onset_spectrum_save_path);

	//// Save onset spectrum index
	//std::string score_onset_spectrum_idx_save_path = music_folder_path + "score_onset_idx_" + att + "_fps_" + std::to_string(fps) + ".txt";
	//std::string ex_onset_spectrum_idx_save_path = music_folder_path + "ex_onset_idx_" + att + "_fps_" + std::to_string(fps) + ".txt";
	//saveTxt2DVec(score_onset_spectrum, score_onset_spectrum_idx_save_path);
	//saveTxt2DVec(ex_onset_spectrum, ex_onset_spectrum_idx_save_path);

}


void ScoreFollowing::getDTWPath(const int fps, const bool is_onset, const std::string att, const bool is_midi_analysis) {
	// 1: MIDI analysis (default(is_midi_analysis): false)
	// 2: getOnsetSpectrum(): include getSpectrum(), onsetDetectingAll()
	// 3: DTW (based on is_onset: full or onset)
	// 4: Save

	if (is_midi_analysis == false) {
		echo("MIDI analysis");
		getMidiToMidiAlignment();
	}

	echo("Get spectrum, onset detecting, and get onset spectrum");
	getOnsetSpectrum(fps, att);


	/* ================================
		 Add note on vector
		 Concatenate note vector 88-dim to CQT 88-dim (Ideally, should estimate the pitch of the sound)
	================================ */
	//int add_condition = 3;
	//for (int i = 0; i < score_onset_spectrum.size(); ++i) {
	//	auto t_vec = score_onset_spectrum[i];
	//	std::vector<int> note_vec(88);
	//	auto idx_vec = vec_op::argsort(t_vec);
	//	for (int j = 0; j < add_condition; ++j) {
	//		int idx = idx_vec.size() - j - 1;
	//		note_vec[idx_vec[idx]] = 1;
	//	}
	//	std::copy(note_vec.begin(), note_vec.end(), std::back_inserter(t_vec));	// concatenate note_vec to t_vec (= tvec <- note_vec)
	//	score_onset_spectrum[i] = t_vec;
	//}
	//for (int i = 0; i < ex_onset_spectrum.size(); ++i) {
	//	auto t_vec = ex_onset_spectrum[i];
	//	std::vector<double> note_vec(88, 0);
	//	auto idx_vec = vec_op::argsort(t_vec);
	//	for (int j = 0; j < add_condition; ++j) {
	//		int idx = idx_vec.size() - j - 1;
	//		note_vec[idx_vec[idx]] = 1.0;
	//	}
	//	std::copy(note_vec.begin(), note_vec.end(), std::back_inserter(t_vec));
	//	ex_onset_spectrum[i] = t_vec;
	//}

	echo("DTW");
	std::string dtw_save_path = music_folder_name + "_fps_" + std::to_string(fps) + ".txt";
	std::shared_ptr<DynamicTimeWarping> DTW = std::make_shared<DynamicTimeWarping>();
	if (is_onset == false) {
		dtw_save_path = "path_full_" + dtw_save_path;
		DTW->setInput(score_spectrum, ex_spectrum);
	}
	else {
		dtw_save_path = "path_onset_" + dtw_save_path;
		DTW->setInput(score_onset_spectrum, ex_onset_spectrum);
	}

	DTW->fastDTW();

	echo("Save path");
	auto path = DTW->getPath();
	//DTW->setSavePath(save_folder_path, dtw_save_path);
	DTW->setSavePath(music_folder_path, dtw_save_path);
	DTW->saveDTWPath();

}


void ScoreFollowing::makePathDetailData(const std::string score_info_path, const std::string onset_dtw_path,
										const std::string score_onset_path, const std::string ex_onset_path) {
	/* ================================
		Return:
			Path detail data
				(old) (score_onset_idx, ex_onset_idx, score_idx, ex_idx, score_msec, ex_msec, score_tempo, first_onset_flag)

				(new)
				(mnn, score_midi_onset, vel, tempo, ex_est_onset, ex_midi_onset, chord_idx, belief(0:match, 1:miss, 2:extra, 3:not_match),
				 path_onset_idx_score, path_onset_idx_ex, path_idx_score, path_idx_ex, msec_score, msec_ex, add_on_score, first_onset_flag)

		Input:
			score_info:		score_following.txt
			dtw_path:		path_onset_<music_folder_name>_fps_<num>.txt
			score_onset:	score_onset_fps_<num>.txt
			ex_osnet:		ex_onset_fps_<num>.txt
	================================ */

	auto score_info = readStdVector2DiTxt(score_info_path);
	auto onset_dtw = readStdVector2DiTxt(onset_dtw_path);
	auto score_onset = readStdVector2DiTxt(score_onset_path);
	auto ex_onset = readStdVector2DiTxt(ex_onset_path);

	std::vector<std::vector<int>> data;
	std::string ret = music_folder_path + "onset_path.txt";
	//echo(onset_dtw_path);
	int pre_score_idx = -1;
	int start_idx = 1;
	int pre_score_msec = 0;
	//for (auto dpath : onset_dtw) {
	//	// 0 start
	//	int score_onset_idx = dpath[0];
	//	int ex_onset_idx = dpath[1];
	//	int score_idx = score_onset[score_onset_idx][0];
	//	int score_msec = score_onset[score_onset_idx][1];
	//	int ex_idx = ex_onset[ex_onset_idx][0];
	//	int ex_msec = ex_onset[ex_onset_idx][1];
	//	//echoes(score_onset_idx, ex_onset_idx, score_idx, score_msec, ex_idx, ex_msec);

	//	// Since score onset might be added in onset detecting.
	//	int mnn = 0;
	//	int tempo = 0;
	//	for (int i = start_idx; i < score_info.size(); ++i) {
	//		int next_score_msec = score_info[i][1];
	//		if (pre_score_msec <= score_msec && score_msec <= next_score_msec) {
	//			mnn = score_info[i][0];
	//			tempo = score_info[i][3];
	//			pre_score_msec = score_msec;
	//			start_idx = i;
	//			break;
	//		}
	//	}

	//	std::vector<int> temp = { score_onset_idx, ex_onset_idx, score_idx, ex_idx, score_msec, ex_msec, mnn, tempo };

	//	if (pre_score_idx < score_onset_idx) {
	//		temp.emplace_back(1);
	//		pre_score_idx = score_onset_idx;
	//	}
	//	else temp.emplace_back(0);
	//	data.emplace_back(temp);
	//}

	int start_idx_path = 0;
	for (auto score : score_info) {
		int score_onset_from_midi = score[1];

		std::vector<std::vector<int>> same_onset_list;
		bool start_idx_update_flag = false;
		for (int i = start_idx_path; i < onset_dtw.size(); ++i) {
			// 0 start
			int score_onset_idx = onset_dtw[i][0];	// path_onset_<>_fps_<>.txt
			int ex_onset_idx = onset_dtw[i][1];

			int score_idx = score_onset[score_onset_idx][1];	// score_onset_fps_<>.txt
			int score_msec = score_onset[score_onset_idx][2];
			int score_add_onset_flag = score_onset[score_onset_idx][3];
			int ex_idx = ex_onset[ex_onset_idx][1];				// ex_onset_fps_<>.txt
			int ex_msec = ex_onset[ex_onset_idx][2];

			if (score_onset_from_midi == score_msec) {
				if (start_idx_update_flag == false) {
					start_idx_path = i;
					start_idx_update_flag = true;
				}
				std::vector<int> t_same_onset = { score_onset_idx, ex_onset_idx, score_idx, ex_idx,
					score_msec, ex_msec, score_add_onset_flag };
				if (same_onset_list.size() == 0) t_same_onset.emplace_back(1);
				else t_same_onset.emplace_back(0);
				same_onset_list.emplace_back(t_same_onset);
			}
			else if (score_onset_from_midi < score_msec) {
				break;
			}
		}
		for (auto slist : same_onset_list) {
			std::vector<int> temp(score);
			temp.insert(temp.end(), slist.begin(), slist.end());
			data.emplace_back(temp);
		}
	}


	saveTxt2DVec(data, ret);
}


void ScoreFollowing::tempoEstimation(const std::string music_folder, const std::string onset_path_path) {

	std::shared_ptr<TempoEstimation> TE = std::make_shared<TempoEstimation>(music_folder, onset_path_path);
	TE->execute();

}

void ScoreFollowing::addTempoInfo(const std::string music_folder, const std::string onset_path_path,
								  const std::string tempo_result_path, const bool is_show) {

	auto path_vec = readStdVector2DiTxt(onset_path_path);
	auto tempos = readStdVector2DTxt(tempo_result_path);

	std::vector<std::vector<double>> mod_onset_path;
	int n = 0;
	int pre_msec = -1;
	for (int i = 0; i < path_vec.size(); ++i) {
		std::vector<double> temp;
		for (int j = 0; j < path_vec[i].size(); ++j) {
			temp.emplace_back(double(path_vec[i][j]));
		}

		//if (path_vec[i][8] == 1) {
		if (pre_msec < path_vec[i][12]) {
			temp.emplace_back(tempos[n][0]);
			temp.emplace_back(tempos[n][1]);
			temp.emplace_back(tempos[n][2]);
			pre_msec = path_vec[i][12];
			++n;
		}
		//else {
		else if (pre_msec == path_vec[i][12]) {
			auto temp_pre = mod_onset_path.back();
			//temp.emplace_back(temp_pre[9]);
			//temp.emplace_back(temp_pre[10]);
			//temp.emplace_back(temp_pre[11]);
			temp.emplace_back(temp_pre[16]);
			temp.emplace_back(temp_pre[17]);
			temp.emplace_back(temp_pre[18]);
		}
		mod_onset_path.emplace_back(temp);
	}

	std::string save_mod_onset_path = music_folder + "mod_onset_path.txt";
	saveTxt2DVec(mod_onset_path, save_mod_onset_path);


	if (is_show == true) {
		std::shared_ptr<TempoEstimation> TE = std::make_shared<TempoEstimation>(music_folder, onset_path_path);
		TE->showResult();
	}
}


void ScoreFollowing::musicHSMM(const std::string music_folder, const std::string mod_onset_path_path, const bool is_show) {

	//std::shared_ptr<TempoEstimation> TE = std::make_shared<TempoEstimation>(music_folder, onset_path_path);
	//TE->execute();

	//if (is_show == true) {
	//	TE->showResult();
	//}
	echo(1);

}



void ScoreFollowing::evalPath(const std::unordered_map<std::string, std::string> path_dic, const int fps, const std::string eval_att) {
	/* ================================
	* Evaluated target: path_info
		[acc_score_msec, acc_ex_msec, path_score_msec, path_ex_msec]
	================================ */

	std::vector<std::vector<int>> path_info;
	if (eval_att == "onset_path") {
		// [mnn, score_midi_onset, vel, tempo, ex_est_onset, ex_midi_onset, chord_idx, belief(0:match, 1:miss, 2:extra, 3:not_match),
		//	path_onset_idx_score, path_onset_idx_ex, path_idx_score, path_idx_ex, msec_score, msec_ex, add_on_score, first_onset_flag]
		std::string path_info_path = path_dic.at(eval_att);
		auto t_path_info = readStdVector2DiTxt(path_info_path);

		// Extract no-duplicated chord_idx and ex_midi_onset is not minus and belief is 0
		int pre_msec = -1;
		for (int i = 0; i < t_path_info.size(); ++i) {
			auto score = t_path_info[i];
			if (pre_msec < score[1]) {
				if (score[7] == 0) {	// belief = match
					// [acc_score_on, acc_ex_on, path_score_on, path_ex_on]
					std::vector<int> path_one_info = { score[1], score[5], score[12], score[13] };
					path_info.emplace_back(path_one_info);

					for (int j = i + 1; j < t_path_info.size(); ++j) {	// Find other pathes to target i
						auto next_score = t_path_info[j];
						if (next_score[15] == 1) break;		// first onset flag = 1 = find next note number
						else {
							path_one_info = { next_score[1], next_score[5], next_score[12], next_score[13] };
							path_info.emplace_back(path_one_info);
						}
					}
					pre_msec = score[1];
				}
				else {	// belief = miss note, etc. -> Find same chord, if not, update pre_msec
					int t_cur_msec = score[1];
					for (int j = i + 1; j < t_path_info.size(); ++j) {
						auto next_score = t_path_info[j];
						if (next_score[7] == 0) {	// Next belief = match
							if (t_cur_msec == next_score[1]) {	// same chord
								std::vector<int> path_one_info = { next_score[1], next_score[5], next_score[12], next_score[13] };
								path_info.emplace_back(path_one_info);

								for (int k = j + 1; k < t_path_info.size(); ++k) {	// Find other pathes
									auto next_next_score = t_path_info[k];
									if (next_next_score[15] == 1) break;		// first onset flag = 1 = find next note number
									else {
										path_one_info = { next_next_score[1], next_next_score[5], next_next_score[12], next_next_score[13] };
										path_info.emplace_back(path_one_info);
									}
								}
								pre_msec = t_cur_msec;
								break;
							}
							else {	// Matched ex_note cannot be found
								std::vector<int> path_one_info = { score[1], -1, -1, -1 };
								path_info.emplace_back(path_one_info);
								pre_msec = t_cur_msec;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	std::string score_path = path_dic.at("score");
	auto score_info = readStdVector2DiTxt(score_path);

	//echo_v2(score_info);
	//echo_v2(path_info);

	// path_info: [acc_score_msec, acc_ex_msec, path_score_msec, path_ex_msec]
	int ex_idx = 0;
	int msec_of_one_frame = int(1000 / fps);
	std::vector<std::vector<int>> ret;
	for (auto one_path : path_info) {
		std::vector<int> temp(one_path);	// For output

		int path_score_msec = one_path[2];
		int path_ex_msec = one_path[3];
		int correct_ex_msec = one_path[1];

		// Find correct msec of score which is matched a msec of ex
		int correct_score_msec = 0;
		for (int i = ex_idx; i < score_info.size(); ++i) {
			int ex_msec = score_info[i][5];
			if (ex_msec - msec_of_one_frame <= path_ex_msec && path_ex_msec <= ex_msec + msec_of_one_frame) {
				correct_score_msec = score_info[i][1];
				ex_idx = i;
				break;
			}
			else if (path_ex_msec < ex_msec - msec_of_one_frame) {
				break;
			}
		}

		// In case of correct path: [acc_score_msec, acc_ex_msec, path_score_msec, path_ex_msec, 1]
		if (correct_ex_msec - msec_of_one_frame <= path_ex_msec && path_ex_msec <= correct_ex_msec + msec_of_one_frame) {
			temp.emplace_back(1);
		}

		// In case of miss path: [acc_score_msec, acc_ex_msec, path_score_msec, path_ex_msec, correct_score_msec]
		// If correct_score_msec is 0, this means the onset is missed in onset detecting.
		else {
			temp.emplace_back(correct_score_msec);
		}
		ret.emplace_back(temp);
	}

	int n_correct = 0;
	int n_exist_data = 0;

	int pre_msec = -1;
	for (int i = 0; i < ret.size(); ++i) {
		if (pre_msec < ret[i][0]) {
			pre_msec = ret[i][0];
			for (int j = i; j < ret.size(); ++j) {	// Whether includes accuracy path or not
				if (ret[i][0] == ret[j][0]) {
					if (ret[j][4] == 1) {
						++n_correct;
						++n_exist_data;
						break;
					}
				}
				else if (ret[i][0] < ret[j][0]) {
					++n_exist_data;
					break;
				}
			}
		}
	}

	echoes("Accuracy:", double(n_correct) / n_exist_data, "(", n_correct, n_exist_data, ")");

	// Save
	//std::string save_eval_path = save_folder_path + "eval_dtw_fps_" + std::to_string(fps) + ".txt";
	std::string save_eval_path = music_folder_path + "eval_dtw_fps_" + std::to_string(fps) + ".txt";
	saveTxt2DVec(ret, save_eval_path);
}


std::unordered_map<std::string, std::string> ScoreFollowing::getResultPath(const int fps, const std::string att) const {
	std::unordered_map<std::string, std::string> ret;

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
	ret["tempo_estimation_path"] = music_folder_path + "tempo_estimation.txt";
	ret["mod_onset_path"] = music_folder_path + "mod_onset_path.txt";

	return ret;

}



void ScoreFollowing::showEvalPianoroll(const std::string eval_path) {
	auto eval_result = readStdVector2DiTxt(eval_path);
	std::vector<std::vector<int>> plot_vec;
	for (auto vec : eval_result) {
		std::vector<int> temp_vec;
		if (vec.size() > 3) {
			//echo_v1(vec);
			if (vec[3] == -1) continue;		// Miss note
			if (vec.size() == 5) temp_vec = { vec[0], vec[1], vec[3], vec[4] };
			else if (vec.size() == 4) temp_vec = { vec[0], vec[1], vec[3], -1 };
			else {
				auto acc = vec[3];
				auto candidate = std::vector<int>(vec.begin() + 4, vec.end());
				int near_idx = 4 + nearIdxFinder(candidate, acc);
				//echo_v1(candidate);
				//echo(near_idx);
				temp_vec = { vec[0], vec[1], vec[3], vec[near_idx] };
				//echo_v1(temp_vec);
			}
			plot_vec.emplace_back(temp_vec);
		}
		else continue;
	}
	//echo_v2(plot_vec);
	std::string t_save_path = save_folder_path + "eval_show.txt";
	saveTxt2DVec(plot_vec, t_save_path);

	//PlotData PD;
	//PD.setData(plot_vec);
	//PD.setPlotType("eval_pianoroll");
	//std::vector<std::shared_ptr<PlotData>> data_vec;
	//data_vec.emplace_back(std::make_shared<PlotData>(PD));

	//MatplotlibCpp PLT(data_vec);
	//PLT.setSaveFolderPath(save_folder_path);
	//PLT.setSaveFileName("pianoroll");
	//PLT.setShowFlag(true);
	//PLT.showInterface();
	//PLT.execute();

}

void ScoreFollowing::testp() {
	echo(1);
}

