#ifndef FFTWSEQUENCE_H
#define FFTWSEQUENCE_H

#include <cstddef>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <common.h>

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

class FFTWSequence
{
public:
    FFTWSequence(int channels, int samples);
    ~FFTWSequence();
    fftwf_complex* data;

    int getChannelCount();
    int getSampleCount();
    fftwf_complex* getRawData();
    fftwf_complex* at(int channel, int sample);
    fftwf_complex* at(int channel, int sample) const;
    void set(int channel, int sample, fftwf_complex value);
    fftwf_complex* getChannelsForSample(int sampleIdx);
    fftwf_complex* getSamplesForChannel(int channelIdx);
    void setSample(int sampleIdx, fftwf_complex* channelData);
    void setChannel(int channelIdx, fftwf_complex* sampleData);
    //FFTWSequence getSamplesSection(int sample_offset, int n_samples);
    //FFTWSequence getChannelsSection(int channel_offset, int n_channels);
    //void loadMemCpy(FFTWSequence *sourceData);
    void integration(int integrationFactor, FFTWSequence *output);
    float magnitude();
    //void toString();


private:
    int channels;
    int samples;
    fftwf_complex* temp_channeldata;
    fftwf_complex* temp_sampledata;
    int getLocation(int channel, int sample) const;
};

#endif // FFTWSEQUENCE_H
