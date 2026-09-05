import numpy as np
import matplotlib.pyplot as plt

files_num = 4 

# File names
filenames = [
    r"D:\My_works\DSP C\7_fourier_transform_2\sig_20_5_fs_1K.dat",
    r"D:\My_works\DSP C\7_fourier_transform_2\Output_bins.dat",
    r"D:\My_works\DSP C\7_fourier_transform_2\Output_mags.dat",
    r"D:\My_works\DSP C\7_fourier_transform_2\Output_phases.dat",
]


def plot_one(filename):
    signal = np.loadtxt(filename)
    samples = np.arange(len(signal))

    plt.figure(figsize=(10, 5))
    plt.plot(samples, signal, marker='.')

    plt.ylabel("Amplitude")
    plt.title("Signal")

    plt.grid(True)
    plt.tight_layout()

    plt.show()

def plot_line(filename_x, filename_y):
    x = np.loadtxt(filename_x)
    y = np.loadtxt(filename_y)
    
    plt.plot(x, y, marker='.')

    plt.xlabel("X")
    plt.ylabel("Y")

    plt.grid(True)
    plt.show()

def plot_dft_fft(signal_file, dft_mags_file, dft_phases_file, fft_mags_file, fft_phases_file, bins_file):
    signal = np.loadtxt(signal_file)
    dft_mags = np.loadtxt(dft_mags_file)
    dft_phases = np.loadtxt(dft_phases_file)
    fft_mags = np.loadtxt(fft_mags_file)
    fft_phases = np.loadtxt(fft_phases_file)
    bins = np.loadtxt(bins_file)

    # only keep the phases where mags are non-zero to avoid plotting noise
    dft_phases = np.where(dft_mags > 1e-6, dft_phases, 0)
    fft_phases = np.where(fft_mags > 1e-6, fft_phases, 0)


    # subplot fo 6 plots in two columsn and three rows
    plt.figure(figsize=(12, 8))
    # row 1, col 1 only the signal
    plt.subplot(3, 2, 1)
    plt.plot(signal, marker='.')
    plt.title("Signal")
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
    plt.grid(True)

    # row 2, col 1 DFT Magnitudes x axis is the bins
    plt.subplot(3, 2, 3)
    plt.plot(bins, dft_mags, marker='.')
    plt.title("DFT Magnitudes")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Magnitude")
    plt.grid(True)

    # row 2, col 2 DFT Phases x axis is the bins
    plt.subplot(3, 2, 4)
    plt.plot(bins, dft_phases, marker='.')
    plt.title("DFT Phases")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Phase")
    plt.grid(True)

    # row 3, col 1 FFT Magnitudes x axis is the bins
    plt.subplot(3, 2, 5)
    plt.plot(bins, fft_mags, marker='.')
    plt.title("FFT Magnitudes")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Magnitude")
    plt.grid(True)

    # row 3, col 2 FFT Phases x axis is the bins
    plt.subplot(3, 2, 6)
    plt.plot(bins, fft_phases, marker='.')
    plt.title("FFT Phases")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Phase")
    plt.grid(True)

    plt.show()

def main():
    # Correct the parameters
    folder = r"D:\My_works\DSP C\7_FFT\\"
    plot_dft_fft(
        signal_file=folder + "sig_99_19_fs_1K.dat",
        dft_mags_file=folder + "Output_mags_dft.dat",
        dft_phases_file=folder + "Output_phases_dft.dat",
        fft_mags_file=folder + "Output_mags_fft.dat",
        fft_phases_file=folder + "Output_phases_fft.dat",
        bins_file=folder + "Output_bins.dat"
    )



main()
