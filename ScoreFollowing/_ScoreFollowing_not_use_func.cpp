
//void ScoreFollowing::evalFullDTWPath(const int fps) {
//	//echoes(111, path_ex_score_full);
//	//echoes(222, est_alignment_txt_path);
//	std::string path_ex_score_full = save_folder_path + "path_full_" + music_folder_name + "_fps_" + std::to_string(fps) + ".txt";
//
//	auto full_path = readStdVector2DiTxt(path_ex_score_full);
//	auto acc_data = readStdVector2DiTxt(music_folder_path + "score_following.txt");
//	std::string t_save_path = save_folder_path + "eval_full_dtw_fps_" + std::to_string(fps) + ".txt";
//	std::vector<std::vector<int>> eval;
//
//	//echoes(full_path.size(), acc_data.size());
//
//	int n_acc = 0;
//	int msec_per_frame = int(1000. / fps);
//	for (auto acc : acc_data) {
//		int mnn = acc[0];
//		int score_onset = acc[1];
//		int ex_onset = acc[3];
//		int aligned_msec_ex = 0;
//		for (auto path : full_path){
//			int frame_idx_score = path[0];
//			int frame_idx_ex = path[1];
//
//			// If full_path[i][0]=1000, fps=50, 20[msec/f], i=20 -> From 950+25 to 1050-25
//			int start_msec_score = (frame_idx_score - 1) * msec_per_frame;
//			if (start_msec_score < 0) start_msec_score = 0;
//			int end_msec_score = (frame_idx_score + 1) * msec_per_frame;
//
//			int start_msec_ex = (frame_idx_ex - 1) * msec_per_frame;
//			if (start_msec_ex < 0) start_msec_ex = 0;
//			int end_msec_ex = (frame_idx_ex + 1) * msec_per_frame;
//
//			//echoes(start_msec_score, score_onset, end_msec_score, start_msec_ex, ex_onset, end_msec_ex);
//
//			if ((start_msec_score < score_onset) && (score_onset <= end_msec_score)) {
//				aligned_msec_ex = frame_idx_ex * msec_per_frame;
//				if ((start_msec_ex < ex_onset) && (ex_onset <= end_msec_ex)) {
//					++n_acc;
//					echoes("Match:", "Score", start_msec_score, score_onset, end_msec_score, "Ex", start_msec_ex, ex_onset, end_msec_ex);
//					std::vector<int> temp = { 1, mnn, score_onset, ex_onset, aligned_msec_ex };
//					eval.emplace_back(temp);
//					break;
//				}
//			}
//			if (score_onset < start_msec_score) {
//				echoes("Not match:", "Score", score_onset, "Ex", ex_onset);
//				std::vector<int> temp = { 0, mnn, score_onset, ex_onset, aligned_msec_ex };
//				eval.emplace_back(temp);
//				break;
//			}
//		}
//	}
//	echoes(n_acc, acc_data.size());
//	std::vector<int> temp = { -1, n_acc, int(acc_data.size()) };
//	eval.emplace_back(temp);
//	
//
//	//for (int i = 0; i < full_path.size() - 1; ++i) {
//	//	echoes(full_path[i][0] * msec_per_frame, full_path[i][1] * msec_per_frame);
//	//}
//	
//	saveTxt2DVec(eval, t_save_path);
//
//}



