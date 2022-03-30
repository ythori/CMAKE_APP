/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	MIDI processor

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	MIDIProperty: Full midi information
		Shared information
			int:							res, fmt, n_track
			std::shared_ptr<EventProperty>:	time_length
			std::string:					title, cpr
			(No implementation: comment)
			(No implemantation: timemode (timemode: 0: TPQN(beat dependent), 24,25,29,30: SMPTE(frame_per_sec)))

		All event information (std::vector<std::shared_ptr<EventProperty>>)
			tempo
			time_signature
			key_signature
			(No implemantation: marker, system_exe_s, system_exe_c)

		Note information (std::vector<std::shared_ptr<NoteProperty>>)
			note_event


		-> Including detail information
			: EventProperty: Every event information
				event name (string):		attribute;
				value (std::vector<int>):	value;
				time information (int):	tick, msec, measure, beat, tick_inbar;


			: NoteProperty: Note event information (All int type)
				midi_note_number
				onset_tick
				onset_measure
				onset_beat
				onset_bdetail
				onset_msec
				duration_tick
				duration_msec;
				chord_idx
				velocity
				tempo

================================ */

#ifndef MIDIProcessor_H
#define MIDIProcessor_H

#include "pch.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"
#include "MidiFile.h"

class EventProperty {
public:
	// Default constructor
	EventProperty();
	// Constructor for analyzing MIDI
	EventProperty(std::vector<int> value, int tick, int msec,
				  int measure, int beat, int tick_inbar, std::string event_name)
		: value(value), tick(tick), msec(msec), measure(measure), beat(beat), tick_inbar(tick_inbar), attribute(event_name) {}
	// Constructor for generating MIDI
	EventProperty(std::vector<int> value, int tick, std::string event_name)
		: value(value), tick(tick), attribute(event_name) {}

	std::vector<int> getValue() { return value; }
	int getTick() const { return tick; }
	int getMsec() const { return msec; }
	int getMeasure() const { return measure; }
	int getBeat() const { return beat; }
	int getTickInBar() const { return tick_inbar; }
	std::string getAttribute() const { return attribute; }

	void setValue(const std::vector<int> val) { value = val; }
	void setTick(const int ttick) { tick = ttick; }
	void setMsec(const int tmsec) { msec = tmsec; }
	void setMBT(const std::tuple<int, int, int> tmbt) { measure = std::get<0>(tmbt); beat = std::get<1>(tmbt); tick_inbar = std::get<2>(tmbt); }
	void setAttribute(const std::string event_name) { attribute = event_name; }

	bool operator>(const EventProperty& another) const {
		return msec > another.msec;
	};

	bool operator<(const EventProperty& another) const {
		return msec < another.msec;
	};


private:
	std::string attribute;
	std::vector<int> value;
	int tick, msec, measure, beat, tick_inbar;

};



class NoteProperty {
public:
	// Default constructor
	NoteProperty();
	// Constructor for analyzing MIDI
	NoteProperty(int mnn, int on_tick, int on_measure, int on_beat, int on_tickbar,
				 int dur, int vel, int tmp, int c_idx,
				 int on_msec, int dur_msec, int ch_num)
		: midi_note_number(mnn), onset_tick(on_tick), onset_measure(on_measure), onset_beat(on_beat),
		onset_tickbar(on_tickbar), duration_tick(dur), velocity(vel), tempo(tmp), chord_idx(c_idx),
		onset_msec(on_msec), duration_msec(dur_msec),
		est_ex_onset_msec(0), mod_chord_idx(0), is_extra(false), is_miss(false), channel_num(ch_num) {}
	//Constructor for creating MIDI
	NoteProperty(int mnn, int vel, int on_msec, int dur_msec)
		//: midi_note_number(mnn), velocity(vel), onset_msec(on_msec), duration_msec(dur_msec) {}
		: midi_note_number(mnn), onset_tick(0), onset_measure(0), onset_beat(0),
		onset_tickbar(0), duration_tick(0), velocity(vel), tempo(0), chord_idx(0),
		onset_msec(on_msec), duration_msec(dur_msec),
		est_ex_onset_msec(0), mod_chord_idx(0), is_extra(false), is_miss(false), channel_num(0) {}

	int getMIDINoteNumber() const { return midi_note_number; }
	int getOnsetTick() const { return onset_tick; }
	int getOnsetMeasure() const { return onset_measure; }
	int getOnsetBeat() const { return onset_beat; }
	int getOnsetTickInBar() const { return onset_tickbar; }
	int getDurationTick() const { return duration_tick; }
	int getChordIdx() const { return chord_idx; }
	int getVelocity() const { return velocity; }
	int getTempo() const { return tempo; }
	int getOnsetMsec() const { return onset_msec; }
	int getDurationMsec() const { return duration_msec; }
	int getChannelNum() const { return channel_num; }
	std::vector<int> getPartData() const { 
		return std::vector<int>{chord_idx, onset_msec, midi_note_number, velocity, duration_msec};}
	std::vector<int> getFullData() const {
		return std::vector<int>{midi_note_number, onset_tick, onset_measure, onset_beat, onset_tickbar,
			duration_tick, chord_idx, velocity, tempo, onset_msec, duration_msec};}

	void setMIDINoteNumber(const int mnn) { midi_note_number = mnn; }
	void setVelocity(const int vel) { velocity = vel; }
	void setOnsetMsec(const int on_msec) { onset_msec = on_msec; }
	void setDurationMsec(const int dur_msec) { duration_msec = dur_msec; }
	void setDurationTick(const int dur_tick) { duration_tick = dur_tick; }

	void modChordIdx(const int mod_idx) { chord_idx = mod_idx; }

