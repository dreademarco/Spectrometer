#include "circulararray2dspectrum.h"
using namespace std;

template <typename T>
CircularArray2DSpectrum<T>::CircularArray2DSpectrum() : Array2DSpectrum<T>()
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = this->getSampleCount()-1;
    refillCounter = 0;
}

template <typename T>
CircularArray2DSpectrum<T>::CircularArray2DSpectrum(int channels, int samples) : Array2DSpectrum<T>(channels,samples)
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = this->getSampleCount()-1;
    refillCounter = 0;
}

template <typename T>
CircularArray2DSpectrum<T>::~CircularArray2DSpectrum()
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = this->getSampleCount()-1;
    refillCounter = 0;
}

template <typename T>
int CircularArray2DSpectrum<T>::getRefillCount(){
    return refillCounter;
}

template <typename T>
void CircularArray2DSpectrum<T>::incrementReadIndex(){
    if(currentSampleReadIdx>=this->getSampleCount()-1){
        currentSampleReadIdx = 0;
    }else{
        ++currentSampleReadIdx;
    }
}
template <typename T>
void CircularArray2DSpectrum<T>::decrementReadIndex(){
    if(currentSampleReadIdx>0){
        --currentSampleReadIdx;
    }else{
        currentSampleReadIdx = this->getSampleCount()-1;
    }
}
template <typename T>
void CircularArray2DSpectrum<T>::incrementWriteIndex(){
    if(currentSampleWriteIdx>=this->getSampleCount()-1){
        currentSampleWriteIdx = 0;
        ++refillCounter; //indicator that we have filled the array and are going back to start for a refill
    }else{
        ++currentSampleWriteIdx;
    }
}
template <typename T>
void CircularArray2DSpectrum<T>::decrementWriteIndex(){
    if(currentSampleWriteIdx>0){
        --currentSampleWriteIdx;
    }else{
        currentSampleWriteIdx = this->getSampleCount()-1;
    }
}

template <typename T>
int CircularArray2DSpectrum<T>::getCurrentSampleWriteIndex()
{
    return currentSampleWriteIdx; //last filled sample;
}
template <typename T>
void CircularArray2DSpectrum<T>::setCurrentSampleWriteIndex(int idx){
     currentSampleWriteIdx = idx;
}
template <typename T>
int CircularArray2DSpectrum<T>::getCurrentSampleReadIndex()
{
    return currentSampleReadIdx;
}
template <typename T>
void CircularArray2DSpectrum<T>::setCurrentSampleReadIndex(int idx){
     currentSampleReadIdx = idx;
}

template <typename T>
void CircularArray2DSpectrum<T>::pushSpectrum(Array2DSpectrum<T> spectrumData)
{
    for (int i = 0; i < spectrumData.getSampleCount(); ++i) {
        this->pushSample(spectrumData.getChannelsForSample(i));
    }
}

template <typename T>
void CircularArray2DSpectrum<T>::pushSample(T* sampleData)
{
    this->setSample(currentSampleWriteIdx,sampleData);
    this->incrementWriteIndex();
}

template <typename T>
T* CircularArray2DSpectrum<T>::popSample()
{
    this->incrementReadIndex();
    return this->getChannelsForSample(currentSampleReadIdx);
}

/*
 * Pops section from current reading index
 */
template <typename T>
void CircularArray2DSpectrum<T>::popSpectrum(int length, Array2DSpectrum<T> *outputSection)
{
    for (int i = 0; i < length; ++i) {
        outputSection->setSample(i,this->popSample());
    }
}

template <typename T>
void CircularArray2DSpectrum<T>::popSpectrumFast(int length, Array2DSpectrum<T> *outputSection){
    //Array2DSpectrum<T> samplesSection(channels,n_samples);
    int cellsToCopy = this->getChannelCount();
    int cellsReadIdx = 0;
    int cellsWriteIdx = 0;
    for (int i = 0; i < length; ++i) {
        this->incrementReadIndex();
        cellsReadIdx = (cellsToCopy*getCurrentSampleReadIndex());
        memcpy(&outputSection->data[cellsWriteIdx],&this->data[cellsReadIdx],cellsToCopy*sizeof(T));
        cellsWriteIdx = cellsWriteIdx+cellsToCopy;
        //this->incrementReadIndex();
    }
//    int cellsToCopy = channels*length;
//    int startReadIndx = this->getCurrentSampleReadIndex();
//    int cellsReadIdx = (channels*startReadIndx)+1;
//    memcpy(&output->data[0],&this->data[cellsReadIdx],cellsToCopy*sizeof(T));
}

template <typename T>
void CircularArray2DSpectrum<T>::popSpectrumFast(int length, CircularArray2DSpectrum<T> *outputSection){
    int cellsToCopy = this->getChannelCount();
    int destinationWrittenCells = 0;
    int sourceReadCells = 0;
    for (int i = 0; i < length; ++i) {
        this->incrementReadIndex();
        destinationWrittenCells = cellsToCopy*outputSection->getCurrentSampleWriteIndex();
        sourceReadCells = cellsToCopy*getCurrentSampleReadIndex();        
        memcpy(&outputSection->data[destinationWrittenCells],&this->data[sourceReadCells],cellsToCopy*sizeof(T));
        outputSection->incrementWriteIndex();
    }
}

template <typename T>
void CircularArray2DSpectrum<T>::pushSpectrumFast(int length, Array2DSpectrum<T> *inputSection){
    int cellsToCopy = inputSection->getChannelCount();
    int cellsReadIdx = 0;
    int cellsWriteIdx = 0;
    for (int i = 0; i < length; ++i) {
        cellsWriteIdx = this->getCurrentSampleWriteIndex()*cellsToCopy;
        cellsReadIdx = cellsToCopy*i;
        memcpy(&this->data[cellsWriteIdx],&inputSection->data[cellsReadIdx],cellsToCopy*sizeof(T));
        this->incrementWriteIndex();
    }
}


template class CircularArray2DSpectrum<int>;
template class CircularArray2DSpectrum<float>;
template class CircularArray2DSpectrum<double>;
