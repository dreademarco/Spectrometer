#ifndef PPF_H
#define PPF_H
#include "array2dspectrum.h"
#include "circulararray2dspectrum.h"

#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <sys/time.h>
////#include <xmmintrin.h> //m128
//#include <immintrin.h> //m256
////#include <zmmintrin.h> //m512
#include <x86intrin.h>
using namespace std;

class PPF
{
public:
    PPF(int filter_taps = 8, int fft_points = 256, int blocks=1, int sampling_rate=1024, int duration_seconds=1000, int n_threads=1, bool generateWisdom=true);
    ~PPF();
    void printCoefficients();

private:
    int complex_size;
    int fs;
    int duration;

    int n_taps;
    int n_fft;
    int n_samp;
    int n_coefficients;
    float* filterCoeffs;
    fftwf_complex* inputBuffer;
    fftwf_complex* outputBuffer;
    //fftwf_complex* fifo;
    fftwf_complex* chirpsignal;
    int fftblocks;
    int numThreads;
    int sse_factor;

    void loadInput(fftwf_complex* input);
    void applyPPF();
    void generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal);
    void createFFTPlanWisdom();
    void calculateTapOutput(int block_counter);
    void fftShift(fftwf_complex* in, fftwf_complex* out);
    void circShift(fftwf_complex* in, fftwf_complex* out, int k);
    void angle(fftwf_complex* in, float* out);
    void ppfcoefficients();
    float hanning(int order);
    float sinc(float x);
};

#endif // PPF_H
