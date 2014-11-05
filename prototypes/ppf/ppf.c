#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <fftw3.h>

int ntaps  = 4;
int nfft   = 1024;
int nsamp  = 65536 * 64;
fftwf_plan plan;

typedef struct
{
    float x, y;
} complex;

void apply_ppf(fftwf_complex* fftw_input, fftwf_complex* fftw_output, float **window, complex *input, complex *output)
{
    // Loop over input blocks of size nfft
    for(unsigned b = 0; b < nsamp; b += nfft)
    {
        // Loop of nfft samples
        for(unsigned s = 0; s < nfft; s++)
        {
            // Loop over ntaps
            for(unsigned t = 0; t < ntaps; t++)
            {
                fftw_input[s][0] += input[b + s].x * window[s][t];
                fftw_input[s][1] += input[b + s].y * window[s][t];
            }
        }

        // Apply fft
        fftwf_execute_dft(plan, fftw_input, fftw_output);

        // Copy fft'ed result to output buffer
        memcpy(output + b, fftw_output, sizeof(complex) * nfft);

        // Adjust FIFOs
        for(unsigned s = 0; s < nfft; s++)
            for(unsigned i = ntaps - 1; i > 0; i--) 
                window[s][i] = window[s][i - 1];                
    }
}

int main()
{
    // Allocate window cofficients
    float **window = (float **) malloc(nfft * sizeof(float *));
    for(unsigned i = 0; i < nfft; i++)
        window[i] = (float *) malloc(ntaps * sizeof(float));

    // Allocate fifo buffer
    complex **fifo = (complex **) malloc(nfft * sizeof(complex *));
    for(unsigned i = 0; i < nfft; i++)
    {
        fifo[i] = (complex *) malloc(ntaps * sizeof(complex));
        memset(fifo[i], 0, ntaps * sizeof(complex));
    }    

    // Allocate temporary buffers
    fftwf_complex* fftw_input = (fftwf_complex *) malloc(nfft * sizeof(fftwf_complex));
    fftwf_complex* fftw_output = (fftwf_complex *) malloc(nfft * sizeof(fftwf_complex));
    memset(fftw_input, 0, nfft * sizeof(fftwf_complex));
    memset(fftw_output, 0, nfft * sizeof(fftwf_complex));

    // Fill up coefficients
    for(unsigned i = 0; i < nfft; i++)
        for (unsigned j = 0; j < ntaps; j++)
            window[i][j] = window[i][j] = 1;

    // Create FFTW plan
    fftwf_complex* in  = (fftwf_complex*) fftwf_malloc(ntaps * nfft * sizeof(fftwf_complex));
    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(ntaps * nfft * sizeof(fftwf_complex));
    plan = fftwf_plan_dft_1d(nfft, in, out, FFTW_FORWARD, FFTW_MEASURE);
    fftwf_free(in);
    fftwf_free(out);

    // Create input and output buffers
    complex *input = (complex *) malloc(nsamp * sizeof(complex));

    // [S0 [C0 C1 C2 ... CN] ... SN [C0 C1 C2 ... CN]]
    complex *output = (complex *) malloc(nsamp * sizeof(complex));
    
    // Initialise input and output buffers
    for(unsigned i = 0; i < nsamp; i++)
        input[i].x = input[i].y = 1;
    memset(output, 0, nsamp * sizeof(complex));

    // ------ Apply PPF -----

    struct timeval start, end;
    long mtime, seconds, useconds;    
    gettimeofday(&start, NULL);

    apply_ppf(fftw_input, fftw_output, window, input, output);

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Time: %ldms\n", mtime);  
}

