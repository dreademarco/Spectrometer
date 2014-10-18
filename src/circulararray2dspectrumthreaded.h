#ifndef CIRCULARARRAY2DSPECTRUMTHREADED_H
#define CIRCULARARRAY2DSPECTRUMTHREADED_H
#include "circulararray2dspectrum.h"

template <typename T>
class CircularArray2DSpectrumThreaded : public CircularArray2DSpectrum<T>
{
public:
    CircularArray2DSpectrumThreaded(int channels, int samples);
    ~CircularArray2DSpectrumThreaded();
    int getNumFreeSpaces();
    int getNumUsedSpaces();
    void fastPushSample(T* sampleData);
    void fastPopBlockSamples(Array2DSpectrum<T> *outputBlock,int blockSize);

private:
    int numberUsedSpaces;
    int numberFreeSpaces;
};

#endif // CIRCULARARRAY2DSPECTRUMTHREADED_H
