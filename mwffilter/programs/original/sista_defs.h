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

/*
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       sista_defs.h
 *
 *  Component:  TP - header file with general defenitions
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       21.06.2006
 */


 /*
 * Other include files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * Global defenitions
 */
/** Indicates successfule function return */
#define     TP_SUCCESS      (0)

/** Indicates general failure during function execution*/
#define     TP_FAILURE      (1)

/** Indicates an error in function input parameters */
#define     TP_ERROR_PARAMS (2)

/** TRUE value for <b>Bool</b> type */
#define     TP_TRUE         (1)

/** FALSE value for <b>Bool</b> type */
#define     TP_FALSE        (0)

/** Each test sequences consists of a list of sentences, this is the number of sentences per list */
#define     TP_NUMBER_SENTENCES_PROCESSED       (1)

/** The default type used for processing samples */
#define     SAMPLETYPE              double

/** String for the default type. Used for type detection. */
#define     SAMPLETYPE_STRING      "double"

/** Max path length in characters supported by program. */
#define     MAX_CONFIG_DATA_LENGTH      (256)

/** Max path length in characters supported by program. */
#define     NUMBER_SENTENECS_PER_LIST   (13)

/** The maximum value for a sample: used when adjusting gain to detect errors */
#define     MAX_SAMPLE_VALUE            (1)

/** The maximum value for a sample: used when adjusting gain to detect errors */
#define     SAMPLING_RATE               (16000)

#if !FIXME_HARDCODED_NUM_CHANNELS
/** The number of channels is hardcoced */
#define     NUMBER_CHANNELS             (4)
#endif

/** For starting the SRT calibrating procedure */
#define     STARTING_MINIMAL_SNR        (-20)

/** We always raise or lower the signal strength by 2dB */
#define     TWO_dB                      (2)

/** Number of sentences used for averagin the SRT */
#define     SENTENCES_AVG_SNR_FOR_SRT   (6)

/* The microphone ordering for the Cochlear Freedom devices
 *       CHANNEL             MIC
 *           1               left - dir
 *           2               left - omni
 *           3               right - dir
 *           4               right - omni
 */
/** Defines which channel contins the singal in the front left microphone */
#define     MIC_DIR                     (0)

/** Defines which channel contins the singal in the front right microphone */
#define     MIC_OMNI                    (1)


/** Defines stereo right channel number in channel array*/
#define     STEREO_RIGHT                (1)

/** Defines stereo left channel number in channel array*/
#define     STEREO_LEFT                 (0)


#define     TP_OUTPUT_FILE_DIR          "data/original/"

/*
 * Declare new global project types
 */

/** TRUE/FALSE type used for logical operations */
typedef int Bool;


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

/*
 * Global shared variables
 */
