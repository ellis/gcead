#include "Filters.h"
#include "AutoCov.h"
#include "WaveProcess.h"
#include "sig_model.h"
#include "conv.h"
#include <stdio.h>

#define FFTLEN 32768

Filters::Filters()
{
    white_len = 0;
    P = 100;                    //order of AR model
    noise_seg_start = 9999;    //index
    noise_seg_end = 39999;      //index
    signal =  0;
    wienerFilt = 0;
    NWMFFilt = 0;
    SNR = 20.0;                  //dB
}

Filters::~Filters()
{
    if(white_len!=0)
        delete whiteFilt;
    if(signal != 0)
        delete signal;
    if(wienerFilt != 0)
        delete wienerFilt;
    if(NWMFFilt != 0)
        delete NWMFFilt;
}

void Filters::set_signal_params(double t_peak_taili, double t_EADi, double t_highpassi, double FWHMi, double fsi, int FFTlen)
{
    signal->set_params(t_peak_taili, t_EADi, t_highpassi, FWHMi, fsi, FFTlen);
}

//private function
void Filters::convolve(double *x, int m, double *y, int n, double *xy)
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

void Filters::convolve_white(double *x, int len, double *xy)
{
    convolve(x, len, whiteFilt, white_len, xy);
}

void Filters::convolve_NWMF(double *x, int len, double *xy)
{
     convolve(x, len, NWMFFilt, NWMFlen, xy);
}

void Filters::convolve_wiener(double *x, int len, double *xy)
{
    convolve(x, len, wienerFilt, wiener_len, xy);
}

void Filters::calcWhiteningFilterYW(double *x)
{
    calcWhiteningFilterYW(P, x, noise_seg_start, noise_seg_end);
}

void Filters::calcWhiteningFilterYW(int Pi, double *x, int SegmentStart, int SegmentStop)
{

    P = Pi;
    noise_seg_start = SegmentStart;    //index
    noise_seg_end = SegmentStop;       //index

    double *xt = &x[SegmentStart];
    int size = SegmentStop - SegmentStart;


    //subtract mean before taking autocorrelation
    double sum = 0.0;
    for(int i = 0; i < size; ++i)
    {
        sum += xt[i];
    }

    sum /= size;

    for(int i = 0; i < size; ++i)
    {
        xt[i] -= sum;
    }



    if(white_len!=0)
        delete whiteFilt;

    whiteFilt = new double[P+1];
    white_len = P + 1;

    autocov *autocorr = new autocov;
    autocorr->calcAutoCov(P+1,size,xt);
    double *autocorra = autocorr->GetAutocorr();
    std_dev = autocorr->GetStddev();

    //add the mean back in...
    for(int i = 0; i < size; ++i)
    {
        xt[i] += sum;
    }

    /*
    FILE *ac;

    ac = fopen("ac.txt","w+");

    for(int i = 0; i < P+1; ++i)
    {
        fprintf(ac, "%8.6e\n", autocorra[i]);
    }

    fclose(ac);

    */

    //solve Yule-Walker equations
    double *YuleWalker = new double[P];
    WaveProcess w;

            //first row  first col          x        solution  matrix size
    w.tsld1(autocorra,   autocorra+1,  autocorra+1, YuleWalker,    P);

    standard_min_prediction_error = 0.0;

    //first coefficient is 1.0
    whiteFilt[0] = 1.0;
    //reverse sign
    int i;
    for(i = 0; i < P; ++i)
    {
        whiteFilt[i+1] = -YuleWalker[i];
        //compute prediction error
        standard_min_prediction_error -= YuleWalker[i]*autocorra[i+1];
    }

    standard_min_prediction_error = sqrt(standard_min_prediction_error + 1);

    //filter with input stddev will output white noise with same stddev
    for(i = 0; i < P+1; ++i)
    {
        whiteFilt[i] /= standard_min_prediction_error;
    }




    /*

    FILE *white_file;

    white_file = fopen("white.txt","w+");

    for(i = 0; i < P+1; ++i)
    {
        fprintf(white_file, "%8.6e\n", whiteFilt[i]);
    }

    fclose(white_file);

    */

    delete autocorr;
    delete YuleWalker;
}

