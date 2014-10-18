#ifndef PRODUCERUNSAFE_H
#define PRODUCERUNSAFE_H

#include <QThread>
#include <QTime>
#include <iostream>
#include <time.h>
#include <stdio.h>

#include "common.h"
#include "circulararray2dworkersafe.h"
#include "circulararray2dworkerthreaded.h"

class ProducerUnsafe : public QThread
{
     Q_OBJECT
public:
    ProducerUnsafe(QObject *parent = 0);
    ProducerUnsafe(QObject *parent = 0, CircularArray2DWorkerThreaded<float> *dataBlock = NULL);
    void run();

signals:
    void bufferFillCountChanged(int bCount);
    void producerCountChanged(int count);

private:
    CircularArray2DWorkerThreaded<float> *dataBlock;
};

#endif // PRODUCERUNSAFE_H
