#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIG_LENGTH 320

extern double InputSignal_f32_1kHz_15kHz[320] ;

void write_signal_into_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr ;
    file_ptr = fopen(filename, "w") ;
    for(int i = 0 ; i < signal_length ; i++){
        fprintf(file_ptr , "\n%lf" , signal[i]) ;
    }
    fclose(file_ptr) ;
}

void running_sum(double *input_signal, int input_signal_length, double *output_signal){
    for (int i = 0 ; i < input_signal_length ; i ++){
        output_signal[i] = 0.0 ;
        if (i != 0)
            output_signal[i] = output_signal[i-1] + input_signal[i] ;
        else
            output_signal[i] = input_signal[i] ;
    }
}

int main()
{
    write_signal_into_file("input_signal.dat", InputSignal_f32_1kHz_15kHz, SIG_LENGTH) ;

    int output_signal_length = SIG_LENGTH;
    double output_signal[output_signal_length] ;
    running_sum(InputSignal_f32_1kHz_15kHz, SIG_LENGTH, output_signal) ;
    write_signal_into_file("output_signal.dat" , output_signal, output_signal_length) ;

    return 0;
}
