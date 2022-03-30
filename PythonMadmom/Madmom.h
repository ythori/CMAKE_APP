/* ================================
Created by Hori on 2020/4/10

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Madmom

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Usage

================================ */

#ifndef MADMOM_H
#define MADMOM_H

//#define BOOST_PYTHON_STATIC_LIB

#include "pch.h"
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

class MadmomCall
{
public:
	MadmomCall();

	void setWavePath(const std::string path) { wave_path = path; }

	void analyzeWave();
	void analyzeSTFT(const int frame, const int hop_msec, const int fft);
	void analyzeCQT(const int fps);
	void analyzeChroma(const int fps);

	int getFs() const { return fs; }
	double getTick() const { return tick; }
	double getLength() const { return music_length; }
	//std::vector<int> getSignal() const { return signal; }
	//std::vector<int> getEstOnset() const { return est_onset; }
	std::vector<double> getFreqs() const { return freqs; }
	std::vector<double> getTimes() const { return times; }
	std::vector<double> getCQTFreqs();
	std::vector<double> getCQTTimes() const { return cqt_times; }
	std::vector<double> getChromaTimes() const { return chroma_times; }
	std::vector<std::vector<double>> getSpectrogram() const { return spectrogram; }
	std::vector<std::vector<double>> getPhase() const { return phase; }
	std::vector<std::vector<double>> getCQT() const { return cqt; }
	std::vector<std::vector<double>> getChroma() const { return chroma; }
	std::vector<std::vector<double>> getUnwrapPhase();

	std::vector<int> onsetDetecting(const int fps) const;	// Return msec

	double mnn2freq(const int mnn);
	double mcent2freq(const int mcent);
	int freq2mnn(const double f);
	int freq2mcent(const double f);



private:
	int fs;
	double tick, music_length;
	bool is_signal, is_spec, is_cqt, is_chroma;
	std::string py_module_folder_path, py_module_file_name, py_module_class_name;
	//std::vector<int> signal, est_onset;
	std::vector<double> freqs, times, cqt_times, chroma_times;
	std::vector<std::vector<double>> spectrogram, phase, cqt, chroma;
	std::string wave_path;
};

#endif // MADMOM_H
