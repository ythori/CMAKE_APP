cmake_minimum_required(VERSION 3.7)
project(C)

set(CMAKE_CXX_STANDARD 14)

#list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-find)

####################################################
#
# set(var, add): set varible as add
#
# find_package(find__, REQUIRED): search find__.cmake or usi cmakeconfig.make,
#								  if REQUIRED, the find module should be execute
#
# include_directories(add): include, can use <___.h> (追加のインクルードディレクトリ)
#	General rule: <name>_INCLUDE_DIRS:
#					: This should be a directory name included include files (ex) python.h)
#						ex) "add/include"
#				: <name>_LIBRARY_DIRS:
#					: This should be a directory name included library files (ex) ___.lib
#						ex) "add/Library" or "add/lib"
#				: <name>_LIBRARIES:
#					: These show ___.lib directlly
#						ex) "add/Library" or "add/lib"
#
# add_subdirectory(add): add directory, can use "./add/___.h"
#	Generarlly set the our created program library
#
# add_executable(project_name[_]exefile.cpp): add execution exefile.cpp as name
#
# link_directories(dirs)
#	: This is the same of "Add dependent directory (<>_LIBRARY_DIRS)" (追加のライブラリディレクトリ)
#
# target_link_libraries(project_name[_]libs): add lib to name (this name is same above)
#	: This is the same of "Add dependent file (<>_LIBRARIES)" (追加の依存ファイル)
#
#
# Since libraries like Eigen are included in this cmakelists, these declearations are not needed in every modules.
# However, to clear the required libraries, write the declearations.
#
####################################################


set(EXTERNAL_LIBRARIES "C:/Users/brume/Documents/Project/Cplusplus/ExternalLibraries/")

# For Eigen
set(EIGEN_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/eigen-3.3.9")
include_directories(${EIGEN_INCLUDE_DIRS})

# For boost
#set(Boost_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/boost_1_75_0/include/boost-1_75")
set(Boost_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/boost_1_75_0")
include_directories(${Boost_INCLUDE_DIRS})
set(Boost_LINK_DIRS "${EXTERNAL_LIBRARIES}/boost_1_75_0/stage/x86/lib")
link_directories(${Boost_LINK_DIRS})
#set(Boost_LINKER_LIB "${Boost_LINK_DIRS}/boost_numpy37-vc142-mt-gd-x32-1_75.dll")

# For GSL (not need include_directories?)
set(GSL_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/gsl_2.4/include")
include_directories(${GSL_INCLUDE_DIRS})
find_library(libcblas NAMES cblas.lib PATHS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.15.26726/atlmfc/lib/x64")
find_library(libgcl NAMES gsl.lib PATHS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.15.26726/atlmfc/lib/x64")

# Python
set(PYTHON_HOME "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python37_86")
#message(${PYTHON_HOME})
include_directories("${PYTHON_HOME}/include")
include_directories("${PYTHON_HOME}/Lib/site-packages/numpy/core/include")
#link_directories("${PYTHON_HOME}/Lib/site-packages")
#link_directories("${PYTHON_HOME}/Lib/site-packages/matplotlib")
link_directories(${PYTHON_HOME}/libs)


# Add my libraries
add_subdirectory(Shared)
add_subdirectory(_CreateTemplate)
add_subdirectory(ProbabilityFunctions)
add_subdirectory(MIDIModules)
add_subdirectory(DTW)
add_subdirectory(PythonMadmom)
add_subdirectory(Evaluation)
add_subdirectory(CrestMuseProcessor)
add_subdirectory(ScoreFollowing)
add_subdirectory(ParticleFilter)
add_subdirectory(TempoEstimation)

add_subdirectory(MusicPreciseAnalysis)


add_executable(Test _Execution/exeTest.cpp)
target_link_libraries(Test
 #       ${Boost_LINKER_LIB}
        modules
        createtemplate
        probabilityfunctions
        midi
        dtw
        madmom
        evaluation
        crestmuse
        scorefollowing
        particlefilter
        tempoestimation
        musicpreciseanalysis
        )
