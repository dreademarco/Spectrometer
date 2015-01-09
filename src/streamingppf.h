#ifndef STREAMINGPPF_H
#define STREAMINGPPF_H

#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <sys/time.h>
#include <x86intrin.h>
#include <cstddef>
#include <stdio.h>
#include <string.h>

#include "fftwsequence.h"
using namespace std;

class StreamingPPF
{
public:
    enum window{HAMMING, HANN};
    StreamingPPF(FFTWSequence* inputStreamBuffer, FFTWSequence* outputStreamBuffer, int filter_taps = 4, int fft_points = 256, int blocks=1, int sampling_rate=1024, int duration_seconds=10, window selected_window = StreamingPPF::HAMMING);
    ~StreamingPPF();
    void applyPPF();

private:
    fftwf_complex *sourceStream;
    fftwf_complex *outputStream;
    fftwf_complex* inputBuffer;
    fftwf_complex* outputBuffer;
    fftwf_complex* fftw_in_threaded;
    fftwf_plan* plans;
    int complex_size;
    StreamingPPF::window m_window;
    //window m_window;
    int fs;
    int duration;
    int n_taps;
    int n_fft;
    int n_samp;
    int n_coefficients;
    float* filterCoeffs;
    int n_blocks;

    void loadFFTWPlans();
    void setupFFTWWisdom();
    void loadFromInputStream();
    void copyToOutputStream();
    void ppfcoefficients();
    float hamming(int order,float alpha);
    float hanning(int order);
    float sinc(float x);
};

#endif // STREAMINGPPF_H
