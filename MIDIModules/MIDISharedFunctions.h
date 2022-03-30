/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	MIDI shared functions

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


================================ */


#ifndef MIDI_SHARED_FUNCTIONS_H
#define MIDI_SHARED_FUNCTIONS_H

#include "pch.h"

namespace midif {
	std::string getKeySignature(const std::pair<int, int> key_val);
	std::string getMIDIEventName(const std::string hex_val);
	std::string getKeyName(const long mnn);
	std::tuple<int, int, int> getMBT(const int res, const int tar_tick,
									 const std::vector<std::vector<int>> timesig_list);
	int getTempo(const int tar_tick, const std::vector<std::vector<int>> tempo_list, const int att=0);	// att=0: tick, 1: msec
	int getIdxFromMIDINoteNumber(const int mnn);
	int getMIDINoteNumberFromIdx(const int idx);

	double tempoMicro2BPM(const int tempo_micro);

	int beat2Tick(const int measure, const int beat, const int res, const std::vector<std::vector<int>> timesig_list);
	int tick2Msec(const int tick, const int res, const std::vector<std::vector<int>> tempo_list);
	int msec2Tick(const int msec, const int res, const std::vector<std::vector<int>> tempo_list);
	int getLengthTick2Msec(const int dur_tick, const int on_tick, const int res, const std::vector<std::vector<int>> tempo_list);
	int getLengthMsec2Tick(const int dur_msec, const int on_msec, const int res, const std::vector<std::vector<int>> tempo_list);

	double mnn2Freq(const int mnn);
	double mcent2Freq(const int mcent);
	int freq2Mnn(const double f);
	int freq2Mcent(const double f);

	int getMIDINoteNumberFromStr(const std::string step, const int alter, const int octave);	// ex) (C, 1, 4) = C#4
}

#endif //MIDI_SHARED_FUNCTIONS




