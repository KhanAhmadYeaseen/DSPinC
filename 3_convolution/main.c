#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIG_LENGTH 320
#define IMP_RES_LENGTH 29

extern double Impulse_response[29] ;
extern double InputSignal_f32_1kHz_15kHz[320] ;

double get_array_val(double *arr, int array_len, int ind){
    if (ind < 0 || ind >= array_len){
        return 0.0 ;
    }
    else{
        return arr[ind] ;
    }
}

void write_signal_into_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr ;
    file_ptr = fopen(filename, "w") ;
    for(int i = 0 ; i < signal_length ; i++){
        fprintf(file_ptr , "\n%lf" , signal[i]) ;
    }
    fclose(file_ptr) ;
}

void convolution(double *input_signal, int input_signal_length, double *impulse_response, int impulse_response_length, double *output_signal){
    int output_signal_length = input_signal_length + impulse_response_length - 1 ;
    int imp_it = 0 ;

    for (int i = 0 ; i < output_signal_length ; i++){
        output_signal[i] = 0.0 ;
        imp_it = 0 ;
        for(int j = i ; j > i - impulse_response_length ; j--){
            output_signal[i] += (get_array_val(input_signal, input_signal_length, j) * impulse_response[imp_it]) ;
            imp_it ++  ;
        }
    }
}

int main()
{
    write_signal_into_file("input_signal.dat", InputSignal_f32_1kHz_15kHz, SIG_LENGTH) ;
    write_signal_into_file("impulse_response.dat" , Impulse_response, IMP_RES_LENGTH) ;

    int output_signal_length = SIG_LENGTH + IMP_RES_LENGTH - 1  ;
    double output_signal[output_signal_length] ;
    convolution(InputSignal_f32_1kHz_15kHz, SIG_LENGTH, Impulse_response, IMP_RES_LENGTH, output_signal) ;

    write_signal_into_file("output_signal.dat" , output_signal, output_signal_length) ;

    return 0;
}
