#include "pch.h"
#include "MIDISharedFunctions.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"

std::string midif::getKeySignature(const std::pair<int, int> key_val) {
	std::unordered_map<std::pair<long, long>, std::string, HashPair> key_sig;
	key_sig[std::make_pair(0, 0)] = "C-Maj";
	key_sig[std::make_pair(1, 0)] = "G-Maj";
	key_sig[std::make_pair(2, 0)] = "D-Maj";
	key_sig[std::make_pair(3, 0)] = "A-Maj";
	key_sig[std::make_pair(4, 0)] = "E-Maj";
	key_sig[std::make_pair(5, 0)] = "B-Maj";
	key_sig[std::make_pair(6, 0)] = "F#-Maj";
	key_sig[std::make_pair(7, 0)] = "C#-Maj";
	key_sig[std::make_pair(-1, 0)] = "F-Maj";
	key_sig[std::make_pair(-2, 0)] = "Bb-Maj";
	key_sig[std::make_pair(-3, 0)] = "Eb-Maj";
	key_sig[std::make_pair(-4, 0)] = "Ab-Maj";
	key_sig[std::make_pair(-5, 0)] = "Db-Maj";
	key_sig[std::make_pair(-6, 0)] = "Gb-Maj";
	key_sig[std::make_pair(-7, 0)] = "Cb-Maj";
	key_sig[std::make_pair(0, 1)] = "A-min";
	key_sig[std::make_pair(1, 1)] = "E-min";
	key_sig[std::make_pair(2, 1)] = "B-min";
	key_sig[std::make_pair(3, 1)] = "F#-min";
	key_sig[std::make_pair(4, 1)] = "C#-min";
	key_sig[std::make_pair(5, 1)] = "G#-min";
	key_sig[std::make_pair(6, 1)] = "D#-min";
	key_sig[std::make_pair(7, 1)] = "A#-min";
	key_sig[std::make_pair(-1, 1)] = "D-min";
	key_sig[std::make_pair(-2, 1)] = "G-min";
	key_sig[std::make_pair(-3, 1)] = "C-min";
	key_sig[std::make_pair(-4, 1)] = "F-min";
	key_sig[std::make_pair(-5, 1)] = "Bb-min";
	key_sig[std::make_pair(-6, 1)] = "Eb-min";
	key_sig[std::make_pair(-7, 1)] = "Ab-min";
	return key_sig[key_val];
}

std::string midif::getMIDIEventName(const std::string hex_val) {
	std::unordered_map<std::string, std::string> event_kind;
	event_kind["0x0"] = "sequence";
	event_kind["0x1"] = "text_event";
	event_kind["0x2"] = "copyright";
	event_kind["0x3"] = "track_name";
	event_kind["0x4"] = "instrument_name";
	event_kind["0x5"] = "lyric";
	event_kind["0x6"] = "marker";
	event_kind["0x7"] = "cue_point";
	event_kind["0x8"] = "program_name";
	event_kind["0x9"] = "device_name";
	event_kind["0x20"] = "channel_prefix";
	event_kind["0x21"] = "port_prefix";
	event_kind["0x2f"] = "end_of_track";
	event_kind["0x51"] = "tempo";
	event_kind["0x54"] = "SMPTE_offset";
	event_kind["0x58"] = "time_signature";
	event_kind["0x59"] = "key_signature";
	event_kind["0x7f"] = "sequencer_specific";

	int noteoff_start = hexadecimalToDecimal("0x80");		// <- 8[0], 0 shows ch number
	int noteon_start = hexadecimalToDecimal("0x90");
	int kat_start = hexadecimalToDecimal("0xa0");
	int cc_start = hexadecimalToDecimal("0xb0");
	int pc_start = hexadecimalToDecimal("0xc0");
	int cat_start = hexadecimalToDecimal("0xd0");
	int pb_start = hexadecimalToDecimal("0xe0");
	for (int i = 0; i < 10; ++i) {
		event_kind[decimalToHexadecimal(noteoff_start + i)] = "noteoff";
		event_kind[decimalToHexadecimal(noteon_start + i)] = "noteon";
		event_kind[decimalToHexadecimal(kat_start + i)] = "key_after_touch";
		event_kind[decimalToHexadecimal(cc_start + i)] = "control_change";
		event_kind[decimalToHexadecimal(pc_start + i)] = "program_change";
		event_kind[decimalToHexadecimal(cat_start + i)] = "channel_after_touch";
		event_kind[decimalToHexadecimal(pb_start + i)] = "pitch_bend";
	}

	event_kind["0xf0"] = "system_exclusive_start";
	event_kind["0xf7"] = "system_exclusive_continue";

	return event_kind[hex_val];
}

