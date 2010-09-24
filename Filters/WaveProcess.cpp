#ifndef WAVEPROCESS_CPP
#define WAVEPROCESS_CPP

#endif // WAVEPROCESS_CPP

#include "WaveProcess.h"
#include "AutoCov.h"

#include <malloc.h>
#include <math.h>

#include "fft.h"
#include "conv.h"
#include "corr.h"


//for now just a function library
WaveProcess::WaveProcess()
{
    return;
}

WaveProcess::~WaveProcess()
{
    //don't mess with anything
   return;
}

void WaveProcess::correlate(double *x, int m, double *y, int n, double *xy)
{
    ap::real_1d_array input_vector_x;
    input_vector_x.setlength(m);
    ap::real_1d_array input_vector_y;
    input_vector_y.setlength(n);
    ap::real_1d_array output_vector_xy;
    output_vector_xy.setlength(n+m-1);

    int i;

    for(i = 0; i < m; ++i)
    {
        input_vector_x(i) = x[i];
    }
    for(i = 0; i < n; ++i)
    {
        input_vector_y(i) = y[i];
    }

    corrr1d(input_vector_x, m, input_vector_y,n, output_vector_xy);

    for(i = 0; i < m+n-1; ++i)
    {
       xy[i]=output_vector_xy(i);
    }

}

//does non-circular convolution in the frequency domain
void WaveProcess::convolve(double *x, int m, double *y, int n, double *xy)
{
    ap::real_1d_array input_vector_x;
    input_vector_x.setlength(m);
    ap::real_1d_array input_vector_y;
    input_vector_y.setlength(n);
    ap::real_1d_array output_vector_xy;
    output_vector_xy.setlength(n+m-1);

    int i;

    for(i = 0; i < m; ++i)
    {
        input_vector_x(i) = x[i];
    }
    for(i = 0; i < n; ++i)
    {
        input_vector_y(i) = y[i];
    }

    convr1d(input_vector_x, m, input_vector_y,n, output_vector_xy);

    for(i = 0; i < m+n-1; ++i)
    {
       xy[i]=output_vector_xy(i);
    }
}

void WaveProcess::ifft(double *Re, double *Im, double *t, int n)
{
    ap::complex_1d_array input_vector;
    input_vector.setlength(n);

    int i;

    for(i = 0; i < n; ++i)
    {
        input_vector(i).x = Re[i];
        input_vector(i).y = Im[i];
    }

    fftc1dinv(input_vector, n);

    for(i = 0; i < n; ++i)
    {
        t[i]=input_vector(i).x;
    }
}
void WaveProcess::ifft(ap::complex_1d_array& a, int n)
{

    fftc1dinv(a, n);
}

void WaveProcess::fft(double *t, double *Re, double *Im, int n)
{
    ap::real_1d_array input_vector;
    input_vector.setlength(n);
    ap::complex_1d_array output_vector;
    output_vector.setlength(n);

    int i;

    for(i = 0; i < n; ++i)
    {
        input_vector(i) = t[i];
    }

    fftr1d(input_vector, n, output_vector);

    for(i = 0; i < n; ++i)
    {
       Re[i] = output_vector(i).x;
       Im[i] = output_vector(i).y;
    }
}

void WaveProcess::fft(const ap::real_1d_array& a, int n, ap::complex_1d_array& f)
{
    fftr1d(a, n, f);
}

//   adapted from fortran version  05/22/10
//
//   tsld1   -  from the Argonne Toeplitz package:
//   solves a general Toeplitz system using the classical Levinson algorithm.
//   This subroutine is described in:
//   O. B. Arushanian et al., "The Toeplitz Package Users' Guide",
//   Report ANL-83-16, Mathematics and Computer Science Division,
//   Argonne National Laboratory; October 1983.

void WaveProcess::tsld1(double *a1, double *a2 ,double *b, double *x, int m)
{
    int i1, i2, n, n1, n2;
    double r1, r2, r3, r5, r6;
    double *c1;
    double *c2;

    c1 = (double*)malloc((m-1)*sizeof(double));
    c2 = (double*)malloc((m-1)*sizeof(double));

        r1 = a1[0];
    x[0] = b[0]/r1;
        if (m != 1)
        {
                for(n = 2; n <= m; ++n)
                {
                        n1 = n - 1;
                        n2 = n - 2;
                        r5 = a2[n1-1];
                        r6 = a1[n-1];
                        if (n != 2)
                        {
                                c1[n1-1] = r2;
                                for(i1 = 1; i1 <= n2;++i1)
                                {
                                        i2 = n - i1;
                                        r5 = r5 + a2[i1-1]*c1[i2-1];
                                        r6 = r6 + a1[i1]*c2[i1-1];
                                }
                        }
                        r2 = -r5/r1;
                        r3 = -r6/r1;
                        r1 = r1 + r5*r3;
                        if (n != 2)
                        {
                                r6 = c2[0];
                                c2[n1-1] = 0.0;
                                for(i1 = 2; i1 <= n1;++i1)
                                {
                                        r5 = c2[i1-1];
                                        c2[i1-1] = c1[i1-1]*r3 + r6;
                                        c1[i1-1] = c1[i1-1] + r6*r2;
                                        r6 = r5;
                                }
                        }
                        r5 = 0.0;
                        c2[0] = r3;
                        for(i1 = 1; i1 <= n1; ++i1)
                        {
                                i2 = n - i1;
                                r5 = r5 + a2[i1-1]*x[i2-1];
                        }
                        r6 = (b[n-1] - r5)/r1;
                        for(i1 = 1; i1 <= n1; ++i1)
                        {
                                x[i1-1] = x[i1-1] + c2[i1-1]*r6;
                        }
                        x[n-1] = r6;
                }
        }
        free(c1);
        free(c2);
        return;
}
