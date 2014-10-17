#ifndef SPECTROGRAMPLOT_H
#define SPECTROGRAMPLOT_H
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_interval.h>
#include <qwt_plot_layout.h>

#include "customcolormap.h"
#include "spectrogramdata.h"

#include <stdio.h>
using namespace std;
#include<iostream>


class SpectrogramPlot : public QwtPlot
{
    Q_OBJECT
public:
    SpectrogramPlot(QWidget * = NULL);
    void pushNewData(SpectrogramData<float> *myData);
    void setupAxis();
    //explicit SpectrogramPlot(QObject *parent = 0);

signals:

public Q_SLOTS:
    void showSpectrogram(bool on);
    void setAlpha(int alpha);

private:
    QwtPlotSpectrogram *mySpectrogramPlot;
    bool initialSetup;

};

#endif // SPECTROGRAMPLOT_H
