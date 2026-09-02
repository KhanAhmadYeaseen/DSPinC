#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define SAMPLING_RATE_HZ 100
#define DURATION_S 2
#define SAMPLES_NUM (DURATION_S * SAMPLING_RATE_HZ)

void write_signal_into_file(char *filename, double *signal, int signal_length){
    FILE *file_ptr ;
    file_ptr = fopen(filename, "w") ;
    for(int i = 0 ; i < signal_length ; i++){
        fprintf(file_ptr , "\n%lf" , signal[i]) ;
    }
    fclose(file_ptr) ;
}

double signal_1[21] = {10, 20, 30, 40, 30, 20, 10, 10 , 10, 10, 20, 30, 40, 40, 40, 40, 0, 1, 2, 3, 4 } ;
double signal_2[7] = {10, 20, 30, 40, 30, 20, 10} ;
double correlated_signal[27]  ;

double get_val(double* signal,int len, int index){
    if (index < 0)
        return 0.0 ;
    else if (index >= len)
        return 0.0 ;
    else
        return signal[index] ;

}

void normalized_correlate_fun(double* signal_1, int signal_1_len, double* signal_2, int signal_2_len, double* output_signal){
    double template_mag = 0.0;
    for (int m = 0 ; m < signal_2_len ; m++)
        template_mag +=  (signal_2[m] * signal_2[m]) ;
    template_mag = sqrt(template_mag) ;

    for(int lag = -(signal_2_len - 1); lag <= (signal_1_len - 1) ; lag ++){
        double sum = 0 ;
        double sig_mag = 0.0 ;
        int m = 0;
        for (int n = lag; n <= (lag + signal_2_len -1) ; n++, m++){
            double s = get_val(signal_1, signal_1_len, n);
            double t = get_val(signal_2, signal_2_len, m);
            sig_mag += s * s;
            sum += s * t;
        }
        sig_mag = sqrt(sig_mag) ;

        output_signal[lag + signal_2_len -1] = (sum / (template_mag * sig_mag));
    }
}

int main()
{
    write_signal_into_file("signal_1.dat", signal_1, 21 ) ;
    write_signal_into_file("signal_2.dat", signal_2, 7 ) ;
    normalized_correlate_fun(signal_1, 21, signal_2, 7, correlated_signal) ;
    write_signal_into_file("correlated.dat", correlated_signal, 27 ) ;

    return 0;
}
