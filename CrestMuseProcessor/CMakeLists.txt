cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
        ../DTW
        ../MIDIModules
)

add_library(crestmuse STATIC
        pch.cpp
        _crestmuseTest.cpp
        CrestMuse.cpp
        )



target_link_libraries(crestmuse
        modules
        dtw
        midi
        )
