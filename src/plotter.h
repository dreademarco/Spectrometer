#ifndef PLOTTER_H
#define PLOTTER_H

#include <QThread>
#include <QTime>
#include <QVector>
#include <common.h>
#include <stdio.h>
#include <sys/time.h>

#include "circulararray2dspectrumthreaded.h"
#include "spectrogramplot.h"

class Plotter : public QThread
{
    Q_OBJECT
public:
    Plotter(QObject *parent = 0, CircularArray2DSpectrumThreaded<float> *sourceDataBlock = NULL, int chunkSize = 0, int integrationFactor = 1, SpectrogramPlot *spectrogramPlot = NULL);
    void run();

signals:
    void readyForPlot();

private:
    CircularArray2DSpectrumThreaded<float> *sourceStream;
    int chunkSize;
    int integrationFactor;
    SpectrogramData<float> *spectrogramData;
    SpectrogramPlot *spectrogramPlot;
    CircularArray2DSpectrum<float> *tempSamples;
    int placements;
    bool loop;

    void fastLoadDataInSpectrogramMemCpy();
};

#endif // PLOTTER_H
