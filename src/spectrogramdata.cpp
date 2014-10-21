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
SpectrogramData<T>::SpectrogramData(CircularArray2DSpectrum<T> *data, int freq_bins,int samples) : QwtRasterData()
{
    setInterval(Qt::XAxis, QwtInterval(0,samples-1));
    setInterval(Qt::YAxis, QwtInterval(0,freq_bins-1));
    setInterval( Qt::ZAxis, QwtInterval(0.0,1.0));
    dataArray = data;
}

template <typename T>
void SpectrogramData<T>::addData(T* sampleData){
    dataArray->pushSample(sampleData);
    if(dataArray->getRefillCount()>0){
        dataArray->incrementReadIndex();
    }
}

template <typename T>
void SpectrogramData<T>::fastAddDataSectionMemCpy(CircularArray2DSpectrumThreaded<T> *sourceDataBlock, int copySize){
    sourceDataBlock->popSampleSectionFast(copySize,this->dataArray);
//    //calculate cells to copy over
//    int cellsToCopy = copySize * sourceDataBlock->getChannelCount();

//    //get byte read offset from source
//    int sourceReadIdx = sourceDataBlock->getCurrentSampleReadIndex()+1;
//    int cellsRead = sourceDataBlock->getChannelCount()*sourceReadIdx;

//    //get byte write offset from here
//    int destWriteIdx = this->dataArray->getCurrentSampleWriteIndex();
//    int cellsWritten = sourceDataBlock->getChannelCount()*destWriteIdx;

//    memcpy(&this->dataArray->data[cellsWritten], &sourceDataBlock->data[cellsRead], cellsToCopy * sizeof(T));

//    for (int i = 0; i < copySize; ++i) {
//        this->dataArray->incrementWriteIndex();
//        sourceDataBlock->incrementReadIndex();
//    }
}

template <typename T>
void SpectrogramData<T>::fastAddDataSection(CircularArray2DSpectrumThreaded<T> *sourceDataBlock,int copySize){
    for (int i = 0; i < copySize; ++i) {
        this->addData(sourceDataBlock->popSample());
    }
}

template <typename T>
void SpectrogramData<T>::addDataSection(Array2DSpectrum<T> *blockData){
    for (int i = 0; i < blockData->getSampleCount(); ++i) {
        this->addData(blockData->getChannelsForSample(i));
    }
}

template <typename T>
void SpectrogramData<T>::resetData(){   
    for (int sampleIdx = 0; sampleIdx < dataArray->getSampleCount(); ++sampleIdx) {
        for (int channelIdx = 0; channelIdx < dataArray->getChannelCount(); ++channelIdx) {
            dataArray->set(channelIdx,sampleIdx,0);
        }
    }
}

template <typename T>
double SpectrogramData<T>::value( double x, double y ) const
{       
    int sample_idx = (int) x;
    int channel_idx = (int) y;
    sample_idx = sample_idx + dataArray->getCurrentSampleReadIndex() + 1; //since ReadIdx might have moved (and +1 because we always need to look at the next location)
    if(sample_idx>=dataArray->getSampleCount()){
        sample_idx = sample_idx - dataArray->getSampleCount();
    }
    //return (double) dataArray->at(channel_idx,sample_idx);
    return (double) dataArray->at(channel_idx,(this->dataArray->getSampleCount()-1)-sample_idx); //to reverse the plot
}

template class SpectrogramData<int>;
template class SpectrogramData<float>;
template class SpectrogramData<double>;


