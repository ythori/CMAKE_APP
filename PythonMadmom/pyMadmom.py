import numpy as np
import random
import madmom
#from madmom.audio.spectrogram import SemitoneBandpassSpectrogram
from madmom.processors import Processor, SequentialProcessor
import librosa


def _dcp_flatten(fs):
    """Flatten spectrograms for DeepChromaProcessor. Needs to be outside
        of the class in order to be picklable for multiprocessing.
    """
    return np.concatenate(fs).reshape(len(fs), -1)


class ModifiedDeepChromaProcessor(SequentialProcessor):
    def __init__(self, fps=10, fmin=65, fmax=2100, unique_filters=True, models=None,
                 **kwargs):
        from madmom.models import CHROMA_DNN
        from madmom.audio.signal import SignalProcessor, FramedSignalProcessor
        from madmom.audio.stft import ShortTimeFourierTransformProcessor
        from madmom.audio.spectrogram import LogarithmicFilteredSpectrogramProcessor
        from madmom.ml.nn import NeuralNetworkEnsemble
        # signal pre-processing
        sig = SignalProcessor(num_channels=1, sample_rate=44100)
        frames = FramedSignalProcessor(frame_size=8192, fps=fps)    # fps10=1f/100msec, fps100=1f/10msec
        stft = ShortTimeFourierTransformProcessor()  # caching FFT window
        spec = LogarithmicFilteredSpectrogramProcessor(
            num_bands=24, fmin=fmin, fmax=fmax, unique_filters=unique_filters)
        # split the spectrogram into overlapping frames
        spec_signal = SignalProcessor(sample_rate=10)
        spec_frames = FramedSignalProcessor(frame_size=15, hop_size=1, fps=10)
        # predict chroma bins with a DNN
        nn = NeuralNetworkEnsemble.load(models or CHROMA_DNN, **kwargs)
        # instantiate a SequentialProcessor
        super(ModifiedDeepChromaProcessor, self).__init__([
            sig, frames, stft, spec, spec_signal, spec_frames, _dcp_flatten, nn
        ])


from madmom.audio.spectrogram import FilteredSpectrogram
from scipy.signal import filtfilt
from madmom.audio.filters import SemitoneBandpassFilterbank
from madmom.audio.signal import FramedSignal, Signal
import madmom.audio.signal

class modSemitoneBandpassSpectrogram(FilteredSpectrogram):
    """
    Construct a semitone spectrogram by using a time domain filterbank of
    bandpass filters as described in [1]_.

    Parameters
    ----------
    signal : Signal
        Signal instance.
    fps : float, optional
        Frame rate of the spectrogram [Hz].
    fmin : float, optional
        Lowest frequency of the spectrogram [Hz].
    fmax : float, optional
        Highest frequency of the spectrogram [Hz].

    References
    ----------
    .. [1] Meinard Müller,
           "Information retrieval for music and motion", Springer, 2007.

    """
    # pylint: disable=super-on-old-class
    # pylint: disable=super-init-not-called
    # pylint: disable=attribute-defined-outside-init

    def __init__(self, signal, fps=50., fmin=27.5, fmax=4200.):
        # this method is for documentation purposes only
        pass

    def __new__(cls, signal, fps=50., fmin=27.5, fmax=4200.):
        # check if we got a mono Signal
        if not isinstance(signal, Signal) or signal.num_channels != 1:
            signal = Signal(signal, num_channels=1)
        sample_rate = float(signal.sample_rate)
        # keep a reference to the original signal
        signal_ = signal
        # determine how many frames the filtered signal will have
        num_frames = np.round(len(signal) * fps / sample_rate) + 1
        # compute the energy of the frames of the bandpass filtered signal
        filterbank = SemitoneBandpassFilterbank(fmin=fmin, fmax=fmax)
        bands = []
        for filt, band_sample_rate in zip(filterbank.filters,
                                          filterbank.band_sample_rates):
            # frames should overlap 50%
            frame_size = np.round(2 * band_sample_rate / float(fps))
            # down-sample audio if needed
            #if band_sample_rate != signal.sample_rate:
            #    signal = madmom.audio.signal.resample(signal_, band_sample_rate)
            # filter the signal
            b, a = filt
            filtered_signal = filtfilt(b, a, signal)
            # normalise the signal if it has an integer dtype
            try:
                filtered_signal /= np.iinfo(signal.dtype).max
            except ValueError:
                pass
            # compute the energy of the filtered signal
            # Note: 1) the energy of the signal is computed with respect to the
            #          reference sampling rate as in the MATLAB chroma toolbox
            #       2) we do not sum here, but rather after splitting the
            #          signal into overlapping frames to avoid doubled
            #          computation due to the overlapping frames
            filtered_signal = filtered_signal ** 2 / band_sample_rate * 22050.
            # split into overlapping frames
            frames = FramedSignal(filtered_signal, frame_size=frame_size,
                                  fps=fps, sample_rate=band_sample_rate,
                                  num_frames=num_frames)
            # finally sum the energy of all frames
            bands.append(np.sum(frames, axis=1))
        # cast as SemitoneBandpassSpectrogram
        obj = np.vstack(bands).T.view(cls)
        # save additional attributes
        obj.filterbank = filterbank
        obj.fps = fps
        return obj

    def __array_finalize__(self, obj):
        if obj is None:
            return
        # set default values here
        self.filterbank = getattr(obj, 'filterbank', None)
        self.fps = getattr(obj, 'fps', None)


