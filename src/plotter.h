#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <common.h>
#include <stdio.h>
#include <sys/time.h>

#include "fftwsequencecircularthreaded.h"
#include "spectrogramplot.h"

class Plotter : public QObject
{
    Q_OBJECT
public:
    Plotter(FFTWSequenceCircularThreaded *sourceDataBlock = NULL, SpectrogramPlot *spectrogramPlot = NULL, int selectedSpectSize = 64, int selectedChans = 256, int selectedFs = 1024, int selectedIntegFactor = 1, QObject *parent = 0);
    ~Plotter();
public slots:
    void start();
    void setupTermination();

signals:
    void readyForPlot();
    void done();

private:
    FFTWSequenceCircularThreaded *sourceStream;
    int highIntensity;
    int samplesToProcess;
    int integrationfactor;    
    int srate;
    int spectSize;
    int nchans;
    int guiUpdateSize; // samples to process before GUI progress update
    int samplesprocessed; //collect and process data from buffer at a different rate
    SpectrogramData *spectrogramData;
    SpectrogramPlot *spectrogramPlot;
    FFTWSequenceCircular *tempSamples;
    int placements;
    int prevPlacements;
    bool loop;

    int fastLoadDataInSpectrogramMemCpy();
    float doMagnitude();
};

#endif // PLOTTER_H
