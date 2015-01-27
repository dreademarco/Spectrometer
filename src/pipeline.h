#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <sys/time.h>

#include "common.h"
#include "stdio.h"
#include "fftwsequencebuffer.h"
#include "ppf.h"

class Pipeline : public QObject
{
    Q_OBJECT
public:
    Pipeline(FFTWSequenceBuffer *outputDataBlock = NULL, int blockSize = 0, int selectedIntegrationFactor = 1, int selectedTaps = 4, int selectedChans=256, int selectedFs = 1024, int selectedBufferSize=1, int selectedPort=10000, int selectedSamplesPerPacket = 64, QObject *parent = 0);
    ~Pipeline();
    //int fastLoadDataInWorkSpaceMemCpy();
    void fastLoadDataToOutputStreamMemCpy();
    void doIntegration();
    void doMagnitude();
    void setupTermination();

public slots:
    void start();

signals:
    void done();

private:        
    PPF *ppf;
    FFTWSequenceBuffer *outputStream;
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
    int tapSamples;
    int numberOfAntennas;
    int port;
    int samplesPerPacket;
    void setupCPU();

    double timestamp, sampRate;
};

#endif // PIPELINE_H
