#include "fftwsequencecircularthreaded.h"
using namespace std;

FFTWSequenceCircularThreaded::FFTWSequenceCircularThreaded(int channels, int samples) : FFTWSequenceCircular(channels, samples)
{
    numberUsedSpaces = 0;
    numberFreeSpaces = samples;
}

FFTWSequenceCircularThreaded::~FFTWSequenceCircularThreaded(){
}

int FFTWSequenceCircularThreaded::getNumFreeSpaces(){
    return numberFreeSpaces;
}

int FFTWSequenceCircularThreaded::getNumUsedSpaces(){
    return numberUsedSpaces;
}

void FFTWSequenceCircularThreaded::writeSample(fftwf_complex* sampleData){
    this->pushSample(sampleData);
    mutex.lock();
    numberUsedSpaces+=1;
    numberFreeSpaces-=1;
    mutex.unlock();
}

fftwf_complex* FFTWSequenceCircularThreaded::loadSample(){
    mutex.lock();
    numberFreeSpaces+=1;
    numberUsedSpaces-=1;
    mutex.unlock();
    return this->popSample();
}

void FFTWSequenceCircularThreaded::fastPopBlockSamples(FFTWSequence *outputBlock, int blockSize){
    this->popSpectrumFast(blockSize,outputBlock);
    mutex.lock();    
    numberFreeSpaces = numberFreeSpaces + blockSize;
    numberUsedSpaces = numberUsedSpaces - blockSize;
    mutex.unlock();
}

void FFTWSequenceCircularThreaded::fastPopBlockSamples(FFTWSequenceCircular *outputBlock, int blockSize){
    this->popSpectrumFast(blockSize,outputBlock);
    mutex.lock();
    numberFreeSpaces = numberFreeSpaces + blockSize;
    numberUsedSpaces = numberUsedSpaces - blockSize;
    mutex.unlock();
}

void FFTWSequenceCircularThreaded::fastPushBlockSamples(FFTWSequence *inputBlock, int blockSize){
    this->pushSpectrumFast(blockSize, inputBlock);
    mutex.lock();
    numberUsedSpaces = numberUsedSpaces + blockSize;
    numberFreeSpaces = numberFreeSpaces - blockSize;
    mutex.unlock();
}

void FFTWSequenceCircularThreaded::increaseFreeSpaces(){
    mutex.lock();
    numberFreeSpaces = numberFreeSpaces+1;
    numberUsedSpaces = numberUsedSpaces-1;
    this->incrementReadIndex();
    mutex.unlock();
}

void FFTWSequenceCircularThreaded::increaseUsedSpaces(){
    mutex.lock();
    numberUsedSpaces = numberUsedSpaces+1;
    numberFreeSpaces = numberFreeSpaces-1;
    this->incrementWriteIndex();
    mutex.unlock();
}


