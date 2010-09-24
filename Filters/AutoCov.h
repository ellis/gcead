#ifndef AUTOCOV_H
#define AUTOCOV_H

class autocov
{
public:
    autocov();
    ~autocov();
    void calcAutoCov(int P, int len, double *x);

    double* GetAutocorr();
    int GetSize();
    double GetStddev();

private:
    double *autocova;
    double std_dev;
    int size;
    int allocated;
};


#endif // AUTOCOV_H
