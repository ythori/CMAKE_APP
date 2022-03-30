cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(evaluation STATIC
        pch.cpp
        _evaluationTest.cpp
        evaluationSystem.cpp
        )



target_link_libraries(evaluation
        modules
        )
