#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <sys/time.h>

#include "common.h"
#include "stdio.h"
#include "circulararray2dspectrumthreaded.h"

class Pipeline : public QObject
{
    Q_OBJECT
public:
    Pipeline(CircularArray2DSpectrumThreaded<float> *sourceDataBlock = NULL, CircularArray2DSpectrumThreaded<float> *outputDataBlock = NULL, int chunkSize = 0, int integrationFactor = 1, QObject *parent = 0);
    ~Pipeline();
    void fastLoadDataInWorkSpaceMemCpy();
    void fastLoadDataToOutputStreamMemCpy();
    void doIntegration();

public slots:
    void start();



private:
    CircularArray2DSpectrumThreaded<float> *sourceStream;
    CircularArray2DSpectrumThreaded<float> *outputStream;
    int chunkSize;
    Array2DSpectrum<float> *inputWorkspace;
    Array2DSpectrum<float> *outputWorkspace;
    int integrationFactor;
    int placements;
    bool loop;
    int samplesToGather;
};

#endif // PIPELINE_H
