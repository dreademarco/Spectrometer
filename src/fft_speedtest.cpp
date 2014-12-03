#include "fft_speedtest.h"

fft_speedtest::fft_speedtest(int filter_taps, int fft_points, int sampling_rate, int duration_seconds, int n_threads)
{
    fs = sampling_rate;
    duration = duration_seconds;
    complex_size = sizeof(float)*2;
    n_taps = filter_taps;
    n_fft = fft_points;
    n_samp = sampling_rate*duration_seconds;

    numThreads = n_threads;
    omp_set_num_threads(numThreads);
    fftwf_init_threads();


    // Input buffer (first tap is also the first n_fft chunk of the signal)
    inputBuffer = (fftwf_complex*) fftwf_malloc((((n_taps-1) * n_fft) + n_samp) * sizeof(fftwf_complex));
    memset(inputBuffer, 0, (((n_taps-1) * n_fft) + n_samp) * sizeof(fftwf_complex));

    // Output buffer
    outputBuffer = (fftwf_complex*) fftwf_malloc(n_samp * sizeof(fftwf_complex));
    memset(outputBuffer, 0, n_samp * sizeof(fftwf_complex));

    // chirp data
    chirpsignal = (fftwf_complex*) fftwf_malloc(fs*duration * sizeof(fftwf_complex));
    memset(chirpsignal, 0, fs*duration * sizeof(fftwf_complex));
    generateLinearChirp(fs,duration,0,duration/4,700,chirpsignal);
    // MATLAB: spectrogram(chirp,256,128,256,1024,'yaxis')
    loadInput(chirpsignal);
    fftwf_free(chirpsignal);

    //###---SEQUENTIAL FFT---###    
    fftwf_complex* fftw_in;
    fftwf_complex* fftw_out;
    fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftwf_plan_with_nthreads(numThreads);
    fftwf_plan seq_plan = fftwf_plan_dft_1d(n_fft, fftw_in, fftw_out, FFTW_FORWARD, FFTW_EXHAUSTIVE);

    int n_runs = 50;
    cout << "Starting sequential FFT..." << endl;
    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
    for (int runs = 0; runs < n_runs; ++runs) {
        apply_seqFFT(seq_plan,fftw_in,fftw_out);
    }
    gettimeofday(&t2, NULL);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << "Elapsed time: " << elapsedTime << " ms" << endl;
    if(elapsedTime>0){
        float datarate = ((((n_samp)/(elapsedTime / n_runs))/1000000))*1000.0;
        cout << "Data rate est.: " << datarate << " Mhz" << endl;
    }else{
        cout << "Data rate est.: n/a"  << " Mhz" << endl;
    }
    cout << "------------------------------------" << endl;
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);

    //###---BLOCK FFT---###
    for (int block_size = 1; block_size < 200; block_size+=1) {
        fftblocks = block_size;
        fftwf_complex* fftw_in;
        fftwf_complex* fftw_out;
        fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex) * fftblocks);
        fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex) * fftblocks);
        int n[] = {n_fft};
        fftwf_plan_with_nthreads(numThreads);
        fftwf_plan block_plan = fftwf_plan_many_dft(1, n, fftblocks,
                                                    fftw_in, n, 1, n_fft,
                                                    fftw_out, n, 1, n_fft,
                                                    FFTW_FORWARD, FFTW_EXHAUSTIVE);

        n_runs = 50;
        cout << "Starting block FFT: " << fftblocks << " blocks" << endl;
        gettimeofday(&t1, NULL);
        for (int runs = 0; runs < n_runs; ++runs) {
            apply_blockFFT(block_plan,fftw_in,fftw_out);
        }
        gettimeofday(&t2, NULL);
        // compute and print the elapsed time in millisec
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
        cout << "Elapsed time: " << elapsedTime << " ms" << endl;
        if(elapsedTime>0){
            float datarate = ((((n_samp)/(elapsedTime / n_runs))/1000000))*1000.0;
            cout << "Data rate est.: " << datarate << " Mhz" << endl;
        }else{
            cout << "Data rate est.: n/a"  << " Mhz" << endl;
        }
        cout << "------------------------------------" << endl;
        fftwf_free(fftw_in);
        fftwf_free(fftw_out);
    }
}

fft_speedtest::~fft_speedtest(){
    fftwf_free(inputBuffer);
    fftwf_free(outputBuffer);
}

void fft_speedtest::loadInput(fftwf_complex* input){
    memcpy(inputBuffer + (n_taps - 1) * n_fft, input, n_samp * sizeof(fftwf_complex));
}

void fft_speedtest::apply_seqFFT(fftwf_plan plan,fftwf_complex* fftw_in,fftwf_complex* fftw_out)
{
    // Loop over input blocks of size nfft
    for (int b =  0; b <  n_samp; b += n_fft){
        fftwf_execute_dft(plan, fftw_in, fftw_out);
    }
}

void fft_speedtest::apply_blockFFT(fftwf_plan plan, fftwf_complex* fftw_in,fftwf_complex* fftw_out)
{
    //fftwf_import_wisdom_from_filename("fftwf_wisdom.dat");

    int* fft_block_order;
    fft_block_order = (int*) fftwf_malloc(fftblocks*sizeof(int));
    int block_counter = 0;

    // Loop over input blocks of size nfft
    for (int b =  0; b <  n_samp; b += n_fft){
        fft_block_order[block_counter] = b;
        if(block_counter == (fftblocks-1)){
            block_counter=0;
            // Apply fft and copy directly to output buffer block
            fftwf_execute_dft(plan, fftw_in, fftw_out);
        }else{
            block_counter++;
        }
    }
}

void fft_speedtest::generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal)
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

void fft_speedtest::createFFTPlanWisdom_sequential()
{
    fftwf_complex* fftw_in;
    fftwf_complex* fftw_out;
    fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex));
    fftwf_plan_with_nthreads(numThreads);
    fftwf_plan plan = fftwf_plan_dft_1d(n_fft, fftw_in, fftw_out, FFTW_FORWARD, FFTW_EXHAUSTIVE);

    fftwf_export_wisdom_to_filename("fftwf_wisdom.dat");
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);
}

void fft_speedtest::createFFTPlanWisdom_block()
{
    fftwf_complex* fftw_in;
    fftwf_complex* fftw_out;
    fftw_in = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex) * fftblocks);
    fftw_out = (fftwf_complex*) fftwf_malloc(n_fft * sizeof(fftwf_complex) * fftblocks);
    fftwf_plan_with_nthreads(numThreads);
    int n[] = {n_fft};
    fftwf_plan plan = fftwf_plan_many_dft(1, n, fftblocks,
                                          fftw_in, n, 1, n_fft,
                                          fftw_out, n, 1, n_fft,
                                          FFTW_FORWARD, FFTW_EXHAUSTIVE);

    fftwf_export_wisdom_to_filename("fftwf_wisdom.dat");
    fftwf_free(fftw_in);
    fftwf_free(fftw_out);
}
