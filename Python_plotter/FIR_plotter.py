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

def plot_fir(signal_file, output_file, filter_file, before_mags_file, before_phases_file, after_mags_file, after_phases_file, bins_file):
    signal = np.loadtxt(signal_file)
    output = np.loadtxt(output_file)
    filter = np.loadtxt(filter_file)
    before_mags = np.loadtxt(before_mags_file)
    before_phases = np.loadtxt(before_phases_file)
    after_mags = np.loadtxt(after_mags_file)
    after_phases = np.loadtxt(after_phases_file)
    bins = np.loadtxt(bins_file)

    # only keep the phases where mags are non-zero to avoid plotting noise
    before_phases = np.where(before_mags > 1e-6, before_phases, 0)
    after_phases = np.where(after_mags > 1, after_phases, 0)

    plt.figure(figsize=(12, 8))
    plt.subplot(4, 2, 1)
    plt.plot(signal, marker='.')
    plt.title("Signal")
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
    plt.grid(True)

    plt.subplot(4, 2, 2)
    plt.plot(output, marker='.')
    plt.title("Output")
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
    plt.grid(True)

    plt.subplot(4, 2, 3)
    plt.plot(filter, marker='.')
    plt.title("Filter")
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
    plt.grid(True)

    plt.subplot(4, 2, 5)
    plt.plot(bins, before_mags, marker='.')
    plt.title("Before filter mags")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Magnitude")
    plt.grid(True)

    plt.subplot(4, 2, 6)
    plt.plot(bins, before_phases, marker='.')
    plt.title("before filter phases")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Phase")
    plt.grid(True)

    plt.subplot(4, 2, 7)
    plt.plot(bins, after_mags, marker='.')
    plt.title("After filter mags")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Magnitude")
    plt.grid(True)

    plt.subplot(4, 2, 8)
    plt.plot(bins, after_phases, marker='.')
    plt.title("After filter phases")
    plt.xlabel("Frequency Bins")
    plt.ylabel("Phase")
    plt.grid(True)

    plt.show()

def main():
    # Correct the parameters
    folder = r"D:\My_works\DSP C\8_FIR\\"
    plot_fir(
        signal_file=folder + "sig_20_100_180_fs_1K.dat",
        output_file=folder + "output_bandstop.dat",
        filter_file=folder + "filter_bandstop.dat",
        before_mags_file = folder + "mags_bef.dat",
        before_phases_file = folder + "phs_bef.dat",
        after_mags_file = folder + "mags_af_bandstop.dat",
        after_phases_file = folder + "phs_af_bandstop.dat",
        bins_file = folder + "bins.dat"
    )


main()
