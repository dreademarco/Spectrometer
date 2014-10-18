#ifndef MYCONSTANTS_H
#define MYCONSTANTS_H
//const int DataSize = 100000;
//const int BufferSize = 8192;


const int overallSamples = 5000000; //total samples to generate
const int samplesSize = 10000; //max samples in buffer
const int spectSize = 5000; //samples to plot
const int guiUpdateSize = 5000; //samples to process before GUI progress update
const int block = 100; //block size for locking
const int freqBins = 255; //spectrogram has 256 frequency bins
//const int batch = freqBins*1000;

#endif // MYCONSTANTS_H
