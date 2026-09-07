#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FS 1000
#define SIG_LENGTH 1024
#define TAPS 51
#define BINS_LENGTH (SIG_LENGTH / 2 + 1)

#define PI 3.14159265358979323846

double low_cutoff_freq = 50.0 ; 
double high_cutoff_freq = 140.0 ; 

void design_lowpass_blackman(double *h, long long int taps, double fc, double fs){
    double wc = 2.0 * PI * fc / fs ; 
    long long int M = (taps - 1) / 2 ; 

    for (long long int n = 0 ; n < taps ; n++){
        long long int k  = n - M ; 
        double sinc ; 
        //Calculating the brick wall filter / sinc 
        if (k == 0 )
            sinc = wc / PI ; 
        else
            sinc = sin(wc * k) / (PI * k) ; 

        double window = 0.42 - 0.5  * cos(2.0 * PI * n / (taps - 1)) + 0.08 * cos(4.0 * PI * n / (taps - 1)); // Window
        h[n] = sinc * window ; //Window * sinc 
    }

    // Normalize DC gain to 1
    double sum = 0.0 ;
    for (long long int i = 0 ; i < taps ;i++)
        sum += h[i] ; 

    for (long long int i = 0 ; i < taps ; i++)
        h[i] /= sum  ;
}

void design_bandpass_blackman(double *h, long long int taps, double low_cutoff_freq, double high_cutoff_freq, double fs){
    double *h_high_cut ; 
    h_high_cut = (double *)malloc(taps *sizeof(double)) ; 
    design_lowpass_blackman(h_high_cut, taps, high_cutoff_freq, fs) ; 

    double *h_low_cut ; 
    h_low_cut = (double *)malloc(taps *sizeof(double)) ; 
    design_lowpass_blackman(h_low_cut, taps, low_cutoff_freq, fs) ; 

    for(long long int i = 0 ;  i < taps; i ++){
        h[i] = h_high_cut[i] - h_low_cut[i] ; 
    }
    free(h_high_cut) ; 
    free(h_low_cut) ;
}

struct complex {
    double real;
    double imag;
};

struct complex  add_complex(struct complex a, struct complex b){
    struct complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

struct complex  sub_complex(struct complex a, struct complex b){
    struct complex result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

struct complex multiply_complex(struct complex a, struct complex b){
    struct complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

struct complex compute_W(long long int k, long long int N){
    struct complex W;
    double angle = 2.0 * PI * k / N;
    W.real = cos(angle);
    W.imag = -sin(angle);
    return W;
}

struct complex* cal_fft(double *signal, int sig_length){
    if (sig_length <= 0 || (sig_length & (sig_length - 1)) != 0){
        printf("The length is not a power of 2\n");
        exit(1);
    }

    struct complex *freq_components = malloc(sig_length * sizeof(struct complex)) ; 

    if (sig_length == 1) {
        freq_components[0].real = signal[0] ; 
        freq_components[0].imag = 0 ;
        return freq_components;
    }

    double even_signal[sig_length / 2 + 1] ;
    double odd_signal[sig_length / 2 + 1] ;

    for (long long int i = 0 ; i < sig_length / 2 ; i++){
        even_signal[i] = signal[2 * i] ; 
        odd_signal[i] = signal[2 * i + 1] ; 
    }

    struct complex *even_freq_components ; 
    struct complex *odd_freq_components ; 

    even_freq_components = cal_fft(even_signal, sig_length / 2) ; 
    odd_freq_components = cal_fft(odd_signal, sig_length / 2) ;

    for( long long int k = 0 ; k < sig_length / 2 ; k++){
        struct  complex W = compute_W(k, sig_length) ; 
        struct complex T = multiply_complex(W, odd_freq_components[k]) ;

        freq_components[k] = add_complex(even_freq_components[k], T) ; 
        freq_components[k + sig_length / 2] = sub_complex(even_freq_components[k], T) ; 
    }

    free(even_freq_components) ; 
    free(odd_freq_components) ;

    return freq_components ; 
}

void write_signal_into_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr ;
    file_ptr = fopen(filename, "w") ;
    
    if (file_ptr == NULL) {
        printf("Error: Could not open file for writing.\n");
        return;
    }

