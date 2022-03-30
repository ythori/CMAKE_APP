cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
        ../ProbabilityFunctions
)

add_library(particlefilter STATIC
        pch.cpp
        _particleFilterTest.cpp
        ParticleFilter.cpp
        ParticleFilterShared.cpp
        ParticleFilterPDF.cpp)



target_link_libraries(particlefilter
        modules
        probabilityfunctions
        )
