#ifndef PPF_H
#define PPF_H

#include "common.h"

class PPF
{
public:
    enum window{HAMMING, HANN};

    int n_blocks;
    int n_taps;
    int n_fft;
    int n_chansInput;
    int n_coefficients;
    int n_samp;


    PPF::window m_window;
    float* filterCoeffs;

    PPF(FFTWSequence* input, FFTWSequence* output, int n_taps, int fft_points, window selected_window);
    void applyPPF();
    ~PPF();

    fftwf_plan* plans;

private:
    fftwf_complex *inputData;
    fftwf_complex *outputData;

    fftwf_complex* inputBuffer;
    fftwf_complex* outputBuffer;

    fftwf_complex* fftw_in;

    // FFT
    void setupFFTWWisdom();
    void loadFFTWPlans();

    // Taps
    void loadFromInputStream(int channel);
    void copyToOutputStream(int channel);

    // Coefficients
    void ppfcoefficients();
    float hamming(int order,float alpha);
    float hanning(int order);
    float sinc(float x);
};

#endif // PPF_H
