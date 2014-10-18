#ifndef CIRCULARARRAY2DSPECTRUM_H
#define CIRCULARARRAY2DSPECTRUM_H
#include <array2dspectrum.h>
#include <iostream>
#include <stdio.h>

template <typename T>
class CircularArray2DSpectrum : public Array2DSpectrum<T>
{
public:
    CircularArray2DSpectrum();
    CircularArray2DSpectrum(int channels, int samples);
    ~CircularArray2DSpectrum();
    void pushSample(T* sampleData);
    void pushSpectrum(Array2DSpectrum<T>);
    T* popSample();
    void popSampleSection(int length, Array2DSpectrum<T> *outputSection);

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