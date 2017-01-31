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
 
#ifndef __APEX_BUFFERDROPCALLBACK_H__
#define __APEX_BUFFERDROPCALLBACK_H__

#include <QString>
#include <callback/callback.h>

#include "status/statusitem.h"

namespace apex {

  //class tError;

namespace stimulus {

  /**
    * BufferDropCallback
    *   object to call when a buffer drops,
    *   just keeps the number of drops.
    *
    *   [ stijn ] modified: send an event to
    *   display this problem in the message window,
    *   now it gets lost in the excess of other output, while this
    *   is like the only output that's truly important since it
    *   yields the entire experiment invalid.
    ************************************************************** */
    class BufferDropCallback : public streamapp::Callback {
  public:
      /**
        * Constructor.
        * @param ac_sDropSource describe what dropped (soundcard or buffer)
        */
      BufferDropCallback( const QString& ac_sDropSource );

      /**
        * Destructor.
        */
      ~BufferDropCallback();

      /**
        * Called when a drop occurs.
        * Increments count and posts the event.
        */
      void mf_Callback();

      /**
        * Get the number of drops so far.
        * @return the number
        */
      const unsigned& mf_nGetNumDrops() const {
          return mv_nDrops;
      }

      /**
        * Reset number of drops.
        */
      void mp_ResetDrops() {
          mv_nDrops = 0;
      }

  private:
    unsigned  mv_nDrops;
    StatusItem   m_pError;
  };
}
}


#endif //#ifndef __APEX_BUFFERDROPCALLBACK_H__
