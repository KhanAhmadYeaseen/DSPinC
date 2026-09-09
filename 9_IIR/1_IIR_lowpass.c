#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FS 1000
#define SIG_LENGTH 1024
#define BINS_LENGTH (SIG_LENGTH / 2 + 1)

#define PI 3.14159265358979323846

double cutoff_freq = 120.0 ; 

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

struct biquad{
    double b0, b1, b2 ; 
    double a1, a2 ;
    double x1, x2 ;
    double y1, y2 ;
} ;

void  biquad_init(struct biquad *lp, double fs, double cutoff_freq, double Q){
    double w0 = 2.0 * PI * cutoff_freq / fs ; 
    double cosw0 = cos(w0);
    double sinw0 = sin(w0);
    double alpha = sinw0 / (2.0 * Q);

    double b0 = (1.0 - cosw0) * 0.5;
    double b1 = 1.0 - cosw0;
    double b2 = (1.0 - cosw0) * 0.5;

    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cosw0;
    double a2 = 1.0 - alpha;

    lp->b0 = b0 / a0;
    lp->b1 = b1 / a0;
    lp->b2 = b2 / a0;
    lp->a1 = a1 / a0;
    lp->a2 = a2 / a0;

    lp->x1 = 0.0 ;
    lp->x2 = 0.0 ;
    lp->y1 = 0.0 ;
    lp->y2 = 0.0 ;
}

double apply_biquad_per_sample(struct biquad *lp, double x0){
    double y0 = lp->b0*x0 
              + lp->b1 * lp->x1 
              + lp->b2 * lp->x2
              - lp->a1 * lp->y1
              - lp->a2 * lp->y2 ; 
    
    lp->x2 = lp->x1 ;
    lp->x1 = x0 ;

    lp->y2 = lp->y1 ;
    lp->y1 = y0;

    return y0 ;
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

    struct biquad lp ;
    biquad_init(&lp, FS, cutoff_freq, 0.707);
    
    double *output = (double *)malloc(SIG_LENGTH * sizeof(double)) ; 
    for (long long  i = 0; i < SIG_LENGTH; i++){
        output[i] = apply_biquad_per_sample(&lp, signal[i]) ; 
    }
    
    write_signal_into_file("output_lowpass.dat", output, SIG_LENGTH) ;

    struct complex *freqs_af ; 
    freqs_af = (struct complex*) malloc (BINS_LENGTH * sizeof(struct  complex)) ; 
    double *mags_af ;
    double *phs_af ;
    mags_af = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    phs_af = (double *)malloc(BINS_LENGTH * sizeof(double)) ;
    freqs_af = cal_fft(output, SIG_LENGTH) ; 
    calculate_fft_mags_phases_deg(freqs_af, mags_af, phs_af, SIG_LENGTH) ; 
    write_signal_into_file("mags_af_lowpass_IIR.dat", mags_af, BINS_LENGTH) ;
    write_signal_into_file("phs_af_lowpass_IIR.dat", phs_af, BINS_LENGTH) ; 

    return 0 ; 
}