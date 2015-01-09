#ifndef FFTWSEQUENCECIRCULARTHREADED_H
#define FFTWSEQUENCECIRCULARTHREADED_H
#include <QMutex>

#include "fftwsequencecircular.h"

class FFTWSequenceCircularThreaded : public FFTWSequenceCircular
{
public:
    FFTWSequenceCircularThreaded(int channels, int samples);
    ~FFTWSequenceCircularThreaded();
    int getNumFreeSpaces();
    int getNumUsedSpaces();
    fftwf_complex* loadSample();
    void writeSample(fftwf_complex* sampleData);
    void fastPopBlockSamples(FFTWSequence *outputBlock,int blockSize);
    void fastPopBlockSamples(FFTWSequenceCircular *outputBlock, int blockSize);
    void fastPushBlockSamples(FFTWSequence *inputBlock, int blockSize);
    void increaseFreeSpaces();
    void increaseUsedSpaces();

private:
    int numberUsedSpaces;
    int numberFreeSpaces;
    QMutex mutex;
};

#endif // FFTWSEQUENCECIRCULARTHREADED_H
