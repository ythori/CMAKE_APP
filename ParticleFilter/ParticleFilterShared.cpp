

#include "ParticleFilterShared.h"
#include "pch.h"


Eigen::MatrixXd setF(const std::string type, const std::string model, const int dim) {
	/* ================================
		x_t = Fx_{t-1} + e
		dim = 1: x_t
		dim = 2: x_t, y_t
	================================ */

	int F_dim = 1;
	Eigen::MatrixXd F = Eigen::MatrixXd::Zero(F_dim, F_dim);
	F << 1;		// x_t = x_{t-1} + e

	if (model == "second_order_difference") {	// x_t, x_{t-1}
		/* ================================
			x_t - x_{t-1} = x_{t-1} - x_{t-2}
				:	[  x_t  ]= [2, -1][x_{t-1}] + [e]
					[x_{t-1}]  [1,  0][x_{t-2}] + [e]
		================================ */
		++F_dim;
		F = Eigen::MatrixXd::Zero(F_dim, F_dim);
		F << 2, -1,
			 1, 0;
	}

	if (dim > 1) {	// Add y_t (and y_{t-1}, ...)
		/* ================================
			ex)	[  x1_t  ]= [2, -1, 0,  0][x1_{t-1}] + [e]
				[x1_{t-1}]  [1,  0, 0,  0][x1_{t-2}] + [e]
				[  x2_t  ]  [0,  0, 2, -1][x2_{t-1}] + [e]
				[x2_{t-1}]  [0,  0, 1,  0][x2_{t-2}] + [e]
		================================ */
		int predim = F_dim;
		F_dim *= dim;
		Eigen::MatrixXd temp_F(F);
		F = Eigen::MatrixXd::Zero(F_dim, F_dim);
		F.block(0, 0, predim, predim) = temp_F;
		for (int i = 1; i < dim; ++i) {
			F.block(predim * i, predim * i, predim, predim) = F.block(predim * (i - 1), predim * (i - 1), predim, predim);
		}
	}

	if ((type == "standard") || (type == "self_organized")) {	// Add lntau2, lnsigma2
		/* ================================
			ex)	[  x1_t  ]= [2, -1, 0,  0, 0, 0][x1_{t-1}] + [e]
				[x1_{t-1}]  [1,  0, 0,  0, 0, 0][x1_{t-2}] + [e]
				[  x2_t  ]  [0,  0, 2, -1, 0, 0][x2_{t-1}] + [e]
				[x2_{t-1}]  [0,  0, 1,  0, 0, 0][x2_{t-2}] + [e]
				[  t2_t ]   [0,  0, 0,  0, 1, 0][t2_{t-1}] + [e]
				[  s2_t ]   [0,  0, 0,  0, 0, 1][s2_{t-1}] + [e]
		================================ */
		int predim = F_dim;
		F_dim = F_dim + 2;
		Eigen::MatrixXd temp_F(F);
		F = Eigen::MatrixXd::Zero(F_dim, F_dim);
		F.block(0, 0, predim, predim) = temp_F;
		if (type == "self_organized") {
			F(F_dim - 2, F_dim - 2) = 1;
			F(F_dim - 1, F_dim - 1) = 1;
		}
	}

	return F;
}

Eigen::MatrixXd setG(const std::string type, const std::string model, const int dim, const int F_dim, const bool cal_prenoise) {
	/* ================================
		x_t = Fx_{t-1} + Gv
		G_row_dim = x_t.size() = F_dim
		G_col = (x_t, t2, s2)
	================================ */

	int G_dim = 1;
	Eigen::MatrixXd G = Eigen::MatrixXd::Zero(F_dim, dim + 2);
	G(0, 0) = 1;

	if (model == "second_order_difference") {	// x_t, x_{t-1}
		/* ================================
			v: System noise (shared by x_t and x_{t-1})
			[  x_t  ]= [2, -1][x_{t-1}] + [1, 0, 0][v_x]
			[x_{t-1}]  [1,  0][x_{t-2}] + [1, 0, 0][t2n_t]
												   [s2n_t]
		================================ */
		++G_dim;
		if (cal_prenoise == true) G(1, 0) = 1;
	}

	if (dim > 1) {
		/* ================================
			ex)	[  x1_t  ]= [2, -1, 0,  0][x1_{t-1}] + [1, 0, 0, 0][v_x1]
				[x1_{t-1}]  [1,  0, 0,  0][x1_{t-2}] + [1, 0, 0, 0][v_x2]
				[  x2_t  ]  [0,  0, 2, -1][x2_{t-1}] + [0, 1, 0, 0][t2n_t]
				[x2_{t-1}]  [0,  0, 1,  0][x2_{t-2}] + [0, 1, 0, 0][s2n_t]
		================================ */
		int predim = G_dim;
		for (int i = 1; i < dim; ++i) {
			G.block(predim * i, i, predim, 1) = G.block(predim * (i - 1), i - 1, predim, 1);
		}
	}

	if ((type == "standard") || (type == "self_organized")) {
		/* ================================
			ex)	[  x1_t  ]= [2, -1, 0,  0, 0, 0][x1_{t-1}] + [1, 0, 0, 0][v_x1]
				[x1_{t-1}]  [1,  0, 0,  0, 0, 0][x1_{t-2}] + [1, 0, 0, 0][v_x2]
				[  x2_t  ]  [0,  0, 2, -1, 0, 0][x2_{t-1}] + [0, 1, 0, 0][t2n_t]
				[x2_{t-1}]  [0,  0, 1,  0, 0, 0][z1_{t-2}] + [0, 1, 0, 0][s2n_t]
				[  t2_t ]   [0,  0, 0,  0, 1, 0][t2_{t-1}] + [0, 0, 1, 0]
				[  s2_t ]   [0,  0, 0,  0, 0, 1][s2_{t-1}] + [0, 0, 0, 1]
		================================ */
		//G.conservativeResize(F_dim, G_dim);
		G(F_dim - 2, dim) = 1;
		G(F_dim - 1, dim + 1) = 1;
	}
	return G;
}

Eigen::MatrixXd setH(const int dim, const int F_dim) {
	/* ================================
		y_t = Hx_t + w
			ex) [y1_t] = [1, 0, 0, 0, 0, 0][  x1_t  ] + [e]
				[y2_t]	 [0, 0, 1, 0, 0, 0][x1_{t-1}] + [e]
										   [  x2_t  ] + [e]
										   [x2_{t-1}] + [e]
										   [  t2_t  ] + [e]
										   [  s2_t  ] + [e]
	================================ */
	Eigen::MatrixXd H = Eigen::MatrixXd::Zero(dim, F_dim);
	for (int i = 0; i < dim; ++i) H(i, 2 * i) = 1;
	return H;
}
