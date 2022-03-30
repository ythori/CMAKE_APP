

#include "pch.h"
#include "CrestMuse.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "../MIDIModules/MIDISharedFunctions.h"
#include "../DTW/DynamicTimeWarping.h"


ScoreFollowingEvent::ScoreFollowingEvent() {}


CrestMuseProcessor::CrestMuseProcessor() {
	DataAddress DA;
	DA.resetSaveFolder();		// set result folder (just in case)
	crestmuse_db_folder = DA.getMusicDB() + "crestmuse/";
	expressed_midi_path = "NULL";
	score_midi_path = "NULL";
	music_folder_path = "NULL";
	ex_analyzer = std::make_shared<MIDIAnalyzer>();
	score_analyzer = std::make_shared<MIDIAnalyzer>();
	mod_ex_noteon.clear();
	mod_score_noteon.clear();
	xml_onset.clear();
	xml_extra_notes.clear();
	xml_miss_notes.clear();
	xml_attack.clear();
	xml_tempo.clear();
	mod_xml_tempo.clear();
	alignment_beat_unit.clear();
	score_following_result.clear();
}

void CrestMuseProcessor::setMusicDataPath(const std::string DBver, const std::string folder_name) {
	//expressed_midi_path = crestmuse_db_folder + DBver + "/" + folder_name + "/" + folder_name + ".mid";
	//score_midi_path = crestmuse_db_folder + DBver + "/" + folder_name + "/score.mid";
	music_folder_path = crestmuse_db_folder + DBver + "/" + folder_name + "/";
	expressed_midi_path = music_folder_path + folder_name + ".mid";
	score_midi_path = music_folder_path + "score.mid";
}

void CrestMuseProcessor::analyzeExMidi() {
	if (expressed_midi_path != "NULL") ex_analyzer->analyzeMIDI(expressed_midi_path);
}

void CrestMuseProcessor::analyzeScoreMidi() {
	if (score_midi_path != "NULL") score_analyzer->analyzeMIDI(score_midi_path);
}

void CrestMuseProcessor::analyzeAllMidi() {
	analyzeExMidi();
	analyzeScoreMidi();
}

void CrestMuseProcessor::analyzeXmlScore(const std::string score_xml_name) {
	/* ================================
		Target: filename+alpha.xml
		To set xml_onset
			: xml_onset: std::vector<std::tuple<int, int, int, int>>>
				: xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
	================================ */
	xml_onset.clear();

	std::string score_xml_path = music_folder_path + score_xml_name;
	boost::property_tree::ptree pt;
	boost::property_tree::xml_parser::read_xml(score_xml_path, pt);

	for (auto& score : pt) {
		int part_id = 0;
		for (auto& it : score.second.get_child("")) {
			if (it.first == "part") {
				for (auto& part : it.second.get_child("")) {	// To search pid at first.
					//echo(part.first);		// <xmlattr>, id, measure
					if (part.first == "<xmlattr>") {
						for (auto& pid : part.second.get_child("")) {
							std::string data = pid.second.data();
							std::regex re("(\\d+)");
							std::smatch m;
							std::regex_search(data, m, re);
							part_id = std::stoi(m.str());
						}
					}
				}

				int measure_num = 0;
				int note_num = 0;
				for (auto& part : it.second.get_child("")) {
					if (part.first == "measure") {
						for (auto& measure : part.second.get_child("")) {	// To search measure number at first.
							if (measure.first == "<xmlattr>") {
								for (auto& num : measure.second.get_child("")) {
									//echo(num.first);
									if (num.first == "number") {
										measure_num = std::stoi(num.second.data());
										note_num = 0;
									}
								}
							}
						}
						for (auto& measure : part.second.get_child("")) {
							if (measure.first == "note") {
								++note_num;
								std::string step;
								int alter = 0;
								int octave;
								//echoes(part_id, measure_num, note_num);
								for (auto& pitch : measure.second.get_child("")) {
									if (pitch.first == "pitch") {
										for (auto& att : pitch.second.get_child("")) {
											if (att.first == "step") step = att.second.data();
											else if (att.first == "alter") alter = std::stoi(att.second.data());
											else if (att.first == "octave") octave = std::stoi(att.second.data());
										}
										int mnn = midif::getMIDINoteNumberFromStr(step, alter, octave);
										// xml_onset: (measure_num, part_id, note_num, mnn)
										xml_onset.emplace_back(std::make_tuple(measure_num, part_id, note_num, mnn));
									}
								}
							}
						}

					}
				}

			}
		}
	}
	//for (auto note : xml_onset) echoes(std::get<0>(note), std::get<1>(note), std::get<2>(note), std::get<3>(note));
}

std::unordered_map<std::tuple<int, int, int>, int, HashTuple> CrestMuseProcessor::getMNNFromXMLScore() const {
	std::unordered_map<std::tuple<int, int, int>, int, HashTuple> score_xml_dic;
	for (auto note : xml_onset) {
		auto measure = std::get<0>(note);
		auto part = std::get<1>(note);
		auto note_num = std::get<2>(note);
		auto mnn = std::get<3>(note);
		score_xml_dic[std::make_tuple(measure, part, note_num)] = mnn;
	}
	return score_xml_dic;
}

