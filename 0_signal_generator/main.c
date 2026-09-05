#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define FS 1000
#define N 1024
#define NUM_FREQS 2
#define PI 3.141592653589793


double signal[N + 1]  = {0};
double times[N + 1] = {0} ;
double freqs_Hz[NUM_FREQS + 1] = {99.609375, 19.53125} ;
double phases_deg[NUM_FREQS + 1] = {45.0, 120.0} ;
double amps[NUM_FREQS + 1] = {20.0, 5.0} ;


void generate_signal() {
    for (long long int i = 0 ; i < N ; i++){
        double time = (double)i / FS ;

        for (long long int k = 0 ; k < NUM_FREQS ; k++){
            signal[i] += amps[k] * sin(2 * PI * freqs_Hz[k] * time + phases_deg[k] * PI / 180) ;
        }
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

int main()
{
    generate_signal()  ;
    write_signal_into_file ("sig_99_19_fs_1K.dat", signal, N) ;
    return 0;
}
