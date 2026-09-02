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


def main():
    plot_one(r"D:\My_works\DSP C\7_fourier_transform_2\sig_20_5_fs_1K.dat")
    plot_line(r"D:\My_works\DSP C\7_fourier_transform_2\Output_bins.dat", r"D:\My_works\DSP C\7_fourier_transform_2\Output_mags.dat" )
    plot_line(r"D:\My_works\DSP C\7_fourier_transform_2\Output_bins.dat", r"D:\My_works\DSP C\7_fourier_transform_2\Output_phases.dat" )

main()
