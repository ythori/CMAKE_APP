cmake_minimum_required(VERSION 3.7)


add_library(modules STATIC
        pch.cpp
        DataProc.cpp
        MathProc.cpp
		Constants.cpp
        ConstantParams.h
        vec_op.h
        _moduleTest.cpp
        passPyCpp.cpp
        matplotlibCpp.cpp
        genToyData.cpp)


target_link_libraries(modules
		${libcblas}
		${libgcl}
        )
