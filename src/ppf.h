#ifndef PPF_H
#define PPF_H
#include "array2dspectrum.h"
#include "circulararray2dspectrum.h"

#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <omp.h>
#include <emmintrin.h>
#include <cstdlib>
#include <sys/time.h>
using namespace std;

class PPF
{
public:
    PPF(int filter_taps = 8, int fft_points = 256, int blocks=1, int sampling_rate=1024, int duration_seconds=1000);
    ~PPF();
    void printCoefficients();

private:
    int complex_size;
    int fs;
    int duration;

    int n_taps;
    int n_fft;
    fftwf_plan plan;
    int n_coefficients;
    float* filterCoeffs;
    fftwf_complex* tappedFilter;
    fftwf_complex* chirpsignal;
    fftwf_complex* in;
    fftwf_complex* out;
    int fftblocks;

    int numThreads;

//    int* indexes_filterCoeffs;
//    int* indexes_tappedFilter;

    void applyPPF(fftwf_complex* input);
    void generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal);
    void createFFTPlan();
    void shiftTapValues(int bytesToShift);
    void pushTapValues(fftwf_complex* inputStream, int copyIndex);
    void calculateTapOutput(int block_counter);
    void ppfcoefficients();
    float hanning(int order);
    float sinc(float x);
};

#endif // PPF_H
