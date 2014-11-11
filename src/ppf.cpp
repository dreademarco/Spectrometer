#include "ppf.h"

PPF::PPF(int filter_taps, int fft_points)
{
    n_taps = filter_taps;
    n_fft = fft_points;
    n_coefficients = n_taps*n_fft;
    filterCoeffs = new Array2DSpectrum<float>(1,n_coefficients);
    tappedFilter = new Array2DSpectrum<complex<float> >(n_fft,n_taps);
    ppfOutput = new Array2DSpectrum<complex<float> >(n_fft,1);
    fftwOutput = new Array2DSpectrum<complex<float> >(n_fft,1);
    ppfcoefficients();
    createFFTPlan();


    int fs=1024;
    //int fs=11025;
    int seconds = 5;
    chirpsignal = new Array2DSpectrum<float>(fs*seconds,1);
    chirpsignalComplex = new Array2DSpectrum<complex<float> >(fs*seconds,1);
    ppfSignalOutput = new Array2DSpectrum<complex<float> >(fs*seconds,1);
    realPpfSignalOutput = new Array2DSpectrum<float>(fs*seconds,1);

    generateLinearChirp(fs,seconds,0,1,100,chirpsignal,chirpsignalComplex);

    applyPPF(chirpsignalComplex);
}

void PPF::applyPPF(Array2DSpectrum<complex<float> > *input)
{
    int fs=1024;
    int seconds = 5;
    int nsamp = fs*seconds;
    Array2DSpectrum<complex<float> > *tempBuffer;
    tempBuffer = new Array2DSpectrum<complex<float> >(n_fft,1);

    // Loop over input blocks of size nfft
    for(int b = 0; b < nsamp; b += n_fft)
    {
        //copy block
        memcpy(&tempBuffer->data[0],&input->data[b],n_fft*sizeof(complex<float>));
        //push block to taps
        pushTapValues(tempBuffer);
        //process taps
        calculateTapOutput();

        //Testing - Overwrite PPF output
        //memcpy(&ppfOutput->data[0],&tempBuffer->data[0],n_fft*sizeof(complex<float>));

        //pass tap output to FFTW, ppfOutput is the input to FFTW
        fftwf_execute_dft(plan, reinterpret_cast<fftwf_complex*>(ppfOutput->data), reinterpret_cast<fftwf_complex*>(fftwOutput->data));
        // Copy fft'ed result to output buffer
        memcpy(&ppfSignalOutput->data[b], &fftwOutput->data[0], sizeof(complex<float>)*n_fft);
    }
    for (int i = 0; i < nsamp; ++i) {
        ppfSignalOutput->data[i] = abs(ppfSignalOutput->data[i]);
        realPpfSignalOutput->data[i] = ppfSignalOutput->data[i].real();
        //cout << realPpfSignalOutput->data[i] << endl;
    }
    //realPpfSignalOutput->toString();
}

void PPF::pushTapValues(Array2DSpectrum<complex<float> > *newTapInput)
{
    int toCopy = tappedFilter->getChannelCount() * (tappedFilter->getSampleCount()-1);
    int startIdx = tappedFilter->getChannelCount();
    memmove(&tappedFilter->data[startIdx],&tappedFilter->data[0],sizeof(complex<float>)*toCopy);
    memcpy(&tappedFilter->data[0],&newTapInput->data[0],sizeof(complex<float>)*newTapInput->getChannelCount());
    //tappedFilter->toString();
}

void PPF::generateLinearChirp(int fs, int duration, float f0, float t1, float f1, Array2DSpectrum<float>* signal, Array2DSpectrum<complex<float> >* complexsignal)
{
    int samples = fs*duration;
    float beta = (f1-f0)/t1;
    float interval = (float)duration/samples;
    for (int i = 0; i < samples; ++i) {
        float t = float(i)*interval;
        signal->data[i] = t;
        signal->data[i] = cos(2*M_PI * ( 0.5* beta * (signal->data[i]*signal->data[i]) + f0*signal->data[i]));
        //cout << signal->data[i] << endl;
    }
    for (int i = 0; i < samples; ++i) {
        complex<float> a(signal->data[i],0);
        complexsignal->data[i] = a;
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
    filterCoeffs->toString();
}

void PPF::ppfcoefficients()
{
    for (int i = 0; i < filterCoeffs->getSampleCount(); ++i) {
        filterCoeffs->set(0,i,((float)i/n_fft - ((float)n_taps/2)));
        filterCoeffs->set(0,i,(sinc(filterCoeffs->at(0,i)) * hanning(i)));
    }
}

void PPF::calculateTapOutput()
{       
    for (int i = 0; i < n_fft; ++i) {
        complex<float> tapSum=0.0;
        for (int j = 0; j < n_taps; ++j) {
            //cout << filterCoeffs->at(0,(j*n_fft)+i) << endl;
            tapSum += filterCoeffs->at(0,(j*n_fft)+i) * tappedFilter->at(i,n_taps-j-1); //n_taps-j since oldest tap is the last in array
        }
        ppfOutput->set(i,0,tapSum);
    }
    //cout << "---" << endl;
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
