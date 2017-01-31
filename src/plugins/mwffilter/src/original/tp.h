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
 
#ifndef TP_H

#define TP_H
/* 
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       tp.h
 *
 *  Component:  TP - header file with general defenitions
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       15.05.2007
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

/** The default type used for processing samples */
#define     SAMPLETYPE      double

/** String for the default type. Used for type detection. */
#define     SAMPLETYPE_STRING    "double"

/** Indicate no test run chosen */
#define     RUN_NO_TEST             (0)

/** Indicate RT test run */
#define     RUN_RT_TEST             (1)

/** Indicate BEAM2 test run */
#define     RUN_BATCH_SRT_TEST      (2)

/** Indicate SRT run with no processing, only wav file playback*/
#define     RUN_NO_PROC_SRT_TEST    (3)

/** Max path length in characters supported by program. */
#define     MAX_CONFIG_DATA_LENGTH      (256)
 
/** Max path length in characters supported by program. */
#define     NUMBER_SENTENECS_PER_LIST   (13)

/** The maximum value for a sample: used when adjusting gain to detect errors */
#define     MAX_SAMPLE_VALUE            (1)

/** We always raise or lower the signal strength by 2dB */
#define     TWO_dB                      (2)

/** Defines which channel contins the singal in the front left microphone */
#define     MIC_DIR                     (0) 

/** Defines which channel contins the singal in the front right microphone */
#define     MIC_OMNI                    (1) 

/** Defines stereo left channel number in channel array*/
#define     STEREO_LEFT                 (0)

/** Defines stereo right channel number in channel array*/
#define     STEREO_RIGHT                (1)

/** The number of channels is hardcoced */
#define     NUMBER_CHANNELS_OUTPUT          (2)

/** The number of channels is hardcoced */
#define     MAX_SOUND_FILE_LENGTH_SECONDS   (10)



/*
 * Declare new global project types
 */
 
/** TRUE/FALSE type used for logical operations */
typedef int Bool_t;

/*
 * Declare new global project variables
 */
extern Bool_t       TP_AudioRunning;
 
extern int          TP_indicateTestRun;

#endif //<TP_H>
