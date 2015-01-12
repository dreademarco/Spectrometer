#include "fftwsequence.h"
using namespace std;

/*
 * Represtents a 2D Array in relation to a Spectrum. Logically, the array is stored as a 1D vector for speed.
 * The order of data input is interpreted as follows:
 *
 * [ | Ch.1 | Ch.2 | Ch.3 | Ch.4 | Ch.5 | .. | Ch.N || Ch.1 | Ch.2 | Ch.3 | Ch.4 | Ch.5 | .. | Ch.N || ]
 *
 * Therefore, all channels for a single sample are contiguous, followed by the channels for the next sample etc.
 *
 * A "row" represents the channels of an entire sample. A "column" represents a particular channel for all the samples.
 */

FFTWSequence::FFTWSequence(int channels, int samples)
{
    this->channels = channels;
    this->samples = samples;
    data = (fftwf_complex*) fftwf_malloc(channels * samples * sizeof(fftwf_complex));
    memset(data, 0, (channels*samples) * sizeof(fftwf_complex));
    temp_channeldata = (fftwf_complex*) fftwf_malloc(channels * sizeof(fftwf_complex));
    memset(temp_channeldata, 0, channels * sizeof(fftwf_complex));
    temp_sampledata = (fftwf_complex*) fftwf_malloc(samples * sizeof(fftwf_complex));
    memset(temp_sampledata, 0, samples * sizeof(fftwf_complex));
}

FFTWSequence::~FFTWSequence(){
    fftwf_free(data);
    fftwf_free(temp_sampledata); //temp_coldata;
    fftwf_free(temp_channeldata); //temp_rowdata;
    channels = 0;
    samples = 0;
}

int FFTWSequence::getChannelCount()
{
    return channels;
}

int FFTWSequence::getSampleCount()
{
    return samples;
}

fftwf_complex* FFTWSequence::getRawData(){
    return data;
}

int FFTWSequence::getLocation(int channel, int sample) const
{
    return ((sample*channels)+channel);
}

fftwf_complex* FFTWSequence::at(int channel, int sample)
{
    int loc = getLocation(channel,sample);
    return data+loc;
//    fftwf_complex* temp = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex));
//    temp[0][0] = data[getLocation(channel,sample)][0];
//    temp[0][1] = data[getLocation(channel,sample)][1];
//    return temp;
    //return data[getLocation(channel,sample)];
}

fftwf_complex* FFTWSequence::at(int channel, int sample) const
{
    return at(channel,sample);
}

void FFTWSequence::set(int channel, int sample, fftwf_complex value)
{
    data[getLocation(channel,sample)][0] = value[0];
    data[getLocation(channel,sample)][1] = value[1];
}

fftwf_complex* FFTWSequence::getChannelsForSample(int sampleIdx)
{
    int dataIdx = sampleIdx*channels;
    memcpy(temp_channeldata,data+dataIdx,channels*sizeof(fftwf_complex));
    return temp_channeldata;
//    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
//        temp_channeldata[channelIdx] = at(channelIdx,sampleIdx);
//    }
//    return temp_channeldata;
}

fftwf_complex* FFTWSequence::getSamplesForChannel(int channelIdx)
{
    for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx) {
        int data_idx = getLocation(channelIdx,sampleIdx);
        memcpy(temp_sampledata+sampleIdx,data+data_idx,sizeof(fftwf_complex));
//        temp_sampledata[sampleIdx] = at(channelIdx,sampleIdx);
    }
    return temp_sampledata;
}

void FFTWSequence::setSample(int sampleIdx, fftwf_complex* channelData)
{
    int dataIdx = sampleIdx*channels;
    memcpy(data+dataIdx,channelData,channels*sizeof(fftwf_complex));

//    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
//        set(channelIdx,sampleIdx,channelData[channelIdx]);
//    }
}

void FFTWSequence::setChannel(int channelIdx, fftwf_complex* sampleData)
{
    for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx) {
        set(channelIdx,sampleIdx,sampleData[sampleIdx]);
    }
}

