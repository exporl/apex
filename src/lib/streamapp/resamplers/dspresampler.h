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
 
/************* NOT FINISHED *************************/

#ifndef APEXDSPRESAMPLER_H
#define APEXDSPRESAMPLER_H

#include "resampler.h"
#include <samplerate.h>

namespace streamapp 
{
    /**
      * DSPResampler class
      *   resampler based on dsplib libraries
      *************************************** */
  class DSPResampler : public CResampler
  {
  public:
    DSPResampler( const unsigned ac_nChan,        
                  const unsigned ac_nBufferSize,            
                  const unsigned ac_nDownFactor,
                  const unsigned ac_nFilterTaps );
    virtual ~DSPResampler();

    CChanStrDouble& mf_UpSample   ( const CChanStrDouble& ac_InputStr );
    CChanStrDouble& mf_DownSample ( const CChanStrDouble& ac_InputStr );

  private:
    SRC_STATE*      m_pState;
    SRC_DATA*       data;
    CChanBufFloat   test;
    CChanBufFloat   test2;
    StreamBuf  test3;
  };

}

#endif

