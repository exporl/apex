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
 
#ifndef TP_JACKCB_H

#define TP_JACKCB_H
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

 
/* 
 * Global defenitions
 */

/*
 * Declare new global project types
 */
 

/*
 * Exported function declarations
 */

/**
 *  @brief Called by JACK AUDIO callback function from test module
 *
 *  @param bufferLeft buffer for left channel
 *  @param bufferRight buffer for right channel
 *
 *  @retval TP_SUCCESS Data processed
 *  @retval TP_FAILURE Failure during processing
 *
  */
int TPTest_proccessDataBlock(SAMPLETYPE *bufferLeft, SAMPLETYPE *bufferRight);

/**
 *  @brief Called by JACK AUDIO callback function from SRTBatch module
 *
 *  @param bufferLeft buffer for left channel
 *  @param bufferRight buffer for right channel
 *  @param numFramesReguested JACK may request a dufferent number of frames then processing block size in program
 *
 *  @retval TP_SUCCESS Data processed
 *  @retval TP_FAILURE Failure during processing
 *
  */
int TPSRTBatch_processDataBlock(SAMPLETYPE *bufferLeft, SAMPLETYPE *bufferRight,int numFramesReguested);

/**
 *  @brief Called by JACK AUDIO callback function from NoProce module
 *
 *  @param bufferLeft buffer for left channel
 *  @param bufferRight buffer for right channel
 *  @param numFramesReguested JACK may request a dufferent number of frames then processing block size in program
 *
 *  @retval TP_SUCCESS Data processed
 *  @retval TP_FAILURE Failure during processing
 *
  */
int TPNoProc_processDataBlock(SAMPLETYPE *bufferLeft, SAMPLETYPE *bufferRight,int numFramesReguested);

#endif //<TP_JACKCB_H>
