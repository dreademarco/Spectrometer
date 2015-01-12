#include "streamingppf.h"

StreamingPPF::StreamingPPF(FFTWSequence* inputStreamBuffer, FFTWSequence* outputStreamBuffer, int filter_taps, int fft_points, int blocks, int sampling_rate, int duration_seconds, window selected_window)
{
    // configure PPF settings
    fs = sampling_rate;
    duration = duration_seconds;
    n_blocks = blocks;
    n_taps = filter_taps;
    n_fft = fft_points;
    n_samp = sampling_rate*duration_seconds;
    n_coefficients = n_taps*n_fft;
    m_window = selected_window;

    //register input and output streams
    sourceStream = inputStreamBuffer->data;
    outputStream = outputStreamBuffer->data;

    // Create Input buffer (first tap is also the first n_fft chunk of the signal)
    inputBuffer = (fftwf_complex*) fftwf_malloc((((n_taps-1) * n_fft) + n_samp) * sizeof(fftwf_complex));
    memset(inputBuffer, 0, (((n_taps-1) * n_fft) + n_samp) * sizeof(fftwf_complex));

    // Create Output buffer
    outputBuffer = (fftwf_complex*) fftwf_malloc(n_samp * sizeof(fftwf_complex));
    memset(outputBuffer, 0, n_samp * sizeof(fftwf_complex));

    // Create fftw_in for all threads
    fftw_in_threaded = (fftwf_complex*) fftwf_malloc(nthreads * n_fft * n_blocks * sizeof(fftwf_complex));
    memset(fftw_in_threaded, 0, nthreads * n_fft * n_blocks * sizeof(fftwf_complex));

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

StreamingPPF::~StreamingPPF(){
    fftwf_free(filterCoeffs);
    fftwf_free(inputBuffer);
    fftwf_free(outputBuffer);
    fftwf_free(fftw_in_threaded);

    // free plans
    for (int i = 0; i < nthreads; ++i) {
        fftwf_destroy_plan(plans[i]);
    }
    fftwf_free(plans);
}

void StreamingPPF::applyPPF()
{
    // start up pre-tap data before rest of input stream
    loadFromInputStream();

    // Loop over input blocks of size nfft
    #pragma omp parallel
    {
        // Get thread id
        int threadID = omp_get_thread_num();
        fftwf_plan plan = plans[threadID];

        void *fifo_ptrs[n_taps]; // an array of pointers to locations that contain fftwf_complex data

        for(int b = threadID * n_fft * n_blocks;
                b < n_samp;
                b += nthreads * n_fft * n_blocks)
        {
            // Reset fftw_in buffer
            memset(fftw_in_threaded[threadID * n_fft * n_blocks], 0, n_fft * n_blocks * sizeof(fftwf_complex));

            for(int t = 0; t < n_taps; t++){
                fifo_ptrs[t] = inputBuffer + (t * n_fft) + b;
            }

            // Loop over ntaps
            for(int t = 0; t < n_taps; t++)
            {
                __m256* pSrc2   = (__m256*) fifo_ptrs[t];
                __m256* pResult = (__m256*) fftw_in_threaded[threadID * n_fft * n_blocks];// + b;

                for(int j = 0; j < n_blocks; j++)
                {
                    __m256* pSrc1   = (__m256*) (filterCoeffs + t * n_fft * 2); //coeffs are repeated for real and complex parts

                // Loop over samples
                    for(int s = 0; s < n_fft; s += sse_factor)
                    {
                       // Apply taps
                        __m256 m1 = _mm256_mul_ps(*pSrc1, *pSrc2);
                        *pResult  = _mm256_add_ps(*pResult, m1);

                        pSrc1++;   // Update coeffs pointer
                        pSrc2++;   // Update fifo pointer
                        pResult++; // Update fftw_in pointer
                    }
                }
            }

            // Apply fft and copy directly to output buffer
            fftwf_execute_dft(plan, fftw_in_threaded+(threadID * n_fft * n_blocks), outputBuffer + b);
       }
    }
    copyToOutputStream();
}

void StreamingPPF::loadFromInputStream(){
    int tapSize = (n_taps-1)*n_fft;
    int totalInputBufferSize = n_samp + tapSize;

    //first copy tap data
    memcpy(inputBuffer, inputBuffer + totalInputBufferSize - tapSize, (n_taps - 1) * n_fft * sizeof(fftwf_complex));
    //memcpy(inputBuffer, inputBuffer + (n_samp - ((n_taps - 1) * n_fft)), (n_taps - 1) * n_fft * sizeof(fftwf_complex));
    // then copy in new data
    memcpy(inputBuffer + ((n_taps - 1) * n_fft), sourceStream, n_samp * sizeof(fftwf_complex));
}

void StreamingPPF::copyToOutputStream(){
//    for (int i = 0; i < n_samp; ++i) {
//        cout << sqrt(outputBuffer[i][0]*outputBuffer[i][0] + outputBuffer[i][1]*outputBuffer[i][1]) << endl;
//    }
    memcpy(outputStream, outputBuffer, n_samp * sizeof(fftwf_complex));
}

void StreamingPPF::loadFFTWPlans(){
    fftwf_import_wisdom_from_filename("fftwf_wisdom.dat");
    for (int i = 0; i < nthreads; ++i) {
        fftwf_complex* fftw_in;
        fftwf_complex* fftw_out;
        fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * n_blocks * sizeof(fftwf_complex));
        fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * n_blocks * sizeof(fftwf_complex));
        int n[] = {n_fft};
        plans[i] = fftwf_plan_many_dft(1, n, n_blocks,
                                       fftw_in, n, 1, n_fft,
                                       fftw_out, n, 1, n_fft,
                                       FFTW_FORWARD, FFTW_ESTIMATE);
        fftwf_free(fftw_in);
        fftwf_free(fftw_out);
    }
}

void StreamingPPF::setupFFTWWisdom(){
    fftwf_complex* fftw_in;
    fftwf_complex* fftw_out;
    fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * n_blocks * sizeof(fftwf_complex));
    fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * n_blocks * sizeof(fftwf_complex));
    fftwf_plan plan;

    int n[] = {n_fft};
    plan = fftwf_plan_many_dft(1, n, n_blocks,
                               fftw_in, n, 1, n_fft,
                               fftw_out, n, 1, n_fft,
                               FFTW_FORWARD, FFTW_ESTIMATE);
                               //FFTW_FORWARD, FFTW_EXHAUSTIVE);

    fftwf_export_wisdom_to_filename("fftwf_wisdom.dat");
    fftwf_destroy_plan(plan);
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);
}

void StreamingPPF::ppfcoefficients()
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

float StreamingPPF::hanning(int order)
{
    return 0.5 * (1 - cos(2*M_PI*order/(n_coefficients-1)));
}

float StreamingPPF::hamming(int order, float alpha)
{
    float beta = 1-alpha;
    return alpha - beta*(cos(2*M_PI*order/(n_coefficients-1)));
}

float StreamingPPF::sinc(float x)
{
    return (x == 0) ? 1 : (sin(M_PI * x)) / (M_PI * x);
}
