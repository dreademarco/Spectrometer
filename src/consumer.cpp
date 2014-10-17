#include "consumer.h"
#include<iostream>
#include <time.h>
#include <stdio.h>
using namespace std;

Consumer::Consumer(QObject *parent) : QThread(parent)
{
}

Consumer::Consumer(QObject *parent, CircularArray2DWorker<float> *dataBlock, SpectrogramData *spectData, SpectrogramPlot *myPlot) : QThread(parent)
{
    this->dataBlock = dataBlock;
    this->mySpectrogramData = spectData;
    this->myPlot = myPlot;
}

void Consumer::run()
{
    time_t start,end;
    time (&start);
//    Array2DWorker<float> *tempColumns;
//    tempColumns = new Array2DWorker<float>(dataBlock->getRowCount(),fillSize);

    for (int i = 0; i < overallSamples; i=i+spectSize) {
        usedSamples.acquire(spectSize);
        //do some shit with the acquired samples e.g. plot
        dataBlock->popColumnSection(spectSize,mySpectrogramData->dataArray);
        myPlot->pushNewData(mySpectrogramData);


        //mySpectrogramData = new SpectrogramData(tempColumns,tempColumns->getRowCount(),tempColumns->getColumnCount());

        //plotter->uploadData(mySpectrogramData);

//        for (int cnt = 0; cnt < fillSize; ++cnt) {
//            float* columnData = dataBlock->popColumn(); //->getColumn(i % dataBlock->getColumnCount() + cnt);
//            cout << i << ": " << cnt << endl;
//        }
        emit spectDataReceived();
        freeSamples.release(spectSize);
        emit bufferFillCountChanged(usedSamples.available());
        emit consumerCountChanged(i);
        //this->msleep(30);
    }
    emit bufferFillCountChanged(usedSamples.available());
    emit consumerCountChanged(overallSamples-1);
    time (&end);
    double dif = difftime (end,start);
    printf ("Consumer elasped time is %.2lf seconds.", dif );
    cout << endl;
}
