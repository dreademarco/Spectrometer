#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pipeline_thread = new QThread; // First thread
    plotter_thread = new QThread; // Second thread

    //int minPossibleBufferValues = nblocks * nthreads * nantennas;
    //int minPossibleBufferValues = nblocks * nantennas;
    int factorSize = ui->comboBox_bufferFactor->currentText().toInt();
    ui->label_bufsize->setText(QString::number(minPossibleBufferValues*factorSize));
    ui->label_samplesperpacket->setText(QString::number((minPossibleBufferValues*factorSize)/nantennas));
    int factorPlotter = ui->comboBox_plotFactor->currentText().toInt();
    int integrationFactor = ui->comboBox_integrationFactor->currentText().toInt();
    ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)*factorSize*factorPlotter));
}

MainWindow::~MainWindow()
{
    if(plotter_thread->isRunning()){
        plotter->setupTermination();
        plotter_thread->terminate();
        delete plotter_thread;
        delete plotter;
    }
    if(pipeline_thread->isRunning()){
        pipeline->setupTermination();
        pipeline_thread->terminate();
        delete pipeline_thread;
        delete pipeline;
        delete pipelineSourceDataBlock;
    }    
    delete ui;
}

void MainWindow::spectrogramPlotUpdate()
{
    ui->plotWidget->replot();
}

void MainWindow::terminatePlotter(){
    plotter_thread->quit();
    //ui->startPushButton->setEnabled(true);
}

void MainWindow::terminatePipeline(){
    pipeline_thread->quit();
}

void MainWindow::on_startPushButton_clicked()
{    
    // disable the start button
    ui->startPushButton->setEnabled(false);

    port = ui->spinBox_port->text().toInt();
    samplesPerPacket = ui->label_samplesperpacket->text().toInt();
    integFactor = ui->comboBox_integrationFactor->currentText().toInt();
    ppftaps = ui->comboBox_taps->currentText().toInt();
    chans = ui->comboBox_chans->currentText().toInt();
    fs = ui->comboBox_srate->currentText().toInt();
    buf_factor = ui->comboBox_bufferFactor->currentText().toInt();
    bufsize = ui->label_bufsize->text().toInt();
    //plotSize = ui->label_plotbuffersamples->text().toInt();
    plotBufferSections = ui->comboBox_plotFactor->currentText().toInt();

    //initialize pipeline datablock    
    pipelineSourceDataBlock = new FFTWSequenceBuffer(chans,bufsize,plotBufferSections);

    // make pipeline thread
    pipeline = new Pipeline(pipelineSourceDataBlock, nblocks, integFactor, ppftaps, chans, fs, bufsize, port, samplesPerPacket);
    connect(pipeline_thread, SIGNAL(started()), pipeline, SLOT(start()));

    // make plotter thread
    plotter = new Plotter(pipelineSourceDataBlock,ui->plotWidget,bufsize/integFactor,chans);
    //plotter = new Plotter(pipelineSourceDataBlock,ui->plotWidget,plotSize,chans);
    connect(plotter_thread, SIGNAL(started()), plotter, SLOT(start()));

    // connect signal/slot for SpectrogramPlot
    connect(plotter, SIGNAL(readyForPlot()),this,SLOT(spectrogramPlotUpdate()));

    // threads start
    plotter->moveToThread(plotter_thread);
    pipeline->moveToThread(pipeline_thread);
    plotter_thread->start();
    pipeline_thread->start();
}

void MainWindow::generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal)
{
    int samples = fs*duration;
    float beta = (f1-f0)/t1;
    float interval = (float)duration/samples;
    for (int i = 0; i < samples; ++i) {
        float t = float(i)*interval;
        signal[i][0] = t;
        signal[i][0] = cos(2*M_PI * ( 0.5* beta * (signal[i][0]*signal[i][0]) + f0*signal[i][0]));
        signal[i][1] = 0;
    }
}

void MainWindow::on_jetRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(true);
    ui->stdRadioButton->setChecked(false);
    ui->grayRadioButton->setChecked(false);
    ui->yellowRadioButton->setChecked(false);
    ui->plotWidget->setColorMap(CustomColorMap::JET);
}

void MainWindow::on_stdRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(false);
    ui->stdRadioButton->setChecked(true);
    ui->grayRadioButton->setChecked(false);
    ui->yellowRadioButton->setChecked(false);
    ui->plotWidget->setColorMap(CustomColorMap::STANDARD);
}

void MainWindow::on_grayRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(false);
    ui->stdRadioButton->setChecked(false);
    ui->grayRadioButton->setChecked(true);
    ui->yellowRadioButton->setChecked(false);
    ui->plotWidget->setColorMap(CustomColorMap::GRAY);
}

void MainWindow::on_stopPushButton_clicked()
{
    plotter->setupTermination();
    pipeline->setupTermination();
    ui->startPushButton->setEnabled(true);
}

void MainWindow::on_yellowRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(false);
    ui->stdRadioButton->setChecked(false);
    ui->grayRadioButton->setChecked(false);
    ui->yellowRadioButton->setChecked(true);
    ui->plotWidget->setColorMap(CustomColorMap::YELLOW);
}

void MainWindow::on_comboBox_bufferFactor_currentIndexChanged(const QString &arg1)
{   
    //int minPossibleBufferValues = nblocks * nthreads * nantennas;
    int factorSize = ui->comboBox_bufferFactor->currentText().toInt();
    ui->label_bufsize->setText(QString::number(minPossibleBufferValues*factorSize));
    ui->label_samplesperpacket->setText(QString::number((minPossibleBufferValues*factorSize)/nantennas));    
    int integrationFactor = ui->comboBox_integrationFactor->currentText().toInt();
    //ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)*factorSize));
    //ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)));
}

void MainWindow::on_comboBox_plotFactor_currentTextChanged(const QString &arg1)
{
    //int minPossibleBufferValues = nblocks * nthreads * nantennas;
    //int factorSize = ui->comboBox_bufferFactor->currentText().toInt();
    //int factorSize = ui->comboBox_plotFactor->currentText().toInt();
    int integrationFactor = ui->comboBox_integrationFactor->currentText().toInt();
    //ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)*factorSize));
    ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)));
}

void MainWindow::on_comboBox_integrationFactor_currentIndexChanged(const QString &arg1)
{
    //int minPossibleBufferValues = nblocks * nthreads * nantennas;
    int factorSize = ui->comboBox_bufferFactor->currentText().toInt();
    int integrationFactor = ui->comboBox_integrationFactor->currentText().toInt();
    //ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)*factorSize));
    ui->label_plotbuffersamples->setText(QString::number((minPossibleBufferValues/integrationFactor)));
}
