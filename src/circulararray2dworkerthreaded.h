#ifndef CIRCULARARRAY2DWORKERTHREADED_H
#define CIRCULARARRAY2DWORKERTHREADED_H
#include "circulararray2dworker.h"

template <typename T>
class CircularArray2DWorkerThreaded : public CircularArray2DWorker<T>
{
public:
    CircularArray2DWorkerThreaded(int rows, int columns);
    ~CircularArray2DWorkerThreaded();
    int getNumFreeSpaces();
    int getNumUsedSpaces();
    void fastPushColumn(T* columnData);
    void fastPopBlock(Array2DWorker<T> *dataBlock,int blockSize);

private:
    int numberUsedSpaces;
    int numberFreeSpaces;
};

#endif // CIRCULARARRAY2DWORKERTHREADED_H