class MadmomAnalyzeWave:
    wave_path = None
    signal = None
    fs = None
    tick = None
    spectrogram = None
    bins = None
    ticks = None
    phase = None
    chroma = None
    est_onset = None
    cqt = None

    def __init__(self, args):
        self.wave_path = args["wave_path"]
        self.signal = None
        self.fs = None
        self.tick = None
        self.spectrogram = None
        self.bins = None
        self.ticks = None
        self.phase = None
        self.chroma = None
        self.est_onset = None
        self.cqt = None

    def getSignal(self, args):
        return self.signal.astype(int)

    def getFs(self, args):
        return self.fs

    def getTick(self, args):
        return self.tick

    def getSpectrogram(self, args):
        return self.spectrogram.astype(float)

    def getFreqBins(self, args):
        return self.bins

    def getPhase(self, args):
        return self.phase.astype(float)

    #def getTicks(self, args):
    #    return self.ticks

    def getChroma(self, args):
        return self.chroma.astype(float)

    def getCQT(self, args):
        return self.cqt.astype(float)

    def getOnset(self, args):
        return self.est_onset

    def analyzeSignal(self, args):
        self.signal, self.fs = madmom.io.audio.load_wave_file(self.wave_path)
        self.tick = 1. / self.fs

    def exeSTFT(self, args):
        frame = args["frame"]
        hop = args["hop"]
        fft = args["fft"]
        #fs = args["fs"]

        # frame: For frames_signal (extract from signal), if frame > fft, using zero-padding
        spec = madmom.audio.stft.ShortTimeFourierTransform(self.wave_path, frame_size=frame, hop_size=hop, fft_size=fft, circular_shift=True)
        self.bins = spec.bin_frequencies
        self.phase = spec.phase()
        self.spectrogram = spec.spec()
        #tick_frame = float(hop) / fs
        #self.ticks = np.zeros(self.spectrogram.shape[0], dtype=float)
        #for i in range(len(self.ticks)):
        #    self.ticks[i] = tick_frame * i

    def exeCQT(self, args):
        #from madmom.audio.spectrogram import SemitoneBandpassSpectrogram
        #n_bins = args["n_bins"]
        fps = args["fps"]
        #signal = madmom.audio.signal.Signal(self.wave_path)
        #self.cqt = modSemitoneBandpassSpectrogram(signal, fps=fps)

        self.analyzeSignal(args)
        y, sr = librosa.load(self.wave_path, sr = self.fs)

        n_octave        = 7
        bins_per_octave = 12

        # If set hop_length from assigned fps, it may be too large.
        # Therefore, firstly, set small hop_length and analyze CQT, nextly, extract nearest frame's spectrum

        hop_length = 256
        spf_hop = float(hop_length) / sr    # Second for one frame(= hop_length), ex) 0.005805
        cqt_fps = 1. / spf_hop  # Actual CQT analysis FPS

        #fps = 20
        spf_fps = 1. / fps  # Expected second of assigned fps

        #t_hop = sr * spf_fps     # Ideal hop size, but should be multiple of 2**octave
        #multi_base = int(2 ** n_octave) # 128
        #multi_rate = int(t_hop / multi_base) if t_hop % multi_base == 0 else int(t_hop / multi_base) - 1
        #hop_length = int(multi_base * multi_rate)   # 128*n (ex) 1024, 2048)

        C = librosa.cqt(y, sr = sr,
                        hop_length = hop_length,
                        n_bins = n_octave*bins_per_octave + 4,  # A0 - C6, 7octave + A, A#, B, C
                        bins_per_octave = bins_per_octave,
                        fmin = 27.5)  # or 32.7
        M  = np.abs(C) 
        spec = M
        #spec = librosa.amplitude_to_db(M, ref = np.max)   # [88, n_time]
        #print(spec.shape)


        spf_hop = float(hop_length) / sr    
        tar_time_idx = []
        sum_time = float(len(y)) / sr   # ex) 90 * 20 = 1800 frame
        sum_frame = int(sum_time * fps)
        tar_time = [spf_fps * i for i in range(sum_frame)]  # [0, 0.05, 0.1, 0.15, ...]
        act_time = [spf_hop * i for i in range(spec.shape[1])]  # [0, 0.0058, 0.0116, 0.0174, ...]
        idx = 0
        add = int((spf_fps / spf_hop) / 2)

        for t_time in tar_time:
            #print(t_time, idx)
            while True:
                if t_time < act_time[idx]:
                    break
                idx += 1
            idx += add
            tar_time_idx.append(idx)

            #if idx < spec.shape[1] - 1:
            #    #print(act_time[idx], act_time[idx+1])
            #    if np.abs(act_time[idx] - t_time) < np.abs(act_time[idx + 1] - t_time):
            #        tar_time_idx.append(idx)
            #        #if act_time[idx] < t_time:
            #        #    tar_time_idx.append(idx)
            #        #else:
            #        #    tar_time_idx.append(idx - 1) if idx - 1 > 0 else tar_time_idx.append(0)
            #        idx += 1
            #    else:
            #        tar_time_idx.append(idx + 1)
            #        #if act_time[idx + 1] < t_time:
            #        #    tar_time_idx.append(idx + 1)
            #        #else:
            #        #    tar_time_idx.append(idx)
            #        idx += 2
            #    #conf = t_time - act_time[tar_time_idx[-1]]
            #    #print(conf)
            #    #if conf < 0:
            #    #    print("Extract spectrum error!!!!!")
            #else:
            #    tar_time_idx.append(spec.shape[1] - 1)
        #print(tar_time_idx)
        self.cqt = spec[:, tar_time_idx].T  # [spec, time] -> [time, spec]


    def analyzeChroma(self, args):
        # fps: 10frame/s = 1frame/0.1s = 1frame/100msec
        fps = args["fps"]
        dcp = ModifiedDeepChromaProcessor(fps=fps)
        self.chroma = dcp(self.wave_path)

    def onsetDetecting(self, args):
        # recommand: num_bands = 24
        fps = args["fps"]
        proc = madmom.features.notes.NotePeakPickingProcessor(fps=fps, pitch_offset=21, pre_max=0.05, post_max=0.05)
        log_filt_spec = madmom.audio.spectrogram.LogarithmicFilteredSpectrogram(self.wave_path, num_bands=24)
        superflux = madmom.features.onsets.superflux(log_filt_spec)
        #self.est_onset = (proc.process_offline(superflux) * 1000).astype(int)
        #print(proc.process_offline(superflux))
        self.est_onset = (proc.process_offline(superflux) * fps * 10).astype(int)


