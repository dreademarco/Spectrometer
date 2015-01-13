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

    int chosen_srate = ui->comboBox_srate->currentText().toInt();
    int chosen_chans = ui->comboBox_chans->currentText().toInt();
    ui->label_bufsize->setText(QString::number(((nblocks * nthreads * chosen_chans)/chosen_srate)*2));
}

MainWindow::~MainWindow()
{
    delete plotter;
    delete pipeline;
    delete plotter_thread;
    delete pipeline_thread;
    delete rawSourceDataBlock;
    delete pipelineSourceDataBlock;
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

    integFactor = ui->comboBox_integrationFactor->currentText().toInt();
    ppftaps = ui->comboBox_taps->currentText().toInt();
    chans = ui->comboBox_chans->currentText().toInt();
    fs = ui->comboBox_srate->currentText().toInt();
    buf_factor = ui->comboBox_bufferFactor->currentText().toInt();
    bufsize = ui->label_bufsize->text().toInt();
    spectsize = ui->comboBox_spectSamples->currentText().toInt();

    // progress bar range setup
    int nsamp = tobs*fs;

    //initialize raw dataBlock, and populate it
    rawSourceDataBlock = new FFTWSequenceCircularThreaded(1,nsamp);
    generateLinearChirp(fs,tobs,0,tobs/2,100,rawSourceDataBlock->data);
    //generateLinearChirp(fs, tobs, 0, tobs/2, 500, rawSourceDataBlock->data);
    //generateLinearChirp(fs,duration,0,duration/4,700,chirpsignal);
    for (int i = 0; i < nsamp; ++i) {
        rawSourceDataBlock->increaseUsedSpaces();
    }

    //initialize pipeline datablock
    pipelineSourceDataBlock = new FFTWSequenceCircularThreaded(chans,(nsamp/integFactor)/chans);

    // make pipeline thread
    pipeline = new Pipeline(rawSourceDataBlock, pipelineSourceDataBlock, nblocks, integFactor, ppftaps, chans, fs, buf_factor*bufsize);
    connect(pipeline_thread, SIGNAL(started()), pipeline, SLOT(start()));

    // make plotter thread
    plotter = new Plotter(pipelineSourceDataBlock,ui->plotWidget,spectsize,chans,fs,integFactor);
    connect(plotter_thread, SIGNAL(started()), plotter, SLOT(start()));

    // connect signal/slot for SpectrogramPlot
    connect(plotter, SIGNAL(readyForPlot()),this,SLOT(spectrogramPlotUpdate()));
    connect(plotter, SIGNAL(done()),this,SLOT(terminatePlotter()));
    connect(pipeline, SIGNAL(done()),this,SLOT(terminatePipeline()));

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
//    FILE *fp = fopen("input.dat", "wb");
//    fwrite(signal, sizeof(fftwf_complex), samples, fp);
//    fclose(fp);
}

void MainWindow::on_comboBox_chans_currentIndexChanged(const QString &arg1)
{
    int chosen_srate = ui->comboBox_srate->currentText().toInt();
    int chosen_chans = arg1.toInt();
    ui->label_bufsize->setText(QString::number(((nblocks * nthreads * chosen_chans)/chosen_srate)*2));
}

void MainWindow::on_comboBox_srate_currentIndexChanged(const QString &arg1)
{
    int chosen_srate = arg1.toInt();
    int chosen_chans = ui->comboBox_chans->currentText().toInt();
    ui->label_bufsize->setText(QString::number(((nblocks * nthreads * chosen_chans)/chosen_srate)*2));
}

void MainWindow::on_jetRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(true);
    ui->stdRadioButton->setChecked(false);
    ui->grayRadioButton->setChecked(false);
    ui->plotWidget->setColorMap(CustomColorMap::JET);
}

void MainWindow::on_stdRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(false);
    ui->stdRadioButton->setChecked(true);
    ui->grayRadioButton->setChecked(false);
    ui->plotWidget->setColorMap(CustomColorMap::STANDARD);
}

void MainWindow::on_grayRadioButton_clicked()
{
    ui->jetRadioButton->setChecked(false);
    ui->stdRadioButton->setChecked(false);
    ui->grayRadioButton->setChecked(true);
    ui->plotWidget->setColorMap(CustomColorMap::GRAY);
}

void MainWindow::on_stopPushButton_clicked()
{
    plotter_thread->exit();
    pipeline_thread->exit();
    ui->startPushButton->setEnabled(true);
}
