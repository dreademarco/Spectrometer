#include "ppf.h"

PPF::PPF(int filter_taps, int fft_points, int fft_block_size)
{
    complex_size = sizeof(float)*2;

    n_taps = filter_taps;
    n_fft = fft_points;
    nfft_blocks = fft_block_size;

    // PPF coefficients
    n_coefficients = n_taps*n_fft;
    filterCoeffs = new float[n_coefficients];
    memset(filterCoeffs, 0, n_coefficients * sizeof(float));

    // FIFO taps
    tappedFilter = new fftwf_complex[n_fft*n_taps];
    memset(tappedFilter, 0, n_fft*n_taps * complex_size);

    // PPF Output
    ppfOutput = new fftwf_complex[n_fft*nfft_blocks];

    // FFT Output
    fftwOutput = new fftwf_complex[n_fft*nfft_blocks];

    ppfcoefficients(); // generate weights
    createFFTPlan(); // create FFTW plan

    int fs=1024;
    int duration = 1000; //seconds
    chirpsignal = new fftwf_complex[fs*duration];


    ppfSignalOutput = new fftwf_complex[fs*duration];
    realPpfSignalOutput = new float[fs*duration];

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
    delete[] filterCoeffs;
    delete[] tappedFilter;
    delete[] ppfOutput;
    delete[] fftwOutput;
}

void PPF::applyPPF(fftwf_complex* input)
{
    int fs=1024;
    int seconds = 1000;
    int nsamp = fs*seconds;
    fftwf_complex* tempBuffer;
    tempBuffer = new fftwf_complex[n_fft];

    int b=0;
    while(b<nsamp){
        //reset tap output
        memset(&ppfOutput[0], 0, n_fft * complex_size);
        //copy block
        memcpy(&tempBuffer[0],&input[b],n_fft*complex_size);
        //push block to taps
        pushTapValues(tempBuffer);
        //process taps
        calculateTapOutput();
        //calculateTapOutput_threaded(2);

        //Testing - Overwrite PPF output
        //memcpy(&ppfOutput[0],&tempBuffer[0],n_fft*sizeof(complex<float>));

        //pass tap output to FFTW, ppfOutput is the input to FFTW
        fftwf_execute_dft(plan, ppfOutput, fftwOutput);
        // Copy fft'ed result to output buffer
        memcpy(&ppfSignalOutput[b], &fftwOutput[0], n_fft*complex_size);
        b+=n_fft;
    }
}

void PPF::pushTapValues(fftwf_complex* newTapInput)
{
    int toCopy = n_fft * (n_taps-1);
    memmove(&tappedFilter[n_fft],&tappedFilter[0],toCopy*complex_size);
    memcpy(&tappedFilter[0],&newTapInput[0],n_fft*complex_size);
}

void PPF::generateLinearChirp(int fs, int duration, float f0, float t1, float f1, fftwf_complex* signal)
{
    int samples = fs*duration;
    float beta = (f1-f0)/t1;
    float interval = (float)duration/samples;
    for (int i = 0; i < samples; ++i) {
        float t = float(i)*interval;
        signal[i][0] = t;
        signal[i][1] = cos(2*M_PI * ( 0.5* beta * (signal[i][0]*signal[i][0]) + f0*signal[i][0]));
        //cout << signal[i].re << endl;
    }
}

void PPF::createFFTPlan()
{
    in  = (fftwf_complex*) fftwf_malloc(n_taps * n_fft * sizeof(fftwf_complex));
    out = (fftwf_complex*) fftwf_malloc(n_taps * n_fft * sizeof(fftwf_complex));
    plan = fftwf_plan_dft_1d(n_fft, in, out, FFTW_FORWARD, FFTW_MEASURE);
    fftwf_free(in);
    fftwf_free(out);
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

void PPF::calculateTapOutput()
{
    ppfOutput = (fftwf_complex*) __builtin_assume_aligned(ppfOutput, 16);
    tappedFilter = (fftwf_complex*) __builtin_assume_aligned(tappedFilter, 16);
    filterCoeffs = (float*) __builtin_assume_aligned(filterCoeffs, 16);

    //initialize offsets
    int coeffOffset = 0;
    int tappedFilterOffset = 0;

    for (int i=0; i<n_taps; ++i)
    {
        coeffOffset = i*n_fft;
        tappedFilterOffset = n_fft*(n_taps-i-1);
        for (int j = 0; j < n_fft; j+=4) {
            ppfOutput[j][0] += filterCoeffs[coeffOffset+j] * tappedFilter[tappedFilterOffset+j][0];
            ppfOutput[j][1] += filterCoeffs[coeffOffset+j] * tappedFilter[tappedFilterOffset+j][1];

            ppfOutput[j+1][0] += filterCoeffs[coeffOffset+j+1] * tappedFilter[tappedFilterOffset+j+1][0];
            ppfOutput[j+1][1] += filterCoeffs[coeffOffset+j+1] * tappedFilter[tappedFilterOffset+j+1][1];

            ppfOutput[j+2][0] += filterCoeffs[coeffOffset+j+2] * tappedFilter[tappedFilterOffset+j+2][0];
            ppfOutput[j+2][1] += filterCoeffs[coeffOffset+j+2] * tappedFilter[tappedFilterOffset+j+2][1];

            ppfOutput[j+3][0] += filterCoeffs[coeffOffset+j+3] * tappedFilter[tappedFilterOffset+j+3][0];
            ppfOutput[j+3][1] += filterCoeffs[coeffOffset+j+3] * tappedFilter[tappedFilterOffset+j+3][1];
        }
    }
}

void PPF::calculateTapOutput_threaded(int nThreads)
{
    ppfOutput = (fftwf_complex*) __builtin_assume_aligned(ppfOutput, 16);
    tappedFilter = (fftwf_complex*) __builtin_assume_aligned(tappedFilter, 16);
    filterCoeffs = (float*) __builtin_assume_aligned(filterCoeffs, 16);

    #pragma omp parallel for num_threads(nThreads)
    for (int i = 0; i < n_fft; ++i)
    {
        for (int j = 0; j < n_taps; j+=4) {
            ppfOutput[i][0] +=filterCoeffs[(j*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-j-1))][0]; //n_taps-j since oldest tap is the last in array
            ppfOutput[i][1] +=filterCoeffs[(j*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-j-1))][1]; //n_taps-j since oldest tap is the last in array

            ppfOutput[i][0] +=filterCoeffs[((j+1)*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-(j+1)-1))][0]; //n_taps-j since oldest tap is the last in array
            ppfOutput[i][1] +=filterCoeffs[((j+1)*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-(j+1)-1))][1]; //n_taps-j since oldest tap is the last in array

            ppfOutput[i][0] +=filterCoeffs[((j+2)*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-(j+2)-1))][0]; //n_taps-j since oldest tap is the last in array
            ppfOutput[i][1] +=filterCoeffs[((j+2)*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-(j+2)-1))][1]; //n_taps-j since oldest tap is the last in array

            ppfOutput[i][0] +=filterCoeffs[((j+3)*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-(j+3)-1))][0]; //n_taps-j since oldest tap is the last in array
            ppfOutput[i][1] +=filterCoeffs[((j+3)*n_fft)+i] * tappedFilter[i+(n_fft*(n_taps-(j+3)-1))][1]; //n_taps-j since oldest tap is the last in array
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
