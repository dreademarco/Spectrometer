#include "circulararray2dworkerthreaded.h"
using namespace std;

template <typename T>
CircularArray2DWorkerThreaded<T>::CircularArray2DWorkerThreaded(int rows, int columns) : CircularArray2DWorker<T>(rows,columns)
{
    numberUsedSpaces = 0;
    numberFreeSpaces = columns;
}

template <typename T>
CircularArray2DWorkerThreaded<T>::~CircularArray2DWorkerThreaded(){
}

template <typename T>
int CircularArray2DWorkerThreaded<T>::getNumFreeSpaces(){
    return numberFreeSpaces;
}

template <typename T>
int CircularArray2DWorkerThreaded<T>::getNumUsedSpaces(){
    return numberUsedSpaces;
}

template <typename T>
void CircularArray2DWorkerThreaded<T>::fastPushColumn(T* columnData){
    this->pushColumn(columnData);
    numberUsedSpaces+=1;
}

template <typename T>
void CircularArray2DWorkerThreaded<T>::fastPopBlock(Array2DWorker<T> *dataBlock, int blockSize){
    this->popColumnSection(blockSize,dataBlock);
//    for (int i = 0; i < blockSize; ++i) {
//        dataBlock->setColumn(i,this->popColumn());
//    }
}

template class CircularArray2DWorkerThreaded<int>;
template class CircularArray2DWorkerThreaded<float>;
template class CircularArray2DWorkerThreaded<double>;
