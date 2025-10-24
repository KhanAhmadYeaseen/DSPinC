#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIG_LENGTH 320

extern double InputSignal_f32_1kHz_15kHz[SIG_LENGTH] ;

double cal_signal_mean(double *signal, int sig_length){
    double mean = 0.0 ;
    for(int i = 0 ; i < sig_length ; i ++){
        mean = mean + signal[i] ;
    }
    mean = mean / (double) sig_length ;
    return mean ;
}

double cal_signal_variance(double *signal, int sig_length){
    double mean = cal_signal_mean(signal, sig_length) ;
    double variance = 0.0 ;
    for (int i = 0 ; i < sig_length ; i ++){
        variance = variance + pow((signal[i] - mean), 2) ;
    }
    variance = variance / ((double) sig_length - 1.0) ;
    return variance ;
}

double cal_signal_SD(double *signal, int sig_length){
    double SD = sqrt(cal_signal_variance(signal, sig_length)) ;
    return SD ;
}


int main()
{
    printf("The mean of the signal \t%lf\n" , cal_signal_mean(InputSignal_f32_1kHz_15kHz , SIG_LENGTH)) ;
    printf("The variance of the signal \t%lf\n" , cal_signal_variance(InputSignal_f32_1kHz_15kHz, SIG_LENGTH)) ;
    printf("The SD of the signal \t%lf\n" , cal_signal_SD(InputSignal_f32_1kHz_15kHz, SIG_LENGTH)) ;
    return 0;
}
