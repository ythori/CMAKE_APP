cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
        ../PythonMadmom
        ../DTW
        ../MIDIModules
        ../CrestMuseProcessor
        ../TempoEstimation
        )

add_library(scorefollowing STATIC
        pch.cpp
        _scoreFollowingTest.cpp
        ScoreFollowing.cpp)



target_link_libraries(scorefollowing
        modules
        madmom
        dtw
        midi
        crestmuse
        tempoestimation
        )
