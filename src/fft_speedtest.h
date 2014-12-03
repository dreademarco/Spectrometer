#ifndef FFT_SPEEDTEST_H
#define FFT_SPEEDTEST_H

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

class fft_speedtest
{
public:
    fft_speedtest(int filter_taps = 4, int fft_points = 256, int sampling_rate = 1024, int duration_seconds=10, int n_threads = 1);
    ~fft_speedtest();
    void printCoefficients();

private:
    int complex_size;
    int fs;
    int duration;

    int n_taps;
    int n_fft;
    int n_samp;
    int numThreads;
    fftwf_complex* inputBuffer;
    fftwf_complex* outputBuffer;
    fftwf_complex* chirpsignal;
    int fftblocks;

    void loadInput(fftwf_complex* input);
    void loadInputFromFile(char *filename);
    void apply_seqFFT(fftwf_plan plan);
    void apply_blockFFT(fftwf_plan plan);
    void generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal);
    void createFFTPlanWisdom_sequential();
    void createFFTPlanWisdom_block();
};
#endif // FFT_SPEEDTEST_H
