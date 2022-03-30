cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
        ../ParticleFilter
)

add_library(tempoestimation STATIC
        pch.cpp
        _tempoEstimationTest.cpp
        TempoEstimation.cpp
        )

target_link_libraries(tempoestimation
        modules
        particlefilter
        )
