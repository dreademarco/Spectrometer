#include "producer.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
using namespace std;

Producer::Producer(QObject *parent) : QThread(parent)
{
}

Producer::Producer(QObject *parent, CircularArray2DWorker<float> *dataBlock) : QThread(parent)
{
    this->dataBlock = dataBlock;
}

void Producer::run()
{
    time_t start,end;
    time (&start);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for (int i = 0; i < overallSamples; i=i+spectSize) {
        //fill in some shit
        freeSamples.acquire(spectSize);
        for (int cnt = 0; cnt < spectSize; ++cnt) {
            float columnData[freqBins];
            for (int j = 0; j < freqBins; ++j) {
                float r = static_cast <float> (qrand()) / static_cast <float> (RAND_MAX);
                columnData[j]=r;
            }
//            if((i % dataBlock->getColumnCount() + cnt)>100000){
//                int help = 1;
//            }
            dataBlock->pushColumn(columnData);
        }
        usedSamples.release(spectSize);
        //pass some GUI update every fillSize fills
        if(i % spectSize == 0){
            emit bufferFillCountChanged(usedSamples.available());
            emit producerCountChanged(i);
        }
        //this->msleep(10);
    }
    emit bufferFillCountChanged(usedSamples.available());
    emit producerCountChanged(overallSamples-1);
    time (&end);
    double dif = difftime (end,start);
    printf ("Producer elasped time is %.2lf seconds.", dif );
    //cout << "Done." << endl;
}
