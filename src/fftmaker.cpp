#include "fftmaker.h"

FFTMaker::FFTMaker(FFT_DATATYPE selected_datatype, FFT_DIM selected_dim, FFT_PROCESS selected_process, FFT_FLAG selected_flag, int N1, int N2, int n_samples)
{
    m_fft_datatype = selected_datatype;
    m_fft_dim = selected_dim;
    m_fft_process = selected_process;
    m_fft_flag = selected_flag;
    this->N1 = N1;
    this->N2 = N2;
    fft_method = 0;
}

void FFTMaker::destroyFFTPlan()
{
    fftw_destroy_plan(fft_plan);
}

void FFTMaker::executeFFTPlan()
{
    fftw_execute(fft_plan);
}

void FFTMaker::clearArrays()
{
    switch(fft_method)
    {
        case 1: { fftw_free(inR); fftw_free(outC); break; }
        case 2: { fftw_free(inC); fftw_free(outR); break; }
        case 3: { fftw_free(inR); fftw_free(outC); break; }
        case 4: { fftw_free(inC); fftw_free(outR); break; }
        case 5: { fftw_free(inC); fftw_free(outC); break; }
        case 6: { fftw_free(inC); fftw_free(outC); break; }
        case 7: { fftw_free(inC); fftw_free(outC); break; }
        case 8: { fftw_free(inC); fftw_free(outC); break; }
    }
}

