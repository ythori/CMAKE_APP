#include<iostream>
#include "../Shared/_moduleTest.h"
#include "../Shared/genToyData.h"
#include "../_CreateTemplate/_createTemplateTest.h"
#include "../MIDIModules/_midiTest.h"
#include "../DTW/_dtwTest.h"
#include "../PythonMadmom/_madmomTest.h"
#include "../Evaluation/_evaluationTest.h"
#include "../CrestMuseProcessor/_crestmuseTest.h"
#include "../ScoreFollowing/_scoreFollowingTest.h"
#include "../ParticleFilter/_particleFilterTest.h"
#include "../ProbabilityFunctions/_probabilityFunctionsTest.h"
#include "../TempoEstimation/_tempoEstimationTest.h"
#include "../MusicPreciseAnalysis/_musicPreciseAnalysisTest.h";


int main() {
    std::string att = "";

    /* ==================================================
        Generate toy data (../Shared/genToyData.cpp)
    ================================================== */
    //genMultiDimSeq();

    /* ==================================================
        Shared modules test (../Shared/_moduleTest.cpp)
    ================================================== */
    //sharedTest();


    /* ==================================================
        Create template (../_CreateTemplate/_createTemplateTest.cpp)
    ================================================== */
    //createTemplateTest();



    /* ==================================================
        MIDI modules test (../MIDIModules/_midiTest.cpp)
    ================================================== */
    //midiTest();


    /* ==================================================
        DTW test (../DTW/_dtwTest.cpp)
    ================================================== */
    //dtwTest();


    /* ==================================================
        PyMadmom test (../PythonMadmom/_madmomTest.cpp)
    ================================================== */
    //madmomTest();


    /* ==================================================
        Evaluation test (../Evaluation/_evaluationTest.cpp)
    ================================================== */
    //evaluationTest();


    /* ==================================================
        Score following test (../ScoreFollowing/_scoreFollowingTest.cpp)
    ================================================== */
    //scoreFollowingTest();

    /* ==================================================
        Particle filter test (../ParticleFilter/_particleFilterTest.cpp)
    ================================================== */
    //particleFilterTest();

    /* ==================================================
        Probability functions test (../ProbabilityFunctionsTest/_probabilityFunctionsTest.cpp)
    ================================================== */
    //probabilityFunctionsTest();

    /* ==================================================
        Tempo estimation test (..//_Test.cpp)
    ================================================== */
    //tempoEstimationTest();

    /* ==================================================
        Music precise analysis test (../MusicPreciseAnalysis/_musicPreciseAnalysisTest.cpp)
    ================================================== */
    musicPreciseAnalysisTest();

    /* ==================================================
    test (..//_Test.cpp)
    ================================================== */


    system("PAUSE");

}