#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <common.h>
#include <stdio.h>
#include <sys/time.h>

#include "circulararray2dspectrumthreaded.h"
#include "spectrogramplot.h"

class Plotter : public QObject
{
    Q_OBJECT
public:
    Plotter(CircularArray2DSpectrumThreaded<float> *sourceDataBlock = NULL, int chunkSize = 0, int integrationFactor = 1, SpectrogramPlot *spectrogramPlot = NULL, QObject *parent = 0);
public slots:
    void start();

signals:
    void readyForPlot();
    void done();

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
