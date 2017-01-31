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
 
#ifndef __USERINPUT_H__
#define __USERINPUT_H__

#include "userinputbase.h"

class QLabel;

namespace guicore
{

    /**
      * UserInput
      *   dialog with two buttons and a text label.
      ********************************************* */
  class UserInput : public UserInputBase
  {
  public:
      /**
        * Constructor.
        * @param ac_sTitle the title
        * @param ac_sLabelText the label text
        */
    UserInput( const QString& ac_sTitle, const QString& ac_sLabelText );

      /**
        * Destructor.
        */
    virtual ~UserInput();

      /**
        * The label.
        * Public so users can freely access it.
        */
    QLabel* m_pLabel;
  };

}

#endif //#ifndef __USERINPUT_H__
