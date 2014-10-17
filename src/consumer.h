#ifndef CONSUMER_H
#define CONSUMER_H

#include <QThread>
#include <QTime>
#include <QVector>

#include "common.h"
#include "stdio.h"
#include "circulararray2dworker.h"
#include "spectrogramplot.h"

class Consumer : public QThread
{
    Q_OBJECT

public:
    Consumer(QObject *parent = 0);
    Consumer(QObject *parent = 0, CircularArray2DWorker<float> *dataBlock = NULL, SpectrogramData<float> *spectData = NULL, SpectrogramPlot *myPlot = NULL);
    void run();

signals:
    //void stringConsumed(const QString &text);
    void bufferFillCountChanged(int cCount);
    void consumerCountChanged(int count);
    void spectDataReceived();

private:
    CircularArray2DWorker<float> *dataBlock;
    //SpectrogramPlot *plotter;
    SpectrogramData<float> *mySpectrogramData;
    SpectrogramPlot *myPlot;
};

#endif // CONSUMER_H
