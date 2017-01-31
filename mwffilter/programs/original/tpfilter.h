/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TPFILTER_H_

#define _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TPFILTER_H_

/*
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       tpfilter.h
 *
 *  Component:  TP - header file for the filter module
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       21.06.2006
 */


/*
 * Other include files
 */
#include "fftw3.h"


/*
 * Declare new global project types
 */
/** Multichannel FIR Filter - shared among all modules in both time and frequency domain */
typedef struct {
    SAMPLETYPE      **dlyLine;
    SAMPLETYPE      **filterTaps;
    int             length;
    int             numChannels;
} MCFIR_Filter;

/** Singlechannel FIR Filter - shared among all modules in both time and frequency domain */
typedef struct {
    SAMPLETYPE      *dlyLine;
    SAMPLETYPE      *filterTaps;
    int             length;
} SCFIR_Filter;

/** Singlechannel IIR Filter - shared among all modules in time domain */
typedef struct {
    SAMPLETYPE      *numerator;
    int             numLength;
    SAMPLETYPE      *dlyLineNum;
    SAMPLETYPE      *denominator;
    int             denLength;
    SAMPLETYPE      *dlyLineDen;
} SCIIR_Filter;

/* FFTW */
typedef struct {
    fftw_plan pFFT;
    fftw_plan pIFFT;
    SAMPLETYPE *fftreal;
    SAMPLETYPE *fftR;
    SAMPLETYPE *fftI;
} FFTW_state;


/*
 * Global defenitions
 */

/*
 * Global shared variables
 */

/*
 * Global function declarations
 */

/**
 *  @brief Singl Input, Multiple Output FIR filtering operation
 *
 *  @param bufferIn points to the data to be filtered (single channel)
 *  @param bufferOut on return points to the filtered data (multi channel)
 *  @param filter pointer to Multi-Channel filter used during operation
 *  @param bufferLength indicates amount of samples in the input buffer
 *
 *  @retval TP_SUCCESS Signal successfuly filtered
 *  @retval TP_FAILURE Error when filtering
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */
 int TPFilter_SIMOFir(SAMPLETYPE *bufferIn, SAMPLETYPE **bufferOut, MCFIR_Filter * filter, int bufferLength);

/**
 *  @brief Mulitple Input, Multiple Output FIR filtering operation
 *
 *  @param bufferIn points to the data to be filtered (mutli channel)
 *  @param bufferOut on return points to the filtered data (multi channel)
 *  @param filter pointer to Multi-Channel filter used during operation
 *  @param bufferLength indicates amount of samples in the input buffer
 *
 *  @retval TP_SUCCESS Sound successfuly filtered
 *  @retval TP_FAILURE Error when filtering
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */

int TPFilter_MIMOFir(SAMPLETYPE **bufferIn, SAMPLETYPE **bufferOut, MCFIR_Filter * filter, int bufferLength);

/**
 *  @brief Prepare a time domain IIR fitler strucutre
 *
 *  @param
 *
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
MCFIR_Filter * TPFilter_prepareFirFilter(const char * path);

/**
 *  @brief Prepare a time domain IIR fitler strucutre
 *
 *  @param
 *
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
int TPFilter_prepareIirFilter(SCIIR_Filter *filter, char * path);

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
*/
int TPFilter_closeFilter(MCFIR_Filter * filter);

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 */
int TPFilter_closeIirFilter(SCIIR_Filter * filter);

/**
*  @brief Single Input, Single Output IIR filtering operation
*
*  @param bufferIn points to the data to be filtered (single channel)
*  @param bufferOut on return points to the filtered data (single channel)
*  @param filter pointer to Single-Channel filter used during operation
*  @param bufferLength indicates amount of samples in the input buffer
*
*  @retval TP_SUCCESS Signal successfuly filtered
*  @retval TP_FAILURE Error when filtering
*  @retval TP_ERROR_PARAMS Error on input params
*
*/
int TPFilter_SISOIir(SAMPLETYPE *bufferIn, SAMPLETYPE *bufferOut, SCIIR_Filter * filter, int bufferLength);


/************************************************************************************************************
 * Frequency domain filtering functions
************************************************************************************************************/

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
int FFTW_Init(FFTW_state *fft, int size);

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
int FFTW_Free(FFTW_state *fft);

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
*/
int MC_FIR_Freq_Init(MCFIR_Filter *MC_Filter, char *filename);

/**
 *  @brief Function used as the Single Input Multiple Output filter with a
 *         mixing procedure of the 2 input signals. Each signal is filtered
 *         with the appropriate filter MC_Filter1 and MC_Filter2 and then sumed.
 *         to use as a SIMO filter without mixing (just one signal)
 *         set the second buffer to zero.
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
int MC_FIR_Freq_SIMO_Mix(SAMPLETYPE *inSpeechBuffer,
                         int numNoiseSources,
                         SAMPLETYPE **inNoiseBuffers,
                         SAMPLETYPE **outBufferMC,
                         int samplesToProcess,
                         MCFIR_Filter *MC_FilterSpeech,
                         MCFIR_Filter **MC_FiltersNoise,
                         SAMPLETYPE mix,
                         FFTW_state *fft);

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
void MC_FIR_Freq_FilterSum(SAMPLETYPE **MC_in, SAMPLETYPE *out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft);

/**
 *  @brief Free the internal multichannel filter memory
 *
 *  @param MC_Filter pointer to the freed filter strucutre
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
int MC_FIR_Freq_Free(MCFIR_Filter *MC_Filter);

/**
 *  @brief Init the single channel filter
 *
 *  @param MC_Filter pointer to the freed filter strucutre
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */
int SC_FIR_Freq_Init(SCFIR_Filter *SC_Filter, char *filename);

/**
 *  @brief Free the internal single channel filter memory
 *
 *  @param MC_Filter pointer to the freed filter strucutre
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */

int SC_FIR_Freq_Free(SCFIR_Filter *SC_Filter);

#endif //<TP_FILTER_H>
