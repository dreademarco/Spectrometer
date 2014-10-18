#ifndef ARRAY2DSPECTRUM_H
#define ARRAY2DSPECTRUM_H
#include <cstddef>
#include <iostream>
#include <stdio.h>

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
class Array2DSpectrum
{
public:
    Array2DSpectrum();
    Array2DSpectrum(int channels, int samples);
    ~Array2DSpectrum();

    int getChannelCount();
    int getSampleCount();
    T* getRawData();

    T at(int channel, int sample);
    T at(int channel, int sample) const;
    void set(int channel, int sample, T value);
    T* getChannelsForSample(int sampleIdx);
    T* getSamplesForChannel(int channelIdx);
    void setSample(int sampleIdx, T* channelData);
    void setChannel(int channelIdx, T* sampleData);

    Array2DSpectrum<T> getSamplesSection(int sample_offset, int n_samples);
    Array2DSpectrum<T> getChannelsSection(int channel_offset, int n_channels);
    Array2DSpectrum<T> transpose();

    void toString();

    T *data;

private:    
    int channels;
    int samples;
    T *temp_channeldata;
    T *temp_sampledata;

    int getLocation(int channel, int sample) const;
};

#endif // ARRAY2DSPECTRUM_H