void Filters::calcWienerFilter()
{

    if(white_len == 0)
    {
        //whitening filter (noise model) must be computed first
        return;
    }
    else
    {
        //using waveprocess as a library at the moment...
        WaveProcess *x = new WaveProcess;

        double *temp = new double[FFTLEN];

        int i;


        for(i = 0; i < white_len; ++i)
        {
            temp[i] = whiteFilt[i];
        }

        //zero padding
        for(i = white_len; i < FFTLEN; ++i)
        {
            temp[i] = 0.0;
        }

        double *Re = new double[FFTLEN];
        double *Im = new double[FFTLEN];
        double *nPSD = new double[FFTLEN];


        x->fft(temp,Re,Im,FFTLEN);

        //compute noise PSD - shouldn't be any zeros in the denominator
        for(i = 0; i < FFTLEN; ++i)
        {
            nPSD[i] = 1/(Re[i]*Re[i] + Im[i]*Im[i]);
        }

        //compute signal PSD

        double *sPSD = new double[FFTLEN];

        //generate signal model
        if(signal != 0)
            delete signal;
        signal = new signal_model();
        signal->gen_signal();
        //take DFT
        x->fft(signal->get_signal(),Re,Im,FFTLEN);

        //compute signal PSD
        for(i = 0; i < FFTLEN; ++i)
        {
            sPSD[i] = (Re[i]*Re[i] + Im[i]*Im[i]);
        }

        //if referenced to NWMF SNR, need its filtering gain
        double lSNR = pow(10, SNR/10);

        for(i = 0; i < FFTLEN; ++i)
        {
            sPSD[i] *= lSNR;
        }

        if(wienerFilt != 0)
            delete wienerFilt;
        wienerFilt = new double[FFTLEN];

        //compute wiener smoother in freq domain
        //circular approximation should result in negligible error
        for(i = 0; i < FFTLEN; ++i)
        {
            Re[i] = sPSD[i]/(sPSD[i]+nPSD[i]);
            Im[i] = 0.0;
        }

        x->ifft(Re,Im,temp,FFTLEN);

        //rotate filter

        int middle = (int)FFTLEN/2;
        for(i = 0; i < middle; ++i)
        {
            wienerFilt[i] = temp[i+middle];
        }
        for(;i<FFTLEN;++i)
        {
            wienerFilt[i] = temp[i-middle];
        }

        //auto covariance is needed for uncertainty calculation
        //should perhaps use ifft of signal PSD instead

        double *temp2 = new double[FFTLEN*2-1];
        double *autocorra = new double[FFTLEN*2-1];

        x->correlate(signal->get_signal(), FFTLEN, signal->get_signal(),FFTLEN,temp2);

        //strange - for some reason this result needs to be rotated...
        //problem with correlate function
        for(i = 0; i < FFTLEN - 1;++i)
        {
            autocorra[i]=temp2[i+FFTLEN];
        }

        for(; i < 2*FFTLEN-1;++i)
        {
            autocorra[i]=temp2[i-FFTLEN + 1];
        }

        delete temp2;

        //calculate uncertainty
        double sum = 0.0;

        for(i = 0;i < middle; ++i)
        {
            sum += temp[i]*autocorra[i+FFTLEN];
        }

        sum *= 2.0;

        wiener_stddev = sqrt(lSNR - sum);
        wiener_len = FFTLEN;

        delete temp;
        delete Re;
        delete Im;
        delete sPSD;
        delete nPSD;
        delete x;
        delete autocorra;

    }
}

void Filters::calcNWMFFilter()
{
    if(white_len == 0)
    {
        //whitening filter (noise model) must be computed first
        return;
    }
    else
    {

        //generate signal model
        if(signal != 0)
            delete signal;

        signal = new signal_model();
        signal->gen_signal();

        //get signal length and replace FFTLEN...

        WaveProcess *x = new WaveProcess;

        double *temp = new double[2*white_len];
        NWMFFilt = new double[2*white_len-1+FFTLEN-1];

        NWMFlen = 2*white_len-1+FFTLEN-1;

        x->correlate(whiteFilt,white_len,whiteFilt,white_len,temp);

        //strange - for some reason this result needs to be rotated...
        //problem with correlate function

        double *temp2 = new double[2*white_len];

        int i;
        for(i = 0; i < white_len - 1;++i)
        {
            temp2[i]=temp[i+white_len];
        }

        for(; i < 2*white_len-1;++i)
        {
            temp2[i]=temp[i-white_len + 1];
        }

        /*
        FILE *tempf;

        tempf = fopen("temp.txt","w+");

        for(i = 0; i < 2*white_len-1; ++i)
        {
            fprintf(tempf, "%8.6e\n", temp2[i]);
        }

        fclose(tempf);
        */
        x->correlate(temp2,2*white_len-1,signal->get_signal(),FFTLEN,NWMFFilt);


        //this result is perfectly fine ... maybe because the sizes are different?

        //check nwmf


          /*
          FILE *nwmf;

          nwmf = fopen("nwmf.txt","w+");

          for(i = 0; i < NWMFlen; ++i)
          {
              fprintf(nwmf, "%8.6e\n", NWMFFilt[i]);
          }

          fclose(nwmf);
          */



        delete temp;
        delete temp2;

        //find maximum value of normalized signal

        double max = 0.0;
        double *sig = signal->get_signal();
        for(i = 0; i < FFTLEN; ++i)
        {
            if(sig[i] > max)
                max = sig[i];
        }

        double sig_in_energy = max*max;

        //find energy of whitened signal

        temp = new double[FFTLEN+white_len-1];

        x->convolve(signal->get_signal(),FFTLEN,whiteFilt,white_len,temp);

        double sig_out_energy = 0.0;
        for(i = 0; i < FFTLEN+white_len-1; ++i)
        {
            sig_out_energy += temp[i]*temp[i];
        }

        NWMF_SNR_gain = sig_out_energy/sig_in_energy;

        sig_out_energy = sqrt(sig_out_energy);

       //scale NWMF output so that output noise level is the same

        for(i = 0; i <  NWMFlen; ++i)
        {
            NWMFFilt[i] /= sig_out_energy;
        }

        delete x;
        delete temp;

    }
}
