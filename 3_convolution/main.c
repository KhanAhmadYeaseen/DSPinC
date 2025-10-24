#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIG_LENGTH 320
#define IMP_RES_LENGTH 29

extern double Impulse_response[29] ;
extern double InputSignal_f32_1kHz_15kHz[320] ;

void write_signal_into_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr ;
    file_ptr = fopen(filename, "w") ;
    for(int i = 0 ; i < signal_length ; i++){
        fprintf(file_ptr , "\n%lf" , signal[i]) ;
    }
    fclose(file_ptr) ;
}

int main()
{
    write_signal_into_file("input_signal.dat", InputSignal_f32_1kHz_15kHz, SIG_LENGTH) ;
    write_signal_into_file("impulse_response.dat" , Impulse_response, IMP_RES_LENGTH) ;

    return 0;
}
