#ifndef CIRCULARARRAY2DSPECTRUMTHREADED_H
#define CIRCULARARRAY2DSPECTRUMTHREADED_H
#include "circulararray2dspectrum.h"
#include <QMutex>

template <typename T>
class CircularArray2DSpectrumThreaded : public CircularArray2DSpectrum<T>
{
public:
    CircularArray2DSpectrumThreaded(int channels, int samples);
    ~CircularArray2DSpectrumThreaded();
    int getNumFreeSpaces();
    int getNumUsedSpaces();
    T* loadSample();
    void writeSample(T* sampleData);
    void fastPopBlockSamples(Array2DSpectrum<T> *outputBlock,int blockSize);
    void fastPopBlockSamples(CircularArray2DSpectrum<T> *outputBlock, int blockSize);
    void fastPushBlockSamples(Array2DSpectrum<T> *inputBlock, int blockSize);
    void increaseFreeSpaces();

private:
    int numberUsedSpaces;
    int numberFreeSpaces;
    QMutex mutex;
};

#endif // CIRCULARARRAY2DSPECTRUMTHREADED_H
