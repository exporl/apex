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
 
/**********************************************************************
 *
 *   Copyright: Cochlear Ltd
 *    $Archive: /SPrint Research Software/Latest/Cpp/Sequence/stimulation_mode.h $
 *   $Revision: 3 $
 *       $Date: 9/10/02 7:00p $
 * $Nokeywords: $
 *     Authors: Colin Irwin
 *    Overview: Type definition for the StimulationMode enumerated type.
 *  References:
 *
 *********************************************************************/

#ifndef StimulationModeType_h

/* Used for automatic documentation purposes only. */
#ifndef DOXYGEN_IGNORE

#define StimulationModeType_h 1

#endif

/*!
 * \file
 * The stimulation mode enumerated type represents the reference electrode,
 * relative to the active electrode, for the specified stimulus.  To specify
 * the electrodes between which current is to flow for a stimulus, a value of
 * type \ref ElectrodeType and and a value of this type are required.
 * \sa See the ElectrodeType enumerated type definition for additional
 * information.
 * \note
 * Currently, only the stimulation modes \ref CG, \ref MP1, \ref MP2 and
 * \ref MP1_2 "MP1+2" are supported.
 */

typedef enum
{
    CG,     /*!< Common ground stimulation. */

    BP,     /*!< Bi-polar stimulation. */
    BP1,    /*!< Bi-polar + 1 stimulation. */
    BP2,    /*!< Bi-polar + 2 stimulation. */
    BP3,    /*!< Bi-polar + 3 stimulation. */
    BP4,    /*!< Bi-polar + 4 stimulation. */
    BP5,    /*!< Bi-polar + 5 stimulation. */
    BP6,    /*!< Bi-polar + 6 stimulation. */
    BP7,    /*!< Bi-polar + 7 stimulation. */
    BP8,    /*!< Bi-polar + 8 stimulation. */
    BP9,    /*!< Bi-polar + 9 stimulation. */
    BP10,   /*!< Bi-polar + 10 stimulation. */

    RBP,     /*!< Reverse bi-polar stimulation. */
    RBP1,    /*!< Reverse bi-polar + 1 stimulation. */
    RBP2,    /*!< Reverse bi-polar + 2 stimulation. */
    RBP3,    /*!< Reverse bi-polar + 3 stimulation. */
    RBP4,    /*!< Reverse bi-polar + 4 stimulation. */
    RBP5,    /*!< Reverse bi-polar + 5 stimulation. */
    RBP6,    /*!< Reverse bi-polar + 6 stimulation. */
    RBP7,    /*!< Reverse bi-polar + 7 stimulation. */
    RBP8,    /*!< Reverse bi-polar + 8 stimulation. */
    RBP9,    /*!< Reverse bi-polar + 9 stimulation. */
    RBP10,   /*!< Reverse bi-polar + 10 stimulation. */

    MP1,    /*!< Mono-polar 1 stimulation */
    MP2,    /*!< Mono-polar 2 stimulation */
    MP1_2,  /*!< Mono-polar 1+2 stimulation */

    RMP1,   /*!< Reverse mono-polar 1 stimulation */
    RMP2,   /*!< Reverse mono-polar 2 stimulation */
    RMP1_2  /*!< Reverse mono-polar 1+2 stimulation */
} StimulationModeType, StimulationMode;

#endif
