#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIG_LENGTH 320
#define IMP_RES_LENGTH 29

double get_array_val(double *arr, int array_len, int ind){
    if (ind < 0 || ind >= array_len){
        return 0.0 ;
    }
    else{
        return arr[ind] ;
    }
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

int main()
{
    return 0;
}
