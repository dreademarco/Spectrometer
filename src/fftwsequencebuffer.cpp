#include "fftwsequencebuffer.h"
using namespace std;

FFTWSequenceBuffer::FFTWSequenceBuffer(int channels, int samplesPerSection, int bufferSections)
{    
    int totalSpace = channels * samplesPerSection * bufferSections;
    data = (fftwf_complex*) fftwf_malloc(totalSpace * sizeof(fftwf_complex));
    memset(data, 0, totalSpace * sizeof(fftwf_complex));

    totalSections = bufferSections;
    this->channels = channels;
    this->samplesPerSection = samplesPerSection;
    sectionWriteIdx = 0;
    sectionReadIdx = totalSections;
}

FFTWSequenceBuffer::~FFTWSequenceBuffer()
{
    fftwf_free(data);
}

void FFTWSequenceBuffer::flush(){
    int totalSpace = channels * samplesPerSection * totalSections;
    memset(data, 0, totalSpace * sizeof(fftwf_complex));
    sectionWriteIdx = 0;
    sectionReadIdx = totalSections;
}

void FFTWSequenceBuffer::addSection(FFTWSequence *input){
    mutex.lock();
        if(sectionWriteIdx==totalSections){
            sectionWriteIdx = 0; //reset
        }
    mutex.unlock();

    int writeOffset = samplesPerSection*channels*sectionWriteIdx;
    int writeCells = channels*samplesPerSection;
    memcpy(data+writeOffset,input->data,writeCells*sizeof(fftwf_complex));

    mutex.lock();
        sectionWriteIdx++;
    mutex.unlock();
}

void FFTWSequenceBuffer::popSection(FFTWSequence *output){
    mutex.lock();
        if(sectionReadIdx==totalSections){
            sectionReadIdx = 0; //reset
        }
    mutex.unlock();

    int readOffset = samplesPerSection*channels*sectionReadIdx;
    int readCells = channels*samplesPerSection;
    memcpy(output->data,data+readOffset,readCells*sizeof(fftwf_complex));

    mutex.lock();
        sectionReadIdx++;
    mutex.unlock();
}
