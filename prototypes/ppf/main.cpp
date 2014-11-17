#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <fftw3.h>
#include <iostream>
#include <omp.h>
using namespace std;

unsigned ntaps  = 8;
unsigned nfft   = 16384;
unsigned nsamp  = 65536 * 64 * 32;
fftwf_plan plan;

typedef struct
{
    float x, y;
} complex;

// Apply polyphase filterbank
void apply_ppf(fftwf_complex* fftw_input, fftwf_complex* fftw_output, float *window, complex *input, complex *output, complex *fifo)
{
    // Create array pointers
    complex *fifo_ptrs[ntaps];

    // Copy fifo to input "space"
    memcpy(input, fifo, ntaps * nfft * sizeof(float));

    // Initialise pointers
    for(unsigned t = 0; t < ntaps; t++)
        fifo_ptrs[t] = input + t * nfft;

    // Set number of openmp threads
    int num_threads = 8;
    omp_set_num_threads(num_threads);

    // Loop over input blocks of size nfft
    #pragma omp parallel
    {
        // Get thread id
        int threadId = omp_get_thread_num();

        for(unsigned b = ntaps * nfft + threadId * nfft;
                     b < nsamp + ntaps * nfft;
                     b += nfft * num_threads)
        {
            // Reset fftw buffers
            for(unsigned i = 0; i < nfft; i++)
                fftw_input[i][0] = fftw_input[i][1] = 0;

            // Loop over ntaps
            for(unsigned t = 0; t < ntaps; t++)
            {
                // Loop over samples
                for(unsigned s = 0; s < nfft; s++)
                {
                    // Pre-load value to improve ILP
                    float coeff = window[t * nfft + s];
                    complex value = *(fifo_ptrs[t]);

                    // Apply taps
                    fftw_input[s][0] += value.x * coeff;
                    fftw_input[s][1] += value.y * coeff;

                    // Update fifo pointer
                    fifo_ptrs[t]++;
                }
            }

            // Apply fft
            fftwf_execute_dft(plan, fftw_input, fftw_output);

            // Copy fft'ed result to output buffer
            memcpy(output + b - (ntaps * nfft), fftw_output, sizeof(complex) * nfft);
        }
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
    float *temp = (float *) malloc(nsamp * sizeof(float));
    fread(temp, sizeof(float), nsamp, fp);
    fclose(fp);

    for(unsigned i = 0; i < nsamp; i++)
    {
        buffer[i].x = temp[i];
        buffer[i].y = 0;
    }

    free(temp);
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
    float *window = (float *) malloc(nfft * ntaps * sizeof(float));

    // Allocate fifo buffer
    complex *fifo = (complex *) malloc(nfft * ntaps * sizeof(complex));

    // Allocate temporary buffers
    fftwf_complex* fftw_input = (fftwf_complex *) malloc(nfft * sizeof(fftwf_complex));
    fftwf_complex* fftw_output = (fftwf_complex *) malloc(nfft * sizeof(fftwf_complex));
    memset(fftw_input, 0, nfft * sizeof(fftwf_complex));
    memset(fftw_output, 0, nfft * sizeof(fftwf_complex));

    // Fill up coefficients
    for(unsigned i = 0; i < nfft * ntaps; i++)
            window[i] = 1.0;

    // Create FFTW plan
    fftwf_complex* in  = (fftwf_complex*) fftwf_malloc(ntaps * nfft * sizeof(fftwf_complex));
    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(ntaps * nfft * sizeof(fftwf_complex));
    plan = fftwf_plan_dft_1d(nfft, in, out, FFTW_FORWARD, FFTW_MEASURE);
    fftwf_free(in);
    fftwf_free(out);

    // Create input and output buffers
    complex *input = (complex *) malloc((nfft * ntaps + nsamp) * sizeof(complex));

    // [S0 [C0 C1 C2 ... CN] ... SN [C0 C1 C2 ... CN]]
    complex *output = (complex *) malloc(nsamp * sizeof(complex));

    // Initialise input and output buffers
    for(unsigned i = 0; i < nfft * ntaps + nsamp; i++)
        input[i].x = input[i].y = 1;
    memset(output, 0, nsamp * sizeof(complex));

    // ------ Generate Fake Data ------
    //generate_data(input);
    //dump_to_disk("input_data.dat", input, sizeof(complex), nsamp);

    // Load generated test data
    //load_generated_data("generated_data.dat", input);


    // ------ Load weights [Needs to be changed] ------
//    char filename[256];
//    sprintf(filename, "coeff_%d_%d.dat", ntaps, nfft);
//    FILE *fp = fopen(filename, "rb");
//    float *temp = (float *) malloc(ntaps * nfft * sizeof(float));
//    fread(temp, sizeof(float), nfft * ntaps, fp);
//    for(unsigned i = 0; i < nfft; i++)
//        for(unsigned j = 0; j < ntaps; j++)
//            window[i][j] = temp[j * nfft + i];
//    free(temp);

    // ------ Apply PPF -----

    struct timeval start, end;
    long mtime, seconds, useconds;
    gettimeofday(&start, NULL);

    int nreps = 10;
    for(unsigned i = 0; i < nreps; i++)
        apply_ppf(fftw_input, fftw_output, window, input, output, fifo);

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Time: %ldms\n", mtime / nreps);

    // Output result to disk
    // dump_to_disk("output_data.dat", output, sizeof(complex), nsamp);
}
