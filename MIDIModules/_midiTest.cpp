

#include "pch.h"
#include "_midiTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "MIDIProcessor.h"
#include "MIDISharedFunctions.h"

void midiTest() {

    midiAnalyzerTest();

    //midiFunctionsTest();
}


void midiAnalyzerTest() {
	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
    std::string crest = DA.getMusicDB() + "crestmuse/";
    std::string mididata_folder = crest + "dataset/bac-inv001-schif-g/";
    //std::string mididata = mididata_folder + "ex.mid";
    std::string mididata = mididata_folder + "score.mid";
    DA.resetSaveFolder();   // Result folder
    DA.setSaveFolder("test_midi");     // result_folder/designed_folder/
    auto save_file_path = DA.getSavePath("test_score_onset.txt");

    mididata = crest + "dataset/_fortest/score_2ch.mid";

    echo(mididata);

    MIDIAnalyzer analyzer;
    analyzer.analyzeMIDI(mididata);
    auto midi_property = analyzer.getMIDIProperty();
    midi_property->showProperties();

    auto noteon_event = midi_property->getNoteEvent();   // std::vector<std::shared_ptr<NoteProperty>>
    std::vector<int> onset_msec(noteon_event.size());
    //for (int i = 0; i < noteon_event.size(); ++i) {
    //    onset_msec[i] = noteon_event[i]->getOnsetMsec();
    //}
    //saveTxt1DVec(onset_msec, save_file_path);

    //int unit_msec = 10;
    //analyzer.makePianorollBasedOnMsec(unit_msec);
    //analyzer.writePianorollBasedOnMsec(save_file_path);

    //MIDIGenerator generator;
    //generator.setMIDIFromPianoroll(save_file_path, unit_msec);
    //auto save_midi_file_path = DA.getSavePath("test_piano_roll.mid");
    //generator.writeMIDI(save_midi_file_path);

}

void midiFunctionsTest() {
    // timesig: vec{ start_tick , end_tick, ts_num, ts_den }
    // tempo: vec{ start_tick, end_tick, start_msec, end_msec, tempo_micro, tempo_bpm }

    // Generate sample data
    int res = 480;
    std::vector<std::vector<int>> timesig, tempo;
    std::vector<int> temp = { 0, 1920, 4, 4 };  // bar 1
    timesig.emplace_back(temp);
    temp = { 1920, 4080, 3, 8 };    // 1920+720*3=1920+2160=4080, 1beat=240, bar 2-4
    timesig.emplace_back(temp);
    temp = { 4080, 6000, 4, 4 };    // 1920+720*3=1920+2160=4080, 1beat=480, bar 5
    timesig.emplace_back(temp);

    temp = { 0, 1920, 0, 2000, 0, 120 };    // bar1,beat1 - bar1,beat4, tempo120: 1beat = 500msec -> 2000msec
    tempo.emplace_back(temp);
    temp = { 1920, 2880, 2000, 4000, 0, 60 };   // bar2,beat1 - bar2,beat2, tempo60: 1beat = 1000msec -> 2000+2000 = 4000msec
    tempo.emplace_back(temp);
    temp = { 2880, 3840, 4000, 5000, 0, 120 };   // bar2,beat3 - bar2,beat4, tempo120: 500ms/b -> 4000+1000 = 5000msec
    tempo.emplace_back(temp);
    temp = { 3840, 5760, 5000, 9000, 0, 60 };  // bar3,beat1 - bar3,beat4, tempo60: 1000ms/b -> 5000+4000 = 9000msec
    tempo.emplace_back(temp);


    // beat to tick
    int test_measure1 = 2;
    int test_beat1 = 2333;
    auto tick1 = midif::beat2Tick(test_measure1, test_beat1, res, timesig);     // 1920 + 240*(2-1) + 240*(333/1000) = 1920+240+80=2240

    int test_measure2 = 4;
    int test_beat2 = 3666;
    auto tick2 = midif::beat2Tick(test_measure2, test_beat2, res, timesig);     // 1920 + 720*2 + 240*(3-1) + 240*(666/1000) = 1920+1440+480+160=4000

    int test_measure3 = 5;
    int test_beat3 = 1250;
    auto tick3 = midif::beat2Tick(test_measure3, test_beat3, res, timesig);     // 1920 + 720*3 + 480*(1-1) + 480*(250/1000) = 1920+2160+0+120=4200

    echoes(tick1, tick2, tick3);    // 2240, 4000, 4200


    // tick to msec
    int test_tick1 = 960;
    auto msec1 = midif::tick2Msec(test_tick1, res, tempo);   // 1000

    int test_tick2 = 2400;
    auto msec2 = midif::tick2Msec(test_tick2, res, tempo);   // 3000

    int test_tick3 = 4800;
    auto msec3 = midif::tick2Msec(test_tick3, res, tempo);   // 7000

    echoes(msec1, msec2, msec3);


    // msec to tick
    int test_msec1 = 1000;
    tick1 = midif::msec2Tick(test_msec1, res, tempo);   // 960

    int test_msec2 = 3000;
    tick2 = midif::msec2Tick(test_msec2, res, tempo);   // 2400

    int test_msec3 = 7000;
    tick3 = midif::msec2Tick(test_msec3, res, tempo);   // 4800

    echoes(tick1, tick2, tick3);

    echoes(midif::getLengthTick2Msec(480, 3000, 480, tempo),    // tempo120 -> 500
           midif::getLengthMsec2Tick(500, 6000, 480, tempo));   // tempo60 -> 240

}
