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
 
#ifndef TP_RESAMPLE_H

#define TP_RESAMPLE_H
/* 
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       tpresample.h
 *
 *  Component:  Resampling functions
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       22.08.2007
 */

 
 /* 
 * Other include files
 */
#include "tpsrt_auxiliary.h"
#include "tp_srtbatch.h"
/*
 * Global defenitions
 */

/*
 * Declare new global project types
 */


/************************************
 * MODULE FUNCTION DEFINITIONS
 ***********************************/
/**
 *  Resampling from 16000 to 32000 .
 *
 *  @param inBuffer 16000Hz input data
 *  @param outBuffer 32000Hz output data
 *  @param testData structure with test enviroment variables
 *
 *  @retval TP_SUCCESS when the resampling is successfuly carried out
 *  @retval TP_FAILURE when an error occures
 */ 
int TPResample_16000to32000(SAMPLETYPE **inBuffer, SAMPLETYPE **outBuffer, struct TPSrtBatch_config_t *testData);

/**
 *  Init filter for resampling from 20480 to 32000 .
 *
 *  @param testData structure with test enviroment variables
 *
 *  @retval TP_SUCCESS when the init is successfuly carried out
 *  @retval TP_FAILURE when an error occures
 */ 
int TPResample_init16000to32000(struct TPSrtBatch_config_t *testData);

/**
 *  Close filter for resampling from 20480 to 32000 .
 *
 *  @param testData structure with test enviroment variables
 *
 *  @retval TP_SUCCESS when the init is successfuly carried out
 *  @retval TP_FAILURE when an error occures
 */ 
int TPResample_close16000to32000(struct TPSrtBatch_config_t *testData);
#endif // <TP_RESAMPLE_H>
