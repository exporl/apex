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

#ifndef __STR_COMPARE_H__
#define __STR_COMPARE_H__

#include "audioformat.h"
#include "containers/array.h"
#include <string>

namespace streamapp
{

    /**
      * Compare
      *   wrapper for simple functions comparing AudioFormat data.
      *   Anout the comparision: number of channels must match,
      *   bitmode must match, and all input0 samples must match.
      *   (so input1 can have more samples then input0, but these
      *   are not checked; input0 cannot have more samples then
      *   input1 however)
      *   @note not thread-safe
      ************************************************************ */
  struct Compare
  {

    typedef ArrayAccess< std::string > mt_Files;
    typedef ArrayAccess< AudioFormatReader* > mt_Readers;

    enum mt_eResult
    {
      mc_eIdentical,      //comparision ok
      mc_eNotIdentical,   //comparision not ok
      mc_eFailedOpen      //a file failed to open
    };

      /**
        * Compare reader data.
        * @param a_Input0 first input
        * @param a_Input1 second input
        * @return true if identical
        */
    static mt_eResult sf_eCompare( AudioFormatReader& a_Input0, AudioFormatReader& a_Input1 ) throw();

      /**
        * Compare reader data;
        * All data from the readers in the array is merged
        * and compared with the data from a_Input1.
        * @param a_Input0 first input
        * @param a_Input1 second input
        * @return true if identical
        */
    static mt_eResult sf_eCompare( mt_Readers& a_Input0, AudioFormatReader& a_Input1 ) throw();

      /**
        * Compare two wavefiles.
        * @param ac_sFile0 first file
        * @param ac_sFile1 second file
        * @return true if identical data (samplerate is ignored though)
        */
    static mt_eResult sf_eCompareWave( const std::string& ac_sFile0, const std::string& ac_sFile1 ) throw();

      /**
        * Compare wavefiles;
        * All data from the files in the array is merged
        * and compared with the data from a_Input1.
        * @param ac_sFile0 files
        * @param ac_sFile1 second file
        * @return true if identical data (samplerate is ignored though)
        */
    static mt_eResult sf_eCompareWave( const mt_Files& ac_sFile0, const std::string& ac_sFile1 ) throw();

    static const unsigned sc_nReadSize = 8192;

  };

}

#endif //#ifndef __STR_COMPARE_H__