void CrestMuseProcessor::analyzeXmlDev(const std::string dev_xml_name) {
	/* ================================
		Target: deviation.xml
		To set xml_extra_notes, xml_miss_notes, xml_attack
			: xml_extra_notes: std::vector<std::tuple<int, int, double>>
				: extra_notes[n]: { (measure_num, mnn, beat), ... }
			: xml_miss_notes: std::vector<std::tuple<int, int, int, int, int, int>>
				: miss_notes[n]:  { (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note), ... }
			: xml_attack: std::vector<std::tuple<int, int, int, double>>
				: attack[n]:      { (measure_num, part_id, note_num, attack_dev), ... }
			: xml_tempo: std::vector<std::tuple<int, int, double, double>>
				: tempo[n]:		  { (measure_num, beat, tempo, tempo_dev) }
	================================ */
	xml_attack.clear();
	xml_miss_notes.clear();
	xml_extra_notes.clear();
	xml_tempo.clear();

	std::string dev_xml_path = music_folder_path + dev_xml_name;
	boost::property_tree::ptree pt;
	boost::property_tree::xml_parser::read_xml(dev_xml_path, pt);
	int part_id = 0;
	int measure_num = 0;
	for (auto& deviation : pt) {
		//echo(deviation.first);
		for (auto& it : deviation.second.get_child("")) {
			/* ================================
				"deviation" tree
					Search: non-partwise, notewise, extra-notes
						non-partwise:
							measure - control - tempo, tempo-deviation
						notewise:
							note-deviation
							miss-note
						extra-notes:
							part - measure - extra-note - pitch - step, alter, octave
			================================ */

			if (it.first == "non-partwise") {
				double tempo = 0;
				for (auto& nonp : it.second.get_child("")) {
					if (nonp.first == "measure") {
						int measure_num = 0;
						int beat = 0;
						double tempo_dev = 0;
						for (auto& measure : nonp.second.get_child("")) {
							if (measure.first == "<xmlattr>") {
								for (auto& att : measure.second.get_child("")) {
									if (att.first == "number") {
										measure_num = std::stoi(att.second.data());
									}
								}
							}
							else if (measure.first == "control") {
								for (auto& ctrl : measure.second.get_child("")) {
									//echo(ctrl.first);
									if (ctrl.first == "<xmlattr>") {
										for (auto& att : ctrl.second.get_child("")) {
											if (att.first == "beat") beat = std::stod(att.second.data());
										}
									}
									else if (ctrl.first == "tempo") tempo = std::stod(ctrl.second.data());
									else if (ctrl.first == "tempo-deviation") {
										tempo_dev = std::stod(ctrl.second.data());
										xml_tempo.emplace_back(std::make_tuple(measure_num, beat, tempo, tempo_dev));
									}
								}
							}
						}
					}
				}
			}

			else if (it.first == "notewise") {
				int consecutive_miss_note = 0;
				for (auto& note_dev : it.second.get_child("")) {
					/* ================================
						"notewise" tree
							Search: note-deviation, miss-note
								note-deviation:
									<xmlattr>: part_id, measure_num, note_num
									attack
								miss-note:
									<xmlattr>: part_id, measure_num, note_num
					================================ */

					//echo(note_dev.first);
					if (note_dev.first == "note-deviation") {
						consecutive_miss_note = 0;
						std::vector<int> part_measure_note;
						double attack = 0;
						for (auto& att : note_dev.second.get_child("")) {	// To read part_id and measure_num at first.
							if (att.first == "<xmlattr>") {
								for (auto& xlink : att.second.get_child("")) {
									std::string data = xlink.second.data();
									std::regex re("(\\d+)");
									std::smatch m;
									part_measure_note.clear();

									while (std::regex_search(data, m, re)) {
										part_measure_note.emplace_back(std::stoi(m.str()));
										data = m.suffix();
									}
									//echo_v1(part_measure_note);
								}
							}
						}
						for (auto& att : note_dev.second.get_child("")) {
							if (att.first == "attack") {
								std::string data = att.second.data();
								if (!isNumber(data)) {
									echo("Contain not number strings");
								}
								else {
									attack = std::stod(data);
									//echoes(data, isNumber(data), std::stod(data));
								}
							}
						}
						// xml_attack: (measure_num, part_id, note_num, attack_dev)
						xml_attack.emplace_back(std::make_tuple(part_measure_note[1], part_measure_note[0], part_measure_note[2], attack));
					}
					else if (note_dev.first == "miss-note") {
						std::vector<int> part_measure_note;
						for (auto& att : note_dev.second.get_child("")) {
							if (att.first == "<xmlattr>") {
								for (auto& xlink : att.second.get_child("")) {
									std::string data = xlink.second.data();
									std::regex re("(\\d+)");
									std::smatch m;
									part_measure_note.clear();

									while (std::regex_search(data, m, re)) {
										part_measure_note.emplace_back(std::stoi(m.str()));
										data = m.suffix();
									}
									//echo_v1(part_measure_note);
								}
							}
						}
						int pre_on_note_num = -1;
						int pre_on_measure_num = -1;
						if (xml_attack.size() > 0) {
							pre_on_measure_num = std::get<0>(xml_attack[xml_attack.size() - 1]);
							pre_on_note_num = std::get<2>(xml_attack[xml_attack.size() - 1]);
						}
						// xml_miss_notes: (measure_num, part_id, note_num, pre_note_num)
						xml_miss_notes.emplace_back(std::make_tuple(part_measure_note[1], part_measure_note[0], part_measure_note[2],
																	pre_on_measure_num, pre_on_note_num, consecutive_miss_note));
						++consecutive_miss_note;
					}
				}
			}
			else if (it.first == "extra-notes") {
				for (auto& part : it.second.get_child("")) {
					int part_id = 0;
					for (auto& att : part.second.get_child("")) {
						// Get part id
						//echo(att.first);
						if (att.first == "<xmlattr>") {
							for (auto& pid : att.second.get_child("")) {
								std::string data = pid.second.data();
								std::regex re("(\\d+)");
								std::smatch m;
								std::regex_search(data, m, re);
								part_id = std::stoi(m.str());
							}
						}
					}

					for (auto& measure : part.second.get_child("")) {
						if (measure.first == "measure") {

							// To extract measure number at first.
							int measure_number;
							for (auto& ex_note : measure.second.get_child("")) {
								if (ex_note.first == "<xmlattr>") {
									for (auto& attr : ex_note.second.get_child("")) {
										if (attr.first == "number") {
											measure_number = std::stoi(attr.second.data());
											//echoes(attr.first, measure_number);
										}
										else echo("measure number read error");
									}
									break;
								}
							}

							// Extract extra-notes in the measure
							double beat_pos;
							std::string mnn_step;
							int mnn, mnn_alter, mnn_octave;
							bool is_beat = false;
							bool is_mnn = false;
							for (auto& ex_note : measure.second.get_child("")) {
								if (ex_note.first == "extra-note") {
									for (auto& att : ex_note.second.get_child("")) {
										if (att.first == "<xmlattr>") {
											for (auto beat : att.second.get_child("")) {
												if (beat.first == "beat") {
													beat_pos = std::stod(beat.second.data());
													//echo(beat_pos);
												}
											}
											is_beat = true;
										}

										if (att.first == "pitch") {
											for (auto& pitch_att : att.second.get_child("")) {
												if (pitch_att.first == "step") {
													mnn_step = pitch_att.second.data();
												}
												else if (pitch_att.first == "alter") {
													mnn_alter = std::stoi(pitch_att.second.data());
												}
												else if (pitch_att.first == "octave") {
													mnn_octave = std::stoi(pitch_att.second.data());
												}
											}
											is_mnn = true;
											//echoes(mnn_step, mnn_octave);
										}
									}
								}
								if ((is_beat == true) && (is_mnn == true)) {
									mnn = midif::getMIDINoteNumberFromStr(mnn_step, mnn_alter, mnn_octave);
									//echoes(measure_number, beat_pos, mnn_step, mnn_alter, mnn_octave, mnn);
									//extra_notes.emplace_back(std::make_tuple(measure_number, beat_pos, mnn));

									// xml_extra_notes: (measure_num, mnn, beat)
									xml_extra_notes.emplace_back(std::make_tuple(measure_number, mnn, beat_pos));
									is_beat = false;
									is_mnn = false;
								}
							}
						}
					}
				}
			}
		}
	}

	//echo("attack dev");
	//for (auto note : xml_attack) echoes(std::get<0>(note), std::get<1>(note), std::get<2>(note), std::get<3>(note));
	//echo("miss-notes");
	//for (auto note : xml_miss_notes) echoes(std::get<0>(note), std::get<1>(note), std::get<2>(note));
	//echo("extra-notes");
	//for (auto note : xml_extra_notes) echoes(std::get<0>(note), std::get<1>(note), std::get<2>(note));
	//echo("tempo");
	//for (auto ctrl : xml_tempo) echoes(std::get<0>(ctrl), std::get<1>(ctrl), std::get<2>(ctrl), std::get<3>(ctrl));

}



void CrestMuseProcessor::analyzeXml(const std::string score_xml_name, const std::string dev_xml_name) {
	/* ================================
		Get note information from score.xml
			xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
	================================ */
	analyzeXmlScore(score_xml_name);

	/* ================================
		Get deviation and error information from dev.xml
			xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }
			xml_miss_notes[n] = { (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note), ... }
			xml_extra_notes[n] = { (measure_num, mnn, beat), ... }
	================================ */
	analyzeXmlDev(dev_xml_name);
}


