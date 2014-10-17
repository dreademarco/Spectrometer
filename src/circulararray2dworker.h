#ifndef CIRCULARARRAY2DWORKER_H
#define CIRCULARARRAY2DWORKER_H
#include <array2dworker.h>
#include <iostream>
#include <stdio.h>

template <typename T>
class CircularArray2DWorker : public Array2DWorker<T>
{
public:
    CircularArray2DWorker();
    CircularArray2DWorker(int rows, int columns);
    ~CircularArray2DWorker();
    void pushColumn(T* columnData);
    void pushArray(Array2DWorker<T>);
    int getCurrentColumnWriteIndex();
    int getCurrentColumnReadIndex();
    void setCurrentColumnWriteIndex(int idx);
    void setCurrentColumnReadIndex(int idx);
    int getRefillCount();
    T* popColumn();
    void popColumnSection(int length, Array2DWorker<T> *tempWorker);
    void incrementColumnReadIndex();
    void incrementColumnWriteIndex();
    void decrementColumnReadIndex();
    void decrementColumnWriteIndex();
    void toString();

private:
    int currentColumnWriteIdx;
    int currentColumnReadIdx;
    int refillCounter;
};

#endif // CIRCULARARRAY2DWORKER_H
