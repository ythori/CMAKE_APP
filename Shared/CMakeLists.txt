cmake_minimum_required(VERSION 3.7)

#set(EIGEN_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/eigen-3.3.9")
#include_directories(${EIGEN_INCLUDE_DIRS})

#set(Boost_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/boost_1_75_0/include/boost-1_75")
#include_directories(${Boost_INCLUDE_DIRS})

#set(GSL_INCLUDE_DIRS "${EXTERNAL_LIBRARIES}/gsl_2.4/include")
#include_directories(${GSL_INCLUDE_DIRS})
#find_library(libcblas NAMES cblas.lib PATHS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.15.26726/atlmfc/lib/x64")
#find_library(libgcl NAMES gsl.lib PATHS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.15.26726/atlmfc/lib/x64")

#set(PYTHONHOME "C:/Python27")
#set(PYTHONHOME "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python37_86")
#include_directories("${PYTHON_HOME}/include")
#include_directories("${PYTHON_HOME}/Lib/site-packages/numpy/core/include")
#link_directories(${PYTHON_HOME}/libs)
#find_library(pythonh NAMES python37.lib PATHS "${PYTHON_HOME}/libs")
#message(${PYTHON_HOME})

add_library(modules STATIC
        pch.cpp
        DataProc.cpp
        MathProc.cpp
		Constants.cpp
        vec_op.h
        _moduleTest.cpp
        passPyCpp.cpp
        matplotlibCpp.cpp
        genToyData.cpp)


target_link_libraries(modules
		${libcblas}
		${libgcl}
#        ${Boost_LINKER_LIB}
 #       ${pythonh}
        )
