#include "spectrogramdata.h"
using namespace std;

//SpectrogramData::SpectrogramData(Array2DWorker<float> *data, int freq_bins,int samples) : QwtRasterData()
//{
//    setInterval(Qt::XAxis, QwtInterval(0,samples-1));
//    setInterval(Qt::YAxis, QwtInterval(0,freq_bins-1));
//    setInterval( Qt::ZAxis, QwtInterval(0.0,1.0));
//    dataArray = data;
//}

template <typename T>
SpectrogramData<T>::SpectrogramData(CircularArray2DWorker<T> *data, int freq_bins,int samples) : QwtRasterData()
{
    setInterval(Qt::XAxis, QwtInterval(0,samples-1));
    setInterval(Qt::YAxis, QwtInterval(0,freq_bins-1));
    setInterval( Qt::ZAxis, QwtInterval(0.0,1.0));
    dataArray = data;
}

template <typename T>
void SpectrogramData<T>::addData(T* columnData){
    dataArray->pushColumn(columnData);
    if(dataArray->getRefillCount()>0){
        dataArray->incrementColumnReadIndex();
    }
}

template <typename T>
void SpectrogramData<T>::addDataSection(Array2DWorker<T> *blockData){
    for (int i = 0; i < blockData->getColumnCount(); ++i) {
        this->addData(blockData->getColumn(i));
    }
}

template <typename T>
void SpectrogramData<T>::resetData(){
    for (int i = 0; i < dataArray->getRowCount(); ++i) {
        for (int j = 0; j < dataArray->getColumnCount(); ++j) {
            dataArray->set(i,j,0);
        }
    }
}

template <typename T>
double SpectrogramData<T>::value( double x, double y ) const
{    
//    int column = (int) x;
//    int row = (int) y;
//    return dataArray->at(row,column); // (1) convert to our notation where we reference by [row,column]

    int column = (int) x;
    int row = (int) y;
    column = column + dataArray->getCurrentColumnReadIndex(); //since ReadIdx might have moved
    if(column>=dataArray->getColumnCount()){
        column = column - dataArray->getColumnCount();
    }
    return (double) dataArray->at(row,column);
}

template class SpectrogramData<int>;
template class SpectrogramData<float>;
template class SpectrogramData<double>;


