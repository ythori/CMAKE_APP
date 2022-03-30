
//#define BOOST_PYTHON_STATIC_LIB

#include "_madmomTest.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include "../Shared/passPyCpp.h"
#include "Madmom.h"

void madmomTest() {
    pyInitialize();

	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
    std::string crest = DA.getMusicDB() + "crestmuse/";
    std::string mididata_folder = crest + "dataset/bac-inv001-schif-g/";
    //std::string mididata = mididata_folder + "ex.mid";
    std::string mididata = mididata_folder + "score.mid";
    std::string wavedata = mididata_folder + "score.wav";

    //echo(mididata);
    //echo(wavedata);

    std::shared_ptr<MadmomCall> MC = std::make_shared<MadmomCall>();
    MC->setWavePath(wavedata);
    MC->analyzeWave();  // Required
    echo(MC->getLength());

    int fps = 100;   // 10msec unit

    // STFT
    //int fft = 1024;
    //int frame = int(fft / 2);
    //int hop = 10;  // msec
    //MC->analyzeSTFT(frame, hop, fft);
    //echo_v1(MC->getFreqs());

    // Unwrap (required STFT)
    //auto unwrap_phase = MC->getUnwrapPhase();
    //echo(unwrap_phase.size());

    // Onset detecting
    //auto est_onset = MC->onsetDetecting(fps);
    //echo_v1(est_onset);

    // Chroma
    //MC->analyzeChroma(fps);

    // CQT
    //MC->analyzeCQT(fps);

    for (int i = 60; i < 72; ++i) {
        echo(i);
        auto freq_mnn = MC->mnn2freq(i);
        auto freq_mcent = MC->mcent2freq(i * 100);
        echoes(freq_mnn, freq_mcent);
        echoes(MC->freq2mnn(freq_mnn), MC->freq2mcent(freq_mcent));
    }


    pyFinalize();

}
