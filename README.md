 # DSP C Experiments

This repository is a sequence of small digital signal processing experiments written in C. The programs start with signal statistics and basic discrete-time operations, then move to the frequency domain and finally to FIR and IIR filters. Most programs write numeric samples to `.dat` files; the scripts in `Python_plotter/` read those files and draw time-domain or frequency-domain plots.

The code is intentionally written close to the equations. It is useful for seeing what each DSP operation does internally before using a library implementation.

## Learning path

1. Generate and inspect a sampled signal.
2. Calculate mean, variance, and standard deviation.
3. Apply convolution and a running sum.
4. Detect a known waveform with correlation.
5. Calculate a DFT directly.
6. Calculate the same spectrum with a recursive radix-2 FFT.
7. Design and apply FIR filters.
8. Design and apply an IIR biquad filter.

## Important signal concepts

### Sampling frequency and samples

`FS` is the sampling frequency in samples per second (Hz). A sampled signal is represented by values `x[n]`, where `n` is an integer sample index. The time of sample `n` is:

```text
t[n] = n / FS
```

The highest frequency that can be represented without aliasing is the Nyquist frequency:

```text
f_Nyquist = FS / 2
```

The frequency spacing of an `N`-sample spectrum is:

```text
delta_f = FS / N
```

These relationships explain most of the constants used in the later programs.

## Project folders

### `0_signal_generator`

`main.c` creates a test signal by adding three sinusoids:

```text
x(t) = 20 sin(2 pi 99.609375 t + 45 degrees)
	+ 5 sin(2 pi 19.53125 t + 120 degrees)
	+ 10 sin(2 pi 179.6875 t + 90 degrees)
```

Parameters:

- `FS = 1000 Hz`: gives a 1 ms sample interval and a Nyquist frequency of 500 Hz.
- `N = 1024`: gives enough samples for a clear spectrum and makes the length a power of two for the later FFT.
- Frequencies are chosen close to integer FFT bins because `1000 / 1024 = 0.9765625 Hz`; this reduces spectral leakage.
- Amplitudes `20`, `5`, and `10` make the components easy to distinguish in a magnitude plot.
- Phases are included so the later phase calculation has meaningful values to recover.

The output is `sig_20_100_180_fs_1K.dat`. The filename is an approximate description of the components, not the exact values in the source.

### `1_mean_standard_deviation`

`main.c` calculates the arithmetic mean of a fixed 320-sample test waveform stored in `waveforms.c`.

```text
mean = (1 / N) * sum(x[n])
```

The waveform is documented as containing 1 kHz and 15 kHz components. `SIG_LENGTH = 320` is the size of the supplied array, so the program demonstrates calculating a statistic over a finite block rather than over an endless signal.

The program prints the result to the terminal. It does not create a data file.

### `2_standard_deviation_variance`

This uses the same 320-sample waveform and calculates:

```text
variance = sum((x[n] - mean)^2) / (N - 1)
standard_deviation = sqrt(variance)
```

The divisor `N - 1` makes this the sample variance estimate. Standard deviation is useful because it is in the same amplitude units as the original signal, while variance is in squared amplitude units. The mean is calculated again inside `cal_signal_variance`, keeping the function independent and easy to study.

The mean, variance, and standard deviation are printed to the terminal.

### `3_convolution`

`main.c` implements linear convolution between the 320-sample input and a 29-sample impulse response from `waveforms.c`:

```text
y[n] = sum(x[n - k] h[k])
```

`get_array_val` returns zero outside the input range. This is zero-padding, and it allows the program to calculate the complete transient at the beginning and end of the convolution. Therefore:

```text
output_length = 320 + 29 - 1 = 348 samples
```

The impulse response is symmetric, which is typical of a linear-phase FIR filter. The program writes:

- `input_signal.dat`
- `impulse_response.dat`
- `output_signal.dat`

This experiment is the direct foundation for the FIR programs: an FIR filter is implemented by convolving the input with its finite impulse response.

