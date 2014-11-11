#ifndef PPF_H
#define PPF_H
#include "array2dspectrum.h"
#include "circulararray2dspectrum.h"

#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
using namespace std;

class PPF
{
public:
    PPF(int filter_taps = 8, int fft_points = 256);
    void printCoefficients();

private:
    int n_taps;
    int n_fft;
    fftwf_plan plan;
    int n_coefficients;
    Array2DSpectrum<float> *filterCoeffs;
    Array2DSpectrum<complex<float> > *tappedFilter;
    Array2DSpectrum<complex<float> > *ppfOutput;
    Array2DSpectrum<complex<float> > *fftwOutput;    
    Array2DSpectrum<float> *chirpsignal;
    Array2DSpectrum<complex<float> > *chirpsignalComplex;
    Array2DSpectrum<complex<float> > *ppfSignalOutput;
    Array2DSpectrum<float> *realPpfSignalOutput;
    fftwf_complex* in;
    fftwf_complex* out;

    void applyPPF(Array2DSpectrum<complex<float> > *input);
    //void generateLinearChirp(int fs, int duration, float f0, float f1, Array2DSpectrum<float>* signal, Array2DSpectrum<complex<float> >* complexsignal);
    void generateLinearChirp(int fs, int duration, float f0, float t1, float f1, Array2DSpectrum<float>* signal, Array2DSpectrum<complex<float> >* complexsignal);
    void createFFTPlan();
    void pushTapValues(Array2DSpectrum<complex<float> > *newTapInput);
    void calculateTapOutput();
    void ppfcoefficients();
    float hanning(int order);
    float sinc(float x);
};

#endif // PPF_H
