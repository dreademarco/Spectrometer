#include "plotter.h"
using namespace std;


//QObject *parent,
Plotter::Plotter(FFTWSequenceCircularThreaded *sourceDataBlock, SpectrogramPlot *spectrogramPlot, int selectedSpectSize, int selectedChans, int selectedFs, int selectedIntegFactor, QObject *parent) : QObject(parent)
{    
    this->integrationfactor = selectedIntegFactor;
    this->srate = selectedFs;
    this->nchans = selectedChans;
    this->spectSize = selectedSpectSize;
    this->guiUpdateSize = spectSize;
    this->samplesprocessed = spectSize;
    this->sourceStream = sourceDataBlock;
    this->samplesToProcess = samplesprocessed;
    this->spectrogramPlot = spectrogramPlot;
    this->tempSamples = new FFTWSequenceCircular(nchans,spectSize);
    this->spectrogramData = new SpectrogramData(tempSamples,nchans,spectSize);
    this->placements = 0;
    this->loop = true;
    this->prevPlacements=-1;
}

Plotter::~Plotter()
{
    samplesToProcess = samplesprocessed;
    delete spectrogramPlot;
    //delete spectrogramData;
    delete tempSamples;
    placements=0;
    prevPlacements=-1;
    loop = false;
}

void Plotter::start()
{
    //omp_set_num_threads(1);

    int nsamp = ((tobs * srate)/nchans)/integrationfactor;

    while(loop){
        // Step (1): Copy next available block to spectrogram data
        int processed = fastLoadDataInSpectrogramMemCpy();

        prevPlacements = placements;
        placements = placements + processed;

        // Step (2): Check if enough data has been put into spectogramData and signal if ready
        if(prevPlacements!=placements){ //something was actually pulled from buffer
            //if(placements % guiUpdateSize == 0){
                doMagnitude();
                spectrogramPlot->pushNewData(spectrogramData);
                emit readyForPlot();
            //}
        }

        if(placements>=nsamp){
            loop=false;
        }
    }
    emit done();
}

int Plotter::fastLoadDataInSpectrogramMemCpy(){
    bool copyMade = false; //do not return from this function until a copy has been made to memory    
    while(!copyMade){
        int currentUsed = sourceStream->getNumUsedSpaces();
        if(currentUsed>=samplesToProcess){
            spectrogramData->fastAddDataSectionMemCpy(sourceStream, samplesToProcess);
            copyMade = true;
            return samplesToProcess;
        }else if(currentUsed<samplesToProcess){
            spectrogramData->fastAddDataSectionMemCpy(sourceStream, currentUsed);
            copyMade = true;
            return currentUsed;
        }
    }
}

void Plotter::doMagnitude(){
    spectrogramData->dataArray->magnitude();
}
