

#ifndef __WAVEPROCESS_H
#define __WAVEPROCESS_H

#include "ap.h"

class WaveProcess
{
    public:
        WaveProcess();
        ~WaveProcess();
        void tsld1(double *a1, double *a2,double *b, double *x, int m);
        void fft(const ap::real_1d_array& a, int n, ap::complex_1d_array& f);
        void fft(double *x, double *Re, double *Im, int n);
        void ifft(ap::complex_1d_array& a, int n);
        void ifft(double *Re, double *Im, double *t, int n);
        void convolve(double *x, int m, double *y, int n, double *xy);
        void correlate(double *x, int m, double *y, int n, double *xy);

};



#endif
