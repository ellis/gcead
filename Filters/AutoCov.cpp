
#include "AutoCov.h"
#include <math.h>
#include <stdio.h>


autocov::autocov()
{
    size = 0;
    allocated = 0;
}

autocov::~autocov()
{
    if(allocated != 0)
        delete autocova;
}

double* autocov::GetAutocorr()
{
    return autocova;
}

int autocov::GetSize()
{
    return size;
}

double autocov::GetStddev()
{
    return std_dev;
}

void autocov::calcAutoCov(int P, int len, double *x)
{
    int i;
    int j;
    double sum, temp;

    if(allocated != 0)
    {
        delete autocova;
    }
    else
    {
        allocated = 1;
    }
    autocova = new double [P];
    size = P;

    /*
    FILE *autod;

    autod = fopen("autod.txt","w+");

    for(i = 0; i < len; ++i)
    {
        fprintf(autod, "%8.6e\n", x[i]);
    }

    fclose(autod);
    */

    //"biased" autocovariance
    for(j = 0; j < P; ++j)
    {
        sum = 0.0;
        for(i = 0;i < len-j;++i)
        {
            sum += x[i]*x[i+j];
        }
        if(j == 0)
        {
            temp = sum;
            std_dev = sqrt(sum/(size /*-1*/));
        }
        autocova[j] = sum/temp;
    }
}


