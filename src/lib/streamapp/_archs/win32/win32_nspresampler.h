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
 
#ifndef __NSPRESAMPLER_H_
#define __NSPRESAMPLER_H_

#include "resamplers/resampler.h"
#include "containers/matrix.h"

#ifdef S_WIN32
#define nsp_UsesFir
#include "nsp.h"
#include "nspfirh.h"
#else
struct _NSPFirState;
#endif

namespace streamapp
{
  class NspFirFilter;

  //FIXME seperate up/down?
  //FIXME forward declare this

    /**
      * NSPResampler class
      *   resampler based on intel NSP libraries
      *   input and taps are split in DownFactor parts so less calculations are needed
      *   downsample will return a ac_nBufferSize / ac_dDownFactor size result!!
      ******************************************************************************** */
  class NSPResampler : public IResampler
  {
  public:
    typedef ArrayStorage<StreamBuf*> StreamBufPtrArr;

    NSPResampler ( const unsigned ac_nChan,
                   const unsigned ac_nBufferSize,
                   const unsigned ac_nDownFactor,
                   const unsigned ac_nFirTaps );
    ~NSPResampler();

    const Stream& mf_UpSample   ( const Stream& ac_InputStr );
    const Stream& mf_DownSample ( const Stream& ac_InputStr );

    typedef MatrixStorage<NSPFirState> CNspFirStatePtrArr;

  private:
    void m_fInitResampler( const unsigned ac_nTaps );
    StreamBuf            m_UpResult;
    StreamBuf            m_DownResult;
    StreamBufPtrArr      m_FirResult;
    StreamBufPtrArr      m_SplitResult;
    StreamBufPtrArr      m_FirTaps;  //split taps
    CNspFirStatePtrArr   m_FirUpStates;
    CNspFirStatePtrArr   m_FirDownStates;
  };



    /**
      * NSPResamplerNorm
      *   doesn't split into nDownFactor parts
      ***************************************** */
  class NSPResamplerNorm : public IResampler
  {
  public:
    NSPResamplerNorm( const unsigned ac_nChan, const unsigned ac_nBufferSize, const unsigned ac_nDownFactor, const unsigned ac_nTaps );
    virtual ~NSPResamplerNorm();

    const Stream& mf_UpSample   ( const Stream& ac_InputStr )
    {
      return ac_InputStr;
    }
    const Stream& mf_DownSample ( const Stream& ac_InputStr );

  private:
    void m_fInitResampler( const unsigned ac_nTaps );
    StreamBuf     m_DownResult;
    NspFirFilter* m_AntiAlias;
  };

}

#endif  //#ifndef __NSPRESAMPLER_H_

