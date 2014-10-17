#ifndef CIRCULARARRAY2DWORKERSAFE_H
#define CIRCULARARRAY2DWORKERSAFE_H
#include <circulararray2dworker.h>
#include <QMutex>
#include <QSemaphore>
#include <QWaitCondition>
#include <iostream>
#include <stdio.h>
#include <QDebug>
#include "myConstants.h"

template <typename T>
class CircularArray2DWorkerSafe : public CircularArray2DWorker<T>
{
public:
    enum priority{READER, WRITER};
    //CircularArray2DWorkerSafe(priority selected_priority, bool safety);
    CircularArray2DWorkerSafe(priority selected_priority, bool safety, int rows, int columns);
    ~CircularArray2DWorkerSafe();
    //T* safePopColumn();
    //void safePushColumn(T* columnData);
    void safePushColumnSemaphore(T* columnData);
    void safePushColumnMutex(T* columnData);
    void fastPushColumn(T* columnData);
    void safePopBlockSemaphore(Array2DWorker<T> *dataBlock);
    void safePopBlockMutex(Array2DWorker<T> *dataBlock);
    void safePopSingleMutex(Array2DWorker<T> *dataBlock);
    int getFreeSlotCountSemaphore();
    int getFreeSlotCountMutex();
    int getFilledSlotCountMutex();
    //int getFreeSlotCount();

    int getNumFreeSpaces();
    int getNumUsedSpaces();
    void modifyNumFreeSpaces(int value);
    void resetFreeSpaces();

private:
    QWaitCondition bufferNotFull;
    QWaitCondition bufferNotEmpty;
    //QWaitCondition bufferEnoughFull;
    QMutex mutex;
    QMutex p_mutex;
    QMutex c_mutex;
    priority m_priority;
    int numberUsedSpaces;
    int numberFreeSpaces;
    bool write_safety_switch;
    bool limiteddata;
};

#endif // CIRCULARARRAY2DWORKERSAFE_H