void FFTMaker::flushData(Array2DSpectrum<double> *outputR, Array2DSpectrum<complex<double> > *outputC)
{
    switch(fft_method)
    {
        case 1: {
            int cellsToCopy = outputC->getChannelCount()*outputC->getSampleCount();
            memcpy(&outputC->data[0],&outC[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 2: {
            int cellsToCopy = outputR->getChannelCount()*outputR->getSampleCount();
            memcpy(&outputR->data[0],&outR[0],cellsToCopy*sizeof(double));
            break;
        }
        case 3: {
            int cellsToCopy = outputC->getChannelCount()*outputC->getSampleCount();
            memcpy(&outputC->data[0],&outC[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 4: {
            int cellsToCopy = outputR->getChannelCount()*outputR->getSampleCount();
            memcpy(&outputR->data[0],&outR[0],cellsToCopy*sizeof(double));
            break;
        }
        case 5: {
            int cellsToCopy = outputC->getChannelCount()*outputC->getSampleCount();
            memcpy(&outputC->data[0],&outC[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 6: {
            int cellsToCopy = outputC->getChannelCount()*outputC->getSampleCount();
            memcpy(&outputC->data[0],&outC[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 7: {
            int cellsToCopy = outputC->getChannelCount()*outputC->getSampleCount();
            memcpy(&outputC->data[0],&outC[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 8: {
            int cellsToCopy = outputC->getChannelCount()*outputC->getSampleCount();
            memcpy(&outputC->data[0],&outC[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
    }
}

void FFTMaker::switchData(Array2DSpectrum<double> *inputR, Array2DSpectrum<complex<double> > *inputC)
{
    switch(fft_method)
    {
        case 1: {
            int cellsToCopy = inputR->getChannelCount()*inputR->getSampleCount();
            memcpy(&this->inR[0],&inputR->data[0],cellsToCopy*sizeof(float));
            break;
        }
        case 2: {
            int cellsToCopy = inputC->getChannelCount()*inputC->getSampleCount();
            memcpy(&this->inC[0],&inputC->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 3: {
            int cellsToCopy = inputR->getChannelCount()*inputR->getSampleCount();
            memcpy(&this->inR[0],&inputR->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 4: {
            int cellsToCopy = inputC->getChannelCount()*inputC->getSampleCount();
            memcpy(&this->inC[0],&inputC->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 5: {
            int cellsToCopy = inputC->getChannelCount()*inputC->getSampleCount();
            memcpy(&this->inC[0],&inputC->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 6: {
            int cellsToCopy = inputC->getChannelCount()*inputC->getSampleCount();
            memcpy(&this->inC[0],&inputC->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 7: {
            int cellsToCopy = inputC->getChannelCount()*inputC->getSampleCount();
            memcpy(&this->inC[0],&inputC->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
        case 8: {
            int cellsToCopy = inputC->getChannelCount()*inputC->getSampleCount();
            memcpy(&this->inC[0],&inputC->data[0],cellsToCopy*sizeof(fftw_complex));
            break;
        }
    }
}


void FFTMaker::createFFTPlan()
{
    switch (m_fft_datatype)
    {
    case REAL:
    {
        switch (m_fft_dim)
        {
        case ONE_D:
            switch (m_fft_process)
            {
            case FORWARD:
            {
                inR = (double*) fftw_malloc(sizeof(double) * N1);
                outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((N1/2)+1));
                fft_method = 1;
                switch (m_fft_flag)
                {
                case MEASURE:
                    int rank = 1; /* not 2: we are computing 1d transforms */
                    int n[] = {N1}; /* 1d transforms of length N1 */
                    int howmany = n_samples;
                    int idist = odist = N1;
                    int istride = ostride = 1; /* contiguous in memory */
                    int *inembed = n, *onembed = n;
                    fft_plan = fftw_plan_many_dft_r2c(rank,n,howmany,inR,inembed,istride,idist,outC,onembed,ostride,odist,FFTW_MEASURE);
                    //fft_plan = fftw_plan_dft_r2c_1d(N1, inR, outC, FFTW_MEASURE);
                    break;
                case ESTIMATE:
                    int rank = 1; /* not 2: we are computing 1d transforms */
                    int n[] = {N1}; /* 1d transforms of length N1 */
                    int howmany = n_samples;
                    int idist = odist = N1;
                    int istride = ostride = 1; /* contiguous in memory */
                    int *inembed = n, *onembed = n;
                    fft_plan = fftw_plan_many_dft_r2c(rank,n,howmany,inR,inembed,istride,idist,outC,onembed,ostride,odist,FFTW_ESTIMATE);
                    //fft_plan = fftw_plan_dft_r2c_1d(N1, inR, outC, FFTW_ESTIMATE);
                    break;
                }
                break;
            }
            case BACKWARD:
            {
                inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((N1/2)+1));
                outR = (double*) fftw_malloc(sizeof(double) * N1);
                fft_method = 2;
                switch (m_fft_flag)
                {
                case MEASURE:
                    int rank = 1; /* not 2: we are computing 1d transforms */
                    int n[] = {N1}; /* 1d transforms of length N1 */
                    int howmany = n_samples;
                    int idist = odist = N1;
                    int istride = ostride = 1; /* contiguous in memory */
                    int *inembed = n, *onembed = n;
                    fft_plan = fftw_plan_many_dft_c2r(rank,n,howmany,inC,inembed,istride,idist,outR,onembed,ostride,odist,FFTW_MEASURE);
                    //fft_plan = fftw_plan_dft_c2r_1d(N1, inC, outR, FFTW_MEASURE);
                    break;
                case ESTIMATE:
                    int rank = 1; /* not 2: we are computing 1d transforms */
                    int n[] = {N1}; /* 1d transforms of length N1 */
                    int howmany = n_samples;
                    int idist = odist = N1;
                    int istride = ostride = 1; /* contiguous in memory */
                    int *inembed = n, *onembed = n;
                    fft_plan = fftw_plan_many_dft_c2r(rank,n,howmany,inC,inembed,istride,idist,outR,onembed,ostride,odist,FFTW_ESTIMATE);
                    //fft_plan = fftw_plan_dft_c2r_1d(N1, inC, outR, FFTW_ESTIMATE);
                    break;
                }
                break;
            }
            }
        case TWO_D:
            switch (m_fft_process)
            {
            case FORWARD:
            {
                inR = (double*) fftw_malloc(sizeof(double) * N1 * N2);
                outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((N1/2)+1) * N2);
                fft_method = 3;
                switch (m_fft_flag)
                {
                case MEASURE:
                    fft_plan = fftw_plan_dft_r2c_2d(N1,N2, inR, outC, FFTW_MEASURE);
                case ESTIMATE:
                    fft_plan = fftw_plan_dft_r2c_2d(N1,N2, inR, outC, FFTW_ESTIMATE);
                }
                break;
            }
            case BACKWARD:
            {
                inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((N1/2)+1) * N2);
                outR = (double*) fftw_malloc(sizeof(double) * N1 * N2);
                fft_method = 4;
                switch (m_fft_flag)
                {
                case MEASURE:
                    fft_plan = fftw_plan_dft_c2r_2d(N1,N2, inC, outR, FFTW_MEASURE);
                case ESTIMATE:
                    fft_plan = fftw_plan_dft_c2r_2d(N1,N2, inC, outR, FFTW_ESTIMATE);
                }
                break;
            }
            }
        }
    }
    case COMPLEX:
    {
        switch (m_fft_dim)
        {
        case ONE_D:
            switch(m_fft_process)
            {
                case FORWARD:
                {
                    inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N1);
                    outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N2);
                    fft_method = 5;
                    switch (m_fft_flag)
                        {
                        case MEASURE:
                        {
                            int rank = 1; /* not 2: we are computing 1d transforms */
                            int n[] = {N1}; /* 1d transforms of length N1 */
                            int howmany = n_samples;
                            int idist = odist = N1;
                            int istride = ostride = 1; /* contiguous in memory */
                            int *inembed = n, *onembed = n;
                            fft_plan = fftw_plan_many_dft(rank,n,howmany,inC,inembed,istride,idist,outC,onembed,ostride,odist,FFTW_FORWARD,FFTW_MEASURE);
                            //fft_plan = fftw_plan_dft_1d(N1, inC, outC, FFTW_FORWARD, FFTW_MEASURE);
                            break;
                        }
                        case ESTIMATE:
                        {
                            int rank = 1; /* not 2: we are computing 1d transforms */
                            int n[] = {N1}; /* 1d transforms of length N1 */
                            int howmany = n_samples;
                            int idist = odist = N1;
                            int istride = ostride = 1; /* contiguous in memory */
                            int *inembed = n, *onembed = n;
                            fft_plan = fftw_plan_many_dft(rank,n,howmany,inC,inembed,istride,idist,outC,onembed,ostride,odist,FFTW_FORWARD,FFTW_ESTIMATE);
                            //fft_plan = fftw_plan_dft_1d(N1, inC, outC, FFTW_FORWARD, FFTW_ESTIMATE);
                            break;
                        }
                    }
                    break;
                }
                case BACKWARD:
                {
                    inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N1);
                    outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N2);
                    fft_method = 6;
                    switch (m_fft_flag)
                        {
                        case MEASURE:
                        {
                            int rank = 1; /* not 2: we are computing 1d transforms */
                            int n[] = {N1}; /* 1d transforms of length N1 */
                            int howmany = n_samples;
                            int idist = odist = N1;
                            int istride = ostride = 1; /* contiguous in memory */
                            int *inembed = n, *onembed = n;
                            fft_plan = fftw_plan_many_dft(rank,n,howmany,inC,inembed,istride,idist,outC,onembed,ostride,odist,FFTW_BACKWARD,FFTW_MEASURE);
                            //fft_plan = fftw_plan_dft_1d(N1, inC, outC, FFTW_BACKWARD, FFTW_MEASURE);
                            break;
                        }
                        case ESTIMATE:
                        {
                            int rank = 1; /* not 2: we are computing 1d transforms */
                            int n[] = {N1}; /* 1d transforms of length N1 */
                            int howmany = n_samples;
                            int idist = odist = N1;
                            int istride = ostride = 1; /* contiguous in memory */
                            int *inembed = n, *onembed = n;
                            fft_plan = fftw_plan_many_dft(rank,n,howmany,inC,inembed,istride,idist,outC,onembed,ostride,odist,FFTW_BACKWARD,FFTW_ESTIMATE);
                            //fft_plan = fftw_plan_dft_1d(N1, inC, outC, FFTW_BACKWARD, FFTW_ESTIMATE);
                            break;
                        }
                    }
                    break;
                }
            }
        case TWO_D:
            switch(m_fft_process)
            {
                case FORWARD:
                {
                    inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N1 * N2);
                    outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N1 * N2);
                    fft_method = 7;
                    switch (m_fft_flag)
                        {
                        case MEASURE:
                        {
                            fft_plan = fftw_plan_dft_2d(N1,N2, inC, outC, FFTW_FORWARD, FFTW_MEASURE);
                            break;
                        }
                        case ESTIMATE:
                        {
                            fft_plan = fftw_plan_dft_2d(N1,N2, inC, outC, FFTW_FORWARD, FFTW_ESTIMATE);
                            break;
                        }
                    }
                    break;
                }
                case BACKWARD:
                {
                    inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N1 * N2);
                    outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N1 * N2);
                    fft_method = 8;
                    switch (m_fft_flag)
                        {
                        case MEASURE:
                        {
                            fft_plan = fftw_plan_dft_2d(N1,N2, inC, outC, FFTW_BACKWARD, FFTW_MEASURE);
                            break;
                        }
                        case ESTIMATE:
                        {
                            fft_plan = fftw_plan_dft_2d(N1,N2, inC, outC, FFTW_BACKWARD, FFTW_ESTIMATE);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    }
}

