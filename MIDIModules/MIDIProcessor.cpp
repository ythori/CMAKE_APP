#include "pch.h"
#include "MIDIProcessor.h"
#include "MIDISharedFunctions.h"

#include "Options.h"
#include <random>

EventProperty::EventProperty() {}

NoteProperty::NoteProperty() {
	est_ex_onset_msec = 0;
	mod_chord_idx = 0;
	is_extra = false;
	is_miss = false;
}

MIDIProperty::MIDIProperty() {

	tempo.clear();
	time_signature.clear();
	key_signature.clear();
	note_event.clear();

	time_length = nullptr;

	pianoroll_msec.clear();

}

void MIDIProperty::showProperties() const {
	echoes("Title: ", title);
	echoes("Copyright mark: ", cpr);
	echoes("Format: ", fmt);
	echoes("Resolution: ", res);
	echoes("Number of tracks: ", n_track);
	if (time_length != nullptr) {
		echoes("Start, End, Length (tick): ", time_length->getTick(), time_length->getValue()[0], time_length->getValue()[0] - time_length->getTick());
		echoes("Start, End, Length (msec): ", time_length->getMsec(), time_length->getValue()[1], time_length->getValue()[1] - time_length->getMsec());
		echoes("Measure (start/end/length): ", time_length->getMeasure(), time_length->getValue()[2], time_length->getValue()[2] - time_length->getMeasure() + 1);
	}
	std::cout << std::endl;
	echo("tempo (tick/msec/measure/beat/det/microsec_beat/bpm): ");
	for (auto x : tempo) {
		echoes(x->getTick(), x->getMsec(), x->getMeasure(), x->getBeat(), x->getTickInBar(),
			   x->getValue()[0], x->getValue()[1]);
	}
	std::cout << std::endl;
	echo("Key signature (tick/msec/measure/beat/det/accidental/tonality/str): ");
	for (auto x : key_signature) {
		echoes(x->getTick(), x->getMsec(), x->getMeasure(), x->getBeat(), x->getTickInBar(),
			   x->getValue()[0], x->getValue()[1], midif::getKeySignature(std::make_pair(x->getValue()[0], x->getValue()[1])));
	}
	std::cout << std::endl;
	echo("Time signature (tick/msec/measure/beat/det/num_b/den_b(2^n)/clock_b/32th_b): ");
	for (auto x : time_signature) {
		echoes(x->getTick(), x->getMsec(), x->getMeasure(), x->getBeat(), x->getTickInBar(),
			   x->getValue()[0], x->getValue()[1], x->getValue()[2], x->getValue()[3]);
	}
	std::cout << std::endl;
	echo("Note on event list (ontick/onmsec/ondet[3]/mnn/chord_id/durtick/durmsec/vel/tempo)");
	for (auto x : note_event) x->showNoteEvent();
}


void MIDIProperty::sortNoteEvent() {
	std::vector<std::shared_ptr<NoteProperty>> mod_note_event;
	for (int i = 0; i < note_event.size(); ++i) {
		auto chord_idx = note_event[i]->getChordIdx();
		auto mnn = note_event[i]->getMIDINoteNumber();
		std::vector<int> same_chord_list = { i };
		std::vector<int> same_chord_mnn_list = { mnn };
		//auto onset_tick = note_event[i]->getOnsetTick();
		if (i < note_event.size() - 1) {
			for (int j = i + 1; j < note_event.size(); ++j) {
				auto next_chord_idx = note_event[j]->getChordIdx();
				if (chord_idx == next_chord_idx) {
					auto next_mnn = note_event[j]->getMIDINoteNumber();
					same_chord_list.emplace_back(j);
					same_chord_mnn_list.emplace_back(next_mnn);
					if (j == note_event.size() - 1) i = j;
				}
				else {
					i = j - 1;
					break;
				}
			}
		}
		auto sorted_idx = argSort(same_chord_mnn_list);
		//echoes(i, note_event.size(), chord_idx, mnn);
		//echo_v1(sorted_idx);
		for (auto idx : sorted_idx) {
			int rearranged_note_event_idx = same_chord_list[idx];
			mod_note_event.emplace_back(note_event[rearranged_note_event_idx]);
		}
	}
	note_event = mod_note_event;
}

