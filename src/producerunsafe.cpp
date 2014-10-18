#include "producerunsafe.h"
#include <ctime>
using namespace std;

ProducerUnsafe::ProducerUnsafe(QObject *parent) : QThread(parent)
{
}

ProducerUnsafe::ProducerUnsafe(QObject *parent, CircularArray2DWorkerThreaded<float> *dataBlock) : QThread(parent)
{
    this->dataBlock = dataBlock;
}

void ProducerUnsafe::run(){
    int placements = 0;
    double sysTime = time(0);
    double sysTimeMS_start = sysTime*1000;
    double sysTime2 = time(0);
    double sysTimeMS_end = sysTime2*1000;

    //qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    while(true){
    //for (int i = 0; i < overallSamples; ++i) {
        float columnData[freqBins];
        for (int j = 0; j < freqBins; ++j) {
            //slow random generation (55Mhz)
            //float r = static_cast <float> (qrand()) / static_cast <float> (RAND_MAX);
            //columnData[j]=r;

            //slightly faster simple value (182 Mhz)
            columnData[j]=(float)j/freqBins;

            //fastest value (256Mhz)
            //columnData[j] = 0;
        }
        dataBlock->fastPushColumn(columnData);
        ++placements;
        if(placements % overallSamples == 0){
            sysTime2 = time(0);
            sysTimeMS_end = sysTime2*1000;
            cout << "Producer elasped time: " << sysTimeMS_end-sysTimeMS_start << " ms" << endl;
            int totalSeconds = (sysTimeMS_end-sysTimeMS_start)/1000;
            float datarate = (((overallSamples*freqBins)/totalSeconds)/1000000);
            cout << "Data rate est.: " << datarate << " Mhz" << endl;
            sysTime = time(0);
            sysTimeMS_start = sysTime*1000;
        }
    }
    //cout << "Producer pushed: " << placements << endl;
//    const long double sysTime2 = time(0);
//    const long double sysTimeMS_end = sysTime2*1000;
//    cout << "Producer elasped time: " << sysTimeMS_end-sysTimeMS_start << " ms" << endl;
//    int totalSeconds = (sysTimeMS_end-sysTimeMS_start)/1000;
//    float datarate = (((overallSamples*freqBins)/totalSeconds)/1000000);
//    cout << "Data rate est.: " << datarate << " Mhz" << endl;
}

//void ProducerUnsafe::run()
//{
//    int placements = 0;
//    time_t start,end;
//    time (&start);
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    for (int i = 0; i < overallSamples; ++i) {
//        float columnData[freqBins];
//        for (int j = 0; j < freqBins; ++j) {
//            //float r = static_cast <float> (qrand()) / static_cast <float> (RAND_MAX);
//            //columnData[j]=r;
//            columnData[j]=(float)j/freqBins;
//        }
//        dataBlock->safePushColumnMutex(columnData);
//        ++placements;

//        // pass GUI update if buffer is full
//        if(dataBlock->getFreeSlotCountMutex()==0){
//            emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
//        }
//        //pass some GUI update every fillSize fills
//        if(i % guiUpdateSize == 0){
//            emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
//            emit producerCountChanged(i);
//        }
//        //this->msleep(0.000001);
//    }
//    cout << "Producer pushed: " << placements << endl;
//    emit bufferFillCountChanged(dataBlock->getNumUsedSpaces());
//    emit producerCountChanged(overallSamples);
//    time (&end);
//    double dif = difftime (end,start);
//    printf("Producer elasped time is %.2lf seconds.", dif );
//    cout << endl;
//}

//void ProducerUnsafe::run()
//{
//    int placements = 0;
//    time_t start,end;
//    time (&start);
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    for (int i = 0; i < overallSamples; ++i) {
//        float columnData[freqBins];
//        for (int j = 0; j < freqBins; ++j) {
//            float r = static_cast <float> (qrand()) / static_cast <float> (RAND_MAX);
//            columnData[j]=r;
//        }
//        dataBlock->safePushColumnSemaphore(columnData);
//        ++placements;

//        // pass GUI update if buffer is full
//        if(dataBlock->getFreeSlotCountSemaphore()==0){
//            emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCountSemaphore());
//        }
//        //pass some GUI update every fillSize fills
//        if(i % guiUpdateSize == 0){
//            emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCountSemaphore());
//            emit producerCountChanged(i);
//        }
//        //this->msleep(0.000001);
//    }
//    cout << "Producer pushed: " << placements << endl;
//    emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCountSemaphore());
//    emit producerCountChanged(overallSamples);
//    time (&end);
//    double dif = difftime (end,start);
//    printf("Producer elasped time is %.2lf seconds.", dif );
//    cout << endl;
//}

//void ProducerUnsafe::run()
//{
//    int placements = 0;
//    //connect(this, SIGNAL(bufferFullDetected(int)),this,SLOT(onBufferFullValueChanged(int)));
//    time_t start,end;
//    time (&start);
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    for (int i = 0; i < overallSamples; ++i) {
//        float columnData[freqBins];
//        for (int j = 0; j < freqBins; ++j) {
//            float r = static_cast <float> (qrand()) / static_cast <float> (RAND_MAX);
//            columnData[j]=r;
//        }
//        dataBlock->safePushColumn(columnData);
//        //dataBlock->safePushColumnSemaphore(columnData);
//        ++placements;

//        // pass GUI update if buffer is full
//        if(dataBlock->getFreeSlotCount()==0){
//            emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCount());
//        }
//        //pass some GUI update every fillSize fills
//        if(i % guiUpdateSize == 0){
//            emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCount());
//            emit producerCountChanged(i);
//        }
//        //this->msleep(1);
//    }
//    emit bufferFillCountChanged(dataBlock->getColumnCount()-dataBlock->getFreeSlotCount());
//    emit producerCountChanged(overallSamples);
//    time (&end);
//    double dif = difftime (end,start);
//    printf("Producer elasped time is %.2lf seconds.", dif );
//    cout << endl;
//}
