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

#ifndef __MIXERFACTORY_H__
#define __MIXERFACTORY_H__

#include "../factorytypes.h"

#include "../utils/vectorutils.h"

#include "mixer.h"
#include "rmemixer.h"

#ifdef S_WIN32
#include "../_archs/win32/win32_mmemixer.h"
#elif defined S_LINUX
//#include "../_archs/linux/linux_ossmixer.h"
#else
#ERROR !no platform defined!
#endif

class QString;

namespace streamapp
{
    /**
      * MixerFactory
      *   creates an IMixer object based on type and OS.
      ************************************************** */
  class MixerFactory
  {
 private:
      /**
        * Private Constructor.
        */
    MixerFactory()
    {
    }

  public:
      /**
        * Destructor.
        */
    ~MixerFactory()
    {
    }

      /**
        * Create an IMixer instance.
        * @param ac_sName the mixer name (refers to hardware name for Mme and Oss
        * @param ac_eType the type
        * @return the mixer, or 0  if there wasn't an implementation of the type
        */
#ifdef S_WIN32
    static IMixer* sf_pCreateMixer( const QString& ac_sName = sc_sDefault, const gt_eMixerType ac_eType = Mme )
#else
    static IMixer* sf_pCreateMixer( const QString& /*ac_sName = sc_sDefault*/, const gt_eMixerType ac_eType = Mme )
#endif
    {
      if( ac_eType == Rme )
      {
        if( rme::RmeMixer::sf_bIsAvailable() )
          return new rme::RmeMixer();
        return 0;
      }
#ifdef S_WIN32
      else if( ac_eType == Mme )
      {
        if( ac_sName == sc_sDefault )
          return new mme::MmeMixer( 0 );
        return new mme::MmeMixer( ac_sName.toStdString() );
      }
#else
      else if( ac_eType == Oss )
        return 0;//new OssMixer( ac_sName );
#endif
      return 0;
    }

      /**
        * Get the names of all available mixers.
        * @param ac_eType the type to get
        * @return vector with names
        */
    static tStringVector sf_saGetMixerNames( const gt_eMixerType ac_eType )
    {
      tStringVector ret;

      if( ac_eType == Rme )
      {
        ret.push_back( "rme" );  //FIXME this is the type actually
      }
#ifdef S_WIN32
      else if( ac_eType == Mme )
        utils::f_AddFrom( mme::MmeMixer::sf_saGetMixerNames(), ret );
#else

#endif
      return ret;
    }
  };

}

#endif //#ifndef __MIXERFACTORY_H__