void MIDIProperty::insertMissNoteEvent(const int idx_event) {
	note_event[idx_event]->setIsMiss(true);
	int original_mod_chord_idx = note_event[idx_event]->getModChordIdx();
	for (int i = idx_event; i > 0; --i) {
		int mod_chord_idx = note_event[i]->getModChordIdx();
		if (mod_chord_idx == original_mod_chord_idx) note_event[i]->setModChordIdx(mod_chord_idx - 1);
		else break;
	}

	for (int i = idx_event + 1; i < note_event.size(); ++i) {
		int mod_chord_idx = note_event[i]->getModChordIdx();
		note_event[i]->setModChordIdx(mod_chord_idx - 1);
	}
}

void MIDIProperty::insertExtraNoteEvent(const std::unordered_map<std::string, int> note_info) {
	int mnn = -1;
	int on_tick = -1;
	int on_measure = -1;
	int on_beat = -1;
	int on_tickbar = -1;
	int dur = -1;
	int vel = 100;
	int tmp = 120;
	int c_idx = -1;
	int on_msec = -1;
	int dur_msec = -1;
	int channel_num = 0;

	int check = 0;
	for (auto key : note_info) {
		if (key.first == "mnn") {
			++check;
			mnn = key.second;
		}
		else if (key.first == "on_tick") {
			++check;
			on_tick = key.second;
		}
		else if (key.first == "on_measure") {
			++check;
			on_measure = key.second;
		}
		else if (key.first == "on_beat") {
			++check;
			on_beat = key.second;
		}
		else if (key.first == "on_tickbar") {
			++check;
			on_tickbar = key.second;
		}
		else if (key.first == "dur") {
			++check;
			dur = key.second;
		}
		else if (key.first == "vel") {
			++check;
			vel = key.second;
		}
		else if (key.first == "tmp") {
			++check;
			tmp = key.second;
		}
		else if (key.first == "on_msec") {
			++check;
			on_msec = key.second;
		}
		else if (key.first == "dur_msec") {
			++check;
			dur_msec = key.second;
		}
		else if (key.first == "channel_num") {
			++check;
			channel_num = key.second;
		}
	}

	if (check == 11) {
		//std::vector<std::shared_ptr<NoteProperty>> note_event;
		bool is_insert = false;
		int idx = -1;
		for (auto noteon : note_event) {
			if (is_insert == false) {
				++idx;
				if (noteon->getOnsetTick() == on_tick) {
					c_idx = noteon->getModChordIdx();
					for (int i = idx; i < note_event.size(); ++i) {
						if (note_event[i]->getOnsetTick() == on_tick) {
							if (mnn > note_event[i]->getMIDINoteNumber()) {
								++idx;
							}
						}
						else break;
					}
					break;
				}
				else if (noteon->getOnsetTick() > on_tick) {
					c_idx = noteon->getModChordIdx();
					//noteon->modChordIdx(c_idx + 1);
					noteon->setModChordIdx(c_idx + 1);
					is_insert = true;
					continue;
				}
			}
			else {
				int pre_c_idx = noteon->getModChordIdx();
				//noteon->modChordIdx(pre_c_idx + 1);
				noteon->setModChordIdx(pre_c_idx + 1);
			}
		}
		note_event.insert(note_event.begin() + idx, std::make_shared<NoteProperty>(mnn, on_tick, on_measure, on_beat, on_tickbar,
																				   dur, vel, tmp, c_idx, on_msec, dur_msec, channel_num));
		note_event[idx]->setModChordIdx(c_idx);
		note_event[idx]->setIsExtra(true);
	}
}

void MIDIProperty::swapNoteEvent(const int num1, const int num2) {
	// Available: std::swap(mod_score_noteon[i], mod_score_noteon[j]);

	int c_idx1 = note_event[num1]->getModChordIdx();
	int c_idx2 = note_event[num2]->getModChordIdx();
	if (c_idx1 != c_idx2) {
		note_event[num1]->setModChordIdx(c_idx2);
		note_event[num2]->setModChordIdx(c_idx1);
	}
	//echoes("ss", num1, num2);
	std::swap(note_event[num1], note_event[num2]);

}


MIDIAnalyzer::MIDIAnalyzer() {
	MIDIP = std::make_shared<MIDIProperty>();
	timesig_list.clear();
	tempo_list.clear();
}

