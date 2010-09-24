#include "sig_model.h"
#include <math.h>
#include <stdio.h>



signal_model::signal_model()
{

    //These parameters should be adjustable via the user interface
    t_peak_tail = 5.0;          //seconds
    t_EAD = 10.0;               //seconds
    t_highpass = 1.591;         //0.1 Hz corner freq
    FWHM = 1.0;                 //sec
    fs = 100.0;                 //sample rate = 100 Hz
    len = FFTLEN;                //power of 2 for efficient FFT
    signal = new double[FFTLEN];

}

void signal_model::set_params(double t_peak_taili, double t_EADi, double t_highpassi, double FWHMi, double fsi, int FFTlen)
{
    if(FFTlen != len)
    {
        delete signal;
        signal = new double[FFTlen];
    }
    t_peak_tail = t_peak_taili;
    t_EAD = t_EADi;
    t_highpass = t_highpassi;
    FWHM = FWHMi;
    fs = fsi;
}

signal_model::~signal_model()
{
    delete signal;
}

void signal_model::gen_gaussian_pulse(double *gaussian)
{

    //delay 10 seconds;
    double t_delay = 10;
    double std_dev = FWHM*0.6;
    double argument;
    double ti;

    for(int i = 0; i < FFTLEN; ++i)
    {
        ti = i/fs;
        argument = (ti - t_delay)/std_dev;
        gaussian[i] = exp(-(argument*argument));
    }

}

void signal_model::single_pole_lowpass(double *x, double t)
{

  //  1/(1-az-1) <-> a^nu[n]

    //solve for a

    double a = exp(-1/(fs*t));


    //for now y[0] = x[0];
    for(int i = 1; i < FFTLEN; ++i)
    {
        x[i] = x[i] + a*x[i-1];
    }
}

void signal_model::single_pole_highpass(double *x, double t)
{

    //  1/(1-az-1) <-> a^nu[n]

      //solve for a

      double a = exp(-1/(fs*t));

      double *y = new double[FFTLEN];

      y[0] = 0;

      int i;
      //for now y[0] = x[0];
      for(i = 1; i < FFTLEN; ++i)
      {
          y[i] = x[i] - x[i-1];
      }
      for(i = 1; i < FFTLEN; ++i)
      {
          x[i] = y[i] + a*x[i-1];
      }

      delete y;

}

void signal_model::gen_signal()
{
    gen_gaussian_pulse(signal);

      single_pole_lowpass(signal, t_peak_tail);
      single_pole_lowpass(signal, t_EAD);
      single_pole_highpass(signal, t_highpass);

    //normalize signal vector
    double sum = 0.0;
    int i;
    for(i = 0; i < FFTLEN; ++i)
    {
        sum += signal[i]*signal[i];
    }

    double norm = sqrt(sum);
    for(i = 0; i < FFTLEN; ++i)
    {
        signal[i] = signal[i]/norm;
    }

    /* debugging
    FILE *sig_file;

    sig_file = fopen("signal.txt","w+");

    for(i = 0; i < FFTLEN; ++i)
    {
        fprintf(sig_file, "%8.3e\n", signal[i]);
    }

    fclose(sig_file);
    */




}
