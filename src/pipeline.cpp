#include "pipeline.h"
using namespace std;

//Pipeline::Pipeline(CircularArray2DSpectrumThreaded<float> *sourceDataBlock, CircularArray2DSpectrumThreaded<float> *outputDataBlock, int chunkSize, int integrationFactor) : QThread(parent)
Pipeline::Pipeline(FFTWSequenceCircularThreaded *sourceDataBlock, FFTWSequenceCircularThreaded *outputDataBlock, int blockSize, int selectedIntegrationFactor, int selectedTaps, int selectedChans, int selectedFs, int selectedBufferTobs, int selectedPort, int selectedSamplesPerPacket, QObject *parent) : QObject(parent){
    // detect CPU architecture and decide on SSE factor
    setupCPU();

    this->tobs_buffer = selectedBufferTobs;
    this->srate = selectedFs;
    this->ntaps = selectedTaps;
    this->nchans = selectedChans;
    this->sourceStream = sourceDataBlock;
    this->outputStream = outputDataBlock;
    this->blocks = blockSize;
    this->integrationFactor = selectedIntegrationFactor;
    this->placements = 0;
    this->prevPlacements = -1;
    this->loop = true;
    this->samplesToGather = srate*tobs_buffer;
    this->tapSamples = (ntaps-1)*nchans;
    this->numberOfAntennas = 1;
    this->port = selectedPort;
    this->samplesPerPacket = selectedSamplesPerPacket;
    //this->inputWorkspace = new FFTWSequence(sourceDataBlock->getChannelCount(),tapSamples+samplesToGather);

    this->inputWorkspace = new FFTWSequence(sourceDataBlock->getChannelCount(),samplesToGather);
    this->inputWorkspace2 = new FFTWSequence(sourceDataBlock->getChannelCount(),samplesToGather);
    this->outputWorkspace = new FFTWSequence(outputDataBlock->getChannelCount(),(samplesToGather/outputDataBlock->getChannelCount())/integrationFactor);
    this->ppf = new StreamingPPF(inputWorkspace,inputWorkspace2,ntaps,nchans,nblocks,srate,tobs_buffer,StreamingPPF::HANN);    
}

Pipeline::~Pipeline()
{
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

void Pipeline::start()
{
    omp_set_num_threads(nthreads);
    //cout << omp_get_max_threads() << endl;
    int nsamp = tobs * srate;
    int totalSamples = (nsamp/nchans)/integrationFactor;

    // Initialise Circular Buffer and set thread priority
    DoubleBuffer doubleBuffer(numberOfAntennas, nchans, nsamp);
    doubleBuffer.start();
    doubleBuffer.setPriority(QThread::TimeCriticalPriority);

    // Initialise UDP Chunker and set thread priority
    PacketChunker chunker(port, numberOfAntennas, nchans, samplesPerPacket, nchans);
    chunker.setDoubleBuffer(&doubleBuffer);
    chunker.start();
    chunker.setPriority(QThread::TimeCriticalPriority);

    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
    while(loop){
        // read raw bytes from DAQ
        //unsigned char *udpBuffer = doubleBuffer.prepareRead(&timestamp, &sampRate);

        // Step (1): Copy next available block to inputWorkspace
        int processed = fastLoadDataInWorkSpaceMemCpy();

        // Step (2): Apply PPF
        ppf->applyPPF();

        // Step (3): Perform Integration and update count
        doIntegration();

        int samples = (processed/nchans)/integrationFactor;
        prevPlacements = placements;
        placements = placements + samples;

        // Step (4): Calculate magnitude spectrum
        //doMagnitude();

        // Step (5): Output results
        fastLoadDataToOutputStreamMemCpy(samples);

        if(placements==totalSamples){
            loop=false;
        }
    }

    gettimeofday(&t2, NULL);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << "Pipeline elasped time: " << elapsedTime << " ms" << endl;
    if(elapsedTime>0){
        float datarate = ((((nsamp)/elapsedTime)/1000000))*1000.0;
        cout << "Pipeline Data rate est.: " << datarate << " Mhz" << endl;
        cout << "Finished with " << placements << " samples plotted" << endl;
    }else{
        cout << "Pipeline Data rate est.: n/a"  << " Mhz" << endl;
    }
    emit done();
}

int Pipeline::fastLoadDataInWorkSpaceMemCpy(){
    bool copyMade = false; //do not return from this function until a copy has been made to memory
    while(!copyMade){
        int currentUsed = sourceStream->getNumUsedSpaces();
        if(currentUsed>=samplesToGather){
            sourceStream->fastPopBlockSamples(inputWorkspace,samplesToGather);
            copyMade = true;
            return samplesToGather;
        }
        else if(currentUsed<samplesToGather){
            sourceStream->fastPopBlockSamples(inputWorkspace,currentUsed);
            copyMade = true;
            return currentUsed;
        }
    }
}

void Pipeline::doIntegration(){
    inputWorkspace2->integration(integrationFactor,outputWorkspace);
    //inputWorkspace->integration(integrationFactor,outputWorkspace); //use when ppf is bypassed
}

void Pipeline::doMagnitude(){
    outputWorkspace->magnitude();
}

void Pipeline::fastLoadDataToOutputStreamMemCpy(int samplesToPush){
    outputStream->fastPushBlockSamples(outputWorkspace,samplesToPush);
}
