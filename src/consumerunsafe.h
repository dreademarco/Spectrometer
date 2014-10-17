#ifndef CONSUMERUNSAFE_H
#define CONSUMERUNSAFE_H

#include <QThread>
#include <QTime>
#include <QVector>

#include "common.h"
#include "stdio.h"
#include "circulararray2dworkersafe.h"
#include "spectrogramplot.h"

class ConsumerUnsafe : public QThread
{
    Q_OBJECT

public:
    ConsumerUnsafe(QObject *parent = 0);
    ConsumerUnsafe(QObject *parent = 0, CircularArray2DWorkerSafe<float> *dataBlock = NULL, SpectrogramData<float> *spectData = NULL, SpectrogramPlot *myPlot = NULL);
    void run();

signals:
    void bufferFillCountChanged(int cCount);
    void consumerCountChanged(int count);
    void spectDataReceived();

private:
    CircularArray2DWorkerSafe<float> *dataBlock;
    SpectrogramData<float> *mySpectrogramData;
    SpectrogramPlot *myPlot;
};

#endif // CONSUMERUNSAFE_H
