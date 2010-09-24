#ifndef FILTERS_H
#define FILTERS_H

class signal_model;

class Filters
{
public:
    Filters();
    ~Filters();
    void calcWhiteningFilterYW(int Pi, double *x, int SegmentStarti, int SegmentStopi);
    void calcWhiteningFilterYW(double *x);
    void calcWienerFilter();
    void calcNWMFFilter();
    int get_NWMFlen(){return NWMFlen;};
    int get_whiteFiltlen(){return white_len;};
    int get_wienerFiltlen(){return wiener_len;};
    void convolve_white(double *x, int len, double *xy);
    void convolve_NWMF(double *x, int len, double *xy);
    void convolve_wiener(double *x, int len, double *xy);
    void set_signal_params(double t_peak_taili, double t_EADi, double t_highpassi, double FWHMi, double fsi, int FFTlen);
    void generate_signal();


private:

    void convolve(double *x, int m, double *y, int n, double *xy);

    //input variables
    int P;
    int noise_seg_start;
    int noise_seg_end;
    double SNR;


    //signal model
    signal_model *signal;


    //filter sequences
    double *whiteFilt;
    double *wienerFilt;
    double *NWMFFilt;

    //filter sequence lengths
    int white_len;
    int NWMFlen;
    int wiener_len;


    //output variables
    double std_dev;
    double standard_min_prediction_error;
    double wiener_stddev;
    double NWMF_SNR_gain;


};



#endif // FILTERS_H