    for(int i = 0 ; i < signal_length ; i++){
        fprintf(file_ptr , "\n%lf" , signal[i]) ;
    }
    fclose(file_ptr) ;
}

void read_signal_from_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr;
    file_ptr = fopen(filename, "r");
    if (file_ptr == NULL){
        printf("Error: Could not open file.\n");
        return;
    }
    for (int i = 0; i < signal_length; i++){
        fscanf(file_ptr, "%lf", &signal[i]);
    }
    fclose(file_ptr);
}

void calculate_bins(double *bins, long long int sam_freq_Hz, long long int sig_length){
    for (long long int k = 0 ; k <= sig_length / 2 ; k++){
        bins[k] =  k * ((double) sam_freq_Hz / (double) sig_length );
    }
}

void calculate_fft_mags_phases_deg(struct complex* freq_components, double *mags, double *phases_deg, long long int sig_length){
    for(long long int k = 0 ; k <= sig_length/ 2 ; k++){
        double magnitude = sqrt(freq_components[k].real * freq_components[k].real + freq_components[k].imag * freq_components[k].imag) ;
        if (k == 0 || k == sig_length / 2)
            mags[k] = magnitude / sig_length;
        else
            mags[k] = 2.0 * magnitude / sig_length;
        phases_deg[k] = 90 + (atan2(freq_components[k].imag, freq_components[k].real) * 180.0 / PI) ; 
    }
}

void convolution_(double *input_signal, long long int input_signal_length, double *impulse_response, long long int impulse_response_length, double *output_signal){
    long long int output_signal_length = input_signal_length + impulse_response_length - 1 ; 
    for (long long int i = 0 ;  i < output_signal_length ; i++){
        output_signal[i] = 0  ; 
    }

    for (long long int k = 0 ; k < input_signal_length ; k++){
        for (long long int j = 0 ; j < impulse_response_length ; j ++){
            output_signal[k + j] += input_signal[k] * impulse_response[j] ; 
        }
    }
}

//Correct group delay of (taps - 1) / 2 samples
void correct_group_delay(double *signal, long long int sig_length, long long int taps){
    long long int group_del = (taps - 1) / 2 ; 
    for (long long int i = 0 ; i < sig_length ; i ++){
        signal[i] = signal[i + group_del] ;
    }
}

int main(){
    double *signal ; 
    signal = (double *)malloc(SIG_LENGTH * sizeof(double)) ; 
    read_signal_from_file("sig_20_100_180_fs_1K.dat", signal, SIG_LENGTH) ;

    double *bins ;
    bins = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    calculate_bins(bins, FS, SIG_LENGTH) ; 
    write_signal_into_file("bins.dat", bins, BINS_LENGTH) ;
    
    struct complex *freqs_bef ; 
    freqs_bef = (struct complex*) malloc (BINS_LENGTH * sizeof(struct  complex)) ; 
    double *mags_bef ;
    double *phs_bef ;
    mags_bef = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    phs_bef = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    freqs_bef = cal_fft(signal, SIG_LENGTH) ; 
    calculate_fft_mags_phases_deg(freqs_bef, mags_bef, phs_bef, SIG_LENGTH) ; 
    write_signal_into_file("mags_bef.dat", mags_bef, BINS_LENGTH) ;
    write_signal_into_file("phs_bef.dat", phs_bef, BINS_LENGTH) ; 

    double *h;
    h = (double *)malloc(TAPS * sizeof(double)) ; 
    design_bandpass_blackman(h, TAPS, low_cutoff_freq, high_cutoff_freq, FS) ;
    write_signal_into_file("filter_bandpass.dat", h, TAPS) ;

    double *output ; 
    output = (double *)malloc((SIG_LENGTH + TAPS - 1) * sizeof(double)) ; 

    convolution_(signal, SIG_LENGTH, h, TAPS, output) ; 
    correct_group_delay(output, SIG_LENGTH, TAPS) ; 
    write_signal_into_file("output_bandpass.dat", output, SIG_LENGTH) ;

    struct complex *freqs_af ; 
    freqs_af = (struct complex*) malloc (BINS_LENGTH * sizeof(struct  complex)) ; 
    double *mags_af ;
    double *phs_af ;
    mags_af = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    phs_af = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    freqs_af = cal_fft(output, SIG_LENGTH) ; 
    calculate_fft_mags_phases_deg(freqs_af, mags_af, phs_af, SIG_LENGTH) ; 
    write_signal_into_file("mags_af_bandpass.dat", mags_af, BINS_LENGTH) ;
    write_signal_into_file("phs_af_bandpass.dat", phs_af, BINS_LENGTH) ; 

    return 0 ; 
}