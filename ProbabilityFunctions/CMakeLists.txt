cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(probabilityfunctions STATIC
        pch.cpp
        _probabilityFunctionsTest.cpp
        ProbabilityFunctions.cpp
        GenRandomNumber.cpp
        )



target_link_libraries(probabilityfunctions
        modules
        )
