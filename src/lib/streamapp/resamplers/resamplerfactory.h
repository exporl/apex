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
 
#ifndef __RESAMPLERFACTORY_H__
#define __RESAMPLERFACTORY_H__

#include "resampler.h"
#ifdef S_WIN32
#include "win32_nspresampler.h"
#endif

namespace streamapp
{

    /**
      * ResamplerFactory
      *   factory for resamplers.
      *************************** */
  class ResamplerFactory
  {
  public:

      /**
        * The resampler types.
        */
    enum mt_eType
    {
      mc_eNSPnorm,    //!> resampler using Intel NSP libraries
      mc_eNSPsplit    //!> same as mc_eNSPnorm but using split filters, @see win32_nspresampler.h
    };

      /**
        * Create a resampler.
        * @param ac_eType the type
        * @see IResampler()
        * @return the resampler or 0 if not available
        */
    static IResampler* sf_pCreateResampler( const unsigned ac_nChan, 
                                            const unsigned ac_nBufferSize,
                                            const unsigned ac_nDownFactor,
                                            const unsigned ac_nFilterTaps,
                                            const mt_eType ac_eType = mc_eNSPnorm )
    {
#ifdef S_WIN32
      if( ac_eType == mc_eNSPnorm )
        return new NSPResamplerNorm( ac_nChan, ac_nBufferSize, ac_nDownFactor, ac_nFilterTaps );
      else if( ac_eType == mc_eNSPsplit )
        return new NSPResampler( ac_nChan, ac_nBufferSize, ac_nDownFactor, ac_nFilterTaps );
      else 
#endif
        return 0;
    }

  private:
      /**
        * Private Constructor.
        */
    ResamplerFactory()
    {}

      /**
        * Private Destructor.
        */
    ~ResamplerFactory()
    {}
  };

}

#endif //#ifndef __RESAMPLERFACTORY_H__
