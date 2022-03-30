/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Evaluation

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	TP: (Ground truth, Detected onset): (1, 1)
	FN: (Ground truth, Detected onset): (1, 0)
	FP: (Ground truth, Detected onset): (0, 1)

	Recall(R)    = TP / (TP + FN)
	Precision(P) = TP / (TP + FP)
	F value = 2R*P / (R + P)

================================ */

#ifndef EVALUATION_SYSTEM_H
#define EVALUATION_SYSTEM_H


#include "pch.h"

double getRecall(const int TP, const int FN);
double getPrecision(const int TP, const int FP);
double getFValue(const double Recall, const double Precision);


/* ================================
	Compare grand truth and estimated data
================================ */
void evaluationMatching(const std::vector<double> data1, const std::vector<double> data2, const double tolerance);


/* ================================
	Compare grand truth set of label & data and estimated set of them
		tolerance:	0: Exact match
					1: Partial match, 1 or more
					n: Partial match, n or more
					-n: Partial match, if N(>n) label is had, N-n or more
					(2./3.): Partial match, if N label is had, (N * 2/3) or more (if N*2/3=0, set 1)
================================ */
bool labelMatching(const std::vector<int> gt_label, const std::vector<int> est_label, const double tolerance);
void evaluationMatchingWithLabel(const std::vector<std::vector<int>> gt_label, const std::vector<std::vector<int>> est_label,
								 const std::vector<std::pair<int, int>> path, const double tolerance);





#endif // EVALUATION_H
