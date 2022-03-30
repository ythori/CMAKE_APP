cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(musicpreciseanalysis STATIC
        pch.cpp
        _musicPreciseAnalysisTest.cpp
        MusicPreciseAnalysis.cpp
        )



target_link_libraries(musicpreciseanalysis
        modules
        scorefollowing
        )
