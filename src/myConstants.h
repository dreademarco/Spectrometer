#ifndef MYCONSTANTS_H
#define MYCONSTANTS_H

// CPU CONFIG
const int sse_factor = 2;
const int nthreads = 1;

// OPTIMIZATION CONFIG
const int nblocks = 16;

// general global variables
const int tobs = 8000;
//const int tobs = 8;

//const int integrationfactor = 1;

//const int tobs_buffer = ((nblocks * nthreads * nchans)/srate)*2;
//const int tobs_buffer = 16;

// plotter variables
//const int spectSize = (((tobs_buffer*srate)/integrationfactor)/nchans); //samples to plot in a spectrogram (spectSize samples with nchans each)
//const int spectSize = 64; //samples to plot in a spectrogram (spectSize samples with nchans each)
//const int guiUpdateSize = spectSize; // samples to process before GUI progress update
//const int samplesprocessed = spectSize; //collect and process data from buffer at a different rate

#endif // MYCONSTANTS_H
