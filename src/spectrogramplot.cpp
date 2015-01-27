#include "spectrogramplot.h"

SpectrogramPlot::SpectrogramPlot(QWidget *parent) :
    QwtPlot(parent)
{
//    CustomColorMap *myColorMap;
//    myColorMap = new CustomColorMap(CustomColorMap::JET);
    myColorMap = new CustomColorMap(CustomColorMap::JET);

    mySpectrogramPlot = new QwtPlotSpectrogram();
    mySpectrogramPlot->setRenderThreadCount(0); //use system specific thread count
    mySpectrogramPlot->setColorMap(myColorMap);
    mySpectrogramPlot->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    //mySpectrogramPlot->setCachePolicy(QwtPlotRasterItem::PaintCache);
    mySpectrogramPlot->attach(this);

    QwtScaleWidget *leftAxis = axisWidget(QwtPlot::yLeft);
    leftAxis->setTitle("Channel");
    QwtScaleWidget *bottomAxis = axisWidget(QwtPlot::xBottom);
    bottomAxis->setTitle("Sample");

    this->initialSetup = true;

    plotLayout()->setAlignCanvasToScales(true);
    //autoReplot();
    replot();
}

void SpectrogramPlot::pushNewData(SpectrogramData *myData){
    //mySpectrogramPlot->invalidateCache();
    mySpectrogramPlot->setData(myData);
    if(initialSetup){
        setupAxis();
    }    
}

void SpectrogramPlot::setColorMap(CustomColorMap::colormap selected_colormap){
    delete myColorMap;
    myColorMap = new CustomColorMap(selected_colormap);
    initialSetup = true;
}

void SpectrogramPlot::showSpectrogram(bool on)
{
    mySpectrogramPlot->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    mySpectrogramPlot->setDefaultContourPen(on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );
    //mySpectrogramPlot->invalidateCache();
    replot();
}

void SpectrogramPlot::setAlpha(int alpha)
{
    mySpectrogramPlot->setAlpha(alpha);
}

void SpectrogramPlot::setupAxis(){
    const QwtInterval zInterval = mySpectrogramPlot->data()->interval(Qt::ZAxis);
    const QwtInterval xInterval = mySpectrogramPlot->data()->interval(Qt::XAxis);
    const QwtInterval yInterval = mySpectrogramPlot->data()->interval(Qt::YAxis);

    // A color bar on the right axis
    rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setTitle("Intensity");
    rightAxis->setColorBarEnabled(true);
    rightAxis->setColorMap(zInterval,myColorMap);

    setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
    enableAxis( QwtPlot::yRight);

    setAxisScale(QwtPlot::xBottom, xInterval.minValue(), xInterval.maxValue());
    setAxisScale(QwtPlot::yLeft, yInterval.minValue(), yInterval.maxValue());
    initialSetup = false;
}
