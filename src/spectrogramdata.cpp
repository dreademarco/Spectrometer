#include "spectrogramdata.h"
using namespace std;

SpectrogramData::SpectrogramData(FFTWSequenceCircular *data, int freq_bins,int samples) : QwtRasterData()
{
    highestIntensity = 1.0; //100.0
    setInterval(Qt::XAxis, QwtInterval(0,samples-1));
    setInterval(Qt::YAxis, QwtInterval(0,freq_bins-1));
    setInterval(Qt::ZAxis, QwtInterval(0.0,highestIntensity));
    //setInterval( Qt::ZAxis, QwtInterval(0.0,1.0));
    dataArray = data;
}

void SpectrogramData::addData(fftwf_complex* sampleData){
    dataArray->pushSample(sampleData);
    if(dataArray->getRefillCount()>0){
        dataArray->incrementReadIndex();
    }
}

void SpectrogramData::fastAddDataSectionMemCpy(FFTWSequenceCircularThreaded *sourceDataBlock, int copySize){    
    sourceDataBlock->fastPopBlockSamples(this->dataArray,copySize);
    this->dataArray->incrementReadIndex(copySize);
//    for (int i = 0; i < copySize; ++i) {
//        this->dataArray->incrementReadIndex();
//    }
}

void SpectrogramData::fastAddDataSection(FFTWSequenceCircularThreaded *sourceDataBlock, int copySize){
    for (int i = 0; i < copySize; ++i) {
        this->addData(sourceDataBlock->loadSample());
    }
}

void SpectrogramData::addDataSection(FFTWSequence *blockData){
    for (int i = 0; i < blockData->getSampleCount(); ++i) {
        this->addData(blockData->getChannelsForSample(i));
    }
}

void SpectrogramData::resetData(){
    for (int sampleIdx = 0; sampleIdx < dataArray->getSampleCount(); ++sampleIdx) {
        for (int channelIdx = 0; channelIdx < dataArray->getChannelCount(); ++channelIdx) {
            dataArray->set(channelIdx,sampleIdx,0);
        }
    }
}

//double SpectrogramData::value( double x, double y ) const
//{
//    int sample_idx = (int) x;
//    int channel_idx = (int) y;
//    sample_idx = sample_idx + dataArray->getCurrentSampleReadIndex() + 1; //since ReadIdx might have moved (and +1 because we always need to look at the next location)
//    if(sample_idx>=dataArray->getSampleCount()){
//        sample_idx = sample_idx - dataArray->getSampleCount();
//    }
//    //return (double) dataArray->at(channel_idx,sample_idx);
//    //return (double) dataArray->at(channel_idx,(this->dataArray->getSampleCount()-1)-sample_idx); //to reverse the plot

//    // return abs of complex value
//    fftwf_complex* value;
//    value = dataArray->at(channel_idx,(this->dataArray->getSampleCount()-1)-sample_idx);
//    return (double) value[0][0];
//}


void SpectrogramData::fixIntensityInterval(double value)
{
    if(value>highestIntensity){
        highestIntensity = value;
        this->setInterval( Qt::ZAxis, QwtInterval(0.0,highestIntensity));
    }
}

double SpectrogramData::value( double x, double y ) const
{
    int sample_idx = (int) x;
    int channel_idx = (int) y;
    sample_idx = sample_idx + dataArray->getCurrentSampleReadIndex() + 1; //since ReadIdx might have moved (and +1 because we always need to look at the next location)
    sample_idx = sample_idx % dataArray->getSampleCount();

    fftwf_complex* value;    
    value = dataArray->at(channel_idx,sample_idx);
    return (double) value[0][0];
}
