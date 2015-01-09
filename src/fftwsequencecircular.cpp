#include "fftwsequencecircular.h"
using namespace std;

FFTWSequenceCircular::FFTWSequenceCircular(int channels, int samples) : FFTWSequence(channels,samples)
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = this->getSampleCount()-1;
    refillCounter = 0;
}

FFTWSequenceCircular::~FFTWSequenceCircular()
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = this->getSampleCount()-1;
    refillCounter = 0;
}

int FFTWSequenceCircular::getRefillCount(){
    return refillCounter;
}

void FFTWSequenceCircular::incrementReadIndex(int length){
    currentSampleReadIdx = (currentSampleReadIdx+length) % (this->getSampleCount()-1);
//    if(currentSampleReadIdx>=this->getSampleCount()-1){
//        currentSampleReadIdx = 0;
//    }else{
//        ++currentSampleReadIdx;
//    }
}

void FFTWSequenceCircular::incrementWriteIndex(int length){
    currentSampleWriteIdx = (currentSampleWriteIdx+length) % (this->getSampleCount());
//    if(currentSampleWriteIdx>=this->getSampleCount()-1){
//        currentSampleWriteIdx = 0;
//        ++refillCounter; //indicator that we have filled the array and are going back to start for a refill
//    }else{
//        ++currentSampleWriteIdx;
//    }
}

void FFTWSequenceCircular::incrementReadIndex(){
    if(currentSampleReadIdx>=this->getSampleCount()-1){
        currentSampleReadIdx = 0;
    }else{
        ++currentSampleReadIdx;
    }
}

void FFTWSequenceCircular::decrementReadIndex(){
    if(currentSampleReadIdx>0){
        --currentSampleReadIdx;
    }else{
        currentSampleReadIdx = this->getSampleCount()-1;
    }
}

void FFTWSequenceCircular::incrementWriteIndex(){
    if(currentSampleWriteIdx>=this->getSampleCount()-1){
        currentSampleWriteIdx = 0;
        ++refillCounter; //indicator that we have filled the array and are going back to start for a refill
    }else{
        ++currentSampleWriteIdx;
    }
}

void FFTWSequenceCircular::decrementWriteIndex(){
    if(currentSampleWriteIdx>0){
        --currentSampleWriteIdx;
    }else{
        currentSampleWriteIdx = this->getSampleCount()-1;
    }
}

int FFTWSequenceCircular::getCurrentSampleWriteIndex()
{
    return currentSampleWriteIdx; //last filled sample;
}

void FFTWSequenceCircular::setCurrentSampleWriteIndex(int idx){
     currentSampleWriteIdx = idx;
}

int FFTWSequenceCircular::getCurrentSampleReadIndex()
{
    return currentSampleReadIdx;
}

void FFTWSequenceCircular::setCurrentSampleReadIndex(int idx){
     currentSampleReadIdx = idx;
}

void FFTWSequenceCircular::pushSpectrum(FFTWSequence spectrumData)
{
    for (int i = 0; i < spectrumData.getSampleCount(); ++i) {
        this->pushSample(spectrumData.getChannelsForSample(i));
    }
}

void FFTWSequenceCircular::pushSample(fftwf_complex* sampleData)
{
    this->setSample(currentSampleWriteIdx,sampleData);
    this->incrementWriteIndex();
}

fftwf_complex* FFTWSequenceCircular::popSample()
{
    this->incrementReadIndex();
    return this->getChannelsForSample(currentSampleReadIdx);
}

void FFTWSequenceCircular::popSpectrum(int length, FFTWSequence *outputSection)
{
    for (int i = 0; i < length; ++i) {
        outputSection->setSample(i,this->popSample());
    }
}

void FFTWSequenceCircular::popSpectrumFast(int length, FFTWSequence *outputSection){
    int cellsToCopy = this->getChannelCount();
    this->incrementReadIndex();
    int cellsReadIdx = cellsToCopy*getCurrentSampleReadIndex();
    memcpy(outputSection->data[0],data+cellsReadIdx,length*cellsToCopy*sizeof(fftwf_complex));
    this->incrementReadIndex(length-1);
}

//void FFTWSequenceCircular::popSpectrumFast(int length, FFTWSequence *outputSection){
//    int cellsToCopy = this->getChannelCount();
//    int cellsReadIdx = 0;
//    int cellsWriteIdx = 0;
//    for (int i = 0; i < length; ++i) {
//        this->incrementReadIndex();
//        cellsReadIdx = (cellsToCopy*getCurrentSampleReadIndex());
//        memcpy(&outputSection->data[cellsWriteIdx],&this->data[cellsReadIdx],cellsToCopy*sizeof(fftwf_complex));
//        cellsWriteIdx = cellsWriteIdx+cellsToCopy;
//    }
//}

void FFTWSequenceCircular::popSpectrumFast(int length, FFTWSequenceCircular *outputSection){
    int cellsToCopy = this->getChannelCount();
    int destinationWrittenCells = 0;
    int sourceReadCells = 0;
    for (int i = 0; i < length; ++i) {
        this->incrementReadIndex();
        destinationWrittenCells = cellsToCopy*outputSection->getCurrentSampleWriteIndex();
        sourceReadCells = cellsToCopy*getCurrentSampleReadIndex();
        memcpy(&outputSection->data[destinationWrittenCells],&this->data[sourceReadCells],cellsToCopy*sizeof(fftwf_complex));
        outputSection->incrementWriteIndex();
    }
}

void FFTWSequenceCircular::pushSpectrumFast(int length, FFTWSequence *inputSection){
    int cellsToCopy = inputSection->getChannelCount();
    int readLoc = 0;
    int toWrite = length;

    while(toWrite>0){
        int cellWriteLoc = this->getCurrentSampleWriteIndex();
        int cellsWriteIdx = cellWriteLoc*cellsToCopy;
        int spaceAvailable = getSampleCount() - cellWriteLoc;
        if(toWrite<=spaceAvailable)
        {
            memcpy(data[cellsWriteIdx],inputSection->data[readLoc],toWrite*cellsToCopy*sizeof(fftwf_complex));
            incrementWriteIndex(toWrite);
            toWrite -= toWrite;
            readLoc += toWrite;
        }else{
            memcpy(data[cellsWriteIdx],inputSection->data[readLoc],spaceAvailable*cellsToCopy*sizeof(fftwf_complex));
            incrementWriteIndex(spaceAvailable);
            toWrite -= spaceAvailable;
            readLoc += spaceAvailable;
        }
    }
}

//void FFTWSequenceCircular::pushSpectrumFast(int length, FFTWSequence *inputSection){
//    int cellsToCopy = inputSection->getChannelCount();
//    int cellsReadIdx = 0;
//    int cellsWriteIdx = 0;
//    for (int i = 0; i < length; ++i) {
//        cellsWriteIdx = this->getCurrentSampleWriteIndex()*cellsToCopy;
//        cellsReadIdx = cellsToCopy*i;
//        memcpy(&this->data[cellsWriteIdx],&inputSection->data[cellsReadIdx],cellsToCopy*sizeof(fftwf_complex));
//        this->incrementWriteIndex();
//    }
//}
