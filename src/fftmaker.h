#ifndef FFTMAKER_H
#define FFTMAKER_H

#include <fftw3.h>
#include <cstddef>
#include <complex>

#include <array2dspectrum.h>
using namespace std;

class FFTMaker
{
public:
    enum FFT_DATATYPE{REAL, COMPLEX};
    enum FFT_DIM{ONE_D, TWO_D};
    enum FFT_PROCESS{FORWARD, BACKWARD};
    enum FFT_FLAG{MEASURE, ESTIMATE};
    FFTMaker(FFT_DATATYPE selected_datatype, FFT_DIM selected_dim, FFT_PROCESS selected_process, FFT_FLAG selected_flag, int N1 = 0, int N2 = 0, int n_samples);
    void createFFTPlan();
    void executeFFTPlan();
    void destroyFFTPlan();
    void clearArrays();
    void switchData(Array2DSpectrum<double> *inputR, Array2DSpectrum<complex<double> > *inputC);
    void flushData(Array2DSpectrum<double> *outputR, Array2DSpectrum<complex<double> > *outputC);

private:
    FFT_DATATYPE m_fft_datatype;
    FFT_DIM m_fft_dim;
    FFT_PROCESS m_fft_process;
    FFT_FLAG m_fft_flag;
    fftw_plan fft_plan;
    int N1;
    int N2;

    double *inR;
    double *outR;
    fftw_complex *inC;
    fftw_complex *outC;
    int fft_method;
};

#endif // FFTMAKER_H
