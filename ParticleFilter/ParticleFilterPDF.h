
/* ================================
Created by Hori on 2021/7/16

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Particle filter
		Self-organizing state space model

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	x_t = Fx_{t-1} + Gv
	y_t = Hx_t + w

================================ */


#ifndef PARTICLE_FILTER_PDF_H
#define PARTICLE_FILTER_PDF_H

#include "pch.h"

double pdfIOI(const double obs_ioi, const double est_tempo, const double score_ioi, const double score_tempo, const double var);


#endif //PARTICLE_FILTER_PDF_H