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
 
#ifndef __ProcessEvtDisp_H__
#define __ProcessEvtDisp_H__

#include "stream.h"
#include "events.h"
#include "streamappdefines.h"
using namespace streamapp;

namespace appcore
{

    /**
      * ProcessEventPoster
      *   posts event when mf_DoProcessing is called.
      *   This can be used to signal something from within
      *   a typical read process write situation.
      **************************************************** */
  template<class tType = int>
  class ProcessEventPoster : public IEventPoster<tType>, public streamapp::IStreamProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nEventType the event to post
        * @param ac_pData the data to post
        */
    ProcessEventPoster( const tType& ac_nEventType, const gt_EventData ac_pData = 0 ) :
      mc_nEventType( ac_nEventType ),
      mc_pData( ac_pData )
    {}

      /**
        * Destructor.
        */
    ~ProcessEventPoster()
    {}

      /**
        * Do processing.
        * Posts the event and the data when called.
        * @param ac_Input unused
        * @return ac_Input unchanged
        */
    INLINE virtual const Stream& mf_DoProcessing( const Stream& ac_Input )
    {
      IEventPoster<tType>::mp_DispatchEventToListener( mc_nEventType );
      return ac_Input;
    }

      /**
        * Do processing.
        * Posts the event and the data when called.
        */
    INLINE virtual void mf_DoProcessReplacing( Stream& )
    {
      IEventPoster<tType>::mp_DispatchEventToListener( mc_nEventType, const_cast<gt_EventData>( mc_pData ) );
    }

  private:
    const tType         mc_nEventType;
    const gt_EventData  mc_pData;

    ProcessEventPoster( const ProcessEventPoster& );
    ProcessEventPoster& operator = ( const ProcessEventPoster& );
  };

}

#endif //__ProcessEvtDisp_H__
