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
    numberUsedSpaces+=1;
}

template <typename T>
void CircularArray2DSpectrumThreaded<T>::fastPopBlockSamples(Array2DSpectrum<T> *outputBlock, int blockSize){
    this->popSampleSectionFast(blockSize,outputBlock);
    numberFreeSpaces = numberFreeSpaces + blockSize;
}

template <typename T>
void CircularArray2DSpectrumThreaded<T>::increaseFreeSpaces(){
    numberFreeSpaces = numberFreeSpaces+1;
    numberUsedSpaces = numberUsedSpaces-1;
    this->incrementReadIndex();
}

template class CircularArray2DSpectrumThreaded<int>;
template class CircularArray2DSpectrumThreaded<float>;
template class CircularArray2DSpectrumThreaded<double>;
