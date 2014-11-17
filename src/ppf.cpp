#include "ppf.h"

PPF::PPF(int filter_taps, int fft_points, int blocks, int sampling_rate, int duration_seconds)
{
    fftwf_init_threads();
    fs = sampling_rate;
    duration = duration_seconds;
    complex_size = sizeof(float)*2;
    fftblocks = blocks;
    n_taps = filter_taps;
    n_fft = fft_points;

    numThreads = 2;
    omp_set_num_threads(numThreads);

//    // Setup indexes for tap calculations
//    int indexes_cnt = 0;
//    int coeffOffset = 0;
//    int tappedFilterOffset = 0;
//    indexes_filterCoeffs = new int[n_fft*n_taps];
//    indexes_tappedFilter = new int[n_fft*n_taps];

//    for (int i=0; i<n_taps; ++i)
//    {
//        coeffOffset = i*n_fft;
//        tappedFilterOffset = n_fft*(n_taps-i-1);
//        for (int j = 0; j < n_fft; ++j) {
//            //indexes_in[indexes_cnt] = block_offset+j;
//            indexes_filterCoeffs[indexes_cnt] = coeffOffset+j;
//            indexes_tappedFilter[indexes_cnt] = tappedFilterOffset+j;
//            ++indexes_cnt;
//        }
//    }

    // PPF coefficients
    n_coefficients = n_taps*n_fft;
    filterCoeffs = (float*) fftwf_malloc(n_coefficients * sizeof(float));
    //filterCoeffs = new float[n_coefficients];
    memset(filterCoeffs, 0, n_coefficients * sizeof(float));

    // FIFO taps
    //tappedFilter = new fftwf_complex[n_fft*n_taps];
    tappedFilter = (fftwf_complex*) fftwf_malloc(n_fft*n_taps * sizeof(fftwf_complex));
    memset(tappedFilter, 0, n_fft*n_taps * sizeof(fftwf_complex));

    // chirp data
    //chirpsignal = new fftwf_complex[fs*duration];
    chirpsignal = (fftwf_complex*) fftwf_malloc(fs*duration * sizeof(fftwf_complex));
    memset(chirpsignal, 0, fs*duration * sizeof(fftwf_complex));

    // PPF Output (and FFTW input)
    in  = (fftwf_complex*) fftwf_malloc(fftblocks * n_fft * sizeof(fftwf_complex));
    // FFTW Output
    out = (fftwf_complex*) fftwf_malloc(fftblocks * n_fft * sizeof(fftwf_complex));
    ppfcoefficients(); // generate weights
    createFFTPlan(); // create FFTW plan

    //generateLinearChirp(fs,duration,0,duration/2,100,chirpsignal);
    // MATLAB: spectrogram(chirp,256,128,256,1024,'yaxis')

    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    applyPPF(chirpsignal);

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
    //delete[] filterCoeffs;
    //delete[] tappedFilter;
    fftwf_free(filterCoeffs);
    fftwf_free(tappedFilter);
    fftwf_free(in);
    fftwf_free(out);
    fftwf_cleanup_threads();
//    delete[] in;
//    delete[] out;
}

void PPF::applyPPF(fftwf_complex* input)
{
    int nsamp = fs*duration;
    int bytesToShift = (n_fft * (n_taps-1))*complex_size;

    int b=0; int block_counter = 0; int block_b = 0;
    while(b<nsamp){
        //reset tap output
        memset(&in[block_counter*n_fft], 0, n_fft * complex_size);

        //push block to taps
        pushTapValues(input,b);

        //process taps
        calculateTapOutput(block_counter);

        //Testing - Overwrite PPF output
        //memcpy(&in[0],&tempBuffer[0],n_fft*complex_size);

        b+=n_fft;  block_b += n_fft; ++block_counter;
        if(block_counter==fftblocks){
            //pass tap output to FFTW, in is the input to FFTW, result is put in the output stream out
            fftwf_execute_dft(plan, in, &out[block_b-(fftblocks*n_fft)]);
            block_counter=0;
            block_b = 0;
        }
        shiftTapValues(bytesToShift);
    }
//    for (int i = 0; i < nsamp; ++i){
//        cout << sqrt((out[i][0]*out[i][0]) + (out[i][1]*out[i][1])) << endl;
//    }
}

void PPF::shiftTapValues(int bytesToShift){
    memmove(&tappedFilter[n_fft],&tappedFilter[0],bytesToShift);
}

void PPF::pushTapValues(fftwf_complex* inputStream, int copyIndex)
{
    memcpy(&tappedFilter[0],&inputStream[copyIndex],n_fft*complex_size);
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
    //plan = fftwf_plan_dft_1d(n_fft, in, out, FFTW_FORWARD, FFTW_MEASURE);
    fftwf_plan_with_nthreads(omp_get_max_threads());
    int n[] = {n_fft};
    plan = fftwf_plan_many_dft(1, n, fftblocks,
                               in, n, 1, n_fft,
                               out, n, 1, n_fft,
                               FFTW_FORWARD, FFTW_MEASURE);
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
}

//void PPF::calculateTapOutput(int block_counter)
//{
//    //initialize offsets
//    int block_offset = block_counter*n_fft;
//    int coeffOffset = 0;
//    int tappedFilterOffset = 0;

//    for (int i=0; i<n_taps; ++i)
//    {
//        coeffOffset = i*n_fft;
//        tappedFilterOffset = n_fft*(n_taps-i-1);
//        #pragma omp parallel
//        {
//            int threadId = omp_get_thread_num();
//            for (int j = threadId * n_fft / numThreads; j < (threadId + 1) * n_fft / numThreads; j++){
//                in[block_offset+j][0] += filterCoeffs[coeffOffset+j] * tappedFilter[tappedFilterOffset+j][0];
//                in[block_offset+j][1] += filterCoeffs[coeffOffset+j] * tappedFilter[tappedFilterOffset+j][1];
//            }
//        }
//    }
//}

void PPF::calculateTapOutput(int block_counter)
{
    //initialize offsets
    int block_offset = block_counter*n_fft;
    int coeffOffset = 0;
    int tappedFilterOffset = 0;

    for (int i=0; i<n_taps; ++i)
    {
        coeffOffset = i*n_fft;
        tappedFilterOffset = n_fft*(n_taps-i-1);
        for (int j = 0; j < n_fft; j+=4) {
            in[block_offset+j][0] += filterCoeffs[coeffOffset+j] * tappedFilter[tappedFilterOffset+j][0];
            in[block_offset+j][1] += filterCoeffs[coeffOffset+j] * tappedFilter[tappedFilterOffset+j][1];

            in[block_offset+j+1][0] += filterCoeffs[coeffOffset+j+1] * tappedFilter[tappedFilterOffset+j+1][0];
            in[block_offset+j+1][1] += filterCoeffs[coeffOffset+j+1] * tappedFilter[tappedFilterOffset+j+1][1];

            in[block_offset+j+2][0] += filterCoeffs[coeffOffset+j+2] * tappedFilter[tappedFilterOffset+j+2][0];
            in[block_offset+j+2][1] += filterCoeffs[coeffOffset+j+2] * tappedFilter[tappedFilterOffset+j+2][1];

            in[block_offset+j+3][0] += filterCoeffs[coeffOffset+j+3] * tappedFilter[tappedFilterOffset+j+3][0];
            in[block_offset+j+3][1] += filterCoeffs[coeffOffset+j+3] * tappedFilter[tappedFilterOffset+j+3][1];
        }
    }
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
