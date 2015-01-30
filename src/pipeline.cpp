#include "pipeline.h"
using namespace std;

#define BUSY_WAIT    1

//Pipeline::Pipeline(CircularArray2DSpectrumThreaded<float> *sourceDataBlock, CircularArray2DSpectrumThreaded<float> *outputDataBlock, int chunkSize, int integrationFactor) : QThread(parent)
Pipeline::Pipeline(FFTWSequenceBuffer *outputDataBlock, int blockSize, int selectedIntegrationFactor, int selectedTaps, int selectedChans, int selectedFs, int selectedBufferSize, int selectedPort, int selectedSamplesPerPacket, QObject *parent) : QObject(parent){
    // detect CPU architecture and decide on SSE factor
    setupCPU();
    this->flowFinished = false;

    this->tobs_buffer = selectedBufferSize;
    this->srate = selectedFs;
    this->ntaps = selectedTaps;
    this->nchans = selectedChans;
    this->outputStream = outputDataBlock;
    this->blocks = blockSize;
    this->integrationFactor = selectedIntegrationFactor;
    this->placements = 0;
    this->prevPlacements = -1;
    this->loop = true;
    //this->numberOfAntennas = nantennas; //remove later (reset to 1)
    this->numberOfAntennas = 1; //remove later (reset to 1)
    this->samplesToGather = tobs_buffer;//*numberOfAntennas;
    this->tapSamples = (ntaps-1)*nchans;    
    this->port = selectedPort;
    this->samplesPerPacket = selectedSamplesPerPacket;
    this->packetsPerHeap = nchans;
    //this->inputWorkspace = new FFTWSequence(sourceDataBlock->getChannelCount(),tapSamples+samplesToGather);

    //this->inputWorkspace = new FFTWSequence(sourceDataBlock->getChannelCount(),samplesToGather);
    this->inputWorkspace = new FFTWSequence(nchans,samplesToGather);
    //this->inputWorkspace2 = new FFTWSequence(sourceDataBlock->getChannelCount(),samplesToGather);
    this->inputWorkspace2 = new FFTWSequence(nchans,samplesToGather);
    //this->outputWorkspace = new FFTWSequence(outputDataBlock->getChannelCount(),(samplesToGather/outputDataBlock->getChannelCount())/integrationFactor);
    //this->outputWorkspace = new FFTWSequence(nchans,(samplesToGather/nchans)/integrationFactor);
    this->outputWorkspace = new FFTWSequence(nchans,samplesToGather/integrationFactor);
    //this->ppf = new StreamingPPF(inputWorkspace,inputWorkspace2,ntaps,nchans,nblocks,srate,tobs_buffer,StreamingPPF::HANN);
    //this->ppf = new StreamingPPF(inputWorkspace,inputWorkspace2,ntaps,nchans,nblocks,srate,tobs_buffer,StreamingPPF::HANN);
    this->ppf = new PPF(inputWorkspace, inputWorkspace2, ntaps, nchans, PPF::HANN);

    // Initialise Circular Buffer and set thread priority
    doubleBuffer = new DoubleBuffer(numberOfAntennas, nchans, samplesPerPacket);
    doubleBuffer->start();
    doubleBuffer->setPriority(QThread::TimeCriticalPriority);

    // Initialise UDP Chunker and set thread priority
    // heap is 1024 channels X 1024 samples, so one packet is one channel from the whole heap

    chunker = new PacketChunker(port, numberOfAntennas, nchans, samplesPerPacket, packetsPerHeap);
    chunker->setDoubleBuffer(doubleBuffer);
    chunker->start();
    chunker->setPriority(QThread::TimeCriticalPriority);
}

Pipeline::~Pipeline()
{
    chunker->terminate();
    doubleBuffer->terminate();
    delete doubleBuffer;
    delete chunker;
    delete inputWorkspace;
    delete inputWorkspace2;
    delete outputWorkspace;
    delete ppf;
    placements = 0;
    prevPlacements = -1;
}

void Pipeline::setupTermination(){
    loop=false;
}

void Pipeline::setupCPU(){
    // Set number of openmp threads
    omp_set_dynamic(0);
    omp_set_num_threads(nthreads);
}

void Pipeline::halt(){
    pause=true;
}

void Pipeline::resume(){
    loop=true;
    pause=false;
}

void Pipeline::start()
{
    omp_set_num_threads(nthreads);

//    // Initialise Circular Buffer and set thread priority
//    //int circ_buffer_nsamp = samplesToGather/numberOfAntennas;
//    DoubleBuffer doubleBuffer(numberOfAntennas, nchans, samplesPerPacket);
//    doubleBuffer.start();
//    doubleBuffer.setPriority(QThread::TimeCriticalPriority);

//    // Initialise UDP Chunker and set thread priority
//    // heap is 1024 channels X 1024 samples, so one packet is one channel from the whole heap

//    PacketChunker chunker(port, numberOfAntennas, nchans, samplesPerPacket, packetsPerHeap);
//    //PacketChunker chunker(port, numberOfAntennas, nchans, samplesPerPacket, nchans);
//    chunker.setDoubleBuffer(&doubleBuffer);
//    chunker.start();
//    chunker.setPriority(QThread::TimeCriticalPriority);

    while(loop){
        flowFinished = false;
        if(!pause){
            //cout << "pipeline starting..." << endl;

            // Get pointer to next buffer
            unsigned char *udpBuffer = doubleBuffer->prepareRead(&timestamp, &sampRate);

            // Copy UDP data to buffer - IMPROVE THIS SHIT OR DIE
            int heapSamples = samplesToGather * nchans;
            #pragma omp parallel
            {
                int threadId = omp_get_thread_num();
                for (int s = threadId * heapSamples / nthreads;
                         s < (threadId + 1) * heapSamples / nthreads;
                         s++){
                    unsigned char value_real = udpBuffer[s*2];
                    unsigned char value_imag = udpBuffer[(s*2)+1];
                    float value_real_f = (float) value_real;
                    float value_imag_f = (float) value_imag;
                    inputWorkspace->data[s][0] = value_real_f;
                    inputWorkspace->data[s][1] = value_imag_f;
                }
            }

//            // Sample data
//            for (int c = 0; c < nchans; ++c) {
//                for (int s = 0; s < samplesToGather; ++s) {
//                    inputWorkspace->data[(c*samplesToGather)+s][0] = c;
//                }
//            }

            // Done reading from buffer
            doubleBuffer.readReady();

            //Pipeline
            // Step (1): Apply PPF
            ppf->applyPPF();
            //memcpy(inputWorkspace2->data,inputWorkspace->data,samplesToGather * nchans * sizeof(fftwf_complex));

            // Step (2): Perform integration
            doIntegration();
            //memcpy(outputWorkspace->data,inputWorkspace2->data,samplesToGather * nchans * sizeof(fftwf_complex));

            // Step (3): Output results
            fastLoadDataToOutputStreamMemCpy();
        }else{
            //cout << "pipeline sleeping..." << endl;
            sleep(BUSY_WAIT);
        }
    }
    flowFinished = true;
}

void Pipeline::doIntegration(){
    inputWorkspace2->integration(integrationFactor,outputWorkspace);    
}

void Pipeline::doMagnitude(){
    outputWorkspace->magnitude();
}

void Pipeline::fastLoadDataToOutputStreamMemCpy(){
    outputStream->addSection(outputWorkspace);
}
