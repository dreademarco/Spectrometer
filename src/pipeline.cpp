#include "pipeline.h"
using namespace std;

Pipeline::Pipeline(QObject *parent, CircularArray2DSpectrumThreaded<float> *sourceDataBlock, CircularArray2DSpectrumThreaded<float> *outputDataBlock, int chunkSize, int integrationFactor) : QThread(parent){
    this->sourceStream = sourceDataBlock;
    this->outputStream = outputDataBlock;
    this->chunkSize = chunkSize;
    this->inputWorkspace = new Array2DSpectrum<float>(sourceDataBlock->getChannelCount(),chunkSize);
    this->outputWorkspace = new Array2DSpectrum<float>(sourceDataBlock->getChannelCount(),chunkSize/integrationFactor);
    this->integrationFactor = integrationFactor;
    this->placements = 0;
    this->loop = true;
}

Pipeline::~Pipeline()
{
    delete inputWorkspace;
    delete outputWorkspace;
}

void Pipeline::run()
{
    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    while(loop){
        // Step (1): Copy next available block to inputWorkspace
        fastLoadDataInWorkSpaceMemCpy();

        // Step (2): Perform Integration
        doIntegration();

        // Step (3): Output results
        fastLoadDataToOutputStreamMemCpy();

        if(placements==samplesSize/integrationFactor){
            loop=false;
        }
    }

    gettimeofday(&t2, NULL);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << "Pipeline elasped time: " << elapsedTime << " ms" << endl;
    float totalSeconds = elapsedTime/1000.0;
    if(totalSeconds>0){
        float datarate = (((samplesSize*freqBins)/totalSeconds)/1000000);
        cout << "Pipeline Data rate est.: " << datarate << " Mhz" << endl;
    }else{
        cout << "Pipeline Data rate est.: n/a"  << " Mhz" << endl;
    }
}

void Pipeline::fastLoadDataInWorkSpaceMemCpy(){
    bool copyMade = false; //do not return from this function until a copy has been made to memory
    while(!copyMade){
        if(sourceStream->getNumUsedSpaces()>=chunkSize){
            sourceStream->fastPopBlockSamples(inputWorkspace,chunkSize);
            copyMade = true;
            placements = placements + (chunkSize/integrationFactor);
        }
    }
}

void Pipeline::doIntegration(){
    inputWorkspace->integration(integrationFactor,outputWorkspace);
//    int outputChannelCounter = 0;
//    int outputSampleCounter = 0;
//    float integration_mean;
//    for (int i = 0; i < inputWorkspace->getChannelCount(); ++i) {
//        for (int j = 0; j < (inputWorkspace->getSampleCount() / integrationFactor); ++j) {
//            integration_mean = 0;
//            for (int k = 0; k < integrationFactor; ++k) {
//                integration_mean = integration_mean + inputWorkspace->at(i,(j*integrationFactor)+k);
//            }
//            integration_mean = integration_mean/integrationFactor;
//            outputWorkspace->set(outputChannelCounter,outputSampleCounter,integration_mean);
//            outputSampleCounter = outputSampleCounter+1;
//        }
//        outputSampleCounter = 0;
//        outputChannelCounter = outputChannelCounter+1;
//    }
}

void Pipeline::fastLoadDataToOutputStreamMemCpy(){
    outputStream->fastPushBlockSamples(outputWorkspace,chunkSize/integrationFactor);
}
