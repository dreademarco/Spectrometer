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
    memset(data, 0, channels * samples * sizeof(fftwf_complex));
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
    return ((channel*samples)+sample);
    //return ((sample*channels)+channel);
}

fftwf_complex* FFTWSequence::at(int channel, int sample)
{
    int loc = getLocation(channel,sample);
    return data+loc;
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

//good
fftwf_complex* FFTWSequence::getSamplesForChannel(int channelIdx)
{
    int dataIdx = channelIdx*samples;
    memcpy(temp_sampledata,data+dataIdx,samples*sizeof(fftwf_complex));
    return temp_sampledata;
}

//good
fftwf_complex* FFTWSequence::getChannelsForSample(int sampleIdx)
{
    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
        int data_idx = getLocation(channelIdx,sampleIdx);
        memcpy(temp_sampledata+channelIdx,data+data_idx,sizeof(fftwf_complex));
    }
    return temp_channeldata;
}

void FFTWSequence::setSample(int sampleIdx, fftwf_complex* channelData)
{
    for (int channelIdx = 0; channelIdx < channels; ++channelIdx) {
        set(channelIdx,sampleIdx,channelData[channelIdx]);
    }
}

void FFTWSequence::setChannel(int channelIdx, fftwf_complex* sampleData)
{
    int dataIdx = channelIdx*samples;
    memcpy(data+dataIdx,sampleData,samples*sizeof(fftwf_complex));
}

void FFTWSequence::integration(int integrationFactor, FFTWSequence *output)
{
    int chans = output->getChannelCount();
    float divisor = (float) 1/integrationFactor;

    memset(output->data, 0, (samples/integrationFactor) * chans * sizeof(fftwf_complex));

    if(integrationFactor==1){
        memcpy(output->data,data,chans*samples*sizeof(fftwf_complex));
    }else
    {
        // Loop over number of channels
        #pragma omp parallel
        {
            int threadId = omp_get_thread_num();
            for (int c = threadId * channels / nthreads;
                     c < (threadId + 1) * channels / nthreads;
                     c++){
            //for (int c = 0; c < channels; ++c) {
                int channel_offset_input = c*samples;
                int channel_offset_output = c*(samples/integrationFactor);

                // Loop over samples (integrated)
                int sampleCount = 0;
                for (int s = 0; s < samples; s+=integrationFactor) {
                    for (int i = 0; i < integrationFactor; ++i) {
                        output->data[channel_offset_output+sampleCount][0] += this->data[channel_offset_input+s+i][0];
                        output->data[channel_offset_output+sampleCount][1] += this->data[channel_offset_input+s+i][1];
                    }
                    output->data[channel_offset_output+sampleCount][0] *= divisor;
                    output->data[channel_offset_output+sampleCount][1] *= divisor;
                    sampleCount++;
                }
            }
        }
    }
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
        }
    }
    return highValue;
}
