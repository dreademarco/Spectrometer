#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H
#include <qwt_raster_data.h>
#include <qwt_interval.h>
#include <iostream>
#include <stdio.h>

#include "array2dworker.h"
#include "circulararray2dworker.h"

template <typename T>
class SpectrogramData : public QwtRasterData
{
public:
    SpectrogramData(CircularArray2DWorker<T> *data, int freq_bins,int samples);
    virtual double value( double x, double y ) const;
    CircularArray2DWorker<T> *dataArray;
    void addData(T* columnData);
    void addDataSection(Array2DWorker<T> *blockData);
    void resetData();
//private:
//    Array2DWorker<float> *dataArray;
};

#endif // SPECTROGRAMDATA_H
