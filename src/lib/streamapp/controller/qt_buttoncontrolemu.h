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
 
#ifndef __QT_BUTTONCONTROLLER_H__
#define __QT_BUTTONCONTROLLER_H__

#include "controller/buttoncontroller.h"

namespace controller
{

    /**
      * QtButtonControlEmu
      *   emulates an IButtonController using Qt.
      *   Works in cooperation with the FrameBuffer emulator:
      *   that derives from QWidget and overrides the keyPressEvent
      *   method to catch keyboard events. After registering
      *   the instance with the FrameBuffer, it's key will be set
      *   when one of the known keyboard keys is pressed (arrows and escape).
      *   This might not be the nicest way to program it but I really don't
      *   care, the emulator stuff is just here for testing the rendering code
      *   before putting it on the dsp/psp. It's horribly slow anyway.
      ************************************************************************ */
  class QtButtonControlEmu : public IButtonController
  {
  public:
      /**
        * Constructor.
        */
    QtButtonControlEmu();

      /**
        * Destructor.
        */
    ~QtButtonControlEmu();

      /**
        * Implementation of the IButtonController method.
        */
    bool mf_bInitController()
    {
      return true;
    }

      /**
        * Implementation of the IButtonController method.
        */
    bool mf_bDeInitController()
    {
      return true;
    }

      /**
        * Implementation of the IButtonController method.
        */
    bool mf_bPoll( mt_eControlCode& a_Code );

      /**
        * Called by FrameBuffer when a key is pressed.
        * Sets the key for the mf_Poll() method.
        */
    void mf_KeyPressed( const int ac_nKey );

  private:
    mt_eControlCode mv_nNewKey;
  };

}

#endif //#ifndef __QT_BUTTONCONTROLLER_H__
