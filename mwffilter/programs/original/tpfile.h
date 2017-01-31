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

#ifndef _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TPFILE_H_

#define _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TPFILE_H_

/*
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       tpfile.h
 *
 *  Component:  TP - header file with file access funtcions
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       21.06.2006
 *
 *  Comment:    only 16kHz 2 channel wave files are supported!!!
 */


 /*
 * Other include files
 */

#include "sndfile.h"

/*
 * Declare new global project types
 */


/*
 * Global defenitions
 */

#define    ACCESS_READ          (SFM_READ)
#define    ACCESS_WRITE         (SFM_WRITE)

/*
 * Global shared variables
 */


/*
 * Global function declarations
 */

/**
 *  Function opens a file.
 *
 *  @param filePath Pointer to a string with the file path.
 *  @param framesLoaded Pointer for number of frames (interleaved values: 1 for each channel) loaded. will be set to zero for ACCESS_WRITE
 *  @param file_desc Pointer for the file descriptor used by read/write/close functions
 *  @param file_fs Pointer for the file samplerate
 *  @param acces_mode ACCESS_READ - read only, ACCESS_WRITE - write only
 *  @param saveFileFs sampling rate of a saved file. Ignored for ACCESS_READ
 *
 *  @retval TP_SUCCESS File opened
 *  @retval TP_FAILURE Error opening file
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */

int TPFile_open(const char * filePath, int *framesLoaded, void **file_desc,int *file_fs, int acces_mode,int saveFileFs);

/**
 *  Function closes a file.
 *
 *  @param file_desc File descriptor obtained using TPFile_open()
 *
 *  @retval TP_SUCCESS File closed
 *  @retval TP_FAILURE Error closing file
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */

int TPFile_close(void **file_desc);

/**
 *  Function loads a frame of data from sound file.
 *
 *  @param file_desc File descriptor obtained using TPFile_open()
 *  @param buffer For read data.
 *  @param framesRequested Desired number of frames (interleaved values: 1 for each channel).
 *  @param framesLoaded Placeholder for number of frames (interleaved values: 1 for each channel) loaded.
 *
 *  @retval TP_SUCCESS Sound data successfuly loaded
 *  @retval TP_FAILURE Error when loading file
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */

int TPFile_readFrame(void **file_desc, SAMPLETYPE * buffer, int framesRequested, int *framesLoaded);

/**
 *  Function saves a frame containig sound data.
 *
 *  @param file_desc File descriptor obtained using TPFile_open()
 *  @param buffer Pointer to the sound data to be saved.
 *  @param samplesPerChannle Indicates the length of the data to be saved in samples per channel
 *  @param saveFs the file might have been resampled, so we state the sampling frequency explicitly
 *
 *  @retval TP_SUCCESS Sound data successfuly saved
 *  @retval TP_FAILURE Error when loading file
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */
int TPfile_writeFrame(void **fileDesc, SAMPLETYPE **buffer, int samplesPerChannel);

#endif //<TPFILE_H>
