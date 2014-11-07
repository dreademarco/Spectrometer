#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <fftw3.h>
#include <iostream>
using namespace std;

unsigned ntaps  = 8;
unsigned nfft   = 1024;
unsigned nsamp  = 65536 * 64;
fftwf_plan plan;

typedef struct
{
    float x, y;
} complex;

// Apply polyphase filterbank
void apply_ppf(fftwf_complex* fftw_input, fftwf_complex* fftw_output, float **window, complex *input, complex *output, complex **fifo)
{
    // Loop over input blocks of size nfft
    for(unsigned b = 0; b < nsamp; b += nfft)
    {
        // Adjust FIFOs
        for(unsigned s = 0; s < nfft; s++)
            for(unsigned i = ntaps - 1; i > 0; i--)
                fifo[s][i] = fifo[s][i - 1];

        // Update FIFOs
        for(unsigned s = 0; s < nfft; s++)
                fifo[s][0] = input[b + s];

        // Reset fftw buffers
        for(unsigned i = 0; i < nfft; i++)
                fftw_input[i][0] = fftw_input[i][1] = 0;




        // Loop of nfft samples
        for(unsigned s = 0; s < nfft; s++)
        {
            // Loop over ntaps
            for(unsigned t = 0; t < ntaps; t++)
            {
                fftw_input[s][0] += fifo[s][t].x * window[s][t];
                fftw_input[s][1] += fifo[s][t].y * window[s][t];
            }
        }

        // TEMP - Override everything
        // memcpy(fftw_input, input + b, sizeof(complex) * nfft);

        // Apply fft
        fftwf_execute_dft(plan, fftw_input, fftw_output);

        // Copy fft'ed result to output buffer
        memcpy(output + b, fftw_output, sizeof(complex) * nfft);
    }
}

// Generate fake data for testing
void generate_data(complex *buffer)
{
    // Simple sinusoids
    for(unsigned i = 0; i < nsamp; i++)
    {
        buffer[i].x = sin(2 * M_PI * i / 40.0) + sin(2 * M_PI * i / 128.0);
        buffer[i].y = 0;
    }
}

// Load data from file
void load_generated_data(char *filename, complex* buffer)
{
    FILE *fp = fopen(filename, "rb");
    fread(buffer, sizeof(complex), nsamp, fp);
    fclose(fp);
}

// Write a buffer to disk
void dump_to_disk(char * filename, void *buffer, int datatype_size, int nelements)
{
    FILE *fp;
    fp = fopen(filename, "wb");
    fwrite(buffer, datatype_size, nelements, fp);
    fclose(fp);
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

    // ------ Generate Fake Data ------
    //generate_data(input);
    //dump_to_disk("input_data.dat", input, sizeof(complex), nsamp);

    // ------  Load Chirp ------
    char filename2[256];
    sprintf(filename2, "generated_data.data");
    load_generated_data(filename2,input);


    // ------ Load weights ------
    char filename[256];
    sprintf(filename, "coeff_%d_%d.dat", ntaps, nfft);
    FILE *fp = fopen(filename, "rb");
    float *temp = (float *) malloc(ntaps * nfft * sizeof(float));
    fread(temp, sizeof(float), nfft * ntaps, fp);
    for(unsigned i = 0; i < nfft; i++)
        for(unsigned j = 0; j < ntaps; j++)
            window[i][j] = temp[j * nfft + i];
    free(temp);

    // ------ Apply PPF -----

    struct timeval start, end;
    long mtime, seconds, useconds;
    gettimeofday(&start, NULL);

    apply_ppf(fftw_input, fftw_output, window, input, output, fifo);

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Time: %ldms\n", mtime);

    // Output result to disk
    dump_to_disk("output_data.dat", output, sizeof(complex), nsamp);
}
