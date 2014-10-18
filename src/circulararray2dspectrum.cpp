#include "circulararray2dspectrum.h"
using namespace std;

template <typename T>
CircularArray2DSpectrum<T>::CircularArray2DSpectrum() : Array2DSpectrum<T>()
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = 0;
    refillCounter = 0;
}

template <typename T>
CircularArray2DSpectrum<T>::CircularArray2DSpectrum(int channels, int samples) : Array2DSpectrum<T>(channels,samples)
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = 0;
    refillCounter = 0;
}

template <typename T>
CircularArray2DSpectrum<T>::~CircularArray2DSpectrum()
{
    currentSampleWriteIdx = 0;
    currentSampleReadIdx = 0;
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
void CircularArray2DSpectrum<T>::popSampleSection(int length, Array2DSpectrum<T> *outputSection)
{
    for (int i = 0; i < length; ++i) {
        outputSection->setSample(i,this->popSample());
    }
}


template class CircularArray2DSpectrum<int>;
template class CircularArray2DSpectrum<float>;
template class CircularArray2DSpectrum<double>;
