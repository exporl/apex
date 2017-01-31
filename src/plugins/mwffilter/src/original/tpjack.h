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
 *  File:       tp.c
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

#include <jack/jack.h>       /* interafce to audio device is done through JACK */
#include "tp.h"
#include "tp_jackcb.h"
/*
 * Global defenitions
 */

/*
 * Declare new global project types
 */

/*
 * Global shared variables
 */

/*
 * Global function declarations
 */

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param samplingRate Placeholder for the returned sampling rate
 *  @param blockSize Indicates size of processing block
 *
 *  @retval TP_SUCCESS when JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */

int TPAudio_start(int * samplingRate,int blockSize);

/**
 *  @brief Start JACK - uesd to communicate with the audio device
 *
 *  @param samplingRate Placeholder for the returned sampling rate
 *
 *  @retval TP_SUCCESS always successwhen JACK succesfully initializes
 *  @retval TP_ERROR_PARAMS when failure occures and audio is not available
 */

int TPAudio_stop(void);
