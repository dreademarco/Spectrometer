#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H
#include <qwt_raster_data.h>
#include <qwt_interval.h>
#include "common.h"

class SpectrogramData : public QwtRasterData
{
public:
    SpectrogramData(FFTWSequence *data, int freq_bins,int samples);
    virtual double value( double x, double y ) const;
    FFTWSequence *dataArray;
    void fixIntensityInterval(double value);

private:
    double highestIntensity;
};

#endif // SPECTROGRAMDATA_H
