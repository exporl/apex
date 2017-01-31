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
 
#ifndef __IFILTER_H__
#define __IFILTER_H__

#include "stream.h"
#include "containers/matrix.h"

struct _NSPFirState;

namespace streamapp
{

  class NspFirFilter : public IStreamProcessor
  {
  public:
    NspFirFilter( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const unsigned ac_nTaps );
    NspFirFilter( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const Stream& ac_Taps );
    ~NspFirFilter();

      //!must match ac_nChannels and ac_nTaps
    void mp_SetFilterTaps( const Stream& ac_Taps );
    unsigned mf_nGetNumFirTaps() const
    { return m_FirTaps.mf_nGetBufferSize(); }
    const Stream& mf_GetFirTaps() const
    { return m_FirTaps; }

    const Stream& mf_DoProcessing( const Stream& ac_Input );
    void mf_DoProcessReplacing( Stream& ac_ToProcess );

    unsigned mf_nGetNumInputChannels() const
    { return m_FirTaps.mf_nGetChannelCount(); }
    unsigned mf_nGetNumOutputChannels() const
    { return mf_nGetNumInputChannels(); }
    unsigned mf_nGetBufferSize() const
    { return m_FirResult.mf_nGetBufferSize(); }

    typedef ArrayStorage<_NSPFirState> tNSPFirStates;

  private:
    tNSPFirStates m_FirState; //one state per channel
    StreamBuf     m_FirTaps;
    StreamBuf     m_FirResult;
  };

}

#endif //#ifndef __IFILTER_H__
