#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define FS 1000
#define N 1024
#define NUM_FREQS 2
#define PI 3.141592653589793


double signal[N + 1]  = {0};
double times[N + 1] = {0} ;
long long int freqs_Hz[NUM_FREQS + 1] = {100, 20} ;
long long int phases_deg[NUM_FREQS + 1] = {45, 120} ;
long long int amps[NUM_FREQS + 1] = {20, 5} ;


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
    write_signal_into_file ("sig_20_5_fs_1K.dat", signal, N) ;
    return 0;
}