	void showNoteEvent() const { echoes(getOnsetTick(), getOnsetMsec(), getOnsetMeasure(), getOnsetBeat(), getOnsetTickInBar(), getMIDINoteNumber(),
										getChordIdx(), getDurationTick(), getDurationMsec(), getVelocity(),getTempo()); }

	// In case of using xml
	void setEstExOnsetMsec(const int msec) { est_ex_onset_msec = msec; }
	void setModChordIdx(const int cidx) { mod_chord_idx = cidx; }
	void setIsMiss(const bool is) { is_miss = is; }
	void setIsExtra(const bool is) { is_extra = is; }
	int getEstExOnsetMsec() const { return est_ex_onset_msec; }
	int getModChordIdx() const { return mod_chord_idx; }
	bool getIsMiss() const { return is_miss; }
	bool getIsExtra() const { return is_extra; }

private:
	int midi_note_number, onset_tick, onset_measure, onset_beat, onset_tickbar;
	int duration_tick, chord_idx, velocity, tempo;
	int onset_msec, duration_msec;
	int channel_num;

	// Using xml
	int mod_chord_idx, est_ex_onset_msec;
	bool is_miss, is_extra;
};


class MIDIProperty {

public:

	MIDIProperty();

	int getResolution() const { return res; }
	int getFormat() const { return fmt; }
	int getNTrack() const { return n_track; }

	std::shared_ptr<EventProperty> getTimeLength() const { return time_length; }

	std::vector<std::shared_ptr<EventProperty>> getTempo() const { return tempo; }
	std::vector<std::shared_ptr<EventProperty>> getKeySignature() const { return key_signature; }
	std::vector<std::shared_ptr<EventProperty>> getTimeSignature() const { return time_signature; }

	std::vector<std::shared_ptr<NoteProperty>> getNoteEvent() const { return note_event; }

	std::vector<std::vector<int>> getPianoRollBasedOnMsec() const { return pianoroll_msec; }


	void setResolution(const int resolution) { res = resolution; }
	void setFormat(const int format) { fmt = format; }
	void setNTrack(const int num_track) { n_track = num_track; }

	void setTitle(const std::string midi_title) { title = midi_title; }
	void setCpr(const std::string copyright) { cpr = copyright; }

	void setTimeLength(const std::shared_ptr<EventProperty> EP) { time_length = EP; }

	void setTempo(const std::shared_ptr<EventProperty> EP) { tempo.emplace_back(EP); }
	void setKeySignature(const std::shared_ptr<EventProperty> EP) { key_signature.emplace_back(EP); }
	void setTimeSignature(const std::shared_ptr<EventProperty> EP) { time_signature.emplace_back(EP); }

	void setNoteEvent(const std::shared_ptr<NoteProperty> NP) { note_event.emplace_back(NP); }

	void setPianoRollBasedOnMsec(const std::vector<std::vector<int>> input_pianoroll_msec) {
		for (auto one_msec: input_pianoroll_msec) pianoroll_msec.emplace_back(one_msec);
	}


	void insertTempo(const std::shared_ptr<EventProperty> EP) { auto it = tempo.begin(); it = tempo.insert(it, EP); }
	void insertKeySignature(const std::shared_ptr<EventProperty> EP) { auto it = key_signature.begin(); it = key_signature.insert(it, EP); }
	void insertTimeSignature(const std::shared_ptr<EventProperty> EP) { auto it = time_signature.begin(); it = time_signature.insert(it, EP); }

	/* ================================
		sortNoteEvent(): If same onset time, sort in ascending order of MNN
		insertExtraNoteEvent(): Insert extra note event, set is_extra to true, and modify mod_chord_idx of <NoteProperty>
		insertMissNoteEvent(): Set is_miss to true and modify mod_chord_idx of <NoteProperty>
	================================ */
	void sortNoteEvent();
	//void insertNoteEvent(const std::shared_ptr<NoteProperty> NP) { auto it = note_event.begin(); it = note_event.insert(it, NP); }
	void insertExtraNoteEvent(const std::unordered_map<std::string, int> note_info);
	void insertMissNoteEvent(const int idx_event);
	void swapNoteEvent(const int num1, const int num2);

	void writePianoRollBasedOnMsec(const std::string output_path) const {
		if (pianoroll_msec.size() > 0) saveTxt2DVec(pianoroll_msec, output_path);
		else echo("No pianoroll data.");
	}


	void showProperties() const;


private:
	int res, fmt, n_track;
	std::string title, cpr;
	std::shared_ptr<EventProperty> time_length;
	std::vector<std::shared_ptr<EventProperty>> tempo, time_signature, key_signature;

	std::vector<std::vector<int>> pianoroll_msec;

	std::vector<std::shared_ptr<NoteProperty>> note_event;

};


class MIDIAnalyzer {
public:

	MIDIAnalyzer();

	std::shared_ptr<MIDIProperty> getMIDIProperty() const { return MIDIP; }

	std::vector<std::vector<int>> getTimeSig() const { return timesig_list; }
	std::vector<std::vector<int>> getTempo() const { return tempo_list; }


//	void makeMIDI(const std::string save_address);
	void analyzeMIDI(const std::string obj_midi_address);
	void makePianorollBasedOnMsec(const int unit_msec);
	void writePianorollBasedOnMsec(const std::string output_path) const { MIDIP->writePianoRollBasedOnMsec(output_path); }


private:
	std::shared_ptr<MIDIProperty> MIDIP;
	std::vector<std::vector<int>> timesig_list, tempo_list;
};


class MIDIGenerator {
public:
	MIDIGenerator();

	void setMIDIFromPianoroll(const std::string pianoroll_path, const int unit_msec);
	void writeMIDI(const std::string output_path) const;

private:
	std::vector<std::shared_ptr<NoteProperty>> note_event;
};

#endif //MIDIProcessor