//void ScoreFollowing::evalOnsetDTWPath(const int fps) {
//	std::string path_ex_score_onset = save_folder_path + "path_onset_" + music_folder_name + "_fps_" + std::to_string(fps) + ".txt";
//	auto full_path = readStdVector2DiTxt(path_ex_score_onset);
//	auto acc_data = readStdVector2DiTxt(music_folder_path + "score_following.txt");
//	std::string t_save_path = save_folder_path + "eval_onset_dtw_fps_" + std::to_string(fps) + ".txt";
//	std::vector<std::vector<int>> eval;
//
//	//echoes(full_path.size(), acc_data.size());
//
//	int n_acc = 0;
//	int msec_per_frame = int(1000. / fps);
//	for (auto acc : acc_data) {
//		int mnn = acc[0];
//		int score_onset = acc[1];
//		int ex_onset = acc[3];
//		int aligned_msec_ex = 0;
//		std::vector<int> t_aligned_msec_ex;
//		for (auto path : full_path) {
//			int frame_idx_score = path[0];
//			int frame_idx_ex = path[1];
//
//			// If full_path[i][0]=1000, fps=50, 20[msec/f], i=20 -> From 950+25 to 1050-25
//			int start_msec_score = frame_idx_score  * msec_per_frame - msec_per_frame;
//			if (start_msec_score < 0) start_msec_score = 0;
//			int end_msec_score = frame_idx_score * msec_per_frame + msec_per_frame;
//
//			int start_msec_ex = frame_idx_ex * msec_per_frame - msec_per_frame * 2;
//			if (start_msec_ex < 0) start_msec_ex = 0;
//			int end_msec_ex = frame_idx_ex * msec_per_frame + msec_per_frame * 2;
//
//			//echoes(start_msec_score, score_onset, end_msec_score, start_msec_ex, ex_onset, end_msec_ex);
//			if ((start_msec_score < score_onset) && (score_onset <= end_msec_score)) {
//				aligned_msec_ex = frame_idx_ex * msec_per_frame;
//				t_aligned_msec_ex.emplace_back(aligned_msec_ex);
//				if ((start_msec_ex < ex_onset) && (ex_onset <= end_msec_ex)) {
//					++n_acc;
//					echoes("Match:", "Score", start_msec_score, score_onset, end_msec_score, "Ex", start_msec_ex, ex_onset, end_msec_ex);
//					std::vector<int> temp = { 1, mnn, score_onset, ex_onset, aligned_msec_ex };
//					eval.emplace_back(temp);
//					break;
//				}
//			}
//			if (score_onset < start_msec_score) {
//				echoes("Not match:", "Score", score_onset, "Ex", ex_onset);
//				//std::vector<int> temp = { 0, score_onset, ex_onset, aligned_msec_ex };
//				std::vector<int> temp = { 0, mnn, score_onset, ex_onset };
//				for (auto tama : t_aligned_msec_ex) temp.emplace_back(tama);
//				eval.emplace_back(temp);
//				break;
//			}
//		}
//	}
//	echoes(n_acc, acc_data.size());
//	std::vector<int> temp = { -1, n_acc, int(acc_data.size()) };
//	eval.emplace_back(temp);
//
//
//	//for (int i = 0; i < full_path.size() - 1; ++i) {
//	//	echoes(full_path[i][0] * msec_per_frame, full_path[i][1] * msec_per_frame);
//	//}
//
//	saveTxt2DVec(eval, t_save_path);
//
//}

