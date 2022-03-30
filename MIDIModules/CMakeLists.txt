cmake_minimum_required(VERSION 3.7)

include_directories(
        ../Shared
)

add_library(midi STATIC
        pch.cpp
        Binasc.cpp
        MidiEvent.cpp
        MidiEventList.cpp
        MidiFile.cpp
        MidiMessage.cpp
        MIDIProcessor.cpp
        MIDISharedFunctions.cpp
        Options.cpp
        _midiTest.cpp
        )



target_link_libraries(midi
        modules
        )
