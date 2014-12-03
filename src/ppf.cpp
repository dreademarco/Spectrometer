#include "ppf.h"

PPF::PPF(int filter_taps, int fft_points, int blocks, int sampling_rate, int duration_seconds, int n_threads, bool generateWisdom)
{
    sse_factor = 4;
    fs = sampling_rate;
    duration = duration_seconds;
    complex_size = sizeof(float)*2;
    fftblocks = blocks;
    n_taps = filter_taps;
    n_fft = fft_points;
    n_samp = sampling_rate*duration_seconds;

    // Set number of openmp threads
    numThreads = n_threads;
    omp_set_num_threads(numThreads);
    //fftwf_init_threads();

//    float* pre_shift; float* post_shift;
//    pre_shift = (float*) fftwf_malloc(5 * sizeof(float)); post_shift = (float*) fftwf_malloc(5 * sizeof(float));
//    memset(post_shift, 0, 5 * sizeof(float));

//    pre_shift[0] = 1; pre_shift[1] = 2; pre_shift[2] = 3; pre_shift[3] = 4; pre_shift[4] = 5;
//    this->circShift(pre_shift,post_shift,2);

    // Input buffer (first tap is also the first n_fft chunk of the signal)
    inputBuffer = (fftwf_complex*) fftwf_malloc((((n_taps-1) * n_fft) + n_samp) * sizeof(fftwf_complex));
    memset(inputBuffer, 0, (((n_taps-1) * n_fft) + n_samp) * sizeof(fftwf_complex));

    // Output buffer
    outputBuffer = (fftwf_complex*) fftwf_malloc(n_samp * sizeof(fftwf_complex));
    memset(outputBuffer, 0, n_samp * sizeof(fftwf_complex));

    // PPF coefficients
    n_coefficients = n_taps*n_fft;
    filterCoeffs = (float*) fftwf_malloc(n_coefficients*2 * sizeof(float)); //*2 for contiguous real/complex weights
    memset(filterCoeffs, 0, n_coefficients*2 * sizeof(float));

//    // FIFO taps
//    fifo = (fftwf_complex*) fftwf_malloc(n_fft*n_taps * sizeof(fftwf_complex));
//    memset(fifo, 0, n_fft*n_taps * sizeof(fftwf_complex));

    // chirp data
//    chirpsignal = (fftwf_complex*) fftwf_malloc(fs*duration * sizeof(fftwf_complex));
//    memset(chirpsignal, 0, fs*duration * sizeof(fftwf_complex));

    ppfcoefficients(); // generate weights
//    if(generateWisdom){
//        createFFTPlanWisdom(); //generate fftw wisdom
//    }

//    generateLinearChirp(fs,duration,0,duration/4,700,chirpsignal);
    // MATLAB: spectrogram(chirp,256,128,256,1024,'yaxis')
//    loadInput(chirpsignal);
//    fftwf_free(chirpsignal);

    loadInputFromFile("input_file.dat");

    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    applyPPF();

    gettimeofday(&t2, NULL);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << "PPF elasped time: " << elapsedTime << " ms" << endl;
    if(elapsedTime>0){
        float datarate = ((((n_samp)/elapsedTime)/1000000))*1000.0;
        cout << "PPF Data rate est.: " << datarate << " Mhz" << endl;
    }else{
        cout << "PPF Data rate est.: n/a"  << " Mhz" << endl;
    }
}

PPF::~PPF(){
    fftwf_free(filterCoeffs);
    fftwf_free(inputBuffer);
    fftwf_free(outputBuffer);
    //fftwf_free(chirpsignal);
    //fftwf_cleanup_threads();
}

void PPF::loadInput(fftwf_complex* input){
    // Copy input block to inputBuffer at the end of FIFO location
    // last tap will point to the first chunk of data of size n_fft

    memcpy(inputBuffer + (n_taps - 1) * n_fft, input, n_samp * sizeof(fftwf_complex));
}

// Load test input signal from file
void PPF::loadInputFromFile(char *filename)
{
    FILE *fp = fopen(filename, "rb");

    float *temp = (float *) malloc(n_samp * sizeof(float));
    fread(temp, sizeof(float), n_samp, fp);

    // Tgerfixa totali
    for(unsigned i = 0; i < n_samp; i++)
    {
        inputBuffer[(n_taps - 1) * n_fft + i][0] = temp[i];
        inputBuffer[(n_taps - 1) * n_fft + i][1] = 0;
        //printf("%f  ", temp[i]);
    }

    free(temp);
    fclose(fp);
}

