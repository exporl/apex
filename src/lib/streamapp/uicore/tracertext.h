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
 
#ifndef __TRACERTEXT_H__
#define __TRACERTEXT_H__

#include "text.h"
#include "utils/tracer.h"

namespace uicore
{

    /**
      * TracerText
      *   Tracer implementation using a TextComponent.
      *   Prints messages to the screen.
      ************************************************ */
  class TracerText : public TextComponent, public utils::Tracer
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent Screen
        */
    TracerText( Screen* a_pParent ) :
      TextComponent( a_pParent )
    {
      TextComponent::mp_SetEraseFirst( true );
    }

      /**
        * Destructor.
        */
    ~TracerText()
    {
      mp_TraceOne( "" );
    }

  protected:
      /**
        * Implementation of the Tracer method.
        */
    void mp_TraceOne( const String& ac_sMessage )
    {
      TextComponent::mp_SetText( ac_sMessage.c_str() );
    }
  };

}

#endif //#ifndef __TRACERTEXT_H__