void MIDIAnalyzer::analyzeMIDI(const std::string obj_midi_address) {
	/* ================================================
		Channel
			0-9(, 11-): track1
			10: percussion track
			15: conductor track (and meta)
	================================================ */
	smf::MidiFile midifile;
	midifile.read(obj_midi_address);

	MIDIP->setResolution(midifile.getTicksPerQuarterNote());
	MIDIP->setNTrack(midifile.getTrackCount());
	if (MIDIP->getNTrack() == 1) MIDIP->setFormat(0); else MIDIP->setFormat(1);
	midifile.doTimeAnalysis();
	midifile.linkNotePairs();
	midifile.joinTracks();

	smf::MidiEvent* mev;

	/* ================================================
		Extract time_signature and tempo information
	================================================ */
	timesig_list.clear();
	tempo_list.clear();
	int start_timesig_tick = 0;
	int ts_num = 4;
	int ts_den = 4;
	int start_tempo_tick = 0;
	int start_tempo_msec = 0;
	int tempo_micro = 500000;
	int tempo_bpm = 120;
	int t_eve_tick = 0;
	int t_eve_msec = 0;
	bool title_flag = false;
	for (int event = 0; event < midifile[0].size(); ++event) {
		mev = &midifile[0][event];
		t_eve_tick = mev->tick;
		t_eve_msec = (int)(mev->seconds * 1000);

		if (t_eve_tick == 0) {
			if (mev->isTimeSignature()) {
				ts_num = (int)(*mev)[3];
				ts_den = (int)pow(2, (int)(*mev)[4]);
			}
			else if (mev->isTempo()) {
				tempo_micro = mev->getTempoMicroseconds();
				tempo_bpm = (int)mev->getTempoBPM();
			}
			else continue;
		}
		else {
			if (mev->isTimeSignature()) {
				std::vector<int> t_vec = { start_timesig_tick , t_eve_tick, ts_num, ts_den };
				timesig_list.emplace_back(t_vec);
				start_timesig_tick = t_eve_tick;
				ts_num = (int)(*mev)[3];
				ts_den = (int)pow(2, (int)(*mev)[4]);
			}
			else if (mev->isTempo()) {
				std::vector<int> t_vec = { start_tempo_tick, t_eve_tick, start_tempo_msec, t_eve_msec, tempo_micro, tempo_bpm};
				tempo_list.emplace_back(t_vec);
				start_tempo_tick = t_eve_tick;
				start_tempo_msec = t_eve_msec;
				tempo_micro = mev->getTempoMicroseconds();
				tempo_bpm = (int)mev->getTempoBPM();
			}
			else continue;
		}
	}
	std::vector<int> t_vec = { start_timesig_tick , t_eve_tick + 1, ts_num, ts_den };
	timesig_list.emplace_back(t_vec);
	t_vec.clear();
	t_vec = { start_tempo_tick, t_eve_tick + 1, start_tempo_msec, t_eve_msec + 1, tempo_micro, tempo_bpm };
	tempo_list.emplace_back(t_vec);

	//echo("time_sig");
	//echo_v2(timesig_list);
	//echo("tempo");
	//echo_v2(tempo_list);


	/* ================================================
		Get event information: TimeLength
			tick, msec, mbt: start time information (first onset)
			val: {tick_length, msec_length, end_measure, end_beat, end_tickinbar}
	================================================ */
	auto TimeLength = std::make_shared<EventProperty>();

	for (int event = 0; event < midifile[0].size(); ++event) {
		mev = &midifile[0][event];
		if (mev->isNoteOn()) {
			int start_tick = mev->tick;
			int start_msec = (int)(mev->seconds * 1000);
			auto mbt = midif::getMBT(MIDIP->getResolution(), start_tick, timesig_list);

			TimeLength->setTick(start_tick);
			TimeLength->setMsec(start_msec);
			TimeLength->setMBT(mbt);
			break;
		}
	}
	int end_idx = midifile[0].size() - 1;
	mev = &midifile[0][end_idx];
	int end_tick = mev->tick;
	int end_msec = (int)(mev->seconds * 1000);		// ex) 50012 -> 
	auto end_mbt = midif::getMBT(MIDIP->getResolution(), end_tick, timesig_list);

	//std::vector<int> time_length = { end_tick - TimeLength->getTick(), end_msec - TimeLength->getMsec(),
	//	std::get<0>(end_mbt), std::get<1>(end_mbt), std::get<2>(end_mbt) };
	std::vector<int> time_length = { end_tick, end_msec, std::get<0>(end_mbt), std::get<1>(end_mbt), std::get<2>(end_mbt) };
	TimeLength->setValue(time_length);
	MIDIP->setTimeLength(TimeLength);


	/* ================================================
		Get event information
	================================================ */
	std::string attribute;
	int c_idx = -1;
	int prev_on_tick = -1;
	int idx_event = 0;
	bool is_init_keysig = false;
	bool is_init_timesig = false;
	bool is_init_tempo = false;
	std::vector<std::vector<int>> current_noteon(90);		// {mnn, onset_offset}, under mnn12=0, over mnn88=90
	for (int event = 0; event < midifile[0].size(); ++event) {
		mev = &midifile[0][event];
		int eve_tick = mev->tick;
		int eve_msec = (int)(mev->seconds * 1000);
		auto mbt = midif::getMBT(MIDIP->getResolution(), eve_tick, timesig_list);

		if (mev->isNoteOn()) {
			/* ================================================
				Set NoteProperty
				NoteProperty(mnn, on_tick, on_measure, on_beat, on_tick_inbar, dur, vel, tempo, c_idx, on_msec, dur_msec)
			================================================ */
			attribute = "note_on";
			int tempo = midif::getTempo(eve_tick, tempo_list);
			int mnn = mev->getKeyNumber();
			int idx_mnn = midif::getIdxFromMIDINoteNumber(mnn);		// midi_note_number -= 20
			int velocity = mev->getVelocity();
			int duration_msec = (int)(mev->getDurationInSeconds() * 1000);
			int duration_tick = mev->getTickDuration();
			int noteoff_tick = eve_tick + duration_tick;
			int channel_num = mev->getChannel();

			// Note-duration duplication check
			//echo(current_noteon[mnn].size());
			if (current_noteon[idx_mnn].size() > 0) {	// If exist, size=3, else size=0
				int idx = current_noteon[idx_mnn][2];
				// full_val: {midi_note_number, onset_tick, onset_measure, onset_beat, onset_tickbar, duration_tick,
				//			  chord_idx, velocity, tempo, onset_msec, duration_msec}
				auto full_val = MIDIP->getNoteEvent()[idx]->getFullData();

				// In case of duplication
				if (full_val[1] + full_val[5] > eve_tick) {
					if (full_val[1] == eve_tick) {	// Same onset time (for example, track1 and track2)
						MIDIP->getNoteEvent()[idx]->setDurationTick(MAX2(full_val[5], duration_tick));
						MIDIP->getNoteEvent()[idx]->setDurationMsec(MAX2(full_val[10], duration_msec));
						current_noteon[idx_mnn][1] = eve_tick + MAX2(full_val[5], duration_tick);
						//echoes("dupli", eve_tick, mnn, full_val[5], duration_tick, full_val[10], duration_msec);
						continue;
					}
					else {
						MIDIP->getNoteEvent()[idx]->setDurationTick(eve_tick - full_val[1]);
						MIDIP->getNoteEvent()[idx]->setDurationTick(eve_msec - full_val[9]);
					}
				}
			}

			if (eve_tick > prev_on_tick) ++c_idx;	// -1 start, first note on is 0, second note on is 1
			prev_on_tick = eve_tick;

			auto NoteEve = std::make_shared<NoteProperty>(mnn, eve_tick, std::get<0>(mbt), std::get<1>(mbt), std::get<2>(mbt),
														  duration_tick, velocity, tempo, c_idx, eve_msec, duration_msec, channel_num);
			MIDIP->setNoteEvent(NoteEve);
			//MIDIP->getNoteEvent()[idx_event]->showNoteEvent();
			current_noteon[idx_mnn] = { eve_tick, noteoff_tick, idx_event };

			++idx_event;	// For next event index
		}


		/* ================================================
			Set EventProperty
			EventProperty(std::vector<int> value, tick, msec, measure, beat, tick_inbar, event_name)
		================================================ */
		else if (mev->isKeySignature()) {
			// Set EventProperty
			if (eve_tick == 0) is_init_keysig = true;
			attribute = "key_signature";
			std::vector<int> event_message = { (int)(*mev)[3], (int)(*mev)[4] };
			auto KeySig = std::make_shared<EventProperty>(event_message, eve_tick, eve_msec,
														  std::get<0>(mbt), std::get<1>(mbt), std::get<2>(mbt), attribute);
			MIDIP->setKeySignature(KeySig);
			continue;
		}
		else if (mev->isTimeSignature()) {
			// Set EventProperty
			if (eve_tick == 0) is_init_timesig = true;
			attribute = "time_signature";
			ts_num = (int)(*mev)[3];
			ts_den = (int)pow(2, (int)(*mev)[4]);
			int ts_nmidiclocks = (int)(*mev)[5];
			int ts_n32ndnotes = (int)(*mev)[6];
			std::vector<int> event_message = { ts_num, ts_den, ts_nmidiclocks, ts_n32ndnotes };
			auto TimeSig = std::make_shared<EventProperty>(event_message, eve_tick, eve_msec,
														   std::get<0>(mbt), std::get<1>(mbt), std::get<2>(mbt), attribute);
			MIDIP->setTimeSignature(TimeSig);
		}
		else if (mev->isTempo()) {
			// Set EventProperty
			if (eve_tick == 0) is_init_tempo = true;
			attribute = "tempo";
			tempo_micro = mev->getTempoMicroseconds();
			tempo_bpm = (int)mev->getTempoBPM();
			std::vector<int> event_message = { tempo_micro, tempo_bpm };
			auto Tempo = std::make_shared<EventProperty>(event_message, eve_tick, eve_msec,
														 std::get<0>(mbt), std::get<1>(mbt), std::get<2>(mbt), attribute);
			MIDIP->setTempo(Tempo);
		}
		else if (mev->isCopyright()) {
			MIDIP->setCpr(mev->getMetaContent());
		}
		else if (mev->isTrackName()) {
			// Temporarily, fist track name is assigned as title
			if (title_flag == false) MIDIP->setTitle(mev->getMetaContent());
			title_flag = true;
		}
		else {
			attribute = "Others";
			//std::vector<int> event_message;
			//for (int i = 0; i < mev->size(); ++i) event_message.emplace_back((int)(*mev)[i]);
			//std::string content = mev->getMetaContent();
			//echoes(eve_tick, event, attribute, content);
			//echo_v1(event_message);
			continue;
		}
	}
	if (is_init_keysig == false) {
		// eve_tick=0, eve_msec=0, measure=1, beat=1, tick_inbar=0
		attribute = "key_signature";
		std::vector<int> event_message = { 0, 0 };
		auto KeySig = std::make_shared<EventProperty>(event_message, 0, 0, 1, 1, 0, attribute);
		MIDIP->insertKeySignature(KeySig);
	}
	if (is_init_timesig == false) {
		// eve_tick=0, eve_msec=0, measure=1, beat=1, tick_inbar=0
		attribute = "time_signature";
		std::vector<int> event_message = { 4, 4, 24, 8 };
		auto TimeSig = std::make_shared<EventProperty>(event_message, 0, 0, 1, 1, 0, attribute);
		MIDIP->insertTimeSignature(TimeSig);
	}
	if (is_init_tempo == false) {
		// eve_tick=0, eve_msec=0, measure=1, beat=1, tick_inbar=0
		attribute = "tempo";
		std::vector<int> event_message = { 500000, 120 };
		auto Tempo = std::make_shared<EventProperty>(event_message, 0, 0, 1, 1, 0, attribute);
		MIDIP->insertTempo(Tempo);
	}

	//MIDIP->showProperties();

}