std::string midif::getKeyName(const long mnn) {
	std::string key_name;
	switch (mnn % 12) {
	case 0:
		key_name = "C_";
		break;
	case 1:
		key_name = "C#(Db)_";
		break;
	case 2:
		key_name = "D_";
		break;
	case 3:
		key_name = "D#(Eb)_";
		break;
	case 4:
		key_name = "E_";
		break;
	case 5:
		key_name = "F_";
		break;
	case 6:
		key_name = "F#(Gb)_";
		break;
	case 7:
		key_name = "G_";
		break;
	case 8:
		key_name = "G#(Ab)_";
		break;
	case 9:
		key_name = "A_";
		break;
	case 10:
		key_name = "A#(Bb)_";
		break;
	case 11:
		key_name = "B_";
		break;
	}

	key_name += std::to_string(mnn / 12 - 1);

	return key_name;
}

std::tuple<int, int, int> midif::getMBT(const int res, const int tar_tick,
								 const std::vector<std::vector<int>> timesig_list) {
	// timesig_list: vec{ start_tick , end_tick, ts_num, ts_den }
	// return (measure, beat, tick_inbar)

	// Make measure list
	std::vector<std::vector<int>> measure_list;
	int start_measure = 0;
	for (auto timesig_eve : timesig_list) {
		int num = timesig_eve[2];		// ex) 480, 4/4, [0,3840]
		int den = timesig_eve[3];
		int one_beat_res = res * (4. / den);	// ex) 480*4/4=480 
		int one_measure_res = num * res * (4. / den);	// ex) 4*480=1920
		int end_measure = (timesig_eve[1] - timesig_eve[0]) / one_measure_res;	// ex) (3840-0)/1920=2	-> [0, 2] (actual: 0, 1)
		std::vector<int> t_vec = { start_measure, start_measure + end_measure };
		measure_list.emplace_back(t_vec);
		start_measure += end_measure;
	}

	// Search measure, beat, tick
	for (int i = 0; i < timesig_list.size(); ++i) {
		int start_tick = timesig_list[i][0];
		int end_tick = timesig_list[i][1];
		if (start_tick <= tar_tick && tar_tick < end_tick) {
			int start_measure = measure_list[i][0];		// ex) [0, 5], 480, 4000
			int end_measure = measure_list[i][1];
			int mod_start_tick = 0;
			int mod_target_tick = tar_tick - start_tick;	// ex) 4000-0=4000
			int mod_end_tick = end_tick - start_tick;		// ex) 9600-0=9600
			int num = timesig_list[i][2];
			int den = timesig_list[i][3];
			int one_beat_res = (int)(res * (4. / den));		// ex) 480
			int one_measure_res = (int)(num * res * (4. / den));	// ex) 1920
			int diff = (int)(mod_target_tick / one_measure_res);	// ex) (int)4000/1920=2
			int measure = (int)(start_measure + diff);		// ex) 0+2=2
			int tick_inbar = (int)(mod_target_tick - (one_measure_res * diff));		// ex) 4000-1920*2=160
			int beat = (int)(tick_inbar / one_beat_res);		// ex) 160/480=0
			return std::make_tuple(measure + 1, beat + 1, tick_inbar);		// For 1 start
		}
		else continue;
		return std::make_tuple(0, 0, 0);
	}
}

int midif::getTempo(const int tar_time, const std::vector<std::vector<int>> tempo_list, const int att) {
	// tempo_list: vec{ start_tick, end_tick, start_msec, end_msec, tempo_micro, tempo_bpm }
	if (att == 0) {		// Based on tick
		for (auto tempo_eve : tempo_list) {
			//if (tempo_eve[0] <= tar_time && tar_time < tempo_eve[1]) return tempo_eve[5];
			if (tempo_eve[0] <= tar_time && tar_time < tempo_eve[1]) return tempo_eve[4];
			else continue;
		}
	}
	else {	// Based on msec
		for (auto tempo_eve : tempo_list) {
			//if (tempo_eve[2] <= tar_time && tar_time < tempo_eve[3]) return tempo_eve[5];
			if (tempo_eve[2] <= tar_time && tar_time < tempo_eve[3]) return tempo_eve[4];
			else continue;
		}
	}
	return 0;
}


int midif::getIdxFromMIDINoteNumber(const int mnn) {
	// Piano: 88 keys, from A0(21) to C8(108)
	// A0(21)=1, C4(60)=40, C8(108)=88, C0-G#0=0, C#8-G9=89
	if (mnn < 21) {
		return 0;
	}
	else if (mnn > 108) {
		return 89;
	}
	else return mnn - 20;
}

int midif::getMIDINoteNumberFromIdx(const int idx) {
	if (idx == 0 || idx == 89) return 0;
	else return idx + 20;
}


double midif::tempoMicro2BPM(const int tempo_micro) {
	// micro: 500000 = tempo: 120
	double quarter_note_sec = tempo_micro / 1000000.;	// 0.5 = length of quarter note in a second
	double bpm = 60 / quarter_note_sec;		// 60 / 0.5 = 120
	return bpm;
}


