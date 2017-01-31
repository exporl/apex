/************************************
 *  Program:    Program for testing noise cancellation in speech
 *
 *  File:       tpresample.c
 *
 *  Component:  TP - resampling functions
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       22.08.2007
 *
 *
 *
 ***********************************/

/************************************
 * INCLUDE FILES
 ***********************************/

#include "tp.h"
#include "tpresample.h"
#include "tpfilter.h"


 /************************************
 * GLOBAL DEFENITIONS
 ***********************************/
/** Filter used in resampling from 16000 to 32000 */
#define     FILTER_16000_TO_32000           ("filters/upsample2_filter.bin")

#define     UP_FACTOR_16000_TO_32000        (2)

/************************************
 * GLOBAL VARIABLES
 ***********************************/


/************************************
 * LOCAL FUNCTIONS
 ***********************************/

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/
/************************************
 * TPResample_init16000to32000()
 ***********************************/
int TPResample_init16000to32000(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;

    testData->resampleFilter = (SCFIR_Filter *) calloc(1, sizeof(SCFIR_Filter));
    SC_FIR_Freq_Init(testData->resampleFilter, FILTER_16000_TO_32000);

    testData->fft_resample = (FFTW_state *) calloc(1, sizeof(FFTW_state));
    FFTW_Init(testData->fft_resample, 2*(testData->resampleFilter->length));

    return retVal;
}

/************************************
 * TPResample_close16000to32000()
 ***********************************/
int TPResample_close16000to32000(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;

    if(testData->resampleFilter != NULL)
    {
        free(testData->resampleFilter);
        testData->resampleFilter = NULL;
    }
    if(testData->fft_resample != NULL)
    {
        free(testData->fft_resample);
        testData->fft_resample = NULL;
    }
    return retVal;
}
