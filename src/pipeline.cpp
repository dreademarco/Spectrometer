#include "pipeline.h"
using namespace std;

//Pipeline::Pipeline(CircularArray2DSpectrumThreaded<float> *sourceDataBlock, CircularArray2DSpectrumThreaded<float> *outputDataBlock, int chunkSize, int integrationFactor) : QThread(parent)
Pipeline::Pipeline(CircularArray2DSpectrumThreaded<float> *sourceDataBlock, CircularArray2DSpectrumThreaded<float> *outputDataBlock, int chunkSize, int integrationFactor, QObject *parent) : QObject(parent){
    this->sourceStream = sourceDataBlock;
    this->outputStream = outputDataBlock;
    this->chunkSize = chunkSize;
    this->integrationFactor = integrationFactor;
    this->placements = 0;
    this->loop = true;
    this->samplesToGather = chunkSize*integrationfactor;
    this->inputWorkspace = new Array2DSpectrum<float>(sourceDataBlock->getChannelCount(),samplesToGather);
    this->outputWorkspace = new Array2DSpectrum<float>(sourceDataBlock->getChannelCount(),samplesToGather/integrationFactor);
}

Pipeline::~Pipeline()
{
    delete inputWorkspace;
    delete outputWorkspace;
}

void Pipeline::start()
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
    if(elapsedTime>0){
        float datarate = ((((samplesSize*freqBins)/elapsedTime)/1000000))*1000.0;
        cout << "Pipeline Data rate est.: " << datarate << " Mhz" << endl;
    }else{
        cout << "Pipeline Data rate est.: n/a"  << " Mhz" << endl;
    }


//    float totalSeconds = elapsedTime/1000.0;

//    if(totalSeconds>0){
//        float datarate = (((samplesSize*freqBins)/totalSeconds)/1000000);
//        cout << "Pipeline Data rate est.: " << datarate << " Mhz" << endl;
//    }else{
//        cout << "Pipeline Data rate est.: n/a"  << " Mhz" << endl;
//    }
}

void Pipeline::fastLoadDataInWorkSpaceMemCpy(){
    bool copyMade = false; //do not return from this function until a copy has been made to memory
    while(!copyMade){
        if(sourceStream->getNumUsedSpaces()>=samplesToGather){
            sourceStream->fastPopBlockSamples(inputWorkspace,samplesToGather);
            copyMade = true;
            placements = placements + (samplesToGather/integrationFactor);
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
    outputStream->fastPushBlockSamples(outputWorkspace,samplesToGather/integrationFactor);
}