int midif::beat2Tick(const int measure, const int beat, const int res, const std::vector<std::vector<int>> timesig_list) {
	// measure: 1 start
	// beat: 1000 over start,   ex) 2125 = beat2, ratio = 0.125
	// timesig_list: vec{ start_tick , end_tick, ts_num, ts_den }
	int beat_num = int(beat / 1000);
	double ratio = (beat - (beat_num * 1000)) / 1000.;

	int start_bar_num = 1;
	int tick_start = 0;
	int tick_end = 0;
	int ret_tick = 0;
	for (auto timesig : timesig_list) {
		auto diff_sig_s_to_e = timesig[1] - timesig[0];
		auto one_beat_res = res * (4. / timesig[3]);
		auto one_bar_res = one_beat_res * timesig[2];	// ex) res480, 3/8 -> 480*3*(4/8)=720

		int add_n_bar = int((double)diff_sig_s_to_e / one_bar_res);	// ex) 480*4=1920 -> 3840/1920=2
		int end_bar_num = start_bar_num + add_n_bar;

		if ((start_bar_num <= measure) && (measure < end_bar_num)) {	// before_n_bar=1, add_n_bar=1 -> n_bar=2: start next timesig bar
			ret_tick = timesig[0];
			int diff = measure - start_bar_num;
			ret_tick += round(one_bar_res * diff);
			ret_tick += round(one_beat_res * (beat_num - 1));
			ret_tick += round(one_beat_res * ratio);
			break;
		}
		start_bar_num = end_bar_num;
	}
	return ret_tick;
}


int midif::tick2Msec(const int tick, const int res, const std::vector<std::vector<int>> tempo_list) {
	// tempo_list: vec{ start_tick, end_tick, start_msec, end_msec, tempo_micro, tempo_bpm }

	int ret = 0;
	for (auto tempo : tempo_list) {
		if ((tempo[0] <= tick) && (tick < tempo[1])) {
			auto start_msec = tempo[2];
			//auto tempo_bpm = tempo[5];
			auto tempo_bpm = midif::tempoMicro2BPM(tempo[4]);
			auto diff_tick = tick - tempo[0];
			int dur = round(1000. * diff_tick / (res * tempo_bpm / 60.));	// bpm=120 = 120/60 = 2beat/sec = res*2/sec -> 1/(res*2)=sec/tick
			ret = start_msec + dur;
			break;
		}
	}
	return ret;
}

int midif::msec2Tick(const int msec, const int res, const std::vector<std::vector<int>> tempo_list) {
	int ret = 0;
	for (auto tempo : tempo_list) {
		if ((tempo[2] <= msec) && (msec < tempo[3])) {
			auto start_tick = tempo[0];
			//auto tempo_bpm = tempo[5];
			auto tempo_bpm = midif::tempoMicro2BPM(tempo[4]);
			auto diff_msec = msec - tempo[2];
			int dur = round(res * (tempo_bpm / 60.) * diff_msec / 1000.);
			ret = start_tick + dur;
			break;
		}
	}
	return ret;
}

int midif::getLengthTick2Msec(const int dur_tick, const int on_tick, const int res, const std::vector<std::vector<int>> tempo_list) {
	int ret = 0;
	for (auto tempo : tempo_list) {
		if ((tempo[0] <= on_tick) && (on_tick < tempo[1])) {
			//auto tempo_bpm = tempo[5];
			auto tempo_bpm = midif::tempoMicro2BPM(tempo[4]);
			ret = round(1000. * dur_tick / (res * tempo_bpm / 60.));
			break;
		}
	}
	return ret;

}

int midif::getLengthMsec2Tick(const int dur_msec, const int on_msec, const int res, const std::vector<std::vector<int>> tempo_list) {
	int ret = 0;
	for (auto tempo : tempo_list) {
		if ((tempo[2] <= on_msec) && (on_msec < tempo[3])) {
			//auto tempo_bpm = tempo[5];
			auto tempo_bpm = midif::tempoMicro2BPM(tempo[4]);
			ret = round(res * (tempo_bpm / 60.) * dur_msec / 1000.);
			break;
		}
	}
	return ret;
}


double midif::mnn2Freq(const int mnn) {
	return 440 * pow(2, double(mnn - 69) / 12);
}

double midif::mcent2Freq(const int mcent) {
	return 440 * pow(2, double(mcent - 6900) / 1200);
}

int midif::freq2Mnn(const double f) {
	auto temp = log2(f / 440.) * 12 + 69;
	return round(temp);
}

int midif::freq2Mcent(const double f) {
	auto temp = log2(f / 440.) * 1200 + 6900;
	return round(temp);
}

int midif::getMIDINoteNumberFromStr(const std::string step, const int alter, const int octave) {
	int mnn = 12;	// C0
	mnn *= (octave + 1);	// ex) C4=60 -> 12*(4+1)=60
	if (step == "D") mnn += 2;
	else if (step == "E") mnn += 4;
	else if (step == "F") mnn += 5;
	else if (step == "G") mnn += 7;
	else if (step == "A") mnn += 9;
	else if (step == "B") mnn += 11;
	mnn += alter;	// C#, D#, F#, G#, A#
	return mnn;
}