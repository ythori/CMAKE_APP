

#include "_crestmuseTest.h"
#include "CrestMuse.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"

void crestmuseTest() {
    DataAddress DA;
    DA.resetSaveFolder();		// set result folder (just in case)
    std::shared_ptr<CrestMuseProcessor> CMP = std::make_shared<CrestMuseProcessor>();
    //auto crest = CMP->getCrestmuseDBPath();
    std::string DBver = "PEDBv2.2";
    std::string target = "bac-inv001-schif-g";
    //CMP->setMusicDataPath(DBver, target);
    //CMP->analyzeAllMidi();
    //auto ex_midi_property = CMP->getExProperty();
    //auto sc_midi_property = CMP->getScoreProperty();
    //ex_midi_property->showProperties();
    //sc_midi_property->showProperties();

    std::string score_xml = "bac-inv001-wiener-p012-013.xml";
    std::string dev_xml = "deviation.xml";
    //CMP->analyzeXml(score_xml, dev_xml);

    //CMP->setEstExOnsetMsecToScoreMidiUsingXml();
    //CMP->setExtraNoteToScoreMidiUsingXml();

    //CMP->scoreFollowingWithMidiAndXml(DBver, target, score_xml, dev_xml);
    //CMP->writeScoreFollowing();

    std::string score_following_txt_path = CMP->getCrestmuseDBPath() + DBver + "/" + target + "/score_following.txt";
    CMP->readScoreFollowing(score_following_txt_path);




}
