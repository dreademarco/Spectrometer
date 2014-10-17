#ifndef PRODUCER_H
#define PRODUCER_H

#include <QThread>
#include <QTime>
#include <iostream>

#include "common.h"
#include "circulararray2dworker.h"
using namespace std;

class Producer : public QThread
{
    Q_OBJECT
public:
    Producer(QObject *parent = 0);
    Producer(QObject *parent = 0, CircularArray2DWorker<float> *dataBlock = NULL);
    void run();

signals:
    void bufferFillCountChanged(int bCount);
    void producerCountChanged(int count);

public slots:

private:
    CircularArray2DWorker<float> *dataBlock;
};

#endif // PRODUCER_H
