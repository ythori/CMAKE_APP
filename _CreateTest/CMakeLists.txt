cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(createtest STATIC
        pch.cpp
        _createTestTest.cpp
        CreateTest.cpp
        )

target_link_libraries(createtest
        modules
        )
