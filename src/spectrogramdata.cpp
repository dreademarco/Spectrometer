#include "spectrogramdata.h"
using namespace std;

SpectrogramData::SpectrogramData(FFTWSequence *data, int freq_bins,int samples) : QwtRasterData()
{
    highestIntensity = 1.0; //100.0
    setInterval(Qt::XAxis, QwtInterval(0,samples-1));
    setInterval(Qt::YAxis, QwtInterval(0,freq_bins-1));
    setInterval(Qt::ZAxis, QwtInterval(0.0,highestIntensity));
    dataArray = data;
}

void SpectrogramData::fixIntensityInterval(double value)
{
    if(value>highestIntensity){
        highestIntensity = value;
        this->setInterval( Qt::ZAxis, QwtInterval(0.0,highestIntensity));
    }
}

double SpectrogramData::value(double x,double y) const
{  
    int sample_idx = (int) x;
    int channel_idx = (int) y;

    fftwf_complex* value;
    value = dataArray->at(channel_idx,sample_idx);
    return (double) value[0][0];
}
