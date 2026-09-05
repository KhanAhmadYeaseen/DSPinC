#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIG_LENGTH 1024
#define DFT_LENGTH (SIG_LENGTH / 2 + 1)
#define samplig_freq_Hz 1000
#define PI 3.14159265358979323846

double signal[SIG_LENGTH + 1] = {0} ;

double sig_cosine[DFT_LENGTH] ;
double sig_sine[DFT_LENGTH] ;
double mags [DFT_LENGTH] ;
double phases [DFT_LENGTH] ;
double freq_bins [DFT_LENGTH] ;

void cal_dft(double *signal, double *sig_cosine, double *sig_sine, double *mags, double *phases, int sig_length){
    for (int j = 0 ; j <= sig_length / 2 ; j++){
        sig_cosine[j] = 0 ;
        sig_sine[j] = 0 ;
    }

    for (int k = 0 ; k <= sig_length / 2 ; k++){
        double scale ;
        if (k == 0 || k == sig_length / 2)
            scale = 1.0 / sig_length;
        else
            scale = 2.0 / sig_length;

        for (int i = 0; i < sig_length ; i++){
            sig_cosine[k] +=  scale * signal[i] * cos (2 * PI * k * i / sig_length) ;
            sig_sine[k]   +=  scale * signal[i] * sin (2 * PI * k * i / sig_length) ;
        }
    }

    for (int k = 0 ; k <= sig_length / 2; k++){
        mags[k] =  sqrt( sig_sine[k] * sig_sine [k] + sig_cosine[k] * sig_cosine[k]) ;
        phases[k] = atan2(sig_cosine[k], sig_sine[k]) * 180.0 / PI;
    }
}

struct complex {
    double real;
    double imag;
};

//struct complex freq_components[SIG_LENGTH + 1] = {0} ; 

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

int main()
{
    read_signal_from_file("sig_99_19_fs_1K.dat", signal, SIG_LENGTH) ;
    cal_dft(signal, sig_cosine, sig_sine, mags, phases, SIG_LENGTH) ;

    write_signal_into_file("Output_mags_dft.dat", mags, DFT_LENGTH) ;
    write_signal_into_file("Output_phases_dft.dat", phases, DFT_LENGTH) ;

    calculate_bins(freq_bins, samplig_freq_Hz, SIG_LENGTH) ; 
    write_signal_into_file("Output_bins.dat", freq_bins, DFT_LENGTH) ;

    double mags_fft[DFT_LENGTH + 1]  ;
    double phases_fft[DFT_LENGTH + 1] ; 
    struct complex *fft_components = cal_fft(signal, SIG_LENGTH) ;
    calculate_fft_mags_phases_deg(fft_components, mags_fft, phases_fft, SIG_LENGTH) ;
    
    write_signal_into_file("Output_mags_fft.dat", mags_fft, DFT_LENGTH) ; 
    write_signal_into_file("Output_phases_fft.dat", phases_fft, DFT_LENGTH) ; 

    return 0;
}
