#include "spectrogramplot.h"

SpectrogramPlot::SpectrogramPlot(QWidget *parent) :
    QwtPlot(parent)
{
    CustomColorMap *myColorMap;
    myColorMap = new CustomColorMap(CustomColorMap::JET);

    mySpectrogramPlot = new QwtPlotSpectrogram();
    mySpectrogramPlot->setRenderThreadCount(1); //use system specific thread count
    mySpectrogramPlot->setColorMap(myColorMap);
    //mySpectrogramPlot->setCachePolicy(QwtPlotRasterItem::PaintCache);
    mySpectrogramPlot->attach(this);
    this->initialSetup = true;

    plotLayout()->setAlignCanvasToScales(true);
    replot();
}

void SpectrogramPlot::pushNewData(SpectrogramData *myData){
    mySpectrogramPlot->setData(myData);
    if(initialSetup){
        setupAxis();
    }    
}

void SpectrogramPlot::showSpectrogram(bool on)
{
    mySpectrogramPlot->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    mySpectrogramPlot->setDefaultContourPen(on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );
    replot();
}

void SpectrogramPlot::setAlpha(int alpha)
{
    mySpectrogramPlot->setAlpha(alpha);
    replot();
}

void SpectrogramPlot::setupAxis(){
    CustomColorMap *myColorMap;
    myColorMap = new CustomColorMap(CustomColorMap::JET);
    const QwtInterval zInterval = mySpectrogramPlot->data()->interval(Qt::ZAxis);
    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setTitle("Intensity");
    rightAxis->setColorBarEnabled(true);
    rightAxis->setColorMap(zInterval,myColorMap);

    setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
    enableAxis( QwtPlot::yRight);
    this->initialSetup = false;
}
