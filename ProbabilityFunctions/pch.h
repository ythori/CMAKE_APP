// pch.h: プリコンパイル済みヘッダー ファイルです。
// 次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
// コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
// ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
// 頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。

#ifndef PCH_PROBABILITY_FUNCTIONS_H
#define PCH_PROBABILITY_FUNCTIONS_H

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/dense>
#include <boost/math/special_functions/digamma.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/gamma_distribution.hpp>
#include <boost/random/beta_distribution.hpp>
#include <boost/random/cauchy_distribution.hpp>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


#endif //PCH_PROBABILITY_FUNCTIONS_H
