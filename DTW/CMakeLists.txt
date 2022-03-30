cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(dtw STATIC
        pch.cpp
        DynamicTimeWarping.cpp
        _dtwTest.cpp
        )



target_link_libraries(dtw
        modules
        )