FFTWSequence FFTWSequence::getSamplesSection(int sample_offset, int n_samples)
{
    FFTWSequence samplesSection(channels,n_samples);
    int dataIdx = sample_offset*channels;
    memcpy(samplesSection.data,this->data+dataIdx,n_samples*channels*sizeof(fftwf_complex));
    return samplesSection;

//    for (int sampleIdx = sample_offset; sampleIdx < n_samples; ++sampleIdx) {
//        samplesSection.setSample(sampleIdx, getChannelsForSample(sampleIdx));
//    }
//    return samplesSection;
}

FFTWSequence FFTWSequence::getChannelsSection(int channel_offset, int n_channels)
{
    FFTWSequence channelsSection(n_channels,samples);
    for (int channelIdx = channel_offset; channelIdx < n_channels; ++channelIdx) {
        channelsSection.setChannel(channelIdx,getSamplesForChannel(channelIdx));
    }
    return channelsSection;
}

void FFTWSequence::loadMemCpy(FFTWSequence *sourceData){
    int cellsToCopy = channels*samples;
    memcpy(data,sourceData->data,cellsToCopy*sizeof(fftwf_complex));
}

void FFTWSequence::toString(){
    for (int sampleIdx = 0; sampleIdx < this->getSampleCount(); ++sampleIdx) {
        for (int channelIdx = 0; channelIdx < this->getChannelCount(); ++channelIdx) {
            cout << this->at(channelIdx,sampleIdx)[0][0] << " , " << this->at(channelIdx,sampleIdx)[0][1] << endl; //"\t";
        }
        //cout << endl;
    }
}

void FFTWSequence::integration(int integrationFactor, FFTWSequence *output)
{
    fftwf_complex* output_array = output->data;
    fftwf_complex* input_array = this->data;

    int chans = output->getChannelCount();

    float* meandivisor = (float*) fftwf_malloc(sse_factor * 2 * sizeof(float));
    memset(meandivisor, 0, sse_factor * 2 * sizeof(float));
    for (int i = 0; i < sse_factor*2; ++i) {
        meandivisor[i] = (float) 1/integrationFactor;
    }

    int globalSamples = (int) ((samples/chans) / integrationFactor);
    //int globalSamples = (int) ((samples/nchans) / integrationFactor);
    memset(output_array, 0, chans * globalSamples * sizeof(fftwf_complex));

    if(integrationFactor==1){
        memcpy(output_array,input_array,chans*globalSamples*sizeof(fftwf_complex));
    }else
    {
        // Loop over number of samples (integrated)
        #pragma omp parallel
        {
            int threadId = omp_get_thread_num();            
            for (int s = threadId * globalSamples / nthreads;
                     s < (threadId + 1) * globalSamples / nthreads;
                     s++){
                //for (int s = 0; s < globalSamples; s++){
                    for (int i = 0; i < integrationFactor; i++)
                    {
                        // Loop over number of samples
                        int offset = (chans * (s * integrationFactor + i));
                        __m256* inSrc  = (__m256*) input_array[offset];
                        __m256* outSrc = (__m256*) output_array[s * chans];

                        for (int c=0; c < chans; c += sse_factor)
                        {
                            *outSrc  = _mm256_add_ps(*outSrc, *inSrc);
                            inSrc++;   // Update input pointer
                            outSrc++;   // Update output pointer
                        }
                    }

                    //Compute mean
                    __m256* outSrc = (__m256*) output_array[s * chans];
                    __m256* divisorSrc = (__m256*) meandivisor;
                    for (int c=0; c < chans; c += sse_factor)
                    {
                        *outSrc = _mm256_mul_ps(*outSrc,*divisorSrc);
                        outSrc++;
                    }
                }
        }
    }
    fftwf_free(meandivisor);
//    this->toString();
//    cout << "------------------" << endl;
//    output->toString();
 }

float FFTWSequence::magnitude()
{
    int highValue = 0;
    omp_set_num_threads(nthreads);
    int globalSamples = (int) (this->getSampleCount()*this->getChannelCount());
    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        for (int s = threadId * globalSamples / nthreads;
                 s < (threadId + 1) * globalSamples / nthreads;
                 s++){
            data[s][0] = sqrt(data[s][0]*data[s][0] + data[s][1]*data[s][1]);
            data[s][1] = 0;
            if(data[s][0]>highValue){
                highValue = data[s][0];
            }
            //cout << data[s][0] << endl;
        }
    }
    return highValue;
}
