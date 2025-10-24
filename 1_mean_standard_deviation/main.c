#include <stdio.h>
#include <stdlib.h>

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


int main()
{
    printf("The mean of the signal %lf" , cal_signal_mean(InputSignal_f32_1kHz_15kHz , SIG_LENGTH)) ;

    return 0;
}