void CrestMuseProcessor::setAlignmentBeatUnit() {
	/* ================================
	Alignment using tempo information
		xml_tempo[n] = { (measure_num, beat, tempo, tempo_dev), ... }

		time_signature: tempo range: score_analyzer->getTimeSig() = std::vector<std::vector<int>>
						tempo event: score_analyzer->getMIDIProperty()->getTimeSignature() = std::vector<std::shared_ptr<EventProperty>>
			If want to get details, to use EventProperty.
				Contain: event_message, tick, msec, measure, beat, tick_inbar, attribute
					 event_message
						tempo = { tempo_micro, tempo_bpm }
						time_signature = { ts_num, ts_den, ts_nmidiclocks, ts_n32ndnotes }
		time_length: score_analyzer->getMIDIProperty()->getTimeLength()
			tick, msec, mbt: start time information (first onset)
			val: {tick_length, msec_length, end_measure, end_beat, end_tickinbar}
	================================ */
	auto timesig = score_analyzer->getMIDIProperty()->getTimeSignature();
	std::vector<std::tuple<int, int, int>> timesig_list;
	for (auto ts : timesig) {
		//echoes(ts->getAttribute(), ts->getMeasure(), ts->getTick(), ts->getMsec());
		//echo_v1(ts->getValue());

		// ts->getValue(): { ts_num, ts_den, ts_nmidiclocks, ts_n32ndnotes }
		timesig_list.emplace_back(std::make_tuple(ts->getMeasure(), ts->getValue()[0], ts->getValue()[1]));
		//echoes(ts->getMeasure(), ts->getValue()[0], ts->getValue()[1]);
	}

	auto score_tempo_list_detail = score_analyzer->getMIDIProperty()->getTempo();
	std::vector<std::tuple<int, int, double>> tempo_list;
	for (auto ts : score_tempo_list_detail) {
		//echoes(ts->getAttribute(), ts->getMeasure(), ts->getTick(), ts->getMsec());
		//echo_v1(ts->getValue());
		double tempo_change = midif::tempoMicro2BPM(ts->getValue()[0]);		// { tempo_micro, tempo_bpm }, micro -> bpm
		tempo_list.emplace_back(std::make_tuple(ts->getMeasure(), ts->getTickInBar(), tempo_change));
		//echoes(measure_tempo_change, tempo_change);
	}

	// If not beat of xml_tempo, tempo_dev = 1
	auto time_length = score_analyzer->getMIDIProperty()->getTimeLength();
	int max_measure_num = time_length->getValue()[2];
	if (time_length->getValue()[4] < 10) --max_measure_num;		// If eot is 24, actually until 23.999
	//echoes(max_measure_num, time_length->getValue()[4]);
	int idx_timesig = 0;
	int n_sum = 0;
	double current_tempo = std::get<2>(xml_tempo[0]);
	int end_n_beat = 0;
	mod_xml_tempo.clear();
	for (int m_count = 0; m_count < max_measure_num; ++m_count) {
		int measure_count = m_count + 1;	// Start from 1
		if (idx_timesig < timesig_list.size() - 1) {
			if (measure_count == std::get<0>(timesig_list[idx_timesig + 1])) ++idx_timesig;
		}
		int n_beat = std::get<1>(timesig_list[idx_timesig]);
		int beat_type = std::get<2>(timesig_list[idx_timesig]);

		std::vector<std::tuple<int, double, double, double>> temp_ctrl;
		for (int i = n_sum; i < xml_tempo.size(); ++i) {	// xml_tempo[n] = { (measure_num, beat, tempo, tempo_dev), ... }
			if (std::get<0>(xml_tempo[i]) == measure_count) {
				temp_ctrl.emplace_back(xml_tempo[i]);
			}
		}
		n_sum += temp_ctrl.size();
	
		int beat_count = 1;
		bool is_eot = false;
		for (auto ctrl : temp_ctrl) {
			double xml_beat = std::get<1>(ctrl);
			current_tempo = std::get<2>(ctrl);
			if (xml_beat > n_beat) {
				end_n_beat = n_beat;
				break;	// In control-tempo of deviation.xml, exist over beat(acc=4/4, but over 5 beat) at the end.
			}
			if (isIntDouble(xml_beat) == true) {	// 1, 2, 3, 4, ..., not 1.5, 2.2, ...
				int xml_beat_int = int(xml_beat + 0.0001);
				if (beat_count == xml_beat_int) {
					mod_xml_tempo.emplace_back(ctrl);
					++beat_count;
				}
				else {	// Exist skip (ex)1, 2, 4 -> in case of 3)
					mod_xml_tempo.emplace_back(std::make_tuple(measure_count, beat_count, current_tempo, 1.0));
					++beat_count;
				}
			}
			// Not add beat_count (ex) in case that 2, 2.5, 3 -> beat_count=3 on 2, therefore beat_count should be left on 2.5)
			else mod_xml_tempo.emplace_back(ctrl);
		}
		// If beat_count < n_beat
		for (int i = beat_count; i < n_beat + 1; ++i) mod_xml_tempo.emplace_back(std::make_tuple(measure_count, i, current_tempo, 1.0));
	}

	bool tempo_dev_eot = true;
	for (int i = mod_xml_tempo.size() - end_n_beat; i < mod_xml_tempo.size(); ++i) {
		if (std::get<3>(mod_xml_tempo[i]) != 1.0) tempo_dev_eot = false;
	}
	if (tempo_dev_eot == true) {
		auto tempo_dev = std::get<3>(mod_xml_tempo[mod_xml_tempo.size() - end_n_beat - 1]);
		for (int i = mod_xml_tempo.size() - end_n_beat; i < mod_xml_tempo.size(); ++i) std::get<3>(mod_xml_tempo[i]) = tempo_dev;
	}

	//for (auto mxt : mod_xml_tempo) {
	//	echoes(std::get<0>(mxt), std::get<1>(mxt), std::get<2>(mxt), std::get<3>(mxt));
	//}

	idx_timesig = 0;
	int idx_tempo = 0;
	double sum_score = 0;
	double sum_ex = 0;
	double prev_beat = 0;
	alignment_beat_unit.clear();
	auto midi_res = score_analyzer->getMIDIProperty()->getResolution();		// quarter_note resolution
	for (auto ctrl : mod_xml_tempo) {	// mod_xml_tempo[n] = { (measure_num, beat, tempo, tempo_dev), ... }
		int measure = std::get<0>(ctrl);
		if (idx_timesig < timesig_list.size() - 1) {
			if (measure == std::get<0>(timesig_list[idx_timesig + 1])) ++idx_timesig;
		}
		int n_beat = std::get<1>(timesig_list[idx_timesig]);
		int beat_type = std::get<2>(timesig_list[idx_timesig]);

		// ex) 1, 2, 2.5, 3, 4
		double beat = std::get<1>(ctrl);	// ex) 2.5

		double change_pos_in_the_beat = 0;
		double prev_tempo = 0;
		bool is_tempo_change = false;
		double next_change_tempo_beat = 0;	// If n_beat=4, from 0 to 3.999
		if (idx_tempo < tempo_list.size() - 1) {	// tempo_list: { measure, tickinbar, tempo_bpm }
			auto next_change_tempo_measure = std::get<0>(tempo_list[idx_tempo + 1]);
			auto next_change_tempo_beat_inbar = std::get<1>(tempo_list[idx_tempo + 1]);
			double next_change_tempo_beat = double(next_change_tempo_beat_inbar) / midi_res;
			if ((measure == next_change_tempo_measure) && (beat > next_change_tempo_beat)) {	// If 480(/480=1), start from beat 2, if include change point in the beat or not
				++idx_tempo;
				// If 0.66, 0.66 should be calculated along with old tempo
				prev_tempo = std::get<2>(tempo_list[idx_tempo - 1]);
				change_pos_in_the_beat = next_change_tempo_beat - int(next_change_tempo_beat);	// ex) 1->0, 1.66->0.66
				is_tempo_change = true;
			}
		}

		auto key = std::make_pair(measure, int(std::get<1>(ctrl) * 100));	// measure, beat*100

		// Score midi tempo and msec
		double tempo = std::get<2>(tempo_list[idx_tempo]);	// Already execute midif::tempoMicro2BPM()
		//double sec_one_quarter_note = 60. / tempo;
		if (is_tempo_change == true) {		// ex) 120 -> 60, 0.66
			auto sec_per_beat_in_old_tempo = (60. / prev_tempo) * 4. / beat_type;		// ex) 60/120*4/4 = 0.5[sec/beat]
			auto sec_per_beat_in_new_tempo = (60. / tempo) * 4. / beat_type;	// ex) 60/60*4/4 = 1.0[sec/beat]
			auto diff_changepos_oldbeat = next_change_tempo_beat - prev_beat;	// Absolutely, prev_beat <= next_change_tempo_beat < beat
			auto diff_newbeat_changepos = beat - next_change_tempo_beat;

			// ex) prev: 2, cpos: 2.2, current: 2.5, prev_tempo: 120(= 0.5[s/b]), new_tempo: 60(= 1.0[s/b])
			//		-> 0.5 * (2.2 - 2) + 1.0 * (2.5 - 2.2) = 0.4[sec] of [prev:current]
			sum_score += (diff_changepos_oldbeat * sec_per_beat_in_old_tempo + diff_newbeat_changepos * sec_per_beat_in_new_tempo);
		}
		else sum_score += ((beat - prev_beat) * (60. / tempo) * 4. / beat_type);	// Add sec at one beat

		// Expressed midi tempo and msec
		auto tempo_base = std::get<2>(ctrl);
		auto tempo_dev = std::get<3>(ctrl);
		auto mod_tempo = tempo_base * tempo_dev;
		sum_ex += ((beat - prev_beat) * (60. / mod_tempo) * 4. / beat_type);

		alignment_beat_unit[key] = std::make_tuple(n_beat, beat_type, sum_score, sum_ex);
		prev_beat = beat;
		if (prev_beat + 0.01 >= n_beat) prev_beat = 0;
	}

	// Show
	//for (auto mxt : mod_xml_tempo) {	// mod_xml_tempo[n] = { (measure_num, beat, tempo, tempo_dev), ... }
	//	int measure = std::get<0>(mxt);
	//	int mod_beat = int(std::get<1>(mxt) * 100);
	//	double mod_tempo = std::get<2>(mxt) * std::get<3>(mxt);
	//	auto key = std::make_pair(measure, mod_beat);
	//	auto al_data = alignment_beat_unit[key];
	//	echoes("Measure:", measure, ", Beat:", std::get<1>(mxt), ", Tempo:", mod_tempo);
	//	echoes("N_beat:", std::get<0>(al_data), "Beat_type:", std::get<1>(al_data),
	//		   "Score_sec:", std::get<2>(al_data), "Ex_sec:", std::get<3>(al_data));
	//}
}


