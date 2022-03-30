cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(__ STATIC
        pch.cpp
        _Test.cpp
        _cls.cpp
        )



target_link_libraries(__
        modules
        )