void MIDIAnalyzer::makePianorollBasedOnMsec(const int unit_msec) {
	/* ================================================
		Make pianoroll (msec base)
			TimeLength
				tick, msec, mbt: start time information (first onset)
				val: {tick_length, msec_length, end_measure, end_beat, end_tickinbar}

			pianoroll
				value = velocity
				duration = -1
				midi note number: from 21 to 108
				index
					0: under mnn 21
					89: over mnn 108
					90: tempo
	================================================ */
	auto note_on_event = MIDIP->getNoteEvent();
	//echo(note_on_event.size());
	if (note_on_event.size() == 0) {
		echo("You should execute analyzeMIDI() in advance");
	}
	else {
		auto time_length = MIDIP->getTimeLength();
		int end_msec = time_length->getValue()[1];
		int n_col = (int)(end_msec / unit_msec) + 1;		// ex) 5012, 100 -> 50+1 = 51
		std::vector<std::vector<int>> pianoroll(91, std::vector<int>(n_col, 0));

		/* ================================================
			Write tempo
		================================================ */
		for (int i = 0; i < n_col; ++i) {
			int msec_unit = (int)(unit_msec * i);
			pianoroll[90][i] = midif::getTempo(msec_unit, tempo_list, 1);
		}

		/* ================================================
			Write others
		================================================ */
		for (auto note_on : note_on_event) {
			auto mnn = note_on->getMIDINoteNumber();
			int idx_mnn = midif::getIdxFromMIDINoteNumber(mnn);
			auto on_msec = note_on->getOnsetMsec();
			int idx_msec = (int)(on_msec / unit_msec);
			auto on_dur = note_on->getDurationMsec();
			int n_dur = (int)(on_dur / unit_msec) - 1;
			if (n_dur < 0) n_dur = 0;	// unit=100[msec], duration=90 -> -1 -> 0
			int velocity = note_on->getVelocity();
			pianoroll[idx_mnn][idx_msec] = velocity;
			for (int i = 0; i < n_dur; ++i) pianoroll[idx_mnn][idx_msec + i + 1] = -1;
		}
		MIDIP->setPianoRollBasedOnMsec(pianoroll);
	}
}


