#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H
#include <qwt_raster_data.h>
#include <qwt_interval.h>
#include <iostream>
#include <stdio.h>

#include "array2dworker.h"
#include "circulararray2dworker.h"
#include "circulararray2dworkerthreaded.h"
#include "circulararray2dspectrum.h"
#include "circulararray2dspectrumthreaded.h"

template <typename T>
class SpectrogramData : public QwtRasterData
{
public:
    SpectrogramData(CircularArray2DSpectrum<T> *data, int freq_bins,int samples);
    virtual double value( double x, double y ) const;
    CircularArray2DSpectrum<T> *dataArray;
    void addData(T* columnData);
    void addDataSection(Array2DSpectrum<T> *blockData);
    void fastAddDataSection(CircularArray2DSpectrumThreaded<T> *sourceDataBlock,int copySize);
    void fastAddDataSectionMemCpy(CircularArray2DSpectrumThreaded<T> *sourceDataBlock, int copySize);
    void resetData();
//private:
//    Array2DWorker<float> *dataArray;
};

#endif // SPECTROGRAMDATA_H