### `4_running_sum`

`running_sum` calculates the cumulative sum:

```text
y[0] = x[0]
y[n] = y[n - 1] + x[n]
```

This is equivalent to filtering with an impulse response made of ones extending indefinitely. In DSP terms it is an accumulator and a simple discrete-time integrator. It emphasizes that feedback or state can make the output depend on all previous samples.

`SIG_LENGTH = 320`, so the output has the same length as the input. The program writes `input_signal.dat` and `output_signal.dat`.

### `5_correlation`

This experiment searches for the seven-sample pattern in `signal_2` inside the 21-sample `signal_1`.

For each lag it calculates normalized correlation:

```text
r[lag] = sum(signal_1[n] * signal_2[m])
	    / (magnitude_of_signal_1_window * magnitude_of_signal_2)
```

Normalization makes the result a similarity measure rather than a value dominated by signal amplitude. The lag range is:

```text
-(7 - 1) through (21 - 1) = -6 through 20
```

That produces `21 + 7 - 1 = 27` correlation samples. Values outside `signal_1` are treated as zero. A large positive peak indicates that the template and the selected window have a similar shape and alignment.

The program writes `signal_1.dat`, `signal_2.dat`, and `correlated.dat`.

## DFT and FFT

### What the DFT does

The discrete Fourier transform represents a finite sequence as a sum of sampled cosine and sine waves. For an `N`-sample signal, the complex DFT is commonly written as:

```text
X[k] = sum from n=0 to N-1 of x[n] * exp(-j * 2*pi*k*n/N)
```

Each index `k` is a frequency bin. Its frequency is:

```text
f[k] = k * FS / N
```

The magnitude tells how much of that frequency is present. The phase tells the phase offset of that component. For a real-valued signal, the positive-frequency half from `k = 0` through `k = N/2` contains the unique information, so these programs store `N/2 + 1` values.

### `6_fourier_transform`

This is the direct DFT implementation. It uses:

- `SIG_LENGTH = 320`: matches the supplied 320-sample two-tone waveform.
- `samplig_freq_Hz = 48000`: converts bin indices into frequency values for the intended 48 kHz sampling rate.
- `DFT_LENGTH = 161`: stores bins from DC through Nyquist.

For every output bin `k`, the code loops over every input sample `i` and accumulates cosine and sine coefficients. This is the straightforward `O(N^2)` method and is useful for understanding the transform, although it becomes slow for large `N`.

The coefficient scale is `1/N` for DC and Nyquist and `2/N` for the other positive-frequency bins. The factor of two accounts for the matching negative-frequency component when plotting a one-sided spectrum.

The program writes `Input_signal.dat`, `Output_cosine.dat`, `Output_sine.dat`, `Output_mags.dat`, `Output_phases.dat`, and `Output_bins.dat`.

### `6_fourier_transform_2`

This is a file-based version of the direct DFT. It reads `sig_20_5_fs_1K.dat`, which should contain 1000 samples at `FS = 1000 Hz`, and produces `N/2 + 1 = 501` positive-frequency bins.

The larger `SIG_LENGTH = 1000` gives a frequency spacing of:

```text
delta_f = 1000 / 1000 = 1 Hz
```

That makes frequencies that are integer numbers of hertz easy to identify. The source still performs the same `O(N^2)` cosine and sine accumulation as the previous experiment.

The output files are `Output_cosine.dat`, `Output_sine.dat`, `Output_mags.dat`, `Output_phases.dat`, and `Output_bins.dat`.

### `7_FFT`

This folder compares the direct DFT with a hand-written recursive radix-2 FFT. It reads `sig_99_19_fs_1K.dat` using:

- `SIG_LENGTH = 1024`: a power of two, which is required by this radix-2 implementation.
- `FS = 1000 Hz`: gives `delta_f = 1000 / 1024 = 0.9765625 Hz`.
- `DFT_LENGTH = 513`: the one-sided output including DC and Nyquist.

