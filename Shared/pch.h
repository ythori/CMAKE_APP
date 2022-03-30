// pch.h: プリコンパイル済みヘッダー ファイルです。
// 次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
// コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
// ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
// 頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。

#ifndef PCH_SHARED_H
#define PCH_SHARED_H

// プリコンパイルするヘッダーをここに追加します
//#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <iterator>
#include <memory>
#include <initializer_list>
#include <list>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <boost/version.hpp>
#include <functional>
#include <numeric>
#include <random>
#include <gsl/gsl_linalg.h>
#include <map>
#include <direct.h>
#include <chrono>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <cmath>

#endif //PCH_SHARED_H