void CrestMuseProcessor::setEstExOnsetMsecToScoreMidiUsingXml() {
	std::string M = "echo";
	/* ================================
	Get estimated onset msec from xml
		Required in advance
			: analyzeXmlScore()
				: To get xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
			  analyzeXmlDev()
				: To get xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }
						 xml_miss_notes[n] = { (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note), ... }
						 xml_extra_notes[n] = { (measure_num, mnn, beat), ... }
			  analyzeAllMidi()
				: To get score_analyzer
						 ex_analyzer
	================================ */

	/* ================================
		Rearrange onset list
			: Consider midi note number sort (ascending order)
		Set mod chord index (for insert note event)
			: Set same number (default setting)
	================================ */
	ex_analyzer->getMIDIProperty()->sortNoteEvent();	// ascending order depended on onset_mnn
	score_analyzer->getMIDIProperty()->sortNoteEvent();

	for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
		auto chord_idx = note->getChordIdx();
		note->setModChordIdx(chord_idx);	// To modify later
	}
	//for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
	//	echo(note->getModChordIdx());
	//}

	//auto mod_ex_noteon = ex_analyzer->getMIDIProperty()->getNoteEvent();
	//auto mod_score_noteon = score_analyzer->getMIDIProperty()->getNoteEvent();
	//for (int i = 0; i < ex_noteon.size(); ++i) {
	//	auto chord_idx = ex_noteon[i]->getChordIdx();
	//	auto mnn = ex_noteon[i]->getMIDINoteNumber();
	//	auto onset_msec = ex_noteon[i]->getOnsetMsec();
	//	auto mod_chord_idx = mod_ex_noteon[i]->getChordIdx();
	//	auto mod_mnn = mod_ex_noteon[i]->getMIDINoteNumber();
	//	auto mod_onset_msec = mod_ex_noteon[i]->getOnsetMsec();
	//	echoes(chord_idx, mnn, onset_msec, mod_chord_idx, mod_mnn, mod_onset_msec);
	//}

	/* ================================
		Set alignment between score and ex
			: std::unordered_map<std::pair<int, int>, std::tuple<int, int, double, double>, HashPair> alignment_beat_unit
				: key:   <score_measure, score_beat>
				  value: <n_beat, beat_type, score_msec, ex_msec>
	================================ */
	setAlignmentBeatUnit();


	/* ================================
		Set estimated ex_onset
			: In this stage, attack deviation is not considered

			1. Set score_analyzer->getMIDIProperty()->getNoteEvent()[i]->setEstExOnsetMsec() which are considered only tempo differencies.
				(In this stage)
			2. Considering onset mnn based on midi and xml using DTW
			3. Add deviation using xml_attack
	================================ */
	int idx_timesig = 0;
	auto timesig = score_analyzer->getMIDIProperty()->getTimeSignature();
	auto res = score_analyzer->getMIDIProperty()->getResolution();
	for (int i = 0; i < score_analyzer->getMIDIProperty()->getNoteEvent().size(); ++i) {
		auto note_eve = score_analyzer->getMIDIProperty()->getNoteEvent()[i];
		auto measure = note_eve->getOnsetMeasure();
		auto beat = note_eve->getOnsetBeat();
		auto tick_inbar = note_eve->getOnsetTickInBar();

		if (timesig.size() > 1) {
			if (measure == timesig[idx_timesig + 1]->getMeasure()) ++idx_timesig;
		}
		auto timesig_val = timesig[idx_timesig]->getValue();
		auto timesig_num = timesig_val[0];
		auto timesig_den = timesig_val[1];
		auto mod_one_beat_tick = tick_inbar % res;	// ex) 1800: 1800%480=(3, 360), mod=360
		double ratio = double(mod_one_beat_tick) / res;

		double one_before_beat_start_score_sec = 0;
		double one_before_beat_start_ex_sec = 0;
		auto sec_score_ex = alignment_beat_unit[std::make_pair(measure, int(beat * 100))];
		auto current_beat_start_score_sec = std::get<2>(sec_score_ex);
		auto current_beat_start_ex_sec = std::get<3>(sec_score_ex);
		if (beat > 1) {
			auto one_before_sec_score_ex = alignment_beat_unit[std::make_pair(measure, int((beat - 1) * 100))];
			one_before_beat_start_score_sec = std::get<2>(one_before_sec_score_ex);
			one_before_beat_start_ex_sec = std::get<3>(one_before_sec_score_ex);
		}
		else {
			if (measure > 1) {
				int one_before_beat = 0;
				for (int t_beat = 1; t_beat < 100; ++t_beat) {
					auto t_key = std::make_pair(measure - 1, int(t_beat * 100));
					if (containsKey(alignment_beat_unit, t_key)) one_before_beat = t_beat;
					else break;
				}
				auto one_before_sec_score_ex = alignment_beat_unit[std::make_pair(measure - 1, int(one_before_beat * 100))];
				one_before_beat_start_score_sec = std::get<2>(one_before_sec_score_ex);
				one_before_beat_start_ex_sec = std::get<3>(one_before_sec_score_ex);
			}
		}
		auto est_ex_sec = one_before_beat_start_ex_sec + (current_beat_start_ex_sec - one_before_beat_start_ex_sec) * ratio;
		score_analyzer->getMIDIProperty()->getNoteEvent()[i]->setEstExOnsetMsec(int(est_ex_sec * 1000));

		auto est_score_sec = one_before_beat_start_score_sec + (current_beat_start_score_sec - one_before_beat_start_score_sec) * ratio;
		auto acc_msec = note_eve->getOnsetMsec();
		//echoes(measure, beat, current_beat_start_score_sec, current_beat_start_ex_sec);
		//echoes(acc_msec, int(est_score_sec * 1000), score_analyzer->getMIDIProperty()->getNoteEvent()[i]->getEstExOnsetMsec());
	}

	//for (auto ne : ex_analyzer->getMIDIProperty()->getNoteEvent()) {
	//	ne->showNoteEvent();
	//}

	/* ================================
		Comparing xml and score-midi using DTW
			xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
			xml_miss_notes[n] = { (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note), ... }
			xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }

			mnn_midi, mnn_xml: Onset midi note number list which is grouped by every measure
			idx_midi, idx_xml: Onset index which is counted along with all data
	================================ */
	auto dic = getMNNFromXMLScore();	// dic[tuple(measure, part, nn)] = mnn

	std::unordered_map<int, int> miss_measure_dic;
	for (auto miss : xml_miss_notes) {
		auto m_measure_num = std::get<0>(miss);
		if (containsKey(miss_measure_dic, m_measure_num)) ++miss_measure_dic[m_measure_num];
		else miss_measure_dic[m_measure_num] = 1;
	}

	// Rearrange data (mnn_midi, idx_midi)
	std::vector<std::vector<int>> mnn_xml, mnn_midi, idx_xml, idx_midi;	// Devided at measure
	int measure_num = 1;
	std::vector<int> temp, temp_for_idx;
	int sum_count = 0;
	std::unordered_map<int, int> idx_start_in_the_measure;
	idx_start_in_the_measure[0] = 0;
	for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()){
		auto measure = note->getOnsetMeasure();
		//echoes(measure, note->getMIDINoteNumber());
		if (measure_num == measure) {
			temp.emplace_back(note->getMIDINoteNumber());
			temp_for_idx.emplace_back(sum_count);
		}
		else {
			if (temp.size() == 0) {
				temp.emplace_back(-1);
				temp_for_idx.emplace_back(-1);
			}
			mnn_midi.emplace_back(temp);
			idx_midi.emplace_back(temp_for_idx);
			temp.clear();
			temp.emplace_back(note->getMIDINoteNumber());
			temp_for_idx.clear();
			temp_for_idx.emplace_back(sum_count);
			++measure_num;
			idx_start_in_the_measure[measure_num] = sum_count;
		}
		++sum_count;
	}
	mnn_midi.emplace_back(temp);
	idx_midi.emplace_back(temp_for_idx);


	// Rearrange data (mnn_xml, idx_xml)
	measure_num = 1;
	sum_count = 0;
	temp.clear();
	temp_for_idx.clear();
	for (auto attack : xml_attack) {
		auto measure = std::get<0>(attack);
		auto part_id = std::get<1>(attack);
		auto note_num = std::get<2>(attack);
		auto mnn = dic[std::make_tuple(measure, part_id, note_num)];
		//echoes(measure, mnn);
		if (measure_num == measure) {
			temp.emplace_back(mnn);
			temp_for_idx.emplace_back(sum_count);
		}
		else {
			if (temp.size() == 0) {
				temp.emplace_back(-1);
				temp_for_idx.emplace_back(-1);
			}
			mnn_xml.emplace_back(temp);
			idx_xml.emplace_back(temp_for_idx);
			temp.clear();
			temp.emplace_back(mnn);
			temp_for_idx.clear();
			temp_for_idx.emplace_back(sum_count);
			++measure_num;
		}
		++sum_count;
	}
	mnn_xml.emplace_back(temp);
	idx_xml.emplace_back(temp_for_idx);

	// Search matching between mnn_midi and mnn_xml
	std::unordered_map<int, int> matching_midi_xml;
	if (mnn_midi.size() != mnn_xml.size()) echo("The numbers of measures of midi and xml are different");
	else {
		DynamicTimeWarping DTW;
		std::unordered_map<int, std::vector<std::tuple<int, int, int, int>>> miss_candidate;
		std::vector<std::vector<std::pair<int, int>>> path;
		for (int m = 0; m < mnn_midi.size(); ++m) {
			//echoes("Measure:", m + 1);
			//echo_v1(mnn_midi[m]);
			//echo_v1(mnn_xml[m]);
			//echo_v1(idx_midi[m]);
			//echo_v1(idx_xml[m]);
			//echo("");
			std::vector<double> double_mnn_midi(mnn_midi[m].begin(), mnn_midi[m].end());
			std::vector<double> double_mnn_xml(mnn_xml[m].begin(), mnn_xml[m].end());
			if (double_mnn_midi.size() > 0 && double_mnn_xml.size() > 0) {
				DTW.setInput(double_mnn_midi, double_mnn_xml);
				DTW.fastDTW();
				auto t_path = DTW.getPath();
				path.emplace_back(t_path);
				std::vector<std::tuple<int, int, int, int>> temp_path_info;
				for (auto p : t_path) {
					//echoes(p.first, p.second, mnn_midi[m][p.first], mnn_xml[m][p.second]);
					if (p.second, mnn_midi[m][p.first] != mnn_xml[m][p.second]) {
						temp_path_info.emplace_back(std::make_tuple(p.first, mnn_midi[m][p.first], p.second, mnn_xml[m][p.second]));
					}
					else {
						auto sum_count_midi = idx_midi[m][p.first];
						auto sum_count_xml = idx_xml[m][p.second];
						matching_midi_xml[sum_count_midi] = sum_count_xml;
					}
				}
				if (temp_path_info.size() > 0) {
					miss_candidate[m + 1] = temp_path_info;
				}
			}
			else {
				std::pair<int, int> t_pair = std::make_pair(0, 0);
				std::vector<std::pair<int, int>> t_path = { t_pair };
				path.emplace_back(t_path);
			}
		}

		for (auto key : matching_midi_xml) {
			//echoes(key.first, key.second);
			auto attack = xml_attack[key.second];	// xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }
			auto measure = std::get<0>(attack);
			auto part_id = std::get<1>(attack);
			auto note_num = std::get<2>(attack);
			auto attack_dev = std::get<3>(attack);
			auto mnn_xml = dic[std::make_tuple(measure, part_id, note_num)];

			auto target_midi_eve = score_analyzer->getMIDIProperty()->getNoteEvent()[key.first];
			auto mnn_midi = target_midi_eve->getMIDINoteNumber();
			auto measure_midi = target_midi_eve->getOnsetMeasure();
			auto beat_midi = target_midi_eve->getOnsetBeat();
			//echoes(mnn_midi, mnn_xml, attack_dev);
			double one_quarter_note_sec = 0;
			for (auto mxt : mod_xml_tempo) {	// mod_xml_tempo[n] = { (measure_num, beat, tempo, tempo_dev), ... }
				int m_count = std::get<0>(mxt);
				int mod_beat_count = int(std::get<1>(mxt) * 100);
				if ((m_count == measure_midi) && (mod_beat_count == int(beat_midi * 100))) {
					//echoes(111, measure_midi, beat_midi, m_count, mod_beat_count);
					one_quarter_note_sec = 60. / (std::get<2>(mxt) * std::get<3>(mxt));
					break;
				}
			}
			auto dev = one_quarter_note_sec * attack_dev;
			int mod_estonset_msec = target_midi_eve->getEstExOnsetMsec() + int(dev * 1000);
			//echoes(222, measure_midi, beat_midi, target_midi_eve->getEstExOnsetMsec(), int(dev * 1000));
			target_midi_eve->setEstExOnsetMsec(mod_estonset_msec);
		}

		// Show to confirm
		//auto note_eve = score_analyzer->getMIDIProperty()->getNoteEvent();
		//for (auto eve : note_eve) {
		//	echoes(eve->getMIDINoteNumber(), eve->getEstExOnsetMsec());
		//}
		//echo("==========");
		//note_eve = ex_analyzer->getMIDIProperty()->getNoteEvent();
		//for (auto eve : note_eve) {
		//	echoes(eve->getMIDINoteNumber(), eve->getOnsetMsec());
		//}


		// Add miss note information to score_analyzer
		// (and check miss note)
		if (miss_candidate.size() > 0) {
			for (auto measure : miss_candidate) {	// miss_candidate[n]: { { idx_of_a_measure_midi, mnn_midi, idx_of_a_measure_xml, mnn_xml } }
				// Set miss note information
				for (auto mc : measure.second) {	// mc: { idx_of_a_measure_midi, mnn_midi, idx_of_a_measure_xml, mnn_xml }
					auto idx_of_the_measure = std::get<0>(mc);
					auto mnn_miss_midi = std::get<1>(mc);
					int idx_miss_midi = idx_start_in_the_measure[measure.first] + idx_of_the_measure;
					auto mnn = score_analyzer->getMIDIProperty()->getNoteEvent()[idx_miss_midi]->getMIDINoteNumber();
					_echoes(M, "Confirm matching of index and mnn: MIDI=", mnn, ", Result of DTW=", mnn_miss_midi);
					score_analyzer->getMIDIProperty()->insertMissNoteEvent(idx_miss_midi);
				}

				// Check (print to console)
				_echoes(M, "===== Check miss note =====");
				_echoes(M, "Measure:", measure.first);
				_echoes(M, "MIDI MNN (ascending order):");
				_echo_v1(M, mnn_midi[measure.first - 1]);
				_echoes(M, "XML MNN:");
				_echo_v1(M, mnn_xml[measure.first - 1]);
				for (auto mc : measure.second) echoes("Estimated miss note idx:", std::get<0>(mc), ", MNN:", std::get<1>(mc));

				std::vector<int> miss_candidate_mnn_xml;
				int n_mc_xml = 0;
				for (auto note : xml_miss_notes) {
					auto measure_miss_xml = std::get<0>(note);
					auto part_id_miss_xml = std::get<1>(note);
					auto note_num_miss_xml = std::get<2>(note);
					auto mnn_miss_xml = dic[std::make_tuple(measure_miss_xml, part_id_miss_xml, note_num_miss_xml)];

					if (measure.first == measure_miss_xml) {
						++n_mc_xml;
						miss_candidate_mnn_xml.emplace_back(mnn_miss_xml);
					}
					else if (measure.first < measure_miss_xml) break;
				}
				_echoes(M, "Miss note from xml, Count:", n_mc_xml);
				_echo_v1(M, miss_candidate_mnn_xml);
				_echo(M, "");
			}
		}

		//for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
		//	echoes(note->getMIDINoteNumber(), note->getOnsetMeasure(), note->getChordIdx(), note->getModChordIdx(), note->getIsMiss());
		//}

	}
}