The FFT works by splitting the signal into even and odd samples, recursively transforming both halves, and combining them with the twiddle factor:

```text
W_N^k = exp(-j * 2*pi*k/N)
```

This changes the computational cost from approximately `O(N^2)` for the direct DFT to `O(N log2 N)` for the FFT. The code uses a small `complex` structure and explicitly implements complex addition, subtraction, multiplication, and the butterfly combination.

The program calculates both DFT and FFT magnitude/phase results so they can be compared. Their magnitudes should agree apart from floating-point error. It writes `Output_mags_dft.dat`, `Output_phases_dft.dat`, `Output_mags_fft.dat`, `Output_phases_fft.dat`, and `Output_bins.dat`.

## FIR filters

FIR means **finite impulse response**. An FIR filter has a finite coefficient array `h[k]` and computes:

```text
y[n] = sum from k=0 to TAPS-1 of h[k] * x[n-k]
```

The filter has no feedback from previous output samples, so it is straightforward to make stable and, with symmetric coefficients, linear phase. The cost is that a sharp transition generally requires more taps and therefore more computation and delay.

### FIR design used here

The programs use the windowed-sinc method. An ideal low-pass filter has a sinc impulse response, but truncating the infinite sinc causes strong ripples. Multiplying it by a Blackman window reduces the sidelobes and makes the finite filter more practical:

```text
h[n] = sinc[n] * Blackman[n]
```

The shared parameters are:

- `FS = 1000 Hz`: all filter cutoff frequencies are below the 500 Hz Nyquist limit.
- `SIG_LENGTH = 1024`: keeps the time-domain and FFT comparisons consistent and provides 0.9765625 Hz spectral spacing.
- `TAPS = 51`: an odd length gives a center tap at index `M = 25`, symmetric coefficients, and a group delay of `(51 - 1) / 2 = 25` samples.
- Blackman coefficients `0.42`, `0.5`, and `0.08`: the standard three-term Blackman window used to reduce truncation sidelobes.
- DC normalization: the low-pass coefficients are divided by their sum so a constant input has approximately unity gain.

Each program first computes the input spectrum, designs the filter, convolves the input with the filter, shifts the result by 25 samples to compensate for the linear-phase delay, and computes the output spectrum.

### `8_FIR/1_convolution.c`

This file contains two general convolution implementations, including the version used by the other FIR examples. Its `main` function currently returns immediately, so it is a reference/helper file rather than a standalone experiment that produces output.

### `8_FIR/2_windowed_sinc_lowpass.c`

Designs a Blackman-windowed low-pass FIR filter with:

```text
cutoff_freq = 120 Hz
```

It should preserve the lower-frequency components and attenuate components above the cutoff. Output files include `filter_lowpass.dat`, `output_lowpass.dat`, `mags_af_lowpass.dat`, and `phs_af_lowpass.dat`, along with the shared input and before-filter spectrum files.

### `8_FIR/3_windowed_sinc_highpass.c`

Uses a 80 Hz cutoff. The high-pass response is formed by spectral inversion of the low-pass coefficients:

```text
h_hp[M] = 1 - h_lp[M]
h_hp[n] = -h_lp[n] for n != M
```

This passes frequencies above approximately 80 Hz and suppresses lower frequencies. The output names use the `highpass` suffix.

### `8_FIR/4_windowed_sinc_bandpass.c`

Uses lower and upper cutoffs of 50 Hz and 140 Hz. It subtracts two low-pass responses:

```text
h_bandpass = h_lowpass(140 Hz) - h_lowpass(50 Hz)
```

The result passes the band between the two cutoffs and attenuates frequencies outside it.

### `8_FIR/5_windowed_sinc_bandstop.c`

Uses the same 50 Hz and 140 Hz boundaries, but adds a low-pass and a high-pass response:

```text
h_bandstop = h_lowpass(50 Hz) + h_highpass(140 Hz)
```

This keeps frequencies below 50 Hz and above 140 Hz while suppressing the middle band.

## IIR filter

