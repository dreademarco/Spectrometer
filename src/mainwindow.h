#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "consumer.h"
#include "producer.h"
#include "producerunsafe.h"
#include "consumerunsafe.h"
#include "spectrogramplot.h"
#include "common.h"
#include "circulararray2dworker.h"
#include "circulararray2dworkersafe.h"
#include "circulararray2dworkerthreaded.h"
#include "circulararray2dspectrumthreaded.h"
#include "pipeline.h"
#include "plotter.h"
#include "ppf.h"
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

private:
    Ui::MainWindow *ui;
    //Producer *producer_thread;
    //Consumer *consumer_thread;
    //ProducerUnsafe *unsafeProducer_thread;
    //ConsumerUnsafe *unsafeConsumer_thread;
    QThread *pipeline_thread;
    QThread *plotter_thread;
    Pipeline *pipeline;
    Plotter *plotter;
    //Pipeline *pipeline_thread;
    //Plotter *plotter_thread;

public slots:
    void onBufferValueChanged(int);
    void onProducerValueChanged(int);
    void onConsumerValueChanged(int);
    void spectrogramPlotUpdate();
    void terminatePlotter();
    void terminatePipeline();
};

#endif // MAINWINDOW_H
