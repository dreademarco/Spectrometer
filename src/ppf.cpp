#include "ppf.h"

PPF::PPF(int filter_taps, int fft_points, int blocks, int sampling_rate, int duration_seconds, int n_threads)
{
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

    // Input buffer (first tap is also the first n_fft chunk of the signal)
    inputBuffer = (fftwf_complex*) fftwf_malloc(((n_taps-1) * n_fft + n_samp) * sizeof(fftwf_complex));
    memset(inputBuffer, 0, ((n_taps-1) * n_fft + n_samp) * sizeof(fftwf_complex));

    // Output buffer
    outputBuffer = (fftwf_complex*) fftwf_malloc(n_samp * sizeof(fftwf_complex));
    memset(outputBuffer, 0, n_samp * sizeof(fftwf_complex));

    // PPF coefficients
    n_coefficients = n_taps*n_fft;
    filterCoeffs = (float*) fftwf_malloc(n_coefficients * sizeof(float));
    memset(filterCoeffs, 0, n_coefficients * sizeof(float));

    // FIFO taps
    fifo = (fftwf_complex*) fftwf_malloc(n_fft*n_taps * sizeof(fftwf_complex));
    memset(fifo, 0, n_fft*n_taps * sizeof(fftwf_complex));

    // chirp data
    chirpsignal = (fftwf_complex*) fftwf_malloc(fs*duration * sizeof(fftwf_complex));
    memset(chirpsignal, 0, fs*duration * sizeof(fftwf_complex));

    // FFTW Input
    fftw_in  = (fftwf_complex*) fftwf_malloc(fftblocks * n_fft * sizeof(fftwf_complex));
    // FFTW Output
    fftw_out = (fftwf_complex*) fftwf_malloc(fftblocks * n_fft * sizeof(fftwf_complex));


    ppfcoefficients(); // generate weights
    createFFTPlan(); // create FFTW plan

    generateLinearChirp(fs,duration,0,duration/2,100,chirpsignal);
    // MATLAB: spectrogram(chirp,256,128,256,1024,'yaxis')
    loadInput(chirpsignal);

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
        float datarate = ((((fs*duration)/elapsedTime)/1000000))*1000.0;
        cout << "PPF Data rate est.: " << datarate << " Mhz" << endl;
    }else{
        cout << "PPF Data rate est.: n/a"  << " Mhz" << endl;
    }
}

PPF::~PPF(){
    fftwf_free(filterCoeffs);
    fftwf_free(fifo);
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);
    fftwf_free(inputBuffer);
    fftwf_free(outputBuffer);
    fftwf_cleanup_threads();
}

void PPF::loadInput(fftwf_complex* input){
    // Copy input block to inputBuffer at the end of FIFO location
    //last tap will point to the first chunk of data of size n_fft
    memcpy(inputBuffer[(n_taps-1)*n_fft], input, n_samp * sizeof(fftwf_complex));
}

void PPF::applyPPF()
{
    fftwf_complex *fifo_ptrs[n_taps]; // an array of pointers to locations that contain fftwf_complex data

    // Initialise pointers
    for(int t = 0; t < n_taps; t++){
        fifo_ptrs[t] = inputBuffer + (t*n_fft);
    }

    // Copy fifo to input "space"
    //memcpy(inputBuffer, fifo, n_taps * n_fft * sizeof(fftwf_complex));

    #pragma omp parallel
    {
        // Get thread id
        int threadId = omp_get_thread_num();
        // Loop over input blocks of size nfft
        //for (int b = (n_taps-1)*n_fft; b < n_samp + ((n_taps-1)*n_fft); b+=n_fft)
        for(int b = ((n_taps-1) * n_fft) + (threadId * n_fft);
                    b < n_samp + ((n_taps-1) * n_fft);
                    b += n_fft * numThreads)
        {
            // Reset fftw_in buffers
            memset(fftw_in, 0, n_fft * sizeof(fftwf_complex));

            // Loop over ntaps
            for(int t = 0; t < n_taps; t++)
            {
                // Loop over samples
                for(int s = 0; s < n_fft; s++)
                {
                    // Pre-load value to improve ILP
                    fftwf_complex value;
                    int location;
                    float coeff = filterCoeffs[t * n_fft + s];
                    value[0] = (*(fifo_ptrs[t]))[0];
                    value[1] = (*(fifo_ptrs[t]))[1];

                    // Apply taps
                    fftw_in[s][0] += value[0] * coeff;
                    fftw_in[s][1] += value[1] * coeff;

                    // Update fifo pointer
                    fifo_ptrs[t]++;
                }
            }

            // Apply fft and copy directly to output buffer
            fftwf_execute_dft(plan, fftw_in, outputBuffer + b - ((n_taps-1) * n_fft));
            //fftwf_execute_dft(plan, fftw_in, fftw_out);
        }
    }

//    for (int i = 0; i < n_samp; ++i){
//        cout << sqrt((outputBuffer[i][0]*outputBuffer[i][0]) + (outputBuffer[i][1]*outputBuffer[i][1])) << endl;
//    }
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
        //cout << signal[i][0] << endl;
    }
}

void PPF::createFFTPlan()
{
    plan = fftwf_plan_dft_1d(n_fft, fftw_in, fftw_out, FFTW_FORWARD, FFTW_MEASURE);
//    fftwf_plan_with_nthreads(omp_get_max_threads());
//    int n[] = {n_fft};
//    plan = fftwf_plan_many_dft(1, n, fftblocks,
//                               fftw_in, n, 1, n_fft,
//                               fftw_out, n, 1, n_fft,
//                               FFTW_FORWARD, FFTW_MEASURE);
}

void PPF::printCoefficients()
{
    for (int i = 0; i < n_coefficients; ++i) {
        cout << filterCoeffs[i] << endl;
    }
}

void PPF::ppfcoefficients()
{
    for (int i = 0; i < n_coefficients; ++i) {
        filterCoeffs[i] = ((float)i/n_fft - ((float)n_taps/2));
        filterCoeffs[i] = sinc(filterCoeffs[i]) * hanning(i);
    }
    //reverse(filterCoeffs, filterCoeffs+n_coefficients);
}


float PPF::hanning(int order)
{
    return 0.5 * (1 - cos(2*M_PI*order/(n_coefficients-1)));
}

float PPF::sinc(float x)
{
    if(x==0){return 1;}
    else {return (sin(M_PI*x))/(M_PI*x);}
}