//void ScoreFollowing::evalDTWPath(const int fps, const bool is_onset) {
//	std::string att;
//	if (is_onset == true) att = "onset_";
//	else att = "full_";
//	std::string path_ex_score = save_folder_path + "path_" + att + music_folder_name + "_fps_" + std::to_string(fps) + ".txt";
//	std::string t_save_path = save_folder_path + "eval_" + att + "dtw_fps_" + std::to_string(fps) + ".txt";
//
//	std::string ex_onset_save_path = music_folder_path + "ex_onset_fps_" + std::to_string(fps) + ".txt";
//	std::string score_onset_save_path = music_folder_path + "score_onset_fps_" + std::to_string(fps) + ".txt";
//
//	auto dtw_path = readStdVector2DiTxt(path_ex_score);
//	auto acc_data = readStdVector2DiTxt(music_folder_path + "score_following.txt");
//
//	auto onset_ex_data = readStdVector2DiTxt(ex_onset_save_path);	// (idx, msec)
//	auto onset_score_data = readStdVector2DiTxt(score_onset_save_path);	// (idx, msec)
//
//	std::vector<std::vector<int>> eval;
//
//	//echoes(full_path.size(), acc_data.size());
//
//	int n_acc = 0;
//	int msec_per_frame = int(1000. / fps);
//	int idx_score_onset = 0;
//	int idx_ex_onset = 0;
//	for (auto acc : acc_data) {	// mnn, on_score, vel, tempo, est_ex_on, ex_on, c_idx, belief
//		int mnn = acc[0];
//		int score_onset = acc[1];
//		int ex_onset = acc[5];
//		int aligned_msec_ex = 0;
//
//		for (int i = idx_score_onset; i < onset_score_data.size() - 1; ++i) {
//			int msec_i = onset_score_data[i][1] - score_onset;
//			int msec_ip1 = onset_score_data[i + 1][1] - score_onset;
//			if (msec_i * msec_i <= msec_ip1 * msec_ip1) {
//				idx_score_onset = i;
//				break;
//			}
//		}
//		for (int i = idx_ex_onset; i < onset_ex_data.size() - 1; ++i) {
//			int msec_i = onset_ex_data[i][1] - ex_onset;
//			int msec_ip1 = onset_ex_data[i + 1][1] - ex_onset;
//			if (msec_i * msec_i < msec_ip1 * msec_ip1) {
//				idx_ex_onset = i;
//				break;
//			}
//		}
//
//		std::vector<int> t_aligned_msec_ex;
//
//		for (auto path : dtw_path) {
//			int frame_idx_score = path[0];
//			int frame_idx_ex = path[1];
//
//			// If full_path[i][0]=1000, fps=50(=20[msec/f]), i=20 -> From 950+25 to 1050-25
//			int start_msec_score = onset_score_data[frame_idx_score][1] - msec_per_frame;
//			if (start_msec_score < 0) start_msec_score = 0;
//			int end_msec_score = onset_score_data[frame_idx_score][1] + msec_per_frame;
//
//			int start_msec_ex = onset_ex_data[frame_idx_ex][1] - msec_per_frame;
//			if (start_msec_ex < 0) start_msec_ex = 0;
//			int end_msec_ex = onset_ex_data[frame_idx_ex][1] + msec_per_frame;
//
//			//echoes(start_msec_score, score_onset, end_msec_score, start_msec_ex, ex_onset, end_msec_ex);
//
//			if ((start_msec_score < score_onset) && (score_onset <= end_msec_score)) {	// Match
//				aligned_msec_ex = onset_ex_data[frame_idx_ex][0] * msec_per_frame;
//
//				if (is_onset == true) t_aligned_msec_ex.emplace_back(aligned_msec_ex);	// Alinged ex onset
//
//				if ((start_msec_ex < ex_onset) && (ex_onset <= end_msec_ex)) {
//					++n_acc;
//					echoes("Match:", "Score", start_msec_score, score_onset, end_msec_score, "Ex", start_msec_ex, ex_onset, end_msec_ex);
//					std::vector<int> temp = { 1, mnn,
//											  score_onset,
//											  ex_onset,
//											  idx_score_onset, idx_ex_onset,
//											  aligned_msec_ex };
//					eval.emplace_back(temp);
//					break;
//				}
//			}
//
//			if (score_onset < start_msec_score) {	// Not found
//				bool is_fail_analysis = false;
//				//echoes(111, frame_idx_ex);
//				for (auto fpath : dtw_path) {
//					int fail_frame_idx_score = fpath[0];
//					int fail_frame_idx_ex = fpath[1];
//					//echoes(222, fail_frame_idx_ex);
//
//					if (fail_frame_idx_ex == frame_idx_ex) {
//						int fail_aligned_msec_score = onset_score_data[fail_frame_idx_score][0] * msec_per_frame;
//						echoes("Not match:", "Score", score_onset, "Ex", ex_onset, "Fail_score", fail_aligned_msec_score);
//						std::vector<int> temp = { 0, mnn,
//												  score_onset,
//												  ex_onset,
//												  idx_score_onset, idx_ex_onset,
//												  fail_aligned_msec_score };
//						if (is_onset == true) {
//							// If full, tame too many, then onset only
//							for (auto tame : t_aligned_msec_ex) temp.emplace_back(tame);
//						}
//						eval.emplace_back(temp);
//						is_fail_analysis = true;
//						break;
//					}
//				}
//
//				//echoes("Not match:", "Score", score_onset, "Ex", ex_onset);
//				//std::vector<int> temp;
//				//if (is_onset == true) {
//				//	temp = { 0, mnn,
//				//			 score_onset, frame_idx_score, start_msec_score, end_msec_score,
//				//			 ex_onset, frame_idx_ex, start_msec_ex, end_msec_ex };
//				//	for (auto tame : t_aligned_msec_ex) temp.emplace_back(tame);	// If full, tame too many, then onset only
//
//				if (is_fail_analysis == true) break;
//				//else temp = { 0, mnn,
//				//			 score_onset, frame_idx_score, start_msec_score, end_msec_score,
//				//			 ex_onset, frame_idx_ex, start_msec_ex, end_msec_ex,
//				//			 aligned_msec_ex };
//				//eval.emplace_back(temp);
//			}
//		}
//		++idx_score_onset;
//		++idx_ex_onset;
//	}
//	echoes(n_acc, acc_data.size(), n_acc*100./acc_data.size());
//	std::vector<int> temp = { -1, n_acc, int(acc_data.size()) };
//	eval.emplace_back(temp);
//
//	//for (int i = 0; i < full_path.size() - 1; ++i) {
//	//	echoes(full_path[i][0] * msec_per_frame, full_path[i][1] * msec_per_frame);
//	//}
//
//	saveTxt2DVec(eval, t_save_path);
//}

