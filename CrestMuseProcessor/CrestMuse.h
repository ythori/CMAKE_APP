
/* ================================
Created by Hori on 2021//

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Crestmuse DB processor

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage
		1. Set

		2. Set

		3. Execute


================================ */

/* ================================

================================ */


#ifndef CREST_MUSE_H
#define CREST_MUSE_H

#include "pch.h"
#include "MIDIProcessor.h"
#include "../Shared/DataProc.h"


class ScoreFollowingEvent {
	/* ================================
		belief:
			0: Almost perfect
			1: extra note
			2: miss note
			3: others
	================================ */
public:
	ScoreFollowingEvent();
	//ScoreFollowingEvent(int mnn, int on_score, int on_est_ex, int on_ex, int c_idx, int b)
	//	: midi_note_number(mnn), onset_msec_score(on_score), onset_msec_est_ex(on_est_ex), onset_msec_ex(on_ex), chord_idx(c_idx), belief(b) {}
	ScoreFollowingEvent(int mnn, int on_score, int vel, int t, int on_est_ex, int on_ex, int c_idx, int b)
		: midi_note_number(mnn), onset_msec_score(on_score), velocity(vel), tempo(t), onset_msec_est_ex(on_est_ex), onset_msec_ex(on_ex), chord_idx(c_idx), belief(b) {}

	void showNoteEvent() const {
		echoes(midi_note_number, onset_msec_score, onset_msec_est_ex, onset_msec_ex, chord_idx, belief);
	}
	
	std::vector<int> getParams() const {
		//return { midi_note_number, onset_msec_score, onset_msec_est_ex, onset_msec_ex, chord_idx, belief };
		return { midi_note_number, onset_msec_score, velocity, tempo, onset_msec_est_ex, onset_msec_ex, chord_idx, belief };
	}

private:
	int midi_note_number, onset_msec_score, onset_msec_est_ex, onset_msec_ex, chord_idx, belief, velocity, tempo;
};


class CrestMuseProcessor {
public:
	CrestMuseProcessor();

	std::string getCrestmuseDBPath() const { return crestmuse_db_folder; }
	void setMusicDataPath(const std::string DBver, const std::string folder_name);
	std::string getExMidiPath() const { return expressed_midi_path; }
	std::string getScoreMidiPath() const { return score_midi_path; }
	std::shared_ptr<MIDIProperty> getExProperty() const { return ex_analyzer->getMIDIProperty(); }
	std::shared_ptr<MIDIProperty> getScoreProperty() const { return score_analyzer->getMIDIProperty(); }

	std::unordered_map<std::tuple<int, int, int>, int, HashTuple> getMNNFromXMLScore() const;

	void analyzeExMidi();
	void analyzeScoreMidi();
	void analyzeAllMidi();
	void analyzeXmlScore(const std::string score_xml_name);
	void analyzeXmlDev(const std::string dev_xml_name);
	void analyzeXml(const std::string score_xml_name, const std::string dev_xml_name);

	void setAlignmentBeatUnit();
	void setEstExOnsetMsecToScoreMidiUsingXml();
	void setExtraNoteToScoreMidiUsingXml();

	void scoreFollowingWithMidiAndXml(const std::string DBver, const std::string folder_name,
									  const std::string score_xml_name, const std::string dev_xml_name);

	void writeScoreFollowing() const;
	void readScoreFollowing(const std::string path);



private:
	std::string crestmuse_db_folder, expressed_midi_path, score_midi_path, music_folder_path;
	std::shared_ptr<MIDIAnalyzer> ex_analyzer, score_analyzer;
	std::vector<std::shared_ptr<NoteProperty>> mod_ex_noteon, mod_score_noteon;

	std::vector<std::tuple<int, int, int, int>> xml_onset;	// (measure_num, part_id, note_num, mnn)
	std::vector<std::tuple<int, int, int, double>> xml_attack;	// (measure_num, part_id, note_num, attack_dev)
	std::vector<std::tuple<int, int, int, int, int, int>> xml_miss_notes;	// (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note)
	std::vector<std::tuple<int, int, double>> xml_extra_notes;	// (measure_num, mnn, beat)
	std::vector<std::tuple<int, double, double, double>> xml_tempo;	// (measure_num, beat, tempo, tempo_dev)
	std::vector<std::tuple<int, double, double, double>> mod_xml_tempo;	// (measure_num, beat, tempo, tempo_dev) included all measure and beat

	// alignment_beat_unit: key=(score_measure, score_beat*100), value=(n_beat, beat_unit, sum_score, sum_ex)
	std::unordered_map<std::pair<int, int>, std::tuple<int, int, double, double>, HashPair> alignment_beat_unit;

	std::vector<std::shared_ptr<ScoreFollowingEvent>> score_following_result;

};



#endif //CREST_MUSE_H