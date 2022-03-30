#include "pch.h"
#include "Madmom.h"
#include "../Shared/DataProc.h"
#include "../Shared/MathProc.h"
#include "../Shared/passPyCpp.h"


MadmomCall::MadmomCall() {
	fs = 0;
	tick = 0;
	music_length = 0;
	py_module_folder_path = "C:/Users/brume/Documents/Project/Cplusplus/CMAKE/PythonMadmom/";
	py_module_file_name = "pyMadmom";
	py_module_class_name = "MadmomAnalyzeWave";
	wave_path = "NULL";
	is_signal = false;
	is_spec = false;
	is_cqt = false;
	is_chroma = false;

	//signal.clear();
	//est_onset.clear();
	freqs.clear();
	times.clear();
	spectrogram.clear();
	phase.clear();

	cqt.clear();
	cqt_times.clear();

	chroma.clear();
	chroma_times.clear();
}


void MadmomCall::analyzeWave() {
	//boost::python::dict dummy;
	//setDictBoost(dummy, "dummy", 0);	// Create dummy dict object
	boost::python::dict pargs;
	setDictBoost(pargs, "wave_path", wave_path);
	auto py_madmom_instance = setClass(py_module_folder_path, py_module_file_name, py_module_class_name, pargs);

	auto pyresult = setMethod(py_madmom_instance, "analyzeSignal", pargs);

	auto pyfs = setMethod(py_madmom_instance, "getFs", pargs);
	auto pysignal = setMethod(py_madmom_instance, "getSignal", pargs);
	auto pytick = setMethod(py_madmom_instance, "getTick", pargs);

	fs = extractInt(pyfs);
	auto signal = extractNumpyIntVec(pysignal);
	tick = extractDouble(pytick);
	music_length = signal.size() * tick;
	is_signal = true;

	//echoes(fs, tick, signal.size());
}


void MadmomCall::analyzeSTFT(const int frame, const int hop_msec, const int fft) {
	// Absolutely, after analyzeWave()

	boost::python::dict pargs;
	setDictBoost(pargs, "wave_path", wave_path);
	auto py_madmom_instance = setClass(py_module_folder_path, py_module_file_name, py_module_class_name, pargs);

	if (is_signal == true) {
		setDictBoost(pargs, "frame", frame);
		int hop = int(fs * hop_msec / 1000.);	// ex) 10msec: 44100 * 0.01 = 441
		setDictBoost(pargs, "hop", hop);
		setDictBoost(pargs, "fft", fft);
		//setDictBoost(pargs, "fs", fs);

		auto pyresult = setMethod(py_madmom_instance, "exeSTFT", pargs);

		auto pyspectrogram = setMethod(py_madmom_instance, "getSpectrogram", pargs);
		auto pyphase = setMethod(py_madmom_instance, "getPhase", pargs);
		auto pyfreqs = setMethod(py_madmom_instance, "getFreqBins", pargs);
		//auto pytimes = setMethod(py_madmom_instance, "getTicks", pargs);

		spectrogram = extractNumpyMat(pyspectrogram);
		phase = extractNumpyMat(pyphase);
		freqs = extractNumpyVec(pyfreqs);
		//times = extractNumpyVec(pytimes);

		times.resize(spectrogram.size());
		for (int i = 0; i < times.size(); ++i) times[i] = i * hop_msec / 1000.;

		is_spec = true;

		//echoes(freqs.size(), times.size());
		echoes(spectrogram.size(), spectrogram[0].size(), phase.size(), phase[0].size());
	}

}

std::vector<std::vector<double>> MadmomCall::getUnwrapPhase() {
	if (is_spec == true) {
		return unwrapNumpy(phase);
	}
	else {
		std::vector<std::vector<double>> ret;
		std::vector<double> temp = { 0 };
		ret.emplace_back(temp);
		return ret;
	}
}


std::vector<int> MadmomCall::onsetDetecting(const int fps) const {
	boost::python::dict pargs;
	setDictBoost(pargs, "wave_path", wave_path);
	auto py_madmom_instance = setClass(py_module_folder_path, py_module_file_name, py_module_class_name, pargs);

	setDictBoost(pargs, "fps", fps);

	auto pyresult = setMethod(py_madmom_instance, "onsetDetecting", pargs);
	auto py_estonset = setMethod(py_madmom_instance, "getOnset", pargs);

	return extractNumpyIntVec(py_estonset);
}

void MadmomCall::analyzeChroma(const int fps) {
	boost::python::dict pargs;
	setDictBoost(pargs, "wave_path", wave_path);
	auto py_madmom_instance = setClass(py_module_folder_path, py_module_file_name, py_module_class_name, pargs);

	setDictBoost(pargs, "fps", fps);

	auto pyresult = setMethod(py_madmom_instance, "analyzeChroma", pargs);
	auto pychroma = setMethod(py_madmom_instance, "getChroma", pargs);

	chroma = extractNumpyMat(pychroma);
	chroma_times.resize(chroma.size());
	for (int i = 0; i < chroma.size(); ++i) chroma_times[i] = fps * i;

	is_chroma = true;
	//echo(chroma.size(), chroma[0].size());
}

void MadmomCall::analyzeCQT(const int fps) {
	boost::python::dict pargs;
	setDictBoost(pargs, "wave_path", wave_path);
	auto py_madmom_instance = setClass(py_module_folder_path, py_module_file_name, py_module_class_name, pargs);

	setDictBoost(pargs, "fps", fps);

	auto pyresult = setMethod(py_madmom_instance, "exeCQT", pargs);
	auto pycqt = setMethod(py_madmom_instance, "getCQT", pargs);

	cqt = extractNumpyMat(pycqt);
	cqt_times.resize(cqt.size());
	for (int i = 0; i < cqt.size(); ++i) cqt_times[i] = fps * i;

	is_cqt = true;
	echoes(cqt.size(), cqt[0].size());
}

std::vector<double> MadmomCall::getCQTFreqs() {
	std::vector<double> freqs(88);
	for (int i = 0; i < freqs.size(); ++i) {
		int mnn = i + 21;	// A0(21) start
		freqs[i] = mnn2freq(mnn);
	}
	return freqs;
}

double MadmomCall::mnn2freq(const int mnn) {
	return 440 * pow(2, double(mnn - 69) / 12);
}

double MadmomCall::mcent2freq(const int mcent) {
	return 440 * pow(2, double(mcent - 6900) / 1200);
}

int MadmomCall::freq2mnn(const double f) {
	auto temp = log2(f / 440.) * 12 + 69;
	return round(temp);
}

int MadmomCall::freq2mcent(const double f) {
	auto temp = log2(f / 440.) * 1200 + 6900;
	return round(temp);
}

