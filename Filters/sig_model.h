#ifndef SIG_MODEL_H
#define SIG_MODEL_H

#endif // SIG_MODEL_H

#define FFTLEN 32768

class signal_model
{
public:
    signal_model();
    ~signal_model();
    void gen_gaussian_pulse(double *gaussian);
    void single_pole_lowpass(double *x, double t);
    void single_pole_highpass(double *x, double t);
    void gen_signal();
    double *get_signal(){return signal;};
    void set_params(double t_peak_taili, double t_EADi, double t_highpassi, double FWHMi, double fsi, int FFTlen);

private:

    double fs;
    double t_peak_tail;
    double t_EAD;
    double t_highpass;
    double FWHM;

    double *signal;
    double len;
};