#if __name__ == "__main__":
#    data_address = "C:/Users/brume/Documents/Project/DB/Research/MusicDB/crestmuse/dataset/bac-inv001-schif-g/"
#    data_ex = data_address + "ex.wav"
#    data_scores = data_address + "score.wav"

#    args = {}
#    args["wave_path"] = data_scores
#    MAW = MadmomAnalyzeWave(args)

#    args["fps"] = 10
#    MAW.exeCQT(args)


#    args["frame"] = 2048
#    args["hop"] = 200
#    args["fft"] = 4096
#    MAW.exeSTFT(args)


#    # Signal, fs
#    #MAW.analyzeSignal()
#    #sig_list = MAW.getSignal()
#    #fs = MAW.getFs()
#    #print(sig_list)
#    #print(fs)

#    # STFT, Spectrogram, bins, phase, tick
#    MAW.exeSTFT(frame_size, hop_size, fft_size)
#    #stft = MAW.getSpectrogram()
#    #print(stft.shape)

#    #for i in range(17000):
#    #    stft.append(MAW.getOneCol(i))
#    #    if i%100 == 0:
#    #        print(i)
#    #        stft = []


#    #stft = MAW.getSpec()
#    #print(123)
#    #print(stft[10000, 1000])
#    #print(2)
#    #spectrogram = MAW.getSpectrogram()
#    #print(3)
#    #bins = MAW.getFreqBins()
#    #print(4)
#    #phase = MAW.getPhase()
#    #print(5)
#    #tick = MAW.getPhase()
#    #print(len(stft), len(stft[0]), len(spectrogram), len(spectrogram[0]), len(bins), len(phase), len(phase[0]), tick)
#    #print(stft[10000, 1000], spectrogram[10000, 1000], bins[:10], phase[10000, 1000])


#    #MAW.analyzeChroma(10)
#    #chroma = MAW.getChroma()
#    #print(chroma.shape)
#    #MAW.onsetDetecting()
#    #onset = MAW.getOnset()
#    #print(onset)




