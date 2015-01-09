#ifndef FFTWSEQUENCECIRCULAR_H
#define FFTWSEQUENCECIRCULAR_H
#include <fftwsequence.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

class FFTWSequenceCircular : public FFTWSequence
{
public:
    FFTWSequenceCircular(int channels, int samples);
    ~FFTWSequenceCircular();

    //slower methods
    fftwf_complex* popSample();
    void popSpectrum(int length, FFTWSequence *outputSection);
    void pushSample(fftwf_complex* sampleData);
    void pushSpectrum(FFTWSequence spectrum);

    //faster methods
    void popSpectrumFast(int length, FFTWSequence *outputSection);
    void popSpectrumFast(int length, FFTWSequenceCircular *outputSection);
    void pushSpectrumFast(int length, FFTWSequence *inputSection);

    void incrementReadIndex(int length);
    void incrementWriteIndex(int length);
    void incrementReadIndex();
    void decrementReadIndex();
    void incrementWriteIndex();
    void decrementWriteIndex();

    int getCurrentSampleWriteIndex();
    void setCurrentSampleReadIndex(int idx);
    int getCurrentSampleReadIndex();
    void setCurrentSampleWriteIndex(int idx);

    int getRefillCount();

private:
    int currentSampleWriteIdx;
    int currentSampleReadIdx;
    int refillCounter;
};

#endif // FFTWSEQUENCECIRCULAR_H
