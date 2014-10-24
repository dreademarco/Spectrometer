#ifndef MYCONSTANTS_H
#define MYCONSTANTS_H
//const int DataSize = 100000;
//const int BufferSize = 8192;


const int overallSamples = 5000000; //total samples to generate
const int samplesSize = 5000000; //max samples in buffer
const int spectSize = 1000; //samples to plot
const int guiUpdateSize = 500; //samples to process before GUI progress update
const int block = 500; //block size for locking
const int freqBins = 256; //spectrogram has 256 frequency bins
const int integrationfactor = 20;

#endif // MYCONSTANTS_H
