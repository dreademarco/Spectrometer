#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
using namespace std;

CircularArray2DWorker<float> *dataBlock;
CircularArray2DSpectrumThreaded<float> *safeDataBlock;
CircularArray2DSpectrumThreaded<float> *outputDataBlock;
SpectrogramData<float> *mySpectData;
CircularArray2DSpectrum<float> *tempColumns;
CircularArray2DWorker<float> *test;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // progress bar range setup
    ui->producerProgressBar->setRange(0, overallSamples);
    ui->consumerProgressBar->setRange(0, overallSamples);
    ui->bufferProgressBar->setRange(0, samplesSize);

    //initialize dataBlock
    //safeDataBlock = new CircularArray2DWorkerSafe<float>(CircularArray2DWorkerSafe<float>::WRITER,false,freqBins,samplesSize);
    safeDataBlock = new CircularArray2DSpectrumThreaded<float>(freqBins,samplesSize);
    outputDataBlock = new CircularArray2DSpectrumThreaded<float>(freqBins,samplesSize);

    //initialize SpectrogramData
    tempColumns = new CircularArray2DSpectrum<float>(freqBins,spectSize);
    mySpectData = new SpectrogramData<float>(tempColumns,freqBins,spectSize);

    int placements = 0;
    while(placements<samplesSize){
        float sampleData[freqBins];
        for (int j = 0; j < freqBins; ++j) {
            //slightly faster simple value (182 Mhz)
            //sampleData[j]=(float)j/freqBins; //constant values along frequency
            sampleData[j]=(float)placements/samplesSize; //constant values along samples
        }
        safeDataBlock->writeSample(sampleData);
        ++placements;
    }

    // make producer thread
    //unsafeProducer_thread = new ProducerUnsafe(this,safeDataBlock);
    // make consumer thread
    unsafeConsumer_thread = new ConsumerUnsafe(this, safeDataBlock, mySpectData, ui->plotWidget);
    // make pipeline_thread
    //pipeline_thread = new Pipeline(this, safeDataBlock, outputDataBlock, block, 2);

    // connect signal/slot for SpectrogramPlot
    connect(unsafeConsumer_thread, SIGNAL(spectDataReceived()),this,SLOT(spectrogramPlotUpdate()));

//    // connect signal/slot for the buffer progress bar
//    connect(unsafeConsumer_thread, SIGNAL(bufferFillCountChanged(int)),this,SLOT(onBufferValueChanged(int)));
//    connect(unsafeProducer_thread, SIGNAL(bufferFillCountChanged(int)),this,SLOT(onBufferValueChanged(int)));

//    // connect signal/slot for consumer/producer progress bar
//    connect(unsafeConsumer_thread, SIGNAL(consumerCountChanged(int)),this,SLOT(onConsumerValueChanged(int)));
//    connect(unsafeProducer_thread, SIGNAL(producerCountChanged(int)),this,SLOT(onProducerValueChanged(int)));


}

MainWindow::~MainWindow()
{
    delete ui;
    delete tempColumns;
    //delete safeDataBlock;
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
    //unsafeProducer_thread->start();
    unsafeConsumer_thread->start();
    //pipeline_thread->start();
}
