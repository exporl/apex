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
 
#ifndef __MENULOOP_H__
#define __MENULOOP_H__

#include "appcore/threads/locks.h"
using namespace appcore;

namespace controller
{
  class IButtonController;
}
using namespace controller;

namespace uicore
{

  class Menu;
  class Screen;

    /**
      * MenuLoop
      *   very simple message loop.
      *   Polls the controller, forwarding it's
      *   button presses to the menu.
      *   @note this is temporary, will be replaced by
      *   the stuff in appcore/messages/
      ************************************************ */
  class MenuLoop
  {
  public:
      /**
        * Constructor.
        * @param a_pMenu the menu to control
        * @param a_pControl the controller
        */
    MenuLoop( Menu* a_pMenu, IButtonController* a_pControl, Screen* a_pScreen );

      /**
        * Destructor.
        */
    ~MenuLoop();

      /**
        * Make the mf_Exec() method return.
        */
    void mp_Stop();

      /**
        * Set the rate to poll the controller at.
        * @param ac_nMilliSeconds period in milliseconds
        */
    void mp_SetPeriod( const unsigned ac_nMilliSeconds = 100 );

      /**
        * Run the loop.
        * Stops when mc_ePower is received,
        * or when mp_Stop() is called.
        */
    void mf_Exec();

  private:
    bool mf_bQuit();

    Menu*                 m_pMenu;
    Screen*               m_pScreen;
    IButtonController*    m_pCtrl;

    bool                  mv_bQuit;
    unsigned              mv_nPeriod;
    const CriticalSection mc_Lock;
  };

}

#endif /*MENULOOP_H_*/
