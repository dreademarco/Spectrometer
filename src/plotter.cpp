#include "plotter.h"
using namespace std;

Plotter::Plotter(QObject *parent, CircularArray2DSpectrumThreaded<float> *sourceDataBlock, int chunkSize, int integrationFactor, SpectrogramPlot *spectrogramPlot) : QThread(parent)
{
    this->sourceStream = sourceDataBlock;
    this->chunkSize = chunkSize;
    this->integrationFactor = integrationFactor;
    this->spectrogramPlot = spectrogramPlot;
    this->tempSamples = new CircularArray2DSpectrum<float>(freqBins,spectSize);
    this->spectrogramData = new SpectrogramData<float>(tempSamples,freqBins,spectSize);
    this->placements = 0;
    this->loop = true;
}

void Plotter::run()
{
    timeval t1, t2;
    double elapsedTime;
    // start timer
    gettimeofday(&t1, NULL);

    while(loop){
        // Step (1): Copy next available block to spectrogram data
        fastLoadDataInSpectrogramMemCpy();

        // Step (2): Check if enough data has been put into spectogramData and signal if ready
        if(placements % guiUpdateSize == 0){
            spectrogramPlot->pushNewData(spectrogramData);            
            emit readyForPlot();
        }
        if(placements==samplesSize/integrationFactor){
            loop=false;
        }
    }
    // stop timer
    gettimeofday(&t2, NULL);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    // print the elapsed time in millisec
    cout << "Plotter elasped time: " << elapsedTime << " ms" << endl;
    float totalSeconds = elapsedTime/1000.0;
    if(totalSeconds>0){
        float datarate = (((samplesSize*freqBins)/totalSeconds)/1000000);
        cout << "Plotter Data rate est.: " << datarate << " Mhz" << endl;
    }else{
        cout << "Plotter Data rate est.: n/a"  << " Mhz" << endl;
    }
}

void Plotter::fastLoadDataInSpectrogramMemCpy(){
    bool copyMade = false; //do not return from this function until a copy has been made to memory
    while(!copyMade){
        if(sourceStream->getNumUsedSpaces()>=(chunkSize/integrationFactor)){
            spectrogramData->fastAddDataSectionMemCpy(sourceStream, chunkSize/integrationFactor);
            placements = placements + (chunkSize/integrationFactor);
            copyMade = true;
        }
    }
}
