#include "pch.h"
#include "evaluationSystem.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"
//#include "../Shared/vec_op.h"

double getRecall(const int TP, const int FN) {
	return double(TP) / (TP + FN);
}

double getPrecision(const int TP, const int FP) {
	return double(TP) / (TP + FP);
}

double getFValue(const double Recall, const double Precision) {
	return 2 * Recall * Precision / (Recall + Precision);
}


void evaluationMatching(const std::vector<double> data1, const std::vector<double> data2, const double tolerance) {
	int TP = 0;
	int FN = 0;
	int FP = 0;

	std::vector<int> confirmed_idx_data1, confirmed_idx_data2;

	for (int i = 0; i < data1.size(); ++i) {
		//if (i > 20) break;
		double gt = data1[i];
		bool is_find = false;
		for (int j = 0; j < data2.size(); ++j) {
			if (idxFinder(confirmed_idx_data2, j) == true) continue;;

			double est = data2[j];
			if ((gt - tolerance <= est) && (est <= gt + tolerance)) {
				++TP;
				confirmed_idx_data2.emplace_back(j);
				is_find = true;
				break;
			}
			else if (gt + tolerance < est) break;
		}
		if (is_find == false) ++FN;
		//echo(FN);
	}

	for (int i = 0; i < data2.size(); ++i) {
		int est = data2[i];
		bool is_find = false;
		for (int j = 0; j < data1.size(); ++j) {
			if (idxFinder(confirmed_idx_data1, j) == true) continue;;

			int gt = data1[j];
			if ((est - tolerance <= gt) && (gt <= est + tolerance)) {
				confirmed_idx_data1.emplace_back(j);
				is_find = true;
				break;
			}
			else if (est + tolerance < gt) break;
		}
		if (is_find == false) ++FP;
	}

	auto Recall = getRecall(TP, FN);
	auto Precision = getPrecision(TP, FP);
	auto Fvalue = getFValue(Recall, Precision);

	echoes(TP, FN, FP);
	echoes(Recall, Precision, Fvalue);
}

bool labelMatching(const std::vector<int> gt_label, const std::vector<int> est_label, const double tolerance) {
	echo_v1(gt_label);
	echo_v1(est_label);
	int n_gt = 0;
	int n_true_match = 0;
	int n_false_match = 0;
	int n_loss = 0;
	int n_extra = 0;
	int n_error = 0;	// Just in case
	for (int i = 0; i < gt_label.size(); ++i) {
		if (gt_label[i] == 1) {
			++n_gt;
			if (est_label[i] == 1) ++n_true_match;
			else if (est_label[i] == 0) ++n_loss;
			else ++n_error;
		}
		else if (gt_label[i] == 0) {
			if (est_label[i] == 1) ++n_extra;
			else if (est_label[i] == 0) ++n_false_match;
			else ++n_error;
		}
		else ++n_error;
	}
	echoes(n_gt, n_true_match, n_false_match, n_loss, n_extra, n_error);

	bool is_match = false;
	if (n_gt < 3) {
		if (n_true_match + n_false_match == gt_label.size()) is_match = true;
	}

	else {
		if (tolerance == 0) {
			// Exact match
			if (n_true_match + n_false_match == gt_label.size()) is_match = true;
		}

		else if (tolerance < 0) {
			// N - n
			int positive_tolerance = int(abs(tolerance));
			int n_required = int(n_gt - positive_tolerance);
			if (n_required < 2) n_required = 2;
			if (n_true_match >= n_required) {
				if ((n_loss <= positive_tolerance) && (n_extra <= positive_tolerance)) is_match = true;
			}
		}

		else if (tolerance >= 1) {
			// N_match >= n
			int n_required = MIN2(n_gt, int(tolerance));
			if (n_true_match >= n_required) {
				int n_not_match = n_gt - int(tolerance);
				if (n_not_match < 0) n_not_match = 0;
				if ((n_loss <= n_not_match) && (n_extra <= n_not_match)) is_match = true;
			}
		}

		else {
			int n_required = int(n_gt * tolerance);
			if (n_required < 2) n_required = 2;
			int n_less_required = n_gt - n_required;
			if (n_less_required < 1) n_less_required = 1;
			echoes(n_required, n_less_required);
			if (n_true_match >= n_required) {
				if ((n_loss <= n_less_required) && (n_extra <= n_less_required)) is_match = true;
			}
		}

	}
	return is_match;
}

void evaluationMatchingWithLabel(const std::vector<std::vector<int>> gt_label, const std::vector<std::vector<int>> est_label,
								 const std::vector<std::pair<int, int>> path, const double tolerance) {
	//echoes(gt_label.size(), est_label.size(), path.size());
	//echo_v2(gt_label);
	//std::cout << std::endl;
	//echo_v2(est_label);
	//std::cout << std::endl;
	//for (auto p : path) echoes(p.first, p.second);
	//echo(tolerance);
	
	for (auto p : path) {
		auto gt_label_data = gt_label[p.first];
		auto est_label_data = est_label[p.second];
		auto is_match = labelMatching(gt_label_data, est_label_data, tolerance);
		echoes("match:", is_match);
	}
}
