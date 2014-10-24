#include "circulararray2dspectrumthreaded.h"
using namespace std;

template <typename T>
CircularArray2DSpectrumThreaded<T>::CircularArray2DSpectrumThreaded(int channels, int samples) : CircularArray2DSpectrum<T>(channels,samples)
{
    numberUsedSpaces = 0;
    numberFreeSpaces = samples;
}

template <typename T>
CircularArray2DSpectrumThreaded<T>::~CircularArray2DSpectrumThreaded(){
}

template <typename T>
int CircularArray2DSpectrumThreaded<T>::getNumFreeSpaces(){
    return numberFreeSpaces;
}

template <typename T>
int CircularArray2DSpectrumThreaded<T>::getNumUsedSpaces(){
    return numberUsedSpaces;
}

template <typename T>
void CircularArray2DSpectrumThreaded<T>::writeSample(T* sampleData){
    this->pushSample(sampleData);
    mutex.lock();
    numberUsedSpaces+=1;
    numberFreeSpaces-=1;
    mutex.unlock();
}

template <typename T>
T* CircularArray2DSpectrumThreaded<T>::loadSample(){
    mutex.lock();
    numberFreeSpaces+=1;
    numberUsedSpaces-=1;
    mutex.unlock();
    return this->popSample();
}

template <typename T>
void CircularArray2DSpectrumThreaded<T>::fastPopBlockSamples(Array2DSpectrum<T> *outputBlock, int blockSize){
    this->popSpectrumFast(blockSize,outputBlock);
    mutex.lock();
    numberFreeSpaces = numberFreeSpaces + blockSize;
    numberUsedSpaces = numberUsedSpaces - blockSize;
    mutex.unlock();
}


template <typename T>
void CircularArray2DSpectrumThreaded<T>::fastPopBlockSamples(CircularArray2DSpectrum<T> *outputBlock, int blockSize){
    this->popSpectrumFast(blockSize,outputBlock);
    mutex.lock();
    numberFreeSpaces = numberFreeSpaces + blockSize;
    numberUsedSpaces = numberUsedSpaces - blockSize;
    mutex.unlock();
}

template <typename T>
void CircularArray2DSpectrumThreaded<T>::fastPushBlockSamples(Array2DSpectrum<T> *inputBlock, int blockSize){
    this->pushSpectrumFast(blockSize, inputBlock);
    mutex.lock();
    numberUsedSpaces = numberUsedSpaces + blockSize;
    numberFreeSpaces = numberFreeSpaces - blockSize;
    mutex.unlock();
}

template <typename T>
void CircularArray2DSpectrumThreaded<T>::increaseFreeSpaces(){
    mutex.lock();
    numberFreeSpaces = numberFreeSpaces+1;
    numberUsedSpaces = numberUsedSpaces-1;
    this->incrementReadIndex();
    mutex.unlock();
}

template class CircularArray2DSpectrumThreaded<int>;
template class CircularArray2DSpectrumThreaded<float>;
template class CircularArray2DSpectrumThreaded<double>;
