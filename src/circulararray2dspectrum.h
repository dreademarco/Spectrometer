#ifndef CIRCULARARRAY2DSPECTRUM_H
#define CIRCULARARRAY2DSPECTRUM_H
#include <array2dspectrum.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

template <typename T>
class CircularArray2DSpectrum : public Array2DSpectrum<T>
{
public:
    CircularArray2DSpectrum();
    CircularArray2DSpectrum(int channels, int samples);
    ~CircularArray2DSpectrum();

    //slower methods
    T* popSample();
    void popSpectrum(int length, Array2DSpectrum<T> *outputSection);
    void pushSample(T* sampleData);
    void pushSpectrum(Array2DSpectrum<T>);    

    //faster methods
    void popSpectrumFast(int length, Array2DSpectrum<T> *outputSection);
    void popSpectrumFast(int length, CircularArray2DSpectrum<T> *outputSection);
    void pushSpectrumFast(int length, Array2DSpectrum<T> *inputSection);

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

#endif // CIRCULARARRAY2DSPECTRUM_H
