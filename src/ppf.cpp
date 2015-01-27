#include "ppf.h"

PPF::PPF(FFTWSequence* input, FFTWSequence* output, int n_taps, int fft_points, window selected_window)
{
    // configure PPF settings
    this->n_taps = n_taps;
    //this->n_taps = 1;
    this->n_fft = fft_points;
    this->n_samp = input->getSampleCount();
    this->n_chansInput = input->getChannelCount();
    this->n_coefficients = n_taps*n_fft;
    this->m_window = selected_window;

    //register input and output streams
    inputData = input->data;
    outputData = output->data;

    // calculate tap width for a perticular channel
    int tapSectionSize = n_taps*n_fft;
    int untappedSectionSize = n_fft;

    /* Create Input buffer (first tap is also the first n_samp chunk of the signal)
     * We will store a tap section for every channel, with a total of n_fft channels
    */
    inputBuffer = (fftwf_complex*) fftwf_malloc(n_chansInput * tapSectionSize * sizeof(fftwf_complex));
    memset(inputBuffer, 0, n_chansInput * tapSectionSize * sizeof(fftwf_complex));

    // Create Output buffer - as in input, we have an output section per channel
    outputBuffer = (fftwf_complex*) fftwf_malloc(n_chansInput * untappedSectionSize * sizeof(fftwf_complex));
    memset(outputBuffer, 0, n_chansInput * untappedSectionSize * sizeof(fftwf_complex));

    // Create fftw_in for all chans
    fftw_in = (fftwf_complex*) fftwf_malloc(n_chansInput * untappedSectionSize * sizeof(fftwf_complex));
    memset(fftw_in, 0, n_chansInput * untappedSectionSize * sizeof(fftwf_complex));

    // create FFTW Wisdom
    setupFFTWWisdom();

    // load FFTW plan per thread
    plans = (fftwf_plan*) fftwf_malloc(nthreads * sizeof(fftwf_plan));
    loadFFTWPlans();

    // create weighting coefficients for this setup
    filterCoeffs = (float*) fftwf_malloc(n_coefficients*2 * sizeof(float)); //*2 for contiguous real/complex weights
    memset(filterCoeffs, 0, n_coefficients*2 * sizeof(float));
    ppfcoefficients();
}

PPF::~PPF()
{
    fftwf_free(filterCoeffs);
    fftwf_free(inputBuffer);
    fftwf_free(outputBuffer);
    fftwf_free(fftw_in);

    // free plans
    for (int i = 0; i < nthreads; ++i) {
        fftwf_destroy_plan(plans[i]);
    }
    fftwf_free(plans);
}

void PPF::applyPPF()
{
    #pragma omp parallel
    {
        // Get thread id
        int threadID = omp_get_thread_num();

        //split these between threads, instead of //for (int chan = 0; chan < n_chansInput; ++chan) {
        for (int chan = threadID * n_chansInput / nthreads;
                 chan < (threadID + 1) * n_chansInput / nthreads;
                 chan++){
            fftwf_plan plan = plans[threadID];

            int chanOffsetInputBuffer = chan * (n_taps*n_fft);
            int chanOffsetOutputBuffer = chan * n_fft;

            // start up pre-tap data before rest of input stream
            loadFromInputStream(chan);

            // Reset fftw_in buffer
            memset(fftw_in + chanOffsetOutputBuffer, 0, n_fft * sizeof(fftwf_complex));

            void *fifo_ptrs[n_taps]; // an array of pointers to locations that contain fftwf_complex data

            for(int t = 0; t < n_taps; t++){
                fifo_ptrs[t] = inputBuffer + (t * n_fft) + chanOffsetInputBuffer;
            }

            // Loop over ntaps
            for(int t = 0; t < n_taps; t++)
            {
                __m128* pSrc2   = (__m128*) fifo_ptrs[t];
                __m128* pResult = (__m128*) fftw_in[chanOffsetOutputBuffer];
                __m128* pSrc1   = (__m128*) (filterCoeffs + t * n_fft * 2); //coeffs are repeated for real and complex parts

                // Loop over samples
                for(int s = 0; s < n_fft; s += sse_factor)
                {
                   // Apply taps
                    __m128 m1 = _mm_mul_ps(*pSrc1, *pSrc2);
                    *pResult  = _mm_add_ps(*pResult, m1);

                    pSrc1++;   // Update coeffs pointer
                    pSrc2++;   // Update fifo pointer
                    pResult++; // Update fftw_in pointer
                }
            }

            // Apply fft and copy directly to output buffer
            //fftwf_execute_dft(plan, fftw_in + chanOffsetOutputBuffer, outputBuffer + chanOffsetOutputBuffer);
            fftwf_execute_dft(plan, fftw_in + chanOffsetOutputBuffer, outputData + chanOffsetOutputBuffer);

            //copyToOutputStream(chan);
        }
    }
}

