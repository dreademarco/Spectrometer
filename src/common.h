#ifndef COMMON_H
#define COMMON_H

#include <QSemaphore>
#include <QVector>
#include "myConstants.h"

typedef QVector<QVector<float> > TwoDQVector;

//extern QVector<QVector<float> > dataBlock(freqBins); //Contains samplesSize float arrays.
//MyVector *dataBlock;
extern QSemaphore freeSpace;
extern QSemaphore usedSpace;

#endif // COMMON_H
