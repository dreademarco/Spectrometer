#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <sys/time.h>

#include "common.h"
#include "stdio.h"
#include "fftwsequencecircularthreaded.h"
#include "streamingppf.h"

class Pipeline : public QObject
{
    Q_OBJECT
public:
    Pipeline(FFTWSequenceCircularThreaded *sourceDataBlock = NULL, FFTWSequenceCircularThreaded *outputDataBlock = NULL, int blockSize = 0, int selectedIntegrationFactor = 1, int selectedTaps = 4, int selectedChans=256, int selectedFs = 1024, int selectedBufferTobs=1, QObject *parent = 0);
    ~Pipeline();
    int fastLoadDataInWorkSpaceMemCpy();
    void fastLoadDataToOutputStreamMemCpy(int samplesToPush);
    void doIntegration();
    void doMagnitude();

public slots:
    void start();

signals:
    void done();

private:        
    StreamingPPF *ppf;
    FFTWSequenceCircularThreaded *sourceStream;
    FFTWSequenceCircularThreaded *outputStream;
    int blocks;
    FFTWSequence *inputWorkspace;
    FFTWSequence *inputWorkspace2;
    FFTWSequence *outputWorkspace;
    int tobs_buffer;
    int integrationFactor;
    int ntaps;
    int nchans;
    int srate;
    int placements;
    int prevPlacements;
    bool loop;
    int samplesToGather;
    void setupCPU();
};

#endif // PIPELINE_H
