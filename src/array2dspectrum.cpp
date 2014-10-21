#include "array2dspectrum.h"
using namespace std;

/*
 * Represtents a 2D Array in relation to a Spectrum. Logically, the array is stored as a single vector for speed.
 * The order of data input is interpreted as follows:
 *
 * [ | Ch.1 | Ch.2 | Ch.3 | Ch.4 | Ch.5 | .. | Ch.N || Ch.1 | Ch.2 | Ch.3 | Ch.4 | Ch.5 | .. | Ch.N || ]
 *
 * Therefore, all channels for a single sample are contiguous, followed by the channels for the next sample etc.
 *
 * A "row" represents the channels of an entire sample. A "column" represents a particular channel for all the samples.
 */

template <typename T>
Array2DSpectrum<T>::Array2DSpectrum()
{
    data = NULL;
    channels = 0; //rows
    samples = 0; //columns
    temp_channeldata = NULL;
    temp_sampledata = NULL;
}

template <typename T>
Array2DSpectrum<T>::Array2DSpectrum(int channels, int samples)
{
    this->channels = channels;
    this->samples = samples;
    data = new T[channels*samples];
    temp_channeldata = new T[channels];
    temp_sampledata = new T[samples];
}

template <typename T>
Array2DSpectrum<T>::~Array2DSpectrum(){
    delete [] data;
    delete [] temp_channeldata; //temp_rowdata;
    delete [] temp_sampledata; //temp_coldata;
    channels = 0;
    samples = 0;
}

template <typename T>
int Array2DSpectrum<T>::getChannelCount()
{
    return channels;
}

template <typename T>
int Array2DSpectrum<T>::getSampleCount()
{
    return samples;
}

template <typename T>
T* Array2DSpectrum<T>::getRawData(){
    return data;
}

template <typename T>
int Array2DSpectrum<T>::getLocation(int channel, int sample) const
{
    return ((sample*channels)+channel);
}

template <typename T>
T Array2DSpectrum<T>::at(int channel, int sample)
{
    if(channel>=getChannelCount()){
        return -1;
    }
    else if(sample>=getSampleCount()){
        return -1;
    }else{
        return data[getLocation(channel,sample)];
    }
}

template <typename T>
T Array2DSpectrum<T>::at(int channel, int sample) const
{
    return at(channel,sample);
//    if(channel>getChannelCount()){
//        return -1;
//    }
//    else if(sample>getSampleCount()){
//        return -1;
//    }else{
//        return data[getLocation(channel,sample)];
//    }
}

template <typename T>
void Array2DSpectrum<T>::set(int channel, int sample, T value)
{
    data[getLocation(channel,sample)] = value;
}

template <typename T>
T* Array2DSpectrum<T>::getChannelsForSample(int sampleIdx)
{
    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
        temp_channeldata[channelIdx] = at(channelIdx,sampleIdx);
    }
    return temp_channeldata;
}

template <typename T>
T* Array2DSpectrum<T>::getSamplesForChannel(int channelIdx)
{
    for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx) {
        temp_sampledata[sampleIdx] = at(channelIdx,sampleIdx);
    }
    return temp_sampledata;
}

template <typename T>
void Array2DSpectrum<T>::setSample(int sampleIdx, T* channelData)
{
    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
        set(channelIdx,sampleIdx,channelData[channelIdx]);
    }
}

template <typename T>
void Array2DSpectrum<T>::setChannel(int channelIdx, T* sampleData)
{
    for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx) {
        set(channelIdx,sampleIdx,sampleData[sampleIdx]);
    }
}

template <typename T>
Array2DSpectrum<T> Array2DSpectrum<T>::getSamplesSection(int sample_offset, int n_samples)
{
    Array2DSpectrum<T> samplesSection(channels,n_samples);
    for (int sampleIdx = sample_offset; sampleIdx < n_samples; ++sampleIdx) {
        samplesSection.setSample(sampleIdx, getChannelsForSample(sampleIdx));
    }
    return samplesSection;
}

template <typename T>
Array2DSpectrum<T> Array2DSpectrum<T>::getChannelsSection(int channel_offset, int n_channels)
{
    Array2DSpectrum<T> channelsSection(n_channels,samples);
    for (int channelIdx = channel_offset; channelIdx < n_channels; ++channelIdx) {
        channelsSection.setChannel(channelIdx,getSamplesForChannel(channelIdx));
    }
    return channelsSection;
}

template <typename T>
void Array2DSpectrum<T>::loadMemCpy(Array2DSpectrum<T> *sourceData){
    int cellsToCopy = channels*samples;
    memcpy(&this->data[0],&sourceData->data[0],cellsToCopy*sizeof(T));
}

template <typename T>
Array2DSpectrum<T> Array2DSpectrum<T>::transpose(){
    Array2DSpectrum<T> transposed(samples,channels);
    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
        for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx) {
            transposed.set(sampleIdx,channelIdx,at(channelIdx,sampleIdx));
        }
    }
    return transposed;
}

template <typename T>
void Array2DSpectrum<T>::toString(){
    for (int sampleIdx = 0; sampleIdx < this->getSampleCount(); ++sampleIdx) {
        for (int channelIdx = 0; channelIdx < this->getChannelCount(); ++channelIdx) {
            cout << this->at(channelIdx,sampleIdx) << "\t";
        }
        cout << endl;
    }
}

template class Array2DSpectrum<int>;
template class Array2DSpectrum<float>;
template class Array2DSpectrum<double>;
