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

#ifndef _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TP_TEST_H_

#define _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TP_TEST_H_
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
 *  @brief Test real-time operation with JACK AUDIO CONNECTION KIT
 *
 *  @param None.
 *
 *  @retval TP_SUCCESS Testing succeeded - regardles of audibility verification
 *  @retval TP_FAILURE Failure during testing
 *
  */

int TPTest_realTime(void);

#endif //<TP_TEST_H>