void CrestMuseProcessor::setExtraNoteToScoreMidiUsingXml() {
	/* ================================
		Add extra note events
			xml_extra_notes[n] = { (measure_num, mnn, beat), ... }
	================================ */
	auto score_timesig_list = score_analyzer->getTimeSig();
	auto score_tempo_list = score_analyzer->getTempo();
	int res = score_analyzer->getMIDIProperty()->getResolution();

	for (auto extra : xml_extra_notes) {
		std::unordered_map<std::string, int> note_info;
		int measure = std::get<0>(extra);
		int beat = int(std::get<2>(extra) * 1000);
		note_info["mnn"] = std::get<1>(extra);
		note_info["on_tick"] = midif::beat2Tick(measure, beat, res, score_timesig_list);

		//std::cout << std::endl;
		//echoes(measure, beat, note_info["mnn"], note_info["on_tick"]);
		//std::cout << std::endl;

		auto mbt = midif::getMBT(res, note_info["on_tick"], score_timesig_list);
		note_info["on_measure"] = std::get<0>(mbt);
		note_info["on_beat"] = std::get<1>(mbt);
		note_info["on_tickbar"] = std::get<2>(mbt);
		note_info["dur"] = 10;
		note_info["vel"] = 100;
		note_info["tmp"] = midif::getTempo(note_info["on_tick"], score_tempo_list, 0);	// 0: msec based
		note_info["on_msec"] = midif::tick2Msec(note_info["on_tick"], res, score_tempo_list);
		note_info["dur_msec"] = midif::getLengthTick2Msec(note_info["dur"], note_info["on_tick"], res, score_tempo_list);
		note_info["channel_num"] = 0;
		score_analyzer->getMIDIProperty()->insertExtraNoteEvent(note_info);
	}
}



void CrestMuseProcessor::scoreFollowingWithMidiAndXml(const std::string DBver, const std::string folder_name,
															 const std::string score_xml_name, const std::string dev_xml_name) {
	/* ================================
	setMusicDataPath(DBver, target)
		: Set path: expressed_midi_path, score_midi_path, music_folder_path

	analyzeAllMidi()
		: Analyze score-midi and ex-mid
			: Get: score_analyzer, ex_analyzer

	analyzeXml(score_xml, dev_xml)
		: Analyze XML file
			: Get: xml_onset, xml_attack, xml_miss_notes, xml_extra_notes, xml_tempo

	setEstExOnsetMsecToScoreMidiUsingXml()
		: Get estimated expressed music onset msec

	setExtraNoteToScoreMidiUsingXml()
		: Get extra note
		int on_score, int on_est_ex, int on_ex, int c_idx, int b
	================================ */
	setMusicDataPath(DBver, folder_name);
	analyzeAllMidi();
	analyzeXml(score_xml_name, dev_xml_name);
	setEstExOnsetMsecToScoreMidiUsingXml();
	//setExtraNoteToScoreMidiUsingXml();
	int idx = 0;
	for (auto eve : score_analyzer->getMIDIProperty()->getNoteEvent()) {
		int mnn = eve->getMIDINoteNumber();
		int on_score = eve->getOnsetMsec();
		int on_est_ex = eve->getEstExOnsetMsec();
		bool is_extra = eve->getIsExtra();
		bool is_miss = eve->getIsMiss();
		int chord_idx = eve->getModChordIdx();
		int on_ex = -1;
		int belief = -1;
		int velocity = eve->getVelocity();
		int tempo = int(midif::tempoMicro2BPM(eve->getTempo()));

		if (is_miss == true) belief = 1;
		else if (is_extra == true) belief = 2;
		//echoes(mnn, on_score, on_est_ex, is_extra, is_miss, chord_idx);

		while (true) {
			auto ex_eve = ex_analyzer->getMIDIProperty()->getNoteEvent()[idx];
			int tar_mnn = ex_eve->getMIDINoteNumber();
			int tar_on = ex_eve->getOnsetMsec();
			if (mnn == tar_mnn) {
				if ((on_est_ex - 50 < tar_on) && (tar_on < on_est_ex + 50)) {
					on_ex = tar_on;
					belief = 0;
					break;
				}
			}
			if (on_est_ex + 50 < tar_on) {
				break;
			}
			++idx;
		}
		idx -= 10;
		if (idx < 0) idx = 0;

		score_following_result.emplace_back(std::make_shared<ScoreFollowingEvent>(mnn, on_score, velocity, tempo, on_est_ex, on_ex, chord_idx, belief));

	}
	//for (auto sf : score_following_result) {
	//	sf->showNoteEvent();
	//}

}


void CrestMuseProcessor::writeScoreFollowing() const {
	std::vector<std::vector<int>> score_following;
	for (auto sf : score_following_result) {
		score_following.emplace_back(sf->getParams());
	}
	std::string path = music_folder_path + "score_following.txt";
	saveTxt2DVec(score_following, path);
}


void CrestMuseProcessor::readScoreFollowing(const std::string path) {
	auto mat = readStdVector2DiTxt(path);
	score_following_result.clear();
	for (auto vec : mat) {
		//echo_v1(vec);
		//score_following_result.emplace_back(std::make_shared<ScoreFollowingEvent>(vec[0], vec[1], vec[2], vec[3], vec[4], vec[5]));
		score_following_result.emplace_back(std::make_shared<ScoreFollowingEvent>(vec[0], vec[1], vec[2], vec[3], vec[4], vec[5], vec[6], vec[7]));
	}
	//for (auto sf : score_following_result) sf->showNoteEvent();
}



//bool cmp_score(const ScoreFollowingEvent& l, const ScoreFollowingEvent& r) {
//	if (l.onset_msec_score != r.onset_msec_score)
//		return l.onset_msec_score < r.onset_msec_score;
//	if (l.midi_note_number != r.midi_note_number)
//		return l.midi_note_number < r.midi_note_number;
//	return true;
//}
//
//bool cmp_ex(const ScoreFollowingEvent& l, const ScoreFollowingEvent& r) {
//	if (l.onset_msec_est_ex != r.onset_msec_est_ex)
//		return l.onset_msec_est_ex < r.onset_msec_est_ex;
//	if (l.midi_note_number != r.midi_note_number)
//		return l.midi_note_number < r.midi_note_number;
//	return true;
//}



