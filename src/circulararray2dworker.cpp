#include "circulararray2dworker.h"
using namespace std;

template <typename T>
CircularArray2DWorker<T>::CircularArray2DWorker() : Array2DWorker<T>()
{
    currentColumnWriteIdx = 0;
    currentColumnReadIdx = 0;
    refillCounter = 0;
}

template <typename T>
CircularArray2DWorker<T>::CircularArray2DWorker(int rows, int columns) : Array2DWorker<T>(rows,columns)
{
    currentColumnWriteIdx = 0;
    currentColumnReadIdx = columns;
    refillCounter = 0;
}

template <typename T>
CircularArray2DWorker<T>::~CircularArray2DWorker()
{
    currentColumnWriteIdx = 0;
    currentColumnReadIdx = 0;
    refillCounter = 0;
}

template <typename T>
int CircularArray2DWorker<T>::getRefillCount(){
    return refillCounter;
}

template <typename T>
void CircularArray2DWorker<T>::incrementColumnReadIndex(){
    if(currentColumnReadIdx>=this->getColumnCount()-1){
        currentColumnReadIdx = 0;
    }else{
        ++currentColumnReadIdx;
    }
}
template <typename T>
void CircularArray2DWorker<T>::decrementColumnReadIndex(){
    if(currentColumnReadIdx>0){
        --currentColumnReadIdx;
    }else{
        currentColumnReadIdx = this->getColumnCount()-1;
    }
}
template <typename T>
void CircularArray2DWorker<T>::incrementColumnWriteIndex(){
    if(currentColumnWriteIdx>=this->getColumnCount()-1){
        currentColumnWriteIdx = 0;
    }else{
        ++currentColumnWriteIdx;
    }
}
template <typename T>
void CircularArray2DWorker<T>::decrementColumnWriteIndex(){
    if(currentColumnWriteIdx>0){
        --currentColumnWriteIdx;
    }else{
        currentColumnWriteIdx = this->getColumnCount()-1;
    }
}

template <typename T>
int CircularArray2DWorker<T>::getCurrentColumnWriteIndex()
{
    return currentColumnWriteIdx; //last filled column;
}

template <typename T>
void CircularArray2DWorker<T>::setCurrentColumnWriteIndex(int idx){
     currentColumnWriteIdx = idx;
}

template <typename T>
int CircularArray2DWorker<T>::getCurrentColumnReadIndex()
{
    return currentColumnReadIdx;
}

template <typename T>
void CircularArray2DWorker<T>::setCurrentColumnReadIndex(int idx){
     currentColumnReadIdx = idx;
}

template <typename T>
void CircularArray2DWorker<T>::pushArray(Array2DWorker<T> columnData)
{
    for (int i = 0; i < columnData.getColumnCount(); ++i) {
        this->pushColumn(columnData.getColumn(i));
    }
}

template <typename T>
void CircularArray2DWorker<T>::pushColumn(T* columnData)
{
    this->setColumn(currentColumnWriteIdx,columnData);
    if(currentColumnWriteIdx>=this->getColumnCount()-1){
        currentColumnWriteIdx = 0;
        ++refillCounter; //indicator that we have filled the array and are going back to start for a refill
    }else if(currentColumnWriteIdx<this->getColumnCount()){
        currentColumnWriteIdx = currentColumnWriteIdx+1;
    }
}

template <typename T>
T* CircularArray2DWorker<T>::popColumn()
{
    if(currentColumnReadIdx>=this->getColumnCount()-1){
        currentColumnReadIdx = 0;
    }else if(currentColumnReadIdx<this->getColumnCount()){
        currentColumnReadIdx = currentColumnReadIdx+1;
    }
    return this->getColumn(currentColumnReadIdx);
}

template <typename T>
void CircularArray2DWorker<T>::popColumnSection(int length, Array2DWorker<T> *tempWorker)
{
    for (int i = 0; i < length; ++i) {
        tempWorker->setColumn(i,this->popColumn());
    }
}

template <typename T>
void CircularArray2DWorker<T>::toString(){
    for (int i = 0; i < this->getRowCount(); ++i) {
        for (int j = 0; j < this->getColumnCount(); ++j) {
            cout << this->at(i,j) << "\t";
        }
        cout << endl;
    }
}

template class CircularArray2DWorker<int>;
template class CircularArray2DWorker<float>;
template class CircularArray2DWorker<double>;