void PPF::applyPPF()
{
    fftwf_import_wisdom_from_filename("fftwf_wisdom.dat");

    // Loop over input blocks of size nfft
    #pragma omp parallel
    {
        // Loop over input blocks of size nfft
        fftwf_complex* fftw_in;
        fftwf_complex* fftw_out;
        fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
        fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
        fftwf_plan plan;

        #pragma omp critical
        {
            plan = fftwf_plan_dft_1d(n_fft, fftw_in, fftw_out, FFTW_FORWARD, FFTW_ESTIMATE);
        }

        void *fifo_ptrs[n_taps]; // an array of pointers to locations that contain fftwf_complex data

        // Get thread id
        int threadID = omp_get_thread_num();

        for(int b = threadID * n_fft; b < n_samp; b += n_fft * numThreads){
        //for (int b =  0; b <  n_samp; b += n_fft){
            // Reset fftw_in buffers
            memset(fftw_in, 0, n_fft * sizeof(fftwf_complex));
            //memcpy(fftw_in, inputBuffer + b + (n_taps - 1) * n_fft, n_fft * sizeof(fftwf_complex));

            for(int t = 0; t < n_taps; t++)
                fifo_ptrs[t] = inputBuffer + (t * n_fft) + b;

            // Loop over ntaps
            for(int t = 0; t < n_taps; t++)
            {
                __m256* pSrc1   = (__m256*) (filterCoeffs + t * n_fft*2); //coeffs are repeated for real and complex parts
                __m256* pSrc2   = (__m256*) fifo_ptrs[t];
                __m256* pResult = (__m256*) fftw_in;

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

            // Apply fft and copy directly to output buffer
            fftwf_execute_dft(plan, fftw_in, outputBuffer + b);
        }

        //free fifo pointers and other temp arrays
        fftwf_free(fftw_in);
        fftwf_free(fftw_out);
    }

    for (int i = 0; i < n_samp; ++i)
        cout << sqrt((outputBuffer[i][0]*outputBuffer[i][0]) + (outputBuffer[i][1]*outputBuffer[i][1])) << endl;

    FILE *fp = fopen("output_file.dat", "wb");
    fwrite(outputBuffer, sizeof(fftwf_complex), n_samp, fp);
    fclose(fp);
}

void PPF::generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal)
{
    int samples = fs*duration;
    float beta = (f1-f0)/t1;
    float interval = (float)duration/samples;
    for (int i = 0; i < samples; ++i) {
        float t = float(i)*interval;
        signal[i][0] = t;
        signal[i][0] = cos(2*M_PI * ( 0.5* beta * (signal[i][0]*signal[i][0]) + f0*signal[i][0]));
        signal[i][1] = 0;
    }

    FILE *fp = fopen("input.dat", "wb");
    fwrite(signal, sizeof(fftwf_complex), samples, fp);
    fclose(fp);
}

void PPF::createFFTPlanWisdom()
{
    fftwf_complex* fftw_in;
    fftwf_complex* fftw_out;
    fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));

    fftwf_plan plan = fftwf_plan_dft_1d(n_fft, fftw_in, fftw_out, FFTW_FORWARD, FFTW_EXHAUSTIVE);
//    fftwf_plan_with_nthreads(omp_get_max_threads());
//    int n[] = {n_fft};
//    plan = fftwf_plan_many_dft(1, n, fftblocks,
//                               fftw_in, n, 1, n_fft,
//                               fftw_out, n, 1, n_fft,
//                               FFTW_FORWARD, FFTW_MEASURE);

    fftwf_export_wisdom_to_filename("fftwf_wisdom.dat");
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);
}

void PPF::ppfcoefficients()
{
    int realIdx = 0;
    for (int i = 0; i < n_coefficients; ++i) {
        realIdx = i*2; //*2 for contiguous real/complex weights
        filterCoeffs[realIdx] = ((float)i/n_fft - ((float)n_taps/2));
        //filterCoeffs[realIdx] = sinc(filterCoeffs[realIdx]) * hanning(i);
        filterCoeffs[realIdx] = sinc(filterCoeffs[realIdx]) * hamming(i,0.54);
        filterCoeffs[realIdx+1] = filterCoeffs[realIdx];
    }
}

void PPF::fftShift(fftwf_complex* in, fftwf_complex* out)
{
    circShift(in,out,n_fft/2);
}

void PPF::circShift(fftwf_complex* in, fftwf_complex* out, int k)
{
    if(k>n_fft)
    {
         k = k % n_fft; //in case k>n_fft
    }
    int idxStartOffset = n_fft-k;
    int idxStartBytesOffset = idxStartOffset*sizeof(fftwf_complex);
    int startBytes = k*sizeof(fftwf_complex);
    memcpy(&out[0],&in[idxStartOffset],startBytes);
    memcpy(&out[k],&in[0],idxStartBytesOffset);
}

void PPF::angle(fftwf_complex* in, float* out){
    for (int i = 0; i < n_fft; ++i) {
        out[i] = atan2f(in[i][1],in[i][0]);
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