void PPF::loadFromInputStream(int channel){
    int tapSize = (n_taps-1)*n_fft;
    int totalInputBufferSize = n_fft + tapSize;

    int channelOffset = channel * totalInputBufferSize;
    int inputChannelOffset = channel * n_samp;

    //first copy tap data
    memcpy(inputBuffer + channelOffset, inputBuffer + channelOffset + totalInputBufferSize - tapSize, tapSize * sizeof(fftwf_complex));
    // then copy in new data
    memcpy(inputBuffer + ((n_taps - 1) * n_fft) + channelOffset, inputData + inputChannelOffset, n_samp * sizeof(fftwf_complex));
}

void PPF::copyToOutputStream(int channel){
    int writeIdx = channel * n_fft;
    memcpy(outputData+writeIdx, outputBuffer+writeIdx, n_fft * sizeof(fftwf_complex));
}

void PPF::loadFFTWPlans(){
    fftwf_import_wisdom_from_filename("fftwf_wisdom.dat");
    for (int i = 0; i < nthreads; ++i) {
        fftwf_complex* fftw_in;
        fftwf_complex* fftw_out;
        fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
        fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));

        plans[i] = fftwf_plan_dft_1d(n_fft,fftw_in,fftw_out,FFTW_FORWARD,FFTW_ESTIMATE);

        fftwf_free(fftw_in);
        fftwf_free(fftw_out);
    }
}

void PPF::setupFFTWWisdom(){
    fftwf_complex* fftw_in;
    fftwf_complex* fftw_out;
    fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftwf_plan plan;

    plan = fftwf_plan_dft_1d(n_fft,fftw_in,fftw_out,FFTW_FORWARD,FFTW_EXHAUSTIVE);

    fftwf_export_wisdom_to_filename("fftwf_wisdom.dat");
    fftwf_destroy_plan(plan);
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);
}

void PPF::ppfcoefficients()
{
    int realIdx = 0;

    switch (m_window)
    {
    case HAMMING:
        for (int i = 0; i < n_coefficients; ++i) {
            realIdx = i*2; //*2 for contiguous real/complex weights
            filterCoeffs[realIdx] = ((float)i/n_fft - ((float)n_taps/2));
            filterCoeffs[realIdx] = sinc(filterCoeffs[realIdx]) * hamming(i,0.54);
            filterCoeffs[realIdx+1] = filterCoeffs[realIdx];
        }
        break;
    case HANN:
        for (int i = 0; i < n_coefficients; ++i) {
            realIdx = i*2; //*2 for contiguous real/complex weights
            filterCoeffs[realIdx] = ((float)i/n_fft - ((float)n_taps/2));
            filterCoeffs[realIdx] = sinc(filterCoeffs[realIdx]) * hanning(i);
            filterCoeffs[realIdx+1] = filterCoeffs[realIdx];
        }
        break;
    }
}

float PPF::hanning(int order)
{
    return 0.5 * (1 - cos(2*M_PI*order/(n_coefficients-1)));
}

float PPF::hamming(int order, float alpha)
{
    float beta = 1-alpha;
    return alpha - beta*(cos(2*M_PI*order/(n_coefficients-1)));
}

float PPF::sinc(float x)
{
    return (x == 0) ? 1 : (sin(M_PI * x)) / (M_PI * x);
}

