#include "plotter.h"
using namespace std;

Plotter::Plotter(FFTWSequenceBuffer *sourceDataBlock, SpectrogramPlot *spectrogramPlot, int selectedSpectSize, int selectedChans, QObject *parent) : QObject(parent)
{    
    this->nchans = selectedChans;
    this->spectSize = selectedSpectSize;
    this->guiUpdateSize = spectSize;
    this->sourceStream = sourceDataBlock;
    this->spectrogramPlot = spectrogramPlot;
    this->tempSamples = new FFTWSequence(nchans,spectSize);
    this->spectrogramData = new SpectrogramData(tempSamples,nchans,spectSize);
    this->loop = true;
    this->highIntensity = 0;
}

Plotter::~Plotter()
{
    delete spectrogramPlot;
    delete spectrogramData;
    delete tempSamples;
    loop = false;
}

void Plotter::setupTermination(){
    loop=false;
}

void Plotter::start()
{
    while(loop){
        loadDataInSpectrogram();

        float highestValue = doMagnitude();
        spectrogramPlot->pushNewData(spectrogramData);
        if(highestValue > highIntensity){
            highIntensity = highestValue;
            spectrogramData->setInterval(Qt::ZAxis, QwtInterval(0.0,highIntensity));
            spectrogramPlot->setupAxis();
        }
        emit readyForPlot();
    }
    emit done();
}

void Plotter::loadDataInSpectrogram(){
    sourceStream->popSection(tempSamples);
}

float Plotter::doMagnitude(){
    return spectrogramData->dataArray->magnitude();
}
