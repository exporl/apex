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
 
#ifndef __ARM_V5T_MSP430_H_
#define __ARM_V5T_MSP430_H_

#include "controller/buttoncontroller.h"

namespace controller
{

    /**
      * MSP430Control
      *   IButtonController implementation for the
      *   Philips MSP430 remote control.
      *   Remote must be set to use DVD code 0x20.
      ******************************************** */
  class MSP430Control : public IButtonController
  {
  public:
      /**
        * Constructor.
        * Calls mf_bInitController().
        */
    MSP430Control();

      /**
        * Destructor.
        * Calls mf_bDeInitController().
        */
    ~MSP430Control();

      /**
        * Implementation of the IButtonController method.
        */
    bool mf_bInitController();

      /**
        * Implementation of the IButtonController method.
        */
    bool mf_bDeInitController();

      /**
        * Implementation of the IButtonController method.
        */
    bool mf_bPoll( mt_eControlCode& a_Code );

  private:
    bool mf_bGetIrVal( int *val );

    bool  mv_bInit;
    int   m_nOldVal;
    int   m_nfdI2C;
  };

}

#endif //#ifndef __ARM_V5T_MSP430_H_
