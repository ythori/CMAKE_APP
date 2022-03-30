
/* ================================
Created by Hori on 2021/7/16

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Particle filter
		Self-organizing state space model

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	x_t = Fx_{t-1} + Gv
	y_t = Hx_t + w

================================ */


#ifndef PARTICLE_FILTER_SHARED_H
#define PARTICLE_FILTER_SHARED_H

#include "pch.h"

Eigen::MatrixXd setF(const std::string type, const std::string model, const int dim);
Eigen::MatrixXd setG(const std::string type, const std::string model, const int dim, const int F_dim, const bool cal_prenoise);
Eigen::MatrixXd setH(const int dim, const int F_dim);


#endif //PARTICLE_FILTER_SHARED_H