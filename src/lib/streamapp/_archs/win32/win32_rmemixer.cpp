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

#include "../../appcore/singleton.h"

#include "../../mixers/rmemixer.h"

#include "../../utils/dataconversion.h"
#include "../../utils/stringexception.h"

#include "defines.h"
#include "win32_headers.h"

#include <winioctl.h>

using namespace streamapp;
using namespace streamapp::rme;

class Win32RmeMixerImpl : public appcore::Singleton<Win32RmeMixerImpl>
{
  friend class appcore::Singleton<Win32RmeMixerImpl>;
public:
  ~Win32RmeMixerImpl()
  {
    if( m_hDev )
      CloseHandle( m_hDev );
    delete mc_pMixerInfo;
  }

  bool mf_bInit()
  {
    if( !m_hDev )
    {
      //get handle
      m_hDev = CreateFile( "\\\\.\\hdspOut0",
                           0x80000000       ,
                           0                ,
                           0                ,
                           OPEN_EXISTING    ,
                           0                ,
                           0                );

      if( m_hDev == INVALID_HANDLE_VALUE )
      {
        m_hDev = 0;
        return false;
      }
      else
      {
        mc_pMixerInfo = new MultiFace();
      }
    }
    return true;
  }

  void mp_SetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
  {
    short nChanCC = mc_pMixerInfo->mc_pChannelCC->mf_Get( ac_nInputChannel, ac_nOutputChannel );
    int nUnkCC = mc_pMixerInfo->mc_pUnknownCC->mf_Get( ac_nInputChannel, ac_nOutputChannel );
    short nGain = gf_nConvertGain( ac_dGainIndB );

      //fixme find out how to do this faster in asm
    __int64 i64Command = ( (__int64) nUnkCC) << 32;

    __int64 c = ( (__int64) nChanCC ) << 16;
    c &= 0x00000000FFFFFFFF;

    __int64 g = ( (__int64) nGain );
    g &= 0x000000000000FFFF;

    i64Command |= c;
    i64Command |= g;

    DWORD   dwBytesReturned = 0;
    DWORD   dwResult        = 0;

    BOOL bResult = DeviceIoControl(  m_hDev                ,
                                     0x001d40c4            ,
                                     &i64Command           ,
                                     sizeof( i64Command )  ,
                                     &dwResult             ,
                                     sizeof( dwResult )    ,
                                     &dwBytesReturned      ,
                                     0                     );

    if( bResult != TRUE )
    {
      throw( utils::StringException( "Win32RmeMixerImpl: DeviceIOControl call failed, gain not set" ) );
    }
  }

  void mp_SetMute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
  {
    short nChanCC = mc_pMixerInfo->mc_pChannelCC->mf_Get( ac_nInputChannel, ac_nOutputChannel );

    __int64 i64Command = 0;

    __int64 c = ( (__int64) nChanCC ) << 16;
    c &= 0x00000000FFFFFFFF;

    i64Command |= c;

    DWORD   dwBytesReturned = 0;
    DWORD   dwResult        = 0;

    BOOL bResult = DeviceIoControl(  m_hDev                ,
                                     0x001d40c4            ,
                                     &i64Command           ,
                                     sizeof( i64Command )  ,
                                     &dwResult             ,
                                     sizeof( dwResult )    ,
                                     &dwBytesReturned      ,
                                     0                     );

    if( bResult != TRUE )
    {
      throw( utils::StringException( "Win32RmeMixerImpl: DeviceIOControl call failed, gain not set" ) );
    }
  }

  void mp_SetUnMute( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
  {
    short nChanCC = mc_pMixerInfo->mc_pChannelCC->mf_Get( ac_nInputChannel, ac_nOutputChannel );
    short nGain = gf_nConvertGain( ac_dGainIndB );

    __int64 i64Command = 0x0000000300000000;

    __int64 c = ( (__int64) nChanCC ) << 16;
    c &= 0x00000000FFFFFFFF;

    __int64 g = ( (__int64) nGain );
    g &= 0x000000000000FFFF;

    i64Command |= c;
    i64Command |= g;

    DWORD   dwBytesReturned = 0;
    DWORD   dwResult        = 0;

    BOOL bResult = DeviceIoControl(  m_hDev                ,
                                     0x001d40c4            ,
                                     &i64Command           ,
                                     sizeof( i64Command )  ,
                                     &dwResult             ,
                                     sizeof( dwResult )    ,
                                     &dwBytesReturned      ,
                                     0                     );

    if( bResult != TRUE )
    {
      throw( utils::StringException( "Win32RmeMixerImpl: DeviceIOControl call failed, gain not set" ) );
    }
  }

  GainType mf_dGetGain( const unsigned /*ac_nInputChannel*/, const unsigned /*ac_nOutputChannel*/ )
  {
    return 0.0;
  }

  const IRmeMixerInfo* mf_pGetRme()
  {
    return mc_pMixerInfo;
  }

private:
  Win32RmeMixerImpl() :
    m_hDev( 0 ),
    mc_pMixerInfo( 0 )
  {}

  HANDLE m_hDev;
  const IRmeMixerInfo* mc_pMixerInfo;
};

bool f_bRmeIsAvailable()
{
  return Win32RmeMixerImpl::sf_pInstance()->mf_bInit();
}

void f_RmeSetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  Win32RmeMixerImpl::sf_pInstance()->mp_SetGain( ac_dGainIndB, ac_nInputChannel, ac_nOutputChannel );
}

GainType f_dRmeGetGain( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  return Win32RmeMixerImpl::sf_pInstance()->mf_dGetGain( ac_nInputChannel, ac_nOutputChannel );
}

void f_RmeMute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  Win32RmeMixerImpl::sf_pInstance()->mp_SetMute( ac_nInputChannel, ac_nOutputChannel );
}

void f_RmeUnMute( const GainType ac_dGain, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  Win32RmeMixerImpl::sf_pInstance()->mp_SetUnMute( ac_dGain, ac_nInputChannel, ac_nOutputChannel );
}

const IRmeMixerInfo* f_RmeCreate()
{
  return Win32RmeMixerImpl::sf_pInstance()->mf_pGetRme();
}
