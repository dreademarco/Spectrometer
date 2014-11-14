#ifndef PPF_H
#define PPF_H
#include "array2dspectrum.h"
#include "circulararray2dspectrum.h"

#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <sys/time.h>
using namespace std;

class PPF
{
public:
    PPF(int filter_taps = 8, int fft_points = 256, int fft_block_size = 10);
    ~PPF();
    void printCoefficients();
//    typedef float cfloat[2];
////    typedef struct
////    {
////        float re, im;
////    } cfloat;

private:
    int complex_size;

    int n_taps;
    int n_fft;
    int nfft_blocks;
    fftwf_plan plan;
    int n_coefficients;
    float* filterCoeffs;
    fftwf_complex* tappedFilter;
    fftwf_complex* ppfOutput;
    fftwf_complex* fftwOutput;


    fftwf_complex* chirpsignal;

    fftwf_complex* ppfSignalOutput;
    float* realPpfSignalOutput;
    fftwf_complex* in;
    fftwf_complex* out;

    void applyPPF(fftwf_complex* input);
    void generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal);
    void createFFTPlan();
    void pushTapValues(fftwf_complex* newTapInput);
    void calculateTapOutput();
    void calculateTapOutput_threaded(int nThreads);
    void ppfcoefficients();
    float hanning(int order);
    float sinc(float x);
};

#endif // PPF_H
