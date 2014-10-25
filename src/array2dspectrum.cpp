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
//    if(channel>=getChannelCount()){
//        return -1;
//    }
//    else if(sample>=getSampleCount()){
//        return -1;
//    }else{
//        return data[getLocation(channel,sample)];
//    }
    return data[getLocation(channel,sample)];
}

template <typename T>
T Array2DSpectrum<T>::at(int channel, int sample) const
{
    return at(channel,sample);
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

//template <typename T>
//void Array2DSpectrum<T>::integration(int integrationFactor, Array2DSpectrum<T> *output){
//    int channelSize = output->getChannelCount();
//    float summed[channelSize];
//    int loopsize = samples/integrationFactor;
//    int offset = 0; int sub_offset = 0;
//    for (int i = 0; i < loopsize; ++i) {
//        for (int j = 0; j < integrationFactor; ++j) {
//            sub_offset = j*channelSize;
//            for (int k = 0; k < channelSize; ++k) {
//                summed[k]=summed[k]+this->data[sub_offset+k];
//            }
//        }
//        offset = i*channels;
//        for (int k = 0; k < channelSize; ++k) {
//            output->data[k+offset] = summed[k]/integrationFactor;
//            summed[k] = 0;
//        }
//    }
//}

template <typename T>
void Array2DSpectrum<T>::integration(int integrationFactor, Array2DSpectrum<T> *output)
{
    T* output_array = (T*) __builtin_assume_aligned(output -> data, 16);
    T* input_array  = (T*) __builtin_assume_aligned(this -> data, 16);
    int globalSamples = (int) (samples / integrationFactor);

    memset(output_array, 0, channels * globalSamples * sizeof(T));

    int numThreads = 2;
    omp_set_num_threads(numThreads);

    // Loop over number of samples (integrated)
    #pragma omp parallel shared(output_array, input_array)
    {
        int threadId = omp_get_thread_num();
        for (unsigned s = threadId * globalSamples / numThreads;
                      s < (threadId + 1) * globalSamples / numThreads;
                      s++)
        //for (unsigned s = 0; s < globalSamples; s++)
        {
            // Loop over integration factor
            for (unsigned i = 0; i < integrationFactor; i++)
            {
                // Loop over number of samples
                int offset = channels * (s * integrationFactor + i);
                for (unsigned c = 0; c < channels; c += 4)
                {
                    output_array[s * channels + c]     += *(input_array + offset + c);
                    output_array[s * channels + c + 1] += *(input_array + offset + c + 1);
                    output_array[s * channels + c + 2] += *(input_array + offset + c + 2);
                    output_array[s * channels + c + 3] += *(input_array + offset + c + 3);
                }
            }
            for (unsigned c = 0; c < channels; c += 4)
            {
                output_array[s * channels + c] /= integrationFactor;
                output_array[s * channels + c + 1]  /= integrationFactor;
                output_array[s * channels + c + 2]  /= integrationFactor;
                output_array[s * channels + c + 3]  /= integrationFactor;
            }
        }
    }
 }

//template <typename T>
//void Array2DSpectrum<T>::integration(int integrationFactor, Array2DSpectrum<T> *output){
//    int loop_size = samples/integrationFactor;
//    float integration_mean;
//    int offset = 0;
//    int loc_counter = 0;
//    int offset_factor = integrationFactor*channels;
//    for (int i = 0; i < loop_size; ++i) {
//        offset = i*offset_factor;
//        for (int j = 0; j < channels; ++j) {
//            integration_mean = 0;
//            //cout << (0*channels)+j+offset << "\t" << (1*channels)+j+offset << endl;
//            for (int k = 0; k < integrationFactor; ++k) {
//                integration_mean = integration_mean + this->data[(k*channels)+j+offset];
//            }
//            //integration_mean = integration_mean / integrationFactor;
//            output->data[loc_counter] = integration_mean / integrationFactor;
//            ++loc_counter;
//        }
//    }
////    cout << loc_counter << endl;
////    cout << loop_size << endl;
//}

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
