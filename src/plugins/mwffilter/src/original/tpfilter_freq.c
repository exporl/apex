/************************************
 *  Program:    Program for testing noise cancellation in speech
 *
 *  File:       tpfilter.c
 *
 *  Component:  TP - file with the filters used
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       21.06.2006
 *
 *
 *
 ***********************************/

/************************************
 * INCLUDE FILES
 ***********************************/
#include "tp.h"
#include "tpfilter.h"

 /************************************
 * GLOBAL DEFENITIONS & TYPES
 ***********************************/

/************************************
 * GLOBAL VARIABLES
 ***********************************/

/************************************
 * LOCAL FUNCTION DECLARATIONS
 ***********************************/
void AddMatrix_data(SAMPLETYPE *mat1_data, SAMPLETYPE *result_data, int N, SAMPLETYPE lambda);

void MC_FIR_Freq_SIMO(SAMPLETYPE *in, SAMPLETYPE **MC_out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft);

void MC_FIR_Freq(SAMPLETYPE **MC_in, SAMPLETYPE **MC_out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft);

void FIR_Freq(SAMPLETYPE *in, SAMPLETYPE *out, int N, SCFIR_Filter *Filter, FFTW_state *fft);

void PointWiseProduct_cplx_data(double *vec1_data_real,
                                double *vec1_data_imag,
                                double *vec2_data_real,
                                double *vec2_data_imag,
                                double *result_data_real,
                                double *result_data_imag,
                                int N);

void MultiplyMatrix_data(double *mat1_data, double factor, int N);

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/


/************************************
 * FFTW_Init()
 ***********************************/
int FFTW_Init(FFTW_state *fft, int size)
{

    int         retVal = TP_SUCCESS;
    int         rank = 0;
    int         howmany_rank = 0;
    int         L1 = 0;
    fftw_iodim  dims[1];
    fftw_iodim  howmany_dims[1];

    /* Initialise fftw plan + data structures (FFT size = N = multiple of 2) */

    rank = 1;               /* Dimension of FFT */
    dims[0].is = 1;         /* Input stride */
    dims[0].os = 1;         /* Output stride */
    howmany_rank = 1;
    howmany_dims[0].n = 1;  /* Number of transforms */
    howmany_dims[0].is = 1; /* Input stride */
    howmany_dims[0].os = 1; /* Output stride */

    // define plan
    L1 = (size/2)+1;
    fft->fftreal = (double *) fftw_malloc(size*sizeof(double)); /* Data vector (size N=2L) for real data of FFT */
    fft->fftR = (double *) fftw_malloc(L1*sizeof(double));  /* Data vector (size L1) for real part of complex data of FFT */
    fft->fftI = (double *) fftw_malloc(L1*sizeof(double));  /* Data vector (size L1) for imaginary part of complex data of FFT */

    dims[0].n = size;    /* Size of FFT */
    fft->pFFT  = fftw_plan_guru_split_dft_r2c(rank,dims,howmany_rank,howmany_dims,fft->fftreal,fft->fftR,fft->fftI,FFTW_ESTIMATE);
    fft->pIFFT = fftw_plan_guru_split_dft_c2r(rank,dims,howmany_rank,howmany_dims,fft->fftR,fft->fftI,fft->fftreal,FFTW_ESTIMATE);

    return(retVal);
}

/************************************
 * FFTW_Free()
 ***********************************/
int FFTW_Free(FFTW_state *fft)
{
    int         retVal = TP_SUCCESS;

    fftw_destroy_plan(fft->pFFT);
    fftw_destroy_plan(fft->pIFFT);
    fftw_free(fft->fftreal);
    fftw_free(fft->fftR);
    fftw_free(fft->fftI);

    return(retVal);
}
/************************************
 * FFTW_Free()
 ***********************************/

