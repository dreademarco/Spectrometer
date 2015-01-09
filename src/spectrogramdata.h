#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H
#include <qwt_raster_data.h>
#include <qwt_interval.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

#include "fftwsequencecircularthreaded.h"
//#include "array2dworker.h"
//#include "circulararray2dworker.h"
//#include "circulararray2dworkerthreaded.h"
//#include "circulararray2dspectrum.h"
//#include "circulararray2dspectrumthreaded.h"

class SpectrogramData : public QwtRasterData
{
public:
    SpectrogramData(FFTWSequenceCircular *data, int freq_bins,int samples);
    virtual double value( double x, double y ) const;
    FFTWSequenceCircular *dataArray;
    void addData(fftwf_complex* columnData);
    void addDataSection(FFTWSequence *blockData);
    void fastAddDataSection(FFTWSequenceCircularThreaded *sourceDataBlock,int copySize);
    void fastAddDataSectionMemCpy(FFTWSequenceCircularThreaded *sourceDataBlock, int copySize);
    void resetData();
    void fixIntensityInterval(double value);

private:
    double highestIntensity;
};

#endif // SPECTROGRAMDATA_H
