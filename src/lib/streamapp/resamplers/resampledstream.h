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
 
#ifndef IRESSTREAMM_H
#define IRESSTREAMM_H

#include "audioformatstream.h"
#include "resamplers/nspresampler.h"

namespace streamapp
{

    //!FIXME U/?D
  class ResampledInputStream : public InputStream
  {
  public:
    ResampledInputStream( AudioFormatReaderStream& ac_Source, const unsigned ac_nFactor ) :
      mc_Reader( ac_Source ),
      mc_pResampler( new NSPResampler( ac_Source.mf_nGetNumChannels(), ac_Source.mf_nGetBufferSize(), ac_nFactor, 50 ) )
    {
    }
    ~ResampledInputStream()
    {
      delete mc_pResampler;
    }

        //!implement IStream
    bool     mf_bIsEndOfStream() const
    { return mc_Reader.mf_bIsEndOfStream(); }
    unsigned mf_nGetNumChannels() const
    { return mc_Reader.mf_nGetNumChannels(); }
    unsigned mf_nGetBufferSize() const
    { return mc_Reader.mf_nGetBufferSize(); }

    const Stream& Read()
    { return mc_pResampler->mf_DownSample( mc_Reader.Read() ); }

  private:
    AudioFormatReaderStream& mc_Reader;
    IResampler* const        mc_pResampler;
  };

}

#endif //#ifndef IRESSTREAMM_H