IIR means **infinite impulse response**. Unlike an FIR filter, an IIR filter feeds previous output samples back into the current calculation:

```text
y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
	  - a1*y[n-1] - a2*y[n-2]
```

Because the output history is reused, the impulse response can continue indefinitely. IIR filters can achieve a sharp response with fewer coefficients than an FIR filter, but their stability depends on the feedback coefficients and they generally do not have linear phase.

### `9_IIR/1_IIR_lowpass.c`

This program implements one second-order low-pass biquad. The coefficients are calculated from the standard cookbook equations:

- `FS = 1000 Hz`: the digital frequency reference.
- `cutoff_freq = 120 Hz`: frequencies below this are intended to pass.
- `Q = 0.707`: approximately `1/sqrt(2)`, the common Butterworth-like choice for a smooth, near-maximally-flat response without a resonant peak.
- `x1`, `x2`, `y1`, and `y2`: the two previous input and output samples needed for the recurrence.

The filter is initialized with zero state, processes one sample at a time, and writes `output_lowpass.dat`. It also calculates before/after FFT magnitudes and phases in `mags_bef.dat`, `phs_bef.dat`, `mags_af_lowpass_IIR.dat`, and `phs_af_lowpass_IIR.dat`.

Compared with the 51-tap FIR low-pass, this IIR uses only two delays in each direction and therefore needs less computation and memory. The tradeoff is nonlinear phase and feedback-related stability considerations.

## Python plotting scripts

The plotting scripts use NumPy and Matplotlib:

- `Plotter.py`: simple one-file and x/y plotting helpers for generated data.
- `FFT_plotter.py`: displays the input signal and DFT/FFT magnitude and phase plots.
- `FIR_plotter.py`: displays the input, filtered output, filter coefficients, and before/after spectra for an FIR experiment.
- `IIR_plotter.py`: displays the input, filtered output, and before/after spectra for the IIR experiment.

The scripts currently contain absolute Windows paths and select one experiment in each `main()` function. If the files are moved, update the `folder` or filename strings. The C programs also use relative filenames, so run an executable with its experiment directory as the working directory if you want the `.dat` files to appear beside that executable.

## Running the experiments

Build an individual C file with GCC and link the math library when required. For example, from the relevant folder:

```text
gcc -Wall -Wextra -O2 main.c -lm -o main.exe
```

For programs that use a separate waveform source, compile both source files:

```text
gcc -Wall -Wextra -O2 main.c waveforms.c -lm -o main.exe
```

Then run the executable from the directory whose input files it expects. For example, the FIR and IIR programs read `sig_20_100_180_fs_1K.dat`, while the FFT program reads `sig_99_19_fs_1K.dat`. Those input files must already exist in the current working directory; they can be generated or copied from the signal-generator experiment after choosing the filename expected by the reader.

Activate the Python environment in `Python_plotter/` and run the matching plotting script after the C program has produced its `.dat` files.

## Output-file convention

The spectrum files use these names consistently:

- `bins.dat`: frequency in Hz for each one-sided FFT bin.
- `mags_*.dat`: magnitude for each bin.
- `phs_*.dat`: phase in degrees for each bin.
- `output_*.dat`: filtered or otherwise processed time-domain signal.
- `filter_*.dat`: FIR coefficients.

The phase plotting scripts replace phase values at very small magnitudes with zero. This avoids displaying meaningless phase noise where the signal has effectively no energy.

## Notes for future changes

- Keep the sample rate, signal length, and filenames synchronized between the signal generator, C readers, and Python plotters.
- A radix-2 FFT requires a power-of-two signal length; `1024` is used for that reason.
- Increasing `N` improves frequency resolution (`FS/N`) but increases direct-DFT work quadratically and requires more input samples.
- Increasing FIR `TAPS` can sharpen the transition band, but it also increases computation and group delay.
- Changing an IIR cutoff or `Q` changes the biquad poles and therefore the frequency response and stability margin; recalculate coefficients instead of editing them by hand.