//
//
//
//
//void CrestMuseProcessor::setExtraNoteToScoreMidiUsingXml(){
//	std::string M = "echo";
//	/* ================================
//	Rearrange onset list
//		miss note:
//			xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
//			xml_miss_notes[n] = { (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note), ... }
//			xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }
//
//			1. Count the number of errors of a measure
//				n_error_in_the_measure
//
//			2. Compare miss note information and attack information
//				miss-related:
//					m_measure_num, m_part_id, miss_mnn, one_before_miss_note_idx(from -1), miss_note_idx_of_the_measure(from 0)
//						miss_note_idx_of_the_measure: Using pre_on_measure_num and pre_on_note_num
//													  If exist miss note in the same measure, increment
//				attack-related:
//					one_before_mnn: mnn of one before miss note
//				others:
//					n_note_of_the_measure
//
//			3. Count the number of note in the measure existed miss note from score-midi
//				n_note_of_the_measure_in_score
//
//			4. Compare attack information, and score-midi
//				TODO: refine
//				Now condition: if (n_note_of_the_measure + n_error_in_the_measure == n_note_of_the_measure_in_score)
//
//				attack-related:
//					one_before_mnn, next_one_before_mnn
//						: one_before_mnn: mnn of one before miss note (ref. 2.)
//						: next_one_before_mnn = mnn of next of miss note
//						: (one_before_mnn -> miss_mnn -> next_one_before_mnn)
//				score-related:
//					score_mnn, next_score_mnn, one_before_mnn_score, n_note_count_in_the_measure, target_miss_note_idx
//						: target_miss_note_idx: Full count of index refered miss_note_idx_of_the_measure
//							: miss_note_idx_of_the_measure was considered multiple miss note in the same measure (ref. 2.)
//						: score_mnn: target_miss_note_idx
//						: next_score_mnn: target_miss_note_idx + 1
//						: one_before_mnn_score: target_miss_note_idx - 1
//
//	================================ */
//	auto dic = getMNNFromXMLScore();	// dic[tuple(measure, part, nn)] = mnn
//
//	std::unordered_map<int, int> miss_measure_dic;
//	for (auto miss : xml_miss_notes) {
//		auto m_measure_num = std::get<0>(miss);
//		if (containsKey(miss_measure_dic, m_measure_num)) ++miss_measure_dic[m_measure_num];
//		else miss_measure_dic[m_measure_num] = 1;
//	}
//
//	/* ================================
//		(Not available, using dtw as follow)
//		Search miss note and correspond to score and xml
//	================================ */
//	//int pre_miss_measure_num = -1;
//	//for (auto miss : xml_miss_notes) {
//	//	auto m_measure_num = std::get<0>(miss);
//	//	auto m_part_id = std::get<1>(miss);
//	//	auto m_note_num = std::get<2>(miss);
//	//	auto pre_on_measure_num = std::get<3>(miss);	// If first onset is miss, -1
//	//	auto pre_on_note_num = std::get<4>(miss);		// If first onset is miss, -1
//	//	auto n_consecutive = std::get<5>(miss);
//	//	auto miss_mnn = dic[std::make_tuple(m_measure_num, m_part_id, m_note_num)];
//	//	auto n_error_in_the_measure = miss_measure_dic[m_measure_num];
//	//	//echoes(m_measure_num, m_note_num, pre_on_measure_num, pre_on_note_num, miss_mnn);
//
//	//	int n_note_of_the_measure = 0;
//	//	int one_before_miss_note_idx = -1;		// if first note is miss, one_before_idx is -1
//	//	int miss_note_idx_of_the_measure = 0;
//	//	int is_find = false;
//	//	for (int i = 0; i < xml_attack.size(); ++i) {
//	//		auto attack = xml_attack[i];
//	//		auto a_measure_num = std::get<0>(attack);
//	//		auto a_part_id = std::get<1>(attack);
//	//		auto a_note_num = std::get<2>(attack);
//	//		if ((pre_on_measure_num == a_measure_num) && (m_part_id == a_part_id)) {
//	//			//echo(miss_note_idx_of_the_measure);
//	//			if (pre_on_note_num == a_note_num) {
//	//				one_before_miss_note_idx = i;
//	//				if (pre_on_measure_num == m_measure_num) ++miss_note_idx_of_the_measure;
//	//				is_find = true;
//	//			}
//	//			if ((is_find == false) && (pre_on_measure_num == m_measure_num)) ++miss_note_idx_of_the_measure;
//	//		}
//	//		if ((m_measure_num == a_measure_num) && (m_part_id == a_part_id)) ++n_note_of_the_measure;
//	//		if ((m_measure_num < a_measure_num) && (m_part_id == a_part_id)) break;
//
//	//	}
//
//	//	if (pre_miss_measure_num == m_measure_num) ++miss_note_idx_of_the_measure;	// Add a count of one before miss note
//	//	pre_miss_measure_num = m_measure_num;
//
//	//	int one_before_mnn = dic[std::make_tuple(pre_on_measure_num, m_part_id, pre_on_note_num)];
//	//	//echoes(m_measure_num, m_note_num, pre_on_measure_num, pre_on_note_num, miss_mnn,
//	//	//	   n_note_of_the_measure, one_before_miss_note_idx, miss_note_idx_of_the_measure);
//
//	//	int n_note_of_the_measure_in_score = 0;
//	//	for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
//	//		auto score_measure = note->getOnsetMeasure();
//	//		if (score_measure == m_measure_num) ++n_note_of_the_measure_in_score;
//	//		if (score_measure > m_measure_num) break;
//	//	}
//
//	//	// TODO more refine
//	//	//echoes(n_note_of_the_measure, n_note_of_the_measure_in_score, n_error_in_the_measure);
//	//	if (n_note_of_the_measure + n_error_in_the_measure == n_note_of_the_measure_in_score) {
//	//		//echoes(n_note_of_the_measure, n_note_of_the_measure_in_score, n_error_in_the_measure);
//	//		auto attack = xml_attack[one_before_miss_note_idx + 1];		// If exist miss note, this shows next of miss note
//	//		auto a_measure_num = std::get<0>(attack);
//	//		auto a_part_id = std::get<1>(attack);
//	//		auto a_note_num = std::get<2>(attack);
//	//		int next_one_before_mnn = dic[std::make_tuple(a_measure_num, a_part_id, a_note_num)];
//	//		//echoes("One before MNN:", one_before_mnn, ", (Current) Miss MNN:", miss_mnn, ", Next MNN:", next_one_before_mnn);
//
//	//		// Search from score-midi
//	//		int n_note_count_in_the_measure = 0;
//	//		int target_miss_note_idx = 0;
//	//		int score_mnn;
//	//		for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
//	//			if (note->getOnsetMeasure() == m_measure_num) {
//	//				//echo(note->getMIDINoteNumber());
//	//				if (n_note_count_in_the_measure == miss_note_idx_of_the_measure) {
//	//					score_mnn = note->getMIDINoteNumber();
//	//					break;
//	//				}
//	//				++n_note_count_in_the_measure;
//	//			}
//	//			++target_miss_note_idx;
//	//		}
//	//		//echoes("Miss measure:", m_measure_num, ", Idx of measure:", miss_note_idx_of_the_measure);
//
//
//	//		// Incase that first onset is miss
//	//		if (one_before_miss_note_idx < 0) {
//	//			int score_mnn = score_analyzer->getMIDIProperty()->getNoteEvent()[0]->getMIDINoteNumber();
//	//			int next_mnn_score = score_analyzer->getMIDIProperty()->getNoteEvent()[1]->getMIDINoteNumber();
//	//			if ((miss_mnn == score_mnn) && (next_one_before_mnn == next_mnn_score)) score_analyzer->getMIDIProperty()->insertMissNoteEvent(0);
//	//		}
//
//	//		// In case that miss notes appeare execept first or last onset
//	//		else if ((0 <= one_before_miss_note_idx) && (one_before_miss_note_idx < xml_attack.size() - 1)) {
//	//			int one_before_mnn_score = score_analyzer->getMIDIProperty()->getNoteEvent()[target_miss_note_idx - 1]->getMIDINoteNumber();
//	//			int next_mnn_score = score_analyzer->getMIDIProperty()->getNoteEvent()[target_miss_note_idx + 1]->getMIDINoteNumber();
//	//			//echoes("Score MNN:", one_before_mnn_score, score_mnn, next_mnn_score);
//
//	//			if ((one_before_mnn == one_before_mnn_score) && (miss_mnn == score_mnn) && (next_one_before_mnn == next_mnn_score)) {
//	//				//score_analyzer->getMIDIProperty()->getNoteEvent()[target_miss_note_idx]->setIsMiss(true);
//	//				score_analyzer->getMIDIProperty()->insertMissNoteEvent(target_miss_note_idx);
//	//			}
//	//		}
//
//	//		// In case that last onset is miss
//	//		else if (one_before_miss_note_idx == xml_attack.size() - 1) {	// In case that last onset is miss
//	//			int note_event_size = score_analyzer->getMIDIProperty()->getNoteEvent().size();
//	//			int one_before_mnn_score = score_analyzer->getMIDIProperty()->getNoteEvent()[note_event_size - 2]->getMIDINoteNumber();
//	//			int score_mnn = score_analyzer->getMIDIProperty()->getNoteEvent()[note_event_size - 1]->getMIDINoteNumber();
//	//			if ((one_before_mnn == one_before_mnn_score) && (miss_mnn == score_mnn)) score_analyzer->getMIDIProperty()->insertMissNoteEvent(note_event_size - 1);
//	//		}
//	//	}
//	//}
//
//	/* ================================
//		Refine comparing xml and score-midi using DTW
//			xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
//			xml_miss_notes[n] = { (measure_num, part_id, note_num, pre_on_measure_num, pre_on_note_num, consecutive_miss_note), ... }
//			xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }
//
//			mnn_midi, mnn_xml: Onset midi note number list which is grouped by every measure
//			idx_midi, idx_xml: Onset index which is counted along with all data
//	================================ */
//
//	// Rearrange data (mnn_midi, idx_midi)
//	std::vector<std::vector<int>> mnn_xml, mnn_midi, idx_xml, idx_midi;	// Devided at measure
//	int measure_num = 1;
//	std::vector<int> temp, temp_for_idx;
//	int sum_count = 0;
//	std::unordered_map<int, int> idx_start_in_the_measure;
//	idx_start_in_the_measure[0] = 0;
//	for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()){
//		auto measure = note->getOnsetMeasure();
//		//echoes(measure, note->getMIDINoteNumber());
//		if (measure_num == measure) {
//			temp.emplace_back(note->getMIDINoteNumber());
//			temp_for_idx.emplace_back(sum_count);
//		}
//		else {
//			if (temp.size() == 0) {
//				temp.emplace_back(-1);
//				temp_for_idx.emplace_back(-1);
//			}
//			mnn_midi.emplace_back(temp);
//			idx_midi.emplace_back(temp_for_idx);
//			temp.clear();
//			temp.emplace_back(note->getMIDINoteNumber());
//			temp_for_idx.clear();
//			temp_for_idx.emplace_back(sum_count);
//			++measure_num;
//			idx_start_in_the_measure[measure_num] = sum_count;
//		}
//		++sum_count;
//	}
//	mnn_midi.emplace_back(temp);
//	idx_midi.emplace_back(temp_for_idx);
//
//
//	// Rearrange data (mnn_xml, idx_xml)
//	measure_num = 1;
//	sum_count = 0;
//	temp.clear();
//	temp_for_idx.clear();
//	for (auto attack : xml_attack) {
//		auto measure = std::get<0>(attack);
//		auto part_id = std::get<1>(attack);
//		auto note_num = std::get<2>(attack);
//		auto mnn = dic[std::make_tuple(measure, part_id, note_num)];
//		//echoes(measure, mnn);
//		if (measure_num == measure) {
//			temp.emplace_back(mnn);
//			temp_for_idx.emplace_back(sum_count);
//		}
//		else {
//			if (temp.size() == 0) {
//				temp.emplace_back(-1);
//				temp_for_idx.emplace_back(-1);
//			}
//			mnn_xml.emplace_back(temp);
//			idx_xml.emplace_back(temp_for_idx);
//			temp.clear();
//			temp.emplace_back(mnn);
//			temp_for_idx.clear();
//			temp_for_idx.emplace_back(sum_count);
//			++measure_num;
//		}
//		++sum_count;
//	}
//	mnn_xml.emplace_back(temp);
//	idx_xml.emplace_back(temp_for_idx);
//
//	// Search matching between mnn_midi and mnn_xml
//	std::unordered_map<int, int> matching_midi_xml;
//	if (mnn_midi.size() != mnn_xml.size()) echo("The numbers of measures of midi and xml are different");
//	else {
//		DynamicTimeWarping DTW;
//		std::unordered_map<int, std::vector<std::tuple<int, int, int, int>>> miss_candidate;
//		std::vector<std::vector<std::pair<int, int>>> path;
//		for (int m = 0; m < mnn_midi.size(); ++m) {
//			//echoes("Measure:", m + 1);
//			//echo_v1(mnn_midi[m]);
//			//echo_v1(mnn_xml[m]);
//			//echo_v1(idx_midi[m]);
//			//echo_v1(idx_xml[m]);
//			//echo("");
//			std::vector<double> double_mnn_midi(mnn_midi[m].begin(), mnn_midi[m].end());
//			std::vector<double> double_mnn_xml(mnn_xml[m].begin(), mnn_xml[m].end());
//			if (double_mnn_midi.size() > 0 && double_mnn_xml.size() > 0) {
//				DTW.setInput(double_mnn_midi, double_mnn_xml);
//				DTW.fastDTW();
//				auto t_path = DTW.getPath();
//				path.emplace_back(t_path);
//				std::vector<std::tuple<int, int, int, int>> temp_path_info;
//				for (auto p : t_path) {
//					//echoes(p.first, p.second, mnn_midi[m][p.first], mnn_xml[m][p.second]);
//					if (p.second, mnn_midi[m][p.first] != mnn_xml[m][p.second]) {
//						temp_path_info.emplace_back(std::make_tuple(p.first, mnn_midi[m][p.first], p.second, mnn_xml[m][p.second]));
//					}
//					else {
//						auto sum_count_midi = idx_midi[m][p.first];
//						auto sum_count_xml = idx_xml[m][p.second];
//						matching_midi_xml[sum_count_midi] = sum_count_xml;
//					}
//				}
//				if (temp_path_info.size() > 0) {
//					miss_candidate[m + 1] = temp_path_info;
//				}
//			}
//			else {
//				std::pair<int, int> t_pair = std::make_pair(0, 0);
//				std::vector<std::pair<int, int>> t_path = { t_pair };
//				path.emplace_back(t_path);
//			}
//		}
//
//		//for (auto key : matching_midi_xml) echoes(key.first, key.second);
//
//		// Check
//		// TODO? no need? check miss note written by xml and score-midi
//		if (miss_candidate.size() > 0) {
//			for (auto measure : miss_candidate) {
//				_echoes(M, "Measure:", measure.first);
//				_echo_v1(M, mnn_midi[measure.first - 1]);
//				_echo_v1(M, mnn_xml[measure.first - 1]);
//				for (auto mc : measure.second) echoes("idx:", std::get<0>(mc), "MNN:", std::get<1>(mc));
//
//				std::vector<int> miss_candidate_mnn_xml;
//				int n_mc_xml = 0;
//				for (auto note : xml_miss_notes) {
//					auto measure_miss_xml = std::get<0>(note);
//					auto part_id_miss_xml = std::get<1>(note);
//					auto note_num_miss_xml = std::get<2>(note);
//					auto mnn_miss_xml = dic[std::make_tuple(measure_miss_xml, part_id_miss_xml, note_num_miss_xml)];
//
//					if (measure.first == measure_miss_xml) {
//						++n_mc_xml;
//						miss_candidate_mnn_xml.emplace_back(mnn_miss_xml);
//					}
//					else if (measure.first < measure_miss_xml) break;
//				}
//
//				_echoes(M, "Miss note from xml, Count:", n_mc_xml);
//				_echo_v1(M, miss_candidate_mnn_xml);
//
//				// Set miss note information
//				for (auto mc : measure.second) {
//					auto idx_of_the_measure = std::get<0>(mc);
//					auto mnn_miss_midi = std::get<1>(mc);
//					int idx_miss_midi = idx_start_in_the_measure[measure.first] + idx_of_the_measure;
//					auto mnn = score_analyzer->getMIDIProperty()->getNoteEvent()[idx_miss_midi]->getMIDINoteNumber();
//					_echoes(M, "Confirm matching of index and mnn: MIDI=", mnn, "XML=", mnn_miss_midi);
//					score_analyzer->getMIDIProperty()->insertMissNoteEvent(idx_miss_midi);
//				}
//				_echo(M, "");
//			}
//		}
//
//		//for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
//		//	echoes(note->getMIDINoteNumber(), note->getOnsetMeasure(), note->getChordIdx(), note->getModChordIdx(), note->getIsMiss());
//		//}
//
//
//		/* ================================
//		Rearrange onset list
//			attack deviation: Confirm deviation of chord, and rearrange the order of score onset list
//				xml_onset[n] = { (measure_num, part_id, note_num, mnn), ... }
//				xml_attack[n] = { (measure_num, part_id, note_num, attack_dev), ... }
//		================================ */
//
//		int idx_midi_in_the_measure = 0;
//		int idx_xml_in_the_measure = 0;
//		int count_sum_midi = 0;
//		int current_measure = 1;
//		int pre_chord_idx = -1;
//		std::vector<std::tuple<int, int, int, int, int>> chord_list_midi, t_chord_list_midi;
//		for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
//			if (note->getIsMiss() == true) {
//				++count_sum_midi;
//				++idx_midi_in_the_measure;
//				continue;
//			}
//
//			if (current_measure != note->getOnsetMeasure()) {
//				idx_midi_in_the_measure = 0;
//				++current_measure;
//			}
//
//			auto mnn = note->getMIDINoteNumber();
//			auto chord_idx = note->getChordIdx();
//			//echoes(current_measure, note->getOnsetMeasure(), mnn);
//			//echoes(current_measure, mnn, chord_idx, pre_chord_idx);
//			//echoes(score_analyzer->getMIDIProperty()->getNoteEvent().size(), count_sum_midi);
//			bool is_chord_find = false;
//			if (pre_chord_idx == chord_idx) {
//				t_chord_list_midi.emplace_back(std::make_tuple(count_sum_midi, current_measure, idx_midi_in_the_measure, chord_idx, mnn));
//
//				if (score_analyzer->getMIDIProperty()->getNoteEvent().size() == count_sum_midi + 1) {
//					if (t_chord_list_midi.size() > 1) {
//						is_chord_find = true;
//						chord_list_midi = t_chord_list_midi;
//					}
//				}
//			}
//			else {
//				if (t_chord_list_midi.size() > 1) {
//					is_chord_find = true;
//					chord_list_midi = t_chord_list_midi;
//				}
//
//				t_chord_list_midi.clear();
//				t_chord_list_midi.emplace_back(std::make_tuple(count_sum_midi, current_measure, idx_midi_in_the_measure, chord_idx, mnn));
//				pre_chord_idx= chord_idx;
//			}
//
//			if (is_chord_find == true) {	// At one chord
//				//echo("==========");
//				std::vector<int> idx_all_midi, idx_all_xml;
//				for (auto chord : chord_list_midi) {
//					//echoes(std::get<0>(chord), std::get<1>(chord), std::get<2>(chord), std::get<3>(chord), std::get<4>(chord));
//					//auto noteeve = score_analyzer->getMIDIProperty()->getNoteEvent()[std::get<0>(chord)];
//					//echoes(noteeve->getChordIdx(), noteeve->getMIDINoteNumber());
//					auto idx_midi = std::get<0>(chord);
//					auto measure = std::get<1>(chord);		// 1 start
//					auto idx_midi_measure = std::get<2>(chord);		// 0 start
//					auto chord_idx_midi = std::get<3>(chord);
//					auto mnn_chord_note = std::get<4>(chord);
//					auto path_measure_fixed = path[measure - 1];
//					auto t_mnn_midi = mnn_midi[measure - 1];
//					auto t_mnn_xml = mnn_xml[measure - 1];
//
//					idx_all_midi.emplace_back(idx_midi);
//					idx_all_xml.emplace_back(matching_midi_xml[idx_midi]);
//
//					//echoes("Measure:", measure);
//					//for (auto p : path_measure_fixed) {
//					//	if ((p.first == idx_midi_measure) && (t_mnn_midi[p.first] == t_mnn_xml[p.second]) && (t_mnn_midi[idx_midi_measure] == mnn_chord_note)) {
//					//		echoes(p.first, idx_midi_measure, t_mnn_midi[p.first], mnn_midi[measure - 1][idx_midi_measure], chord_idx_midi);
//					//	}
//					//}
//				}
//
//				//auto noteeve = score_analyzer->getMIDIProperty()->getNoteEvent();
//				//for (int i = 0; i < idx_all_midi.size(); ++i) {
//				//	auto attack = xml_attack[idx_all_xml[i]];
//				//	auto dev = std::get<3>(attack);
//				//	auto mnn_attack = dic[std::make_tuple(std::get<0>(attack), std::get<1>(attack), std::get<2>(attack))];
//				//	echoes(noteeve[idx_all_midi[i]]->getMIDINoteNumber(), noteeve[idx_all_midi[i]]->getChordIdx(), mnn_attack, dev);
//				//}
//
//
//				// Swap based on attack deviation of chord
//				//std::vector<std::pair<std::tuple<int, int, double>, std::tuple<int, int, double>>> swap_list;
//				std::vector<double> dev_list;
//				for (int i = 0; i < idx_all_midi.size(); ++i) {
//					auto attack = xml_attack[idx_all_xml[i]];
//					auto dev = std::get<3>(attack);
//					auto mnn_attack = dic[std::make_tuple(std::get<0>(attack), std::get<1>(attack), std::get<2>(attack))];
//					dev_list.emplace_back(dev);
//				}
//
//				auto argsort_dev = argSort(dev_list);	// ex) argsort_dev = [1, 3, 0, 2], idx_all_midi = [3, 4, 5, 6]
//				auto cp_argsort_dev = argsort_dev;
//				//echo_v1(dev_list);
//				//echo_v1(argsort_dev);
//				for (int ordered_num = 0; ordered_num < argsort_dev.size(); ++ordered_num) {	// ex) 0
//					for (int i = 0; i < argsort_dev.size(); ++i) {
//						if (argsort_dev[i] == ordered_num) {	// argsort_dev[i]: 1 -> 3 -> 0	<--> 0
//							if (ordered_num == i) break;
//							else {
//								// Get indexes that is counted in a row
//								auto idx_observed_midi = idx_all_midi[argsort_dev[ordered_num]];	// argsort_dev[0] = 1 -> idx_all_midi[1] = 4
//								auto idx_target_midi = idx_all_midi[argsort_dev[i]];	// argsort_dev[2] = 0 -> idx_all_midi[0] = 3
//								auto idx_observed_xml = idx_all_xml [argsort_dev[ordered_num]] ;
//								auto idx_target_xml = idx_all_xml[argsort_dev[i]];
//
//								// Get MNN from midi
//								auto mnn_observed_midi = score_analyzer->getMIDIProperty()->getNoteEvent()[idx_observed_midi]->getMIDINoteNumber();
//								auto mnn_target_midi = score_analyzer->getMIDIProperty()->getNoteEvent()[idx_target_midi]->getMIDINoteNumber();
//
//								// Get attack deviation from xml
//								auto attack_observed = xml_attack[idx_observed_xml];
//								auto dev_observed = std::get<3>(attack_observed);
//								auto attack_target = xml_attack[idx_target_xml];
//								auto dev_target = std::get<3>(attack_target);
//
//								//echoes("swap", mnn_observed_midi, mnn_target_midi);
//								//echoes(mnn_observed_midi, mnn_target_midi, dev_observed, dev_target);
//
//								score_analyzer->getMIDIProperty()->swapNoteEvent(idx_observed_midi, idx_target_midi);
//								//echo("pre");
//								//echo_v1(argsort_dev);
//								std::swap(argsort_dev[ordered_num], argsort_dev[i]);
//								//echo("aft");
//								//echo_v1(argsort_dev);
//
//								// swap_list: std::vector<std::pair<std::tuple<int, int, double>, std::tuple<int, int, double>>>
//								//std::tuple<int, int, double> swap_observed_info = std::make_tuple(idx_observed_midi, idx_observed_xml, dev_observed);
//								//std::tuple<int, int, double> swap_target_info = std::make_tuple(idx_target_midi, idx_target_xml, dev_target);
//								//auto swap_pair = std::make_pair(swap_observed_info, swap_target_info);
//								//swap_list.emplace_back(swap_pair);
//
//								break;
//							}
//						}
//					}
//				}
//
//
//				// Reswap: In case of (one_before_mnn > mnn && one_before_dev == dev), Usually none
//				bool is_swap = false;
//				for (int i = 0; i < cp_argsort_dev.size(); ++i) {
//					if (i != cp_argsort_dev[i]) {
//						is_swap = true;
//						break;
//					}
//				}
//				if (is_swap == true) {
//					//echo_v1(idx_all_midi);
//					//echo_v1(idx_all_xml);
//
//					bool is_reswap = true;
//					while (true) {
//						if (is_reswap == false) break;
//
//						is_reswap = false;
//						auto one_before_mnn = score_analyzer->getMIDIProperty()->getNoteEvent()[idx_all_midi[0]]->getMIDINoteNumber();
//						auto one_before_attack = xml_attack[idx_all_xml[cp_argsort_dev[0]]];
//						auto one_before_dev = std::get<3>(one_before_attack);
//						for (int i = 1; i < idx_all_midi.size(); ++i) {
//							auto mnn = score_analyzer->getMIDIProperty()->getNoteEvent()[idx_all_midi[i]]->getMIDINoteNumber();
//							auto attack = xml_attack[idx_all_xml[cp_argsort_dev[i]]];
//							auto dev = std::get<3>(attack);
//							//auto mnn_attack = dic[std::make_tuple(std::get<0>(attack), std::get<1>(attack), std::get<2>(attack))];
//							//echoes(mnn, dev, mnn_attack);
//
//							if (one_before_mnn > mnn) {
//								//echo("ob_mnn > mnn");
//								if (one_before_dev == dev) {
//									//echo("reswap");
//									score_analyzer->getMIDIProperty()->swapNoteEvent(i - 1, i);
//									is_reswap = true;
//								}
//							}
//							one_before_mnn = mnn;
//							one_before_dev = dev;
//						}
//					}
//				}
//			}
//			++count_sum_midi;
//			++idx_midi_in_the_measure;
//		}
//	}
//
//	//for (auto note : score_analyzer->getMIDIProperty()->getNoteEvent()) {
//	//	echoes(note->getMIDINoteNumber(), note->getModChordIdx());
//	//}
//
//
//	/* ================================
//		Add extra note events
//			xml_extra_notes[n] = { (measure_num, mnn, beat), ... }
//	================================ */
//	auto score_timesig_list = score_analyzer->getTimeSig();
//	auto score_tempo_list = score_analyzer->getTempo();
//	int res = score_analyzer->getMIDIProperty()->getResolution();
//
//	for (auto extra : xml_extra_notes) {
//		std::unordered_map<std::string, int> note_info;
//		int measure = std::get<0>(extra);
//		int beat = int(std::get<2>(extra) * 1000);
//		note_info["mnn"] = std::get<1>(extra);
//		note_info["on_tick"] = midif::beat2Tick(measure, beat, res, score_timesig_list);
//
//		//std::cout << std::endl;
//		//echoes(measure, beat, note_info["mnn"], note_info["on_tick"]);
//		//std::cout << std::endl;
//
//		auto mbt = midif::getMBT(res, note_info["on_tick"], score_timesig_list);
//		note_info["on_measure"] = std::get<0>(mbt);
//		note_info["on_beat"] = std::get<1>(mbt);
//		note_info["on_tickbar"] = std::get<2>(mbt);
//		note_info["dur"] = 10;
//		note_info["vel"] = 100;
//		note_info["tmp"] = midif::getTempo(note_info["on_tick"], score_tempo_list, 0);	// 0: msec based
//		note_info["on_msec"] = midif::tick2Msec(note_info["on_tick"], res, score_tempo_list);
//		note_info["dur_msec"] = midif::getLengthTick2Msec(note_info["dur"], note_info["on_tick"], res, score_tempo_list);
//		note_info["channel_num"] = 0;
//		score_analyzer->getMIDIProperty()->insertExtraNoteEvent(note_info);
//	}
//
//	mod_score_noteon = score_analyzer->getMIDIProperty()->getNoteEvent();	// Reload
//
//	// TODO, miss note remove
//
//	//for (auto noteon : mod_score_noteon) {
//	//	echoes(noteon->getMIDINoteNumber(), noteon->getOnsetTick(), noteon->getChordIdx(), noteon->getModChordIdx(), noteon->getIsExtra());
//	//}
//}

