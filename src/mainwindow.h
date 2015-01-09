#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "spectrogramplot.h"
#include "common.h"
#include "fftwsequence.h"
#include "fftwsequencecircularthreaded.h"
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
    ~MainWindow();

private slots:
    void on_startPushButton_clicked();

    void on_comboBox_chans_currentIndexChanged(const QString &arg1);

    void on_comboBox_srate_currentIndexChanged(const QString &arg1);

private:
    FFTWSequenceCircularThreaded *rawSourceDataBlock;
    FFTWSequenceCircularThreaded *pipelineSourceDataBlock;
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
    int spectsize;

public slots:
    void spectrogramPlotUpdate();
    void terminatePlotter();
    void terminatePipeline();
};

#endif // MAINWINDOW_H
