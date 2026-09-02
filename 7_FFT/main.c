#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
Aperiodic-con --> Fourier transdorm
Periodic-con ---> Fourier series
Aperiodic-dis --> DTFT
Periodic-dis  --> DFT

DFT engine :

x[N] ---> DFT ---> cosine coefficient C[N/2+1]
             |----> sine coefficient S[N/2+1]

*/

#define SIG_LENGTH 1000
#define DFT_LENGTH (SIG_LENGTH / 2 + 1)
#define samplig_freq_Hz 1000
#define PI 3.14159265358979323846

double signal[SIG_LENGTH + 1] = {0} ;

double sig_cosine[DFT_LENGTH] ;
double sig_sine[DFT_LENGTH] ;
double mags [DFT_LENGTH] ;
double phases [DFT_LENGTH] ;
double freq_bins [DFT_LENGTH] ;

void cal_dft(double *signal, double *sig_cosine, double *sig_sine, double *mags, double *phases, double *bins, int sig_length, long long int sam_freq_Hz){
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
        phases[k] = atan2(sig_cosine[k], sig_sine[k]) * 180.0 / PI;;
        bins[k] = k * ((double) sam_freq_Hz / (double) sig_length );
    }
}

void cal_fft(){

}

void write_signal_into_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr ;
    file_ptr = fopen(filename, "w") ;
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

int main()
{
    read_signal_from_file("sig_20_5_fs_1K.dat", signal, SIG_LENGTH) ;
    cal_dft(signal, sig_cosine, sig_sine, mags, phases, freq_bins, SIG_LENGTH, samplig_freq_Hz) ;

    write_signal_into_file("Output_cosine.dat", sig_cosine, DFT_LENGTH) ;
    write_signal_into_file("Output_sine.dat", sig_sine, DFT_LENGTH) ;
    write_signal_into_file("Output_mags.dat", mags, DFT_LENGTH) ;
    write_signal_into_file("Output_phases.dat", phases, DFT_LENGTH) ;
    write_signal_into_file("Output_bins.dat", freq_bins, DFT_LENGTH) ;

    return 0;
}
