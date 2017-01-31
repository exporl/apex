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
 
#ifndef __BUTTONCONTROLLER__
#define __BUTTONCONTROLLER__

  /**
    * namespace with hardware controllers
    **************************************/
namespace controller
{
    /**
      * IButtonController
      *   simple interface for button controllers.
      *   Eg remote control, joystick, XY pad.
      ******************************************** */
  class IButtonController
  {
  protected:
      /**
        * Protected Constructor.
        */
    IButtonController()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~IButtonController()
    {
    }

      /**
        * Control codes.
        * These should be sufficient for testing.
        */
    enum mt_eControlCode
    {
      mc_eNoButton,
      mc_eUnkButton,    //!< unknown
      mc_eUp,
      mc_eDown,
      mc_eLeft,
      mc_eRight,
      mc_eSelect,
      mc_ePower,
      mc_eUser = 0x100  //!< user defined should start at this offset
    };

      /**
        * Init the controller.
        * Must be used before calling mf_Poll().
        * @return false on error, true if ok or already initialized.
        */
    virtual bool mf_bInitController() = 0;

      /**
        * DeInit the controller.
        * mf_Poll() should not be used after calling this method.
        * @return false on error
        */
    virtual bool mf_bDeInitController() = 0;

      /**
        * Poll the controller.
        * @param a_Code receives the code, mc_eNoButton when nothing is being pressed
        * @return false on receive error
        */
    virtual bool mf_bPoll( mt_eControlCode& a_Code ) = 0;
  };

}

#endif //#ifndef __BUTTONCONTROLLER__
