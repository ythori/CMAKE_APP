cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(createtemplate STATIC
        pch.cpp
        _createTemplateTest.cpp
        CreateTemplate.cpp
        )



target_link_libraries(createtemplate
        modules
        )
