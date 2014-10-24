#ifndef PIPELINE_H
#define PIPELINE_H

#include <QThread>
#include <QTime>
#include <QVector>
#include <sys/time.h>

#include "common.h"
#include "stdio.h"
#include "circulararray2dspectrumthreaded.h"

class Pipeline : public QThread
{
    Q_OBJECT
public:
    Pipeline(QObject *parent = 0, CircularArray2DSpectrumThreaded<float> *sourceDataBlock = NULL, CircularArray2DSpectrumThreaded<float> *outputDataBlock = NULL, int chunkSize = 0, int integrationFactor = 1);
    ~Pipeline();
    void run();

    void fastLoadDataInWorkSpaceMemCpy();
    void fastLoadDataToOutputStreamMemCpy();
    void doIntegration();

private:
    CircularArray2DSpectrumThreaded<float> *sourceStream;
    CircularArray2DSpectrumThreaded<float> *outputStream;
    int chunkSize;
    Array2DSpectrum<float> *inputWorkspace;
    Array2DSpectrum<float> *outputWorkspace;
    int integrationFactor;
    int placements;
    bool loop;
};

#endif // PIPELINE_H
