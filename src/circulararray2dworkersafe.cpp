#include "circulararray2dworkersafe.h"
using namespace std;

QSemaphore freeSpace(samplesSize);
QSemaphore usedSpace(0);

//template <typename T>
//CircularArray2DWorkerSafe<T>::CircularArray2DWorkerSafe(priority selected_priority, bool safety) : CircularArray2DWorker<T>(){
//    m_priority = selected_priority;
//    numberUsedSpaces = 0;
//    numberFreeSpaces = columns;
//    write_safety_switch = safety;

//}

template <typename T>
CircularArray2DWorkerSafe<T>::CircularArray2DWorkerSafe(priority selected_priority, bool safety, int rows, int columns) : CircularArray2DWorker<T>(rows,columns){
    m_priority = selected_priority;
    numberUsedSpaces = 0;
    numberFreeSpaces = columns;
    write_safety_switch = safety;
    limiteddata = false;
}

template <typename T>
CircularArray2DWorkerSafe<T>::~CircularArray2DWorkerSafe(){

}

template <typename T>
int CircularArray2DWorkerSafe<T>::getNumFreeSpaces(){
//    int tempFreeSpaces = 0;
//    mutex.lock();
//    tempFreeSpaces = numberFreeSpaces;
//    mutex.unlock();
//    return tempFreeSpaces;
    return numberFreeSpaces;
}

template <typename T>
int CircularArray2DWorkerSafe<T>::getNumUsedSpaces(){
//    int tempUsedSpaces = 0;
//    mutex.lock();
//    tempUsedSpaces = numberUsedSpaces;
//    mutex.unlock();
//    return tempUsedSpaces;
    return numberUsedSpaces;
}

template <typename T>
void CircularArray2DWorkerSafe<T>::modifyNumFreeSpaces(int value){
    mutex.lock();
    numberFreeSpaces = numberFreeSpaces + value;
    numberUsedSpaces = numberUsedSpaces - value;
    mutex.unlock();
}

template <typename T>
void CircularArray2DWorkerSafe<T>::resetFreeSpaces(){
    mutex.lock();
    numberFreeSpaces = this->getColumnCount();
    numberUsedSpaces = 0;
    mutex.unlock();
}

template <typename T>
void CircularArray2DWorkerSafe<T>::fastPushColumn(T* columnData){
    this->pushColumn(columnData);
}

template <typename T>
void CircularArray2DWorkerSafe<T>::safePushColumnMutex(T* columnData){
    if(write_safety_switch){
        //mutex.lock();
        if(getNumFreeSpaces()==0){ //if buffer is full we just wait - in reality this wont happen
            bufferNotFull.wait(&p_mutex);
        }
        //mutex.unlock();
    }

    switch(m_priority)
    {
    case WRITER:
        //writer has priority
        //mutex.lock();
        if(getNumFreeSpaces()>0){
            modifyNumFreeSpaces(-1);
            //numberFreeSpaces = numberFreeSpaces-1; numberUsedSpaces = numberUsedSpaces+1;
            this->pushColumn(columnData);
            if(getNumFreeSpaces()>block){
                bufferNotEmpty.wakeAll();
            }
        }else{
            //buffer is full, so start overwriting
            resetFreeSpaces();
            //numberFreeSpaces = this->getColumnCount(); numberUsedSpaces = 0;
        }
        //mutex.unlock();
        break;
    case READER:
        //reader has priority
        //mutex.lock();
        modifyNumFreeSpaces(-1);
        //numberFreeSpaces = numberFreeSpaces-1; numberUsedSpaces = numberUsedSpaces+1;
        this->pushColumn(columnData);
        //mutex.unlock();
        break;
    }
}

template <typename T>
void CircularArray2DWorkerSafe<T>::safePushColumnSemaphore(T* columnData){
    if(write_safety_switch){
        if(freeSpace.available()==0){ //if buffer is full we just wait - in reality this wont happen
            freeSpace.acquire(1);
            freeSpace.release(1); //once we have free space, release immediately and continue
        }
    }
    switch(m_priority)
    {
    case WRITER:
        //writer has priority
        if(freeSpace.tryAcquire()){
            //buffer not full
            this->pushColumn(columnData);
            usedSpace.release();
        }else{
            //buffer is full, so reset reader position behind writer
            this->setCurrentColumnReadIndex(this->getCurrentColumnWriteIndex()-1);
            int currentUsed = usedSpace.available();
            freeSpace.release(currentUsed);
            usedSpace.acquire(currentUsed);
        }
        break;
    case READER:
        //reader has priority
        freeSpace.acquire(); //try to write but wait of necessary
        this->pushColumn(columnData);
        usedSpace.release();
        break;
    }
}

template <typename T>
void CircularArray2DWorkerSafe<T>::safePopBlockMutex(Array2DWorker<T> *dataBlock){    
    //mutex.lock();
    if(getNumUsedSpaces()==0){ //if buffer is empty we just wait - in reality this wont happen
        bufferNotEmpty.wait(&c_mutex);
    }

    for (int i = 0; i < block; ++i) {
        dataBlock->setColumn(i,this->popColumn());
    }
    modifyNumFreeSpaces(block);
    //numberUsedSpaces = numberUsedSpaces-block; numberFreeSpaces = numberFreeSpaces+block;
    bufferNotFull.wakeAll();
    //mutex.unlock();
}


