#ifndef FFTWSEQUENCEBUFFER_H
#define FFTWSEQUENCEBUFFER_H

#include <common.h>

class FFTWSequenceBuffer
{
public:
    FFTWSequenceBuffer(int channels, int samplesPerSection, int bufferSections);
    ~FFTWSequenceBuffer();

    fftwf_complex* data;
    void addSection(FFTWSequence *input);
    void popSection(FFTWSequence *output);

private:
    int sectionWriteIdx;
    int sectionReadIdx;
    int totalSections;
    int samplesPerSection;
    int channels;
    QMutex mutex;
};

#endif // FFTWSEQUENCEBUFFER_H