MIDIGenerator::MIDIGenerator() {}

void MIDIGenerator::setMIDIFromPianoroll(const std::string pianoroll_path, const int unit_msec) {
	/* ================================================
		output_path: output MIDI path (../../___.mid)
		pianoroll_path: input pianoroll text path (../../___.txt)
			[0, :]:		Under mnn 21
			[89, :]:	Over mnn 108
			[90, :]:	Tempo
		unit_msec: msec assigned one column
	================================================ */
	note_event.clear();
	auto pianoroll = readStdVector2DiTxt(pianoroll_path);
	int n_rows = pianoroll.size();	// 91
	int n_cols = pianoroll[0].size();
	for (int c = 0; c < n_cols; ++c) {
		for (int idx_mnn = 1; idx_mnn < n_rows - 2; ++idx_mnn) {	// 0: Under mnn 21, 89: Over mnn 108, 90: Tempo -> From 1 to 88
			if (pianoroll[idx_mnn][c] > 0) {
				int mnn = midif::getMIDINoteNumberFromIdx(idx_mnn);
				int velocity = pianoroll[idx_mnn][c];
				int dur = 1;
				while (true) {
					int idx_tar = c + dur;
					if (idx_tar == n_cols) break;
					else {
						if (pianoroll[idx_mnn][idx_tar] == -1) ++dur;
						else break;
					}
				}
				int onset_msec = (int)(c * unit_msec);
				int duration_msec = (int)(dur * unit_msec);		// Minimub duration is over unit_msec
				//echoes(mnn, velocity, onset_msec, duration_msec);
				auto NoteEve = std::make_shared<NoteProperty>(mnn, velocity, onset_msec, duration_msec);
				note_event.emplace_back(NoteEve);
			}
		}
	}
}

void MIDIGenerator::writeMIDI(const std::string output_path) const {
	echo("=== Now creating a midi ===");
	smf::MidiFile midifile;

	int track = 0;
	int channel = 0;
	int pc_value = 1;		// program change value
	int tick_of_pc = 0;
	midifile.addTimbre(track, tick_of_pc, channel, pc_value);

	int tpq = 480;
	midifile.setTPQ(tpq);
	midifile.addTempo(track, 0, 120);
	midifile.addTimeSignature(track, 0, 4, 4, 24, 8);	// (track, tick, num, denom, clocks_per_click, num32ds_per_quarter

	for (auto note : note_event) {
		int mnn = note->getMIDINoteNumber();
		int velocity = note->getVelocity();
		int start_msec = note->getOnsetMsec();
		int end_msec = start_msec + note->getDurationMsec();
		int start_tick = (int)(start_msec / 500. * tpq);	// tempo:120 -> 500[msec/tpq]
		int end_tick = (int)(end_msec / 500. * tpq);
		midifile.addNoteOn(track, start_tick, channel, mnn, velocity);
		midifile.addNoteOff(track, end_tick, channel, mnn);
	}
	midifile.sortTracks();
	midifile.write(output_path);
}


