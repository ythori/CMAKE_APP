cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(madmom STATIC
        pch.cpp
        Madmom.cpp
        _madmomTest.cpp
        )



target_link_libraries(madmom
        modules
        )
