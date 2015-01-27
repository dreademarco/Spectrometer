#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "spectrogramplot.h"
#include "common.h"
#include "fftwsequence.h"
#include "fftwsequencebuffer.h"
#include "pipeline.h"
#include "plotter.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void terminatePlotter();
    void terminatePipeline();
    ~MainWindow();

private slots:
    void on_startPushButton_clicked();

    void on_jetRadioButton_clicked();

    void on_stdRadioButton_clicked();

    void on_grayRadioButton_clicked();

    void on_stopPushButton_clicked();

    void on_yellowRadioButton_clicked();

    void on_comboBox_bufferFactor_currentIndexChanged(const QString &arg1);

    void on_comboBox_plotFactor_currentTextChanged(const QString &arg1);

    void on_comboBox_integrationFactor_currentIndexChanged(const QString &arg1);

private:
    //FFTWSequenceCircularThreaded *pipelineSourceDataBlock;
    FFTWSequenceBuffer *pipelineSourceDataBlock;
    Ui::MainWindow *ui;
    QThread *pipeline_thread;
    QThread *plotter_thread;
    Pipeline *pipeline;
    Plotter *plotter;
    void generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal);

    //config vars
    int integFactor;
    int ppftaps;
    int chans;
    int fs;
    int buf_factor;
    int bufsize;
    int port;
    int samplesPerPacket;
    int plotSize;
    int plotBufferSections;

public slots:
    void spectrogramPlotUpdate();
};

#endif // MAINWINDOW_H
