#ifndef PLOTTER_H
#define PLOTTER_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <common.h>
#include <stdio.h>
#include <sys/time.h>

#include "fftwsequencebuffer.h"
#include "spectrogramplot.h"

class Plotter : public QObject
{
    Q_OBJECT
public:
    //Plotter(FFTWSequenceCircularThreaded *sourceDataBlock = NULL, SpectrogramPlot *spectrogramPlot = NULL, int selectedSpectSize = 64, int selectedChans = 256, int selectedFs = 1024, int selectedIntegFactor = 1, QObject *parent = 0);
    Plotter(FFTWSequenceBuffer *sourceDataBlock = NULL, SpectrogramPlot *spectrogramPlot = NULL, int selectedSpectSize = 64, int selectedChans = 256, QObject *parent = 0);
    ~Plotter();
    bool flowFinished;

public slots:
    void start();
    void setupTermination();
    void halt();
    void resume();

signals:
    void readyForPlot();
    void done();

private:
    FFTWSequenceBuffer *sourceStream;
    int highIntensity;
    int integrationfactor;    
    int spectSize;
    int nchans;
    int guiUpdateSize; // samples to process before GUI progress update
    SpectrogramData *spectrogramData;
    SpectrogramPlot *spectrogramPlot;
    FFTWSequence *tempSamples;
    bool loop;
    bool pause;

    void loadDataInSpectrogram();
    //int fastLoadDataInSpectrogramMemCpy();
    float doMagnitude();
};

#endif // PLOTTER_H
