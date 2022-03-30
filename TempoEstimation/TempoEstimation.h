
/* ================================
Created by Hori on 2021//

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    Tempo Estimation

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Set

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef TEMPO_ESTIMATION_H
#define TEMPO_ESTIMATION_H

#include "pch.h"

class TempoEstimation {
public:
	TempoEstimation();
	TempoEstimation::TempoEstimation(const std::string spath, const std::string path);

	std::vector<std::vector<double>> setInput();
	void execute();

	void showResult();


private:
	bool is;
	std::string save_folder_path, save_pf_result_path, save_mod_onset_path;
	std::string path_path;

	int step, dim;

	std::vector<std::vector<int>> onset_arr;

};



#endif //TEMPO_ESTIMATION_H

