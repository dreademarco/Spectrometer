#include "consumerunsafe.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
using namespace std;

ConsumerUnsafe::ConsumerUnsafe(QObject *parent) : QThread(parent)
{
}

ConsumerUnsafe::ConsumerUnsafe(QObject *parent, CircularArray2DWorkerSafe<float> *dataBlock, SpectrogramData<float> *spectData, SpectrogramPlot *myPlot) : QThread(parent)
{
    this->dataBlock = dataBlock;
    this->mySpectrogramData = spectData;
    this->mySpectrogramData->resetData();
    this->myPlot = myPlot;
}

void ConsumerUnsafe::run()
{
    int placements = 0;
    time_t start,end;
    time (&start);
    Array2DWorker<float> *tempData;
    tempData = new Array2DWorker<float>(freqBins,block);
//    Array2DWorker<float> *singleTempData;
//    singleTempData = new Array2DWorker<float>(freqBins,1);

    bool exitCondition = false;
    while(!exitCondition){
        while(dataBlock->getNumUsedSpaces()>0){
            //cout << dataBlock->getNumUsedSpaces() << endl;
            if(dataBlock->getNumUsedSpaces()>=block){
                dataBlock->safePopBlockMutex(tempData);
                mySpectrogramData->addDataSection(tempData);
                placements+=block;
                if(placements % guiUpdateSize == 0){
                    myPlot->pushNewData(mySpectrogramData);
                    emit spectDataReceived();
                    emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
                    emit consumerCountChanged(placements);
                }
            }else{
                emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
                emit consumerCountChanged(placements);
//                dataBlock->safePopSingleMutex(singleTempData);
//                mySpectrogramData->addData(singleTempData->getRawData());
//                placements+=1;
//                if(placements % guiUpdateSize == 0){
//                    myPlot->pushNewData(mySpectrogramData);
//                    emit spectDataReceived();
//                    emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
//                    emit consumerCountChanged(placements);
//                }
            }
        }
        if(placements==overallSamples){
            exitCondition = true;
        }
    }
    cout << "Consumer popped: " << placements << endl;
    emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
    emit consumerCountChanged(placements);
    emit spectDataReceived();

    time (&end);
    double dif = difftime (end,start);
    printf ("Consumer elasped time is %.2lf seconds.", dif );
    cout << endl;
}

//void ConsumerUnsafe::run()
//{
//    int placements = 0;
//    time_t start,end;
//    time (&start);
//    Array2DWorker<float> *tempData;
//    tempData = new Array2DWorker<float>(freqBins,block);

//    for (int i = 0; i < overallSamples; i+=block) {
//        dataBlock->safePopBlockSemaphore(tempData);
//        mySpectrogramData->addDataSection(tempData);
//        placements+=block;
//        if(i % guiUpdateSize == 0){
//            myPlot->pushNewData(mySpectrogramData);
//            emit spectDataReceived();
//            emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCountSemaphore());
//            emit consumerCountChanged(i);
//        }
//        //this->msleep(30);
//    }
//    cout << "Consumer popped: " << placements << endl;
//    emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCountSemaphore());
//    emit consumerCountChanged(overallSamples);
//    emit spectDataReceived();

//    time (&end);
//    double dif = difftime (end,start);
//    printf ("Consumer elasped time is %.2lf seconds.", dif );
//    cout << endl;
//}

//void ConsumerUnsafe::run()
//{
//    int placements = 0;
//    time_t start,end;
//    time (&start);
//    for (int i = 0; i < overallSamples; ++i) {
//        mySpectrogramData->addData(dataBlock->safePopColumn());
//        ++placements;
//        if(i % guiUpdateSize == 0){
//            myPlot->pushNewData(mySpectrogramData);
//            emit spectDataReceived();
//            emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCount());
//            emit consumerCountChanged(i);
//        }
//        //this->msleep(30);
//    }
//    cout << "Consumer popped: " << placements << endl;
//    emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCount());
//    emit consumerCountChanged(overallSamples);
//    emit spectDataReceived();

//    time (&end);
//    double dif = difftime (end,start);
//    printf ("Consumer elasped time is %.2lf seconds.", dif );
//    cout << endl;
//}
