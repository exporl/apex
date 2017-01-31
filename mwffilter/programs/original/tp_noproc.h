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

#ifndef _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TP_NOPROC_H_

#define _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TP_NOPROC_H_
/*
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       tp_srtBatch.h
 *
 *  Component:  SRT batch testing suite
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

/* The microphone ordering for the Cochlear Freedom devices
 *       CHANNEL             MIC
 *           1               left - dir
 *           2               left - omni
 *           3               right - dir
 *           4               right - omni
 */

/*
 * Declare new global project types
 */


/************************************
 * MODULE FUNCTION DEFINITIONS
 ***********************************/
/**
 *  Perform the basic SRT test consisting of 13 sentence without preforming any processing.
 *
 *  @param None
 *
 *  @retval TP_SUCCESS when the test is successfuly carried out
 *  @retval TP_FAILURE when an error has been encountered during the test
 */

int TPNoProc_test(void);
#endif // <TP_NOPROC_H>