int MC_FIR_Freq_Init(MCFIR_Filter *MC_Filter, char *filename)
{
    /* Initialise Multi-channel FIR filter structure (FFT-filtering) */
    /* data (filename) contains time-domain filter coefficients */
    /* MC_Filter->Length will be changed to power of 2 */

    int             retVal = TP_SUCCESS;
    int             i = 0;
    int             k = 0;
    int             l = 0;
    int             L1 = 0;
    int             dataLength = 0;
    Bool_t          file_open = TP_FALSE;
    FILE            *fid;
    SAMPLETYPE      **taps;
    FFTW_state      *fft;

    if (MC_Filter == NULL)
    {
        printf("\nMC_FIR_Freq_Init(): ERROR! *MC_Filter is NULL!");
        retVal = TP_FAILURE;
    }

    if (retVal == TP_SUCCESS)
    {
        // Read filter data
        fid = fopen(filename,"rb");
        if (fid == NULL)
        {
            printf("\nMC_FIR_Freq_Init(): ERROR! Filter file %s not opened correctly", filename);
            retVal = TP_FAILURE;
        }
        else
        {
            file_open = TP_TRUE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        read_params_from_file(fid, &MC_Filter->numChannels, &dataLength);

        taps = (SAMPLETYPE **) calloc(MC_Filter->numChannels, sizeof(double *));
        if (taps == NULL)
        {
            printf("\nTPFilter_perpareFilter(): ERROR! Filter taps array allocation failed");
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        for (i = 0; (i < MC_Filter->numChannels) && (retVal == TP_SUCCESS); i++)
        {
            taps[i] = (SAMPLETYPE *) calloc(dataLength, sizeof(SAMPLETYPE));
            if (taps[i] == NULL)
            {
                printf("\nTPFilter_perpareFilter(): ERROR! Filter taps array allocation failed");
                retVal = TP_FAILURE;
            }
        }
        read_data_from_file(fid, taps, MC_Filter->numChannels, dataLength);
     }

    if (retVal == TP_SUCCESS)
    {
        // Make filterlength power of 2
        MC_Filter->length = 1; // MC_Filter->Length is power of 2 (initialisation 2^0)
        while (MC_Filter->length < dataLength) {
            MC_Filter->length *= 2;
        }
        L1 = MC_Filter->length+1;

        // FFTW structure
        fft = (FFTW_state *) calloc(1, sizeof(FFTW_state));
        FFTW_Init(fft, 2*MC_Filter->length);

        // Frequency-domain filter
        MC_Filter->filterTaps = (double **) calloc(MC_Filter->numChannels, sizeof(double *));
        for (i=0; i<MC_Filter->numChannels; i++) {
            MC_Filter->filterTaps[i] = (double *) calloc(2*L1, sizeof(double));   // Store both real + imaginary part

            /* Initialise to zero */
            memset(fft->fftreal, 0, 2*MC_Filter->length*sizeof(SAMPLETYPE));

            memcpy(fft->fftreal,taps[i],dataLength*sizeof(double));                  // First dataLength = taps, Rest + second half = zero
            fftw_execute_split_dft_r2c(fft->pFFT,fft->fftreal,fft->fftR,fft->fftI);
            memcpy(MC_Filter->filterTaps[i],fft->fftR,L1*sizeof(double));         // First part = real
            memcpy(MC_Filter->filterTaps[i]+L1,fft->fftI,L1*sizeof(double));      // Second part = imaginary
        }

        // Initialise delayline
        MC_Filter->dlyLine = (double **) calloc(MC_Filter->numChannels, sizeof(double*));
        for (i=0; i<MC_Filter->numChannels; i++) {
            MC_Filter->dlyLine[i] = (double *) calloc(MC_Filter->length, sizeof(double));
            /* Initialise to zero */
            memset(MC_Filter->dlyLine[i], 0, MC_Filter->length*sizeof(SAMPLETYPE));
        }

        // Free data
        if (taps != NULL)
        {
            for (i = 0; (i < MC_Filter->numChannels) && (taps[i] != NULL);  i++)
            {
                free(taps[i]);
            }
            free(taps);
        }

        FFTW_Free(fft);
        free(fft);
    }

    if (file_open == TP_TRUE)
    {
        if (0 != fclose(fid))
        {
            printf("\nTPFilter_perpareFilter(): WARNING! Problems closing file!");
        }
    }

    return(retVal);
}

/************************************
 * MC_FIR_Freq_SIMO_Mix()void AddMatrix_data(double *mat1_data, double *result_data, int N, double lambda)

 ***********************************/

int MC_FIR_Freq_SIMO_Mix(SAMPLETYPE *speech,
                         int numNoiseSources,
                         SAMPLETYPE **noise,
                         SAMPLETYPE **MC_out,
                         int N,
                         MCFIR_Filter *MC_FilterSpeech,
                         MCFIR_Filter **MC_FilterNoise,
                         SAMPLETYPE mix,
                         FFTW_state *fft)
{
/* Filter 2 signals using multichannel FIR filters + mix output
    /* out = mix (MC_Filter1 o speech) + (MC_Filter2 o noise) with o = convolution */
/* SIMO FIR filter: speech, noise               : input (size N x 1)
    MC_out                  : output (size N x NrChannels)
    MC_FilterSpeech, MC_Filter2  : filter structure (NrChannels)
    mix                     : mixing factor */

    int          i = 0;
    int          j = 0;
    int          NrChannels = 0;
    SAMPLETYPE   **dataSpeech;
    SAMPLETYPE   **data[numNoiseSources];

    NrChannels = MC_FilterNoise[0]->numChannels; /* there always is at least one noise source, even when speech not present */

    /* Temporary data structure: calloc assures memory is zeroed*/
    dataSpeech = (SAMPLETYPE **) calloc(NrChannels, sizeof(SAMPLETYPE *));
    for (i=0; i<NrChannels; i++) {
        dataSpeech[i] = (SAMPLETYPE *) calloc(N, sizeof(SAMPLETYPE));
    }

    for (i=0; i < numNoiseSources; i++)
    {
        data[i] = (SAMPLETYPE **) calloc(NrChannels, sizeof(SAMPLETYPE *));
        for (j=0; j<NrChannels; j++)
        {
            data[i][j] = (SAMPLETYPE *) calloc(N, sizeof(SAMPLETYPE));
        }
    }

    /* Multi-channel filtering */
    if (MC_FilterSpeech != NULL)
    {
        MC_FIR_Freq_SIMO(speech, dataSpeech, N, MC_FilterSpeech, fft);
    }

    for (i = 0; i < numNoiseSources; i++)
    {
        if (MC_FilterNoise[i] != NULL)
        {
            MC_FIR_Freq_SIMO(noise[i], data[i], N, MC_FilterNoise[i], fft);
        }
    }

    for (i = 0; i < numNoiseSources; i++)
    {
        /* Mixing */
        for (j=0; (j<NrChannels) && (MC_FilterNoise[i] != NULL); j++)
        {
            AddMatrix_data(data[i][j], MC_out[j], N, 1);
        }
    }

    /* now scale all noises */
    for (i=0; (i<NrChannels); i++)
    {
        for (j=0;j<N;j++)
        {
            MC_out[i][j] = (MC_out[i][j])/sqrt(numNoiseSources);
        }
    }

    /* Mixing */
    for (i=0; i<NrChannels; i++)
    {
        AddMatrix_data(dataSpeech[i], MC_out[i], N, mix);
    }
    /* Free data */
    for (i=0; i<NrChannels; i++) {
        free(dataSpeech[i]);
    }
    free(dataSpeech);

    for (i = 0; i < numNoiseSources; i++)
    {
        for (j=0; j<NrChannels; j++)
        {
            free(data[i][j]);
        }
        free(data[i]);
    }
}
/************************************
 * MC_FIR_Freq_FilterSum()
 ***********************************/
void MC_FIR_Freq_FilterSum(SAMPLETYPE **MC_in, SAMPLETYPE *out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft)
{
/* Multi-channel FIR filter:    MC_in       : input (size N x NrChannels)
    out         : output (size N)
    MC_Filter   : filter structure (NrChannels) */

    int             i = 0;
    SCFIR_Filter    *FIR1;  // Single-channel FIR Filter
    SAMPLETYPE      *tmp_data;

    /* Single channel filter */
    FIR1 = (SCFIR_Filter *) calloc(1, sizeof(SCFIR_Filter));
    FIR1->length = MC_Filter->length;

    tmp_data = (SAMPLETYPE *) calloc(N, sizeof(SAMPLETYPE));

    /*Initialise to zero */
    memset(out, 0, N*sizeof(SAMPLETYPE)); /* Initialise output to zero */

    for (i=0; i<MC_Filter->numChannels; i++) {
        FIR1->filterTaps = MC_Filter->filterTaps[i];        /* Initialise filter for channel i */
        FIR1->dlyLine = MC_Filter->dlyLine[i];

        FIR_Freq(MC_in[i], tmp_data, N, FIR1, fft);     /* Filter channel i */
        AddMatrix_data(tmp_data, out, N, 1);    /* Add filtered channel i to output */
    }

    /* Free data */
    free(tmp_data);
    free(FIR1);
}

/**
 * Need a seperate functio for initializing single channel filters
 *
 **/


int SC_FIR_Freq_Init(SCFIR_Filter *SC_Filter, char *filename)
{
    /* Initialise Multi-channel FIR filter structure (FFT-filtering) */
    /* data (filename) contains time-domain filter coefficients */
    /* MC_Filter->Length will be changed to power of 2 */

    int             retVal = TP_SUCCESS;
    FILE            *fid;
    int             i = 0;
    int             k,l;
    int             dataLength, L1;
    int             numChannels;
    SAMPLETYPE      *taps;
    FFTW_state      *fft;
    size_t          dataRead;

    // Read filter data
    fid = fopen(filename,"rb");
    if (fid != NULL)
    {
        read_params_from_file(fid, &numChannels, &dataLength);

        if (numChannels != 1)
        {
            printf("\nSC_FIR_Freq_Init() multichannel filter detected, numChannels: %d", numChannels);
            retVal = TP_FAILURE;
        }
        else
        {
            taps = (SAMPLETYPE *) calloc(dataLength, sizeof(SAMPLETYPE));
            if (taps == NULL)
            {
                printf("\nSC_FIR_Freq_Init(): ERROR! Filter taps array allocation failed");
                retVal = TP_FAILURE;
            }

            dataRead = fread(taps, sizeof(SAMPLETYPE), dataLength, fid);
            if(dataRead != (size_t)dataLength)
            {
                retVal = TP_FAILURE;
                printf("\nSC_FIR_Freq_Init(): ERROR! Wrong number of coeffs read for channel filter.");
            }
            fclose(fid);

            // Make filterlength power of 2
            SC_Filter->length = 1; // MC_Filter->Length is power of 2 (initialisation 2^0)
            while (SC_Filter->length < dataLength) {
                SC_Filter->length *= 2;
            }
            L1 = SC_Filter->length+1;

            // FFTW structure
            fft = (FFTW_state *) calloc(1, sizeof(FFTW_state));
            FFTW_Init(fft, 2*SC_Filter->length);

            // Frequency-domain filter
            SC_Filter->filterTaps = (double *) calloc(2*L1, sizeof(double));   // Store both real + imaginary part

            /* Initialise to zero */
            memset(fft->fftreal, 0, 2*SC_Filter->length*sizeof(SAMPLETYPE));

            memcpy(fft->fftreal,taps,dataLength*sizeof(double));                  // First dataLength = taps, Rest + second half = zero
            fftw_execute_split_dft_r2c(fft->pFFT,fft->fftreal,fft->fftR,fft->fftI);
            memcpy(SC_Filter->filterTaps,fft->fftR,L1*sizeof(double));         // First part = real
            memcpy(SC_Filter->filterTaps+L1,fft->fftI,L1*sizeof(double));      // Second part = imaginary

            // Initialise delayline
            SC_Filter->dlyLine = (double *) calloc(SC_Filter->length, sizeof(double));
            /* Initialise to zero */
            memset(SC_Filter->dlyLine, 0, SC_Filter->length*sizeof(SAMPLETYPE));

            // Free data
            if (taps != NULL)
            {
                free(taps);
            }

            FFTW_Free(fft);
            free(fft);
        }
    }
    else
    {
        printf("\nSC_FIR_Freq_Init(): ERROR! Filter file %s not opened correctly", filename);
        retVal = TP_FAILURE;
    }

    return(retVal);
}

/*****
 * Need a seperate functio for freeing single channel filters
 *
 **/


int SC_FIR_Freq_Free(SCFIR_Filter *SC_Filter)
{
    int             retVal = TP_SUCCESS;

    if (SC_Filter == NULL)
    {
        printf("\nSC_FIR_Freq_Free(): ERROR input filter is NULL!");
        retVal = TP_FAILURE;
    }
    else
    {
        if (SC_Filter->filterTaps != NULL)
        {
            free(SC_Filter->filterTaps);
        }

        if (SC_Filter->dlyLine != NULL)
        {
            free(SC_Filter->dlyLine);
        }
    }

    return(retVal);
}

/************************************
 * LOCAL FUNCTION DEFINITIONS
 ***********************************/
/**
 *
 */
void AddMatrix_data(SAMPLETYPE *mat1_data, SAMPLETYPE *result_data, int N, SAMPLETYPE lambda)
{
    /* Add scaled matrix to other matrix */
    /* out = out+lambda*mat1; */
    /* COMPLEXITY = N ADD + N MULT (lambda ~= 1) */
    /*            = N ADD + 0 MULT (lambda == 1) */
    int i;

    for (i=0;i<N;i++) {
        *result_data += lambda*(*mat1_data++);
        result_data++;
    }
}

/**
 *
 */

void MC_FIR_Freq_SIMO(SAMPLETYPE *in, SAMPLETYPE **MC_out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft)
{
/* SIMO FIR filter: in              : input (size N x 1)
    MC_out          : output (size N x NrChannels)
    MC_Filter       : filter structure (NrChannels) */

    int         i = 0;
    SAMPLETYPE  **MC_in;

    MC_in = (SAMPLETYPE **) calloc(MC_Filter->numChannels, sizeof(SAMPLETYPE *));

    // replicate data from single-channel input to multi-channel input
    // hence the function MC_FIR can be used
    for (i=0; i<MC_Filter->numChannels; i++)
    {
        MC_in[i] = (SAMPLETYPE *) calloc(N, sizeof(SAMPLETYPE));
        memcpy(MC_in[i], in, N*sizeof(SAMPLETYPE));
    }
    MC_FIR_Freq(MC_in, MC_out, N, MC_Filter, fft);
    /* Free data */
    for (i=0; i<MC_Filter->numChannels; i++) {free(MC_in[i]);}
    free(MC_in);

}

/**
 *
 */

void MC_FIR_Freq(SAMPLETYPE **MC_in, SAMPLETYPE **MC_out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft)
{
/* Multi-channel FIR filter:    MC_in       : input (size N x NrChannels)
    MC_out      : output (size N x NrChannels)
    MC_Filter   : filter structure (NrChannels) */

    int             i = 0;
    SCFIR_Filter    *FIR1;  // Single-channel FIR Filter

    FIR1 = (SCFIR_Filter *) calloc(1, sizeof(SCFIR_Filter));
    FIR1->length = MC_Filter->length;

    for (i=0; i<MC_Filter->numChannels; i++) {
        FIR1->filterTaps = MC_Filter->filterTaps[i];
        FIR1->dlyLine = MC_Filter->dlyLine[i];

        FIR_Freq(MC_in[i], MC_out[i], N, FIR1, fft);
    }

    free(FIR1);
}

/**
 *
 */

void FIR_Freq(SAMPLETYPE *in, SAMPLETYPE *out, int N, SCFIR_Filter *Filter, FFTW_state *fft)
{
    /* Frequency-domain FIR filtering */
    /* Dimensions: in (N), out (N) */
    /* Filter->Length (L), Filter->DlyLine (L), Filter->Taps 2*(L+1) (real+imaginary part)*/
    /* !! N needs to be a multiple of L !! */
    /* FFT size = 2L is preferably power of 2 */

    int         k, L1, nr_blocks;
    SAMPLETYPE  inv2L;
    SAMPLETYPE  *data, *tmp_data, *fftreal2;
    SAMPLETYPE  *data_real, *data_imag;
    SAMPLETYPE  *W_data_real, *W_data_imag;

    // Input parameters
    L1 = Filter->length+1;
    nr_blocks = (int) (N/Filter->length);  // Number of frames: should be integer !!
    inv2L = (SAMPLETYPE) 1/(2*Filter->length);  // Normalisation factor IFFT

    // Data definitions
    W_data_real = Filter->filterTaps;             // First part of Filter->Taps = real
    W_data_imag = Filter->filterTaps + L1;        // Second part of Filter->Taps = imaginary
    fftreal2 = fft->fftreal + Filter->length;   // Second half of vector

    // Memory allocation
    data = (SAMPLETYPE *) calloc(Filter->length + N, sizeof(SAMPLETYPE));
    data_real = (SAMPLETYPE *) calloc(L1, sizeof(SAMPLETYPE));
    data_imag = (SAMPLETYPE *) calloc(L1, sizeof(SAMPLETYPE));

    // Copy delayline + input to data
    tmp_data = data;
    memcpy(tmp_data, Filter->dlyLine, Filter->length*sizeof(SAMPLETYPE));
    tmp_data += Filter->length;
    memcpy(tmp_data, in, N*sizeof(SAMPLETYPE));

    // FFT Filtering
    tmp_data = data;

    for (k=0; k<nr_blocks; k++)
    {
        // FFT of input
        memcpy(fft->fftreal,tmp_data,2*Filter->length*sizeof(SAMPLETYPE));
        fftw_execute_split_dft_r2c(fft->pFFT,fft->fftreal,fft->fftR,fft->fftI);

        // Filtering
        PointWiseProduct_cplx_data(fft->fftR,fft->fftI,W_data_real,W_data_imag,data_real,data_imag,L1);

        // IFFT of result
        memcpy(fft->fftR,data_real,L1*sizeof(SAMPLETYPE));
        memcpy(fft->fftI,data_imag,L1*sizeof(SAMPLETYPE));
        fftw_execute_split_dft_c2r(fft->pIFFT,fft->fftR,fft->fftI,fft->fftreal); /* FACTOR 2L */
        MultiplyMatrix_data(fftreal2,inv2L,Filter->length);

        // Store only second-half
        memcpy(out,fftreal2,Filter->length*sizeof(SAMPLETYPE));

        tmp_data += Filter->length; /* Next frame */
        out += Filter->length; /* Next frame */
    }

    // Update delay line
    memcpy(Filter->dlyLine, tmp_data, Filter->length*sizeof(SAMPLETYPE));

    // Free data
    free(data);
    free(data_real);
    free(data_imag);
}
/**
 *
 */
void PointWiseProduct_cplx_data(double *vec1_data_real,
                                double *vec1_data_imag,
                                double *vec2_data_real,
                                double *vec2_data_imag,
                                double *result_data_real,
                                double *result_data_imag,
                                int N)
        /* Calculate pointwise product of 2 complex vectors (of equal length) */
        /* out = vec1.*vec2; */
        /* COMPLEXITY = 2N ADD + 4N MULT */
{
    int i;
    double a1R,a2R,a1I,a2I;

    for (i=0;i<N;i++) {
        a1R = *vec1_data_real++;
        a1I = *vec1_data_imag++;
        a2R = *vec2_data_real++;
        a2I = *vec2_data_imag++;

        *result_data_real++ = a1R*a2R-a1I*a2I;
        *result_data_imag++ = a1R*a2I+a1I*a2R;
    }
}
/**
 *
 */

void MultiplyMatrix_data(double *mat1_data, double factor, int N)
        /* Multiply real matrix with scalar */
        /* mat1 = factor*mat1 */
        /* COMPLEXITY = 0 ADD + N MULT */
{
    int i;

    for (i=0;i<N;i++) {
        *mat1_data *= factor;
        mat1_data++;
    }
}

/*******************************
 * MC_FIR_Freq_Free
 *******************************/
int MC_FIR_Freq_Free(MCFIR_Filter *MC_Filter)
{
    /* Initialise Multi-channel FIR filter structure (FFT-filtering) */
    /* data (filename) contains time-domain filter coefficients */
    /* MC_Filter->Length will be changed to power of 2 */

    int             retVal = TP_SUCCESS;
    int             i = 0;

    if (MC_Filter == NULL)
    {
        printf("\nMC_FIR_Freq_Free(): ERROR! *MC_Filter is NULL!");
        retVal = TP_FAILURE;
    }

    if (retVal == TP_SUCCESS)
    {
        if(MC_Filter->filterTaps != NULL)
        {
            for (i=0; i<MC_Filter->numChannels; i++)
            if(MC_Filter->filterTaps[i] != NULL)
            {
                free(MC_Filter->filterTaps[i]);
            }
        free(MC_Filter->filterTaps);
        }

        if(MC_Filter->dlyLine != NULL)
        {
            for (i=0; i<MC_Filter->numChannels; i++)
            if(MC_Filter->dlyLine[i] != NULL)
            {
                free(MC_Filter->dlyLine[i]);
            }
        free(MC_Filter->dlyLine);
        }
    }
    return(retVal);
}
