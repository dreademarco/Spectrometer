#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
using namespace std;

CircularArray2DWorker<float> *dataBlock;
CircularArray2DWorkerSafe<float> *safeDataBlock;
SpectrogramData<float> *mySpectData;
//Array2DWorker<float> *tempColumns;
CircularArray2DWorker<float> *tempColumns;
CircularArray2DWorker<float> *test;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    //testing ground
//    float coldata[3] = {0.1,0.2,0.3};
//    test = new CircularArray2DWorker<float>(3,5);
//    mySpectData = new SpectrogramData(test,3,5);
//    test->toString();
//    mySpectData->addData(coldata);
//    mySpectData->addData(coldata);
//    mySpectData->addData(coldata);
//    mySpectData->addData(coldata);
//    float coldata2[3] = {0.4,0.5,0.6};
//    mySpectData->addData(coldata2); //we have filled it first time
//    cout << mySpectData->dataArray->getCurrentColumnReadIndex() << endl;
//    cout << mySpectData->dataArray->getCurrentColumnWriteIndex() << endl;
//    mySpectData->dataArray->toString();
//    float value1 = mySpectData->value(0,1);


//    mySpectData->addData(coldata2);
//    cout << mySpectData->dataArray->getCurrentColumnReadIndex() << endl;
//    cout << mySpectData->dataArray->getCurrentColumnWriteIndex() << endl;
//    mySpectData->dataArray->toString();
//    float value2 = mySpectData->value(0,1);

    //initialize dataBlock
    safeDataBlock = new CircularArray2DWorkerSafe<float>(CircularArray2DWorkerSafe<float>::WRITER,false,freqBins,samplesSize);

    //initialize SpectrogramData
    //tempColumns = new Array2DWorker<float>(freqBins,fillSize);
    tempColumns = new CircularArray2DWorker<float>(freqBins,spectSize);
    mySpectData = new SpectrogramData<float>(tempColumns,freqBins,spectSize);

    ui->setupUi(this);
    // progress bar range setup
    ui->producerProgressBar->setRange(0, overallSamples);
    ui->consumerProgressBar->setRange(0, overallSamples);
    ui->bufferProgressBar->setRange(0, samplesSize);

    // make producer thread
    unsafeProducer_thread = new ProducerUnsafe(this,safeDataBlock);
    // make consumer thread
    unsafeConsumer_thread = new ConsumerUnsafe(this, safeDataBlock, mySpectData, ui->plotWidget);

    // connect signal/slot for the buffer progress bar
    connect(unsafeConsumer_thread, SIGNAL(bufferFillCountChanged(int)),this,SLOT(onBufferValueChanged(int)));
    connect(unsafeProducer_thread, SIGNAL(bufferFillCountChanged(int)),this,SLOT(onBufferValueChanged(int)));

    // connect signal/slot for consumer/producer progress bar
    connect(unsafeConsumer_thread, SIGNAL(consumerCountChanged(int)),this,SLOT(onConsumerValueChanged(int)));
    connect(unsafeProducer_thread, SIGNAL(producerCountChanged(int)),this,SLOT(onProducerValueChanged(int)));

    // connect signal/slot for SpectrogramPlot
    connect(unsafeConsumer_thread, SIGNAL(spectDataReceived()),this,SLOT(spectrogramPlotUpdate()));

//    //WORKING CODE
//    //initialize dataBlock
//    dataBlock = new CircularArray2DWorker<float>(freqBins,samplesSize);

//    //initialize SpectrogramData
//    tempColumns = new Array2DWorker<float>(freqBins,fillSize);
//    mySpectData = new SpectrogramData(tempColumns,freqBins,fillSize);

//    ui->setupUi(this);

//    // progress bar range setup
//    ui->producerProgressBar->setRange(0, overallSamples-1);
//    ui->consumerProgressBar->setRange(0, overallSamples-1);
//    ui->bufferProgressBar->setRange(0, samplesSize-1);

//    // make two threads
//    producer_thread = new Producer(this, dataBlock);
//    consumer_thread = new Consumer(this, dataBlock, mySpectData, ui->plotWidget);

//    // connect signal/slot for the buffer progress bar
//    connect(consumer_thread, SIGNAL(bufferFillCountChanged(int)),this,SLOT(onBufferValueChanged(int)));
//    connect(producer_thread, SIGNAL(bufferFillCountChanged(int)),this,SLOT(onBufferValueChanged(int)));

//    // connect signal/slot for consumer/producer progress bar
//    connect(consumer_thread, SIGNAL(consumerCountChanged(int)),this,SLOT(onConsumerValueChanged(int)));
//    connect(producer_thread, SIGNAL(producerCountChanged(int)),this,SLOT(onProducerValueChanged(int)));

//    // connect signal/slot for SpectrogramPlot
//    connect(consumer_thread, SIGNAL(spectDataReceived()),this,SLOT(spectrogramPlotUpdate()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tempColumns;
    delete safeDataBlock;
    delete dataBlock;
}

void MainWindow::spectrogramPlotUpdate()
{
    ui->plotWidget->replot();
}

void MainWindow::onBufferValueChanged(int bCount)
{
    ui->bufferProgressBar->setValue(bCount);
}

void MainWindow::onProducerValueChanged(int pCount)
{
    ui->producerProgressBar->setValue(pCount);
}

void MainWindow::onConsumerValueChanged(int cCount)
{
    ui->consumerProgressBar->setValue(cCount);
}

void MainWindow::on_startPushButton_clicked()
{
    // disable the start button
    ui->startPushButton->setEnabled(false);

    // threads start
    unsafeProducer_thread->start();
    //unsafeConsumer_thread->start();
}
