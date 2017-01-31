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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_OUTPUTTEST_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_OUTPUTTEST_H_

#include "streamapp/typedefs.h"

class QString;

namespace apex
{

  namespace data
  {
    class DatablocksData;
  }

  namespace stimulus
  {

      /**
        * WavOutputTest
        *   compares soundcard output with input files.
        *   This allows to check if the entire processing/buffering works properly.
        *   If the input below is not specified, the test assumes all available datablocks were played
        *   sequential. Filters and connections are not taken into account. (eg if two datablocks are defined,
        *   the test just checks if the output is identical to the two wavefiles played right after each other).
        *   To allow testing filter/loop/connections functionality, create a wavefile that mimics all the processing,
        *   and specify the path below, and the test will use it to compare against the output.
        *   See the example tests for more info.
        *************** */
    class WavOutputTest
    {
    public:
        /**
          * Constructor.
          * @param a_DBlocks reference to the datablocks
          * @param ac_sFileToCompareWith wavefile to compare with, can be empty if datablocks are used
          */
      WavOutputTest( const data::DatablocksData& a_DBlocks, const QString& ac_sFileToCompareWith ) throw();

        /**
          * Destructor.
          */
      ~WavOutputTest() throw();

        /**
          * Runs the comparision.
          */
      void mf_Run() const throw();

    private:
      void mp_Init( const data::DatablocksData& a_DBlocks, const QString& ac_sFileToCompareWith ) throw();

      streamapp::tStringVector m_Files;
    };

  }
}


#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_OUTPUTTEST_H_
