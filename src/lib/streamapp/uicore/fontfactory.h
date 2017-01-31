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
 
#ifndef __FONTFACTORY_H__
#define __FONTFACTORY_H__

#include "font.h"
#include "appcore/deleter.h"

#if defined S_WIN32 || defined S_POSIX || defined S_ARM
#include "freetype2_font.h"
#endif

namespace uicore
{

  namespace
  {
    const char*    sc_DefFont  = "d:/decker.ttf";
    const unsigned sc_nDefSize = 12;
  }

    /**
      * FontFactory
      *   chooses the Font implementation depending on the platform.
      *************************************************************** */
  class FontFactory
  {
  private:
      /**
        * Constructor.
        */
    FontFactory()
    {}

      /**
        * Destructor.
        */
    ~FontFactory()
    {}

  public:
      /**
        * Get the font implementation.
        * @param ac_bInitDef true to use default font set.
        * @return the font, or 0 for error
        */
    static Font* sf_pCreateFont( const bool ac_bInitDef = true )
    {
      Font* pRet = 0;
      #if defined S_WIN32 || defined S_POSIX || defined S_ARM
      pRet = new FreeType2Font();
      if( ac_bInitDef )
      {
        if( !pRet->mp_bInitialize( sc_DefFont ) )
        {
          appcore::ZeroizingDeleter()( pRet );
        }
        else
        {
          pRet->mp_SetFontSize( sc_nDefSize );
        }
      }
      #endif
      return pRet;
    }
  };

}

#endif //#ifndef __FONTFACTORY_H__