template <typename T>
void CircularArray2DWorkerSafe<T>::safePopSingleMutex(Array2DWorker<T> *dataBlock){
    //mutex.lock();
    if(getNumUsedSpaces()>0){
        dataBlock->setColumn(0,this->popColumn());
        modifyNumFreeSpaces(1);
        //numberUsedSpaces = numberUsedSpaces-1; numberFreeSpaces = numberFreeSpaces+1;
        bufferNotFull.wakeAll();
    }
    //mutex.unlock();
}

template <typename T>
void CircularArray2DWorkerSafe<T>::safePopBlockSemaphore(Array2DWorker<T> *dataBlock){
    if(usedSpace.tryAcquire(block,50)){
        for (int i = 0; i < block; ++i) {
            dataBlock->setColumn(i,this->popColumn());
        }
        freeSpace.release(block);
    }else{
//        int lastUnits = samplesSize - freeSpace.available();
//        usedSpace.acquire(lastUnits);
//        for (int i = 0; i < lastUnits; ++i) {
//            dataBlock->setColumn(i,this->popColumn());
//        }
//        freeSpace.release(lastUnits);
    }

//    usedSpace.acquire(block); //try to acquire a block of written data, or wait until there is enough
//    for (int i = 0; i < block; ++i) {
//        dataBlock->setColumn(i,this->popColumn());
//    }
//    freeSpace.release(block);
}

//template <typename T>
//void CircularArray2DWorkerSafe<T>::safePushColumn(T* columnData)
//{
//    //first check if we have filled our buffer - this needs to be removed if we cannot control the writer
//    if(write_safety_switch){
//        mutex.lock();
//        if (numberUsedSpaces == this->getColumnCount()){
//            cout << "Will wait, buffer is full" << endl;
//            bufferNotFull.wait(&mutex);
//        }
//        mutex.unlock();
//    }

//    //    //now check if there are a specific amount of blocks in the buffer
//    //    mutex.lock();
//    //    if(numberUsedSpaces >= blockSize){
//    //        bufferEnoughFull.wakeAll();
//    //    }
//    //    mutex.unlock();

//    //arrived here, so we are authorized to make writes (empty space or no writer control)
//    mutex.lock();
//    switch (m_priority)
//    {
//    case WRITER:
//        //cout << "Write collision on PUSH - WRITER priority" << endl;
//        if(this->getCurrentColumnReadIndex()==this->getCurrentColumnWriteIndex()){ //buffer is full so we overwrite, and flush the rest
//            this->setCurrentColumnReadIndex(this->getCurrentColumnWriteIndex()-1); //set reader as last written location
//            numberUsedSpaces = 0; //reset used spaces
//        }
//        this->pushColumn(columnData); //written (or overwritten)
//        ++numberUsedSpaces;
//        bufferNotEmpty.wakeAll();
//        break;
//    case READER:
//        //cout << "Write collision on PUSH - READER priority" << endl;
//        if(this->getCurrentColumnReadIndex()==this->getCurrentColumnWriteIndex()){ //buffer is full so wait for reader since it has priority
//            bufferNotFull.wait(&mutex);
//        }
//        this->pushColumn(columnData);
//        ++numberUsedSpaces;
//        bufferNotEmpty.wakeAll();
//    }
//    mutex.unlock();
//}

//template <typename T>
//T* CircularArray2DWorkerSafe<T>::safePopColumn(){
//    T* result;
//    //first check if something has been written to the buffer
//    mutex.lock();
//    if (numberUsedSpaces == 0){
//        bufferNotEmpty.wait(&mutex);
//    }
//    mutex.unlock();

//    //arrived here, so something must have been written by now, and we can try to read something
//    mutex.lock();
//    switch (m_priority){
//    case WRITER:
//        //cout << "Read collision on POP - WRITER priority" << endl;
//        if(this->getCurrentColumnReadIndex()==this->getCurrentColumnWriteIndex()){ //buffer is full so wait, since writer has priority
//            bufferNotFull.wait(&mutex);
//        }
//        result = this->popColumn();
//        --numberUsedSpaces;
//        bufferNotFull.wakeAll();
//        break;
//    case READER:
//        //cout << "Read collision on POP - READER priority" << endl;
//        result = this->popColumn();
//        --numberUsedSpaces;
//        bufferNotFull.wakeAll();
//        break;
//    }
//    mutex.unlock();
//    return result;
//}

template <typename T>
int CircularArray2DWorkerSafe<T>::getFreeSlotCountSemaphore(){
    return freeSpace.available();
}

template <typename T>
int CircularArray2DWorkerSafe<T>::getFreeSlotCountMutex(){
    return numberFreeSpaces;
}

template <typename T>
int CircularArray2DWorkerSafe<T>::getFilledSlotCountMutex(){
    return numberUsedSpaces;
}

//template <typename T>
//int CircularArray2DWorkerSafe<T>::getFreeSlotCount(){
//    //return this->getColumnCount()-numberUsedSpaces;
//    return freeSpace.available();
//}

template class CircularArray2DWorkerSafe<int>;
template class CircularArray2DWorkerSafe<float>;
template class CircularArray2DWorkerSafe<double>;
