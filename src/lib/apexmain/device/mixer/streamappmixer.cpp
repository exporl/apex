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
#ifdef SETMIXER

#include "streamappmixer.h"
#include "exceptions.h"
#include <mixers/mixerfactory.h>
#include <utils/stringexception.h>

//from libdata
#include "device/mixer/mixerparameters.h"

using namespace apex;
using namespace apex::device;
using apex::data::MixerParameters;

StreamAppMixer::StreamAppMixer( data::MixerParameters* a_pParameters ) :
    IMixer( a_pParameters ),
  m_pMixer( 0 )
{
  const QString sName = a_pParameters->name();
  const data::MixerType eType = a_pParameters->type();
  streamapp::gt_eMixerType eSType;
  try
  {
      if( eType != data::ANY_MIXER )
    {
    if( eType == data::DEFAULT_MIXER )
  #ifdef WIN32
        eSType = streamapp::Mme;
  #else
        eSType = streamapp::Oss;
  #endif
      else if( eType == data::RME_MIXER )
        eSType = streamapp::Rme;
      else
        throw( ApexStringException( "StreamAppMixer: unknown type" ) );

      try {
        m_pMixer = streamapp::MixerFactory::sf_pCreateMixer( sName, eSType );
      } catch (utils::StringException &e) {
        throw ApexStringException( /*e.what()*/ e.mc_sError.c_str() );
      } catch (...)   {   // FIXME
        throw ApexStringException( "Unknown error creating mme mixer");
    }

      if( !m_pMixer )
          throw( ApexStringException( a_pParameters->id() + ": Requested mixer device not found" ) );
      else
      {
          //by default all gains are 0.0dB. That's ok for output, but also mute hardware inputs
          //FIXME for Mme, would be better to mute all software outputs except wave
          //since there are also bass boost, aux, line in etc
        m_pMixer->mp_Initialize();
      }
    }
    else
    {
      //... try them all
    }
  }
  catch( utils::StringException& e )
  {
    throw( ApexStringException( /*QString( e.what() )*/ e.mc_sError.c_str()) );
  }
}

StreamAppMixer::~StreamAppMixer()
{
  delete m_pMixer;
}

double StreamAppMixer::mf_dGetMaxGain() const
{
  assert( m_pMixer );
  return m_pMixer->mf_tGetRange().m_dMaximum;
}

double StreamAppMixer::mf_dGetMinGain() const
{
  assert( m_pMixer );
  return m_pMixer->mf_tGetRange().m_dMinimum;
}

void StreamAppMixer::mp_SetGain( const double ac_dGainIndB )
{
  assert( m_pMixer );
  m_pMixer->mp_SetMasterOutputGain( ac_dGainIndB );
}

void StreamAppMixer::mp_SetGain( const double ac_dGainIndB, const unsigned ac_nChannel )
{
  assert( m_pMixer );
  m_pMixer->mp_SetMasterOutputGain( ac_dGainIndB, ac_nChannel );
}

double StreamAppMixer::mf_dGetGain( const unsigned ac_nChannel ) const
{
  assert( m_pMixer );
  return m_pMixer->mf_dGetMasterOutputGain( ac_nChannel );
}

unsigned StreamAppMixer::mf_nGetNumChannels() const
{
  assert( m_pMixer );
  return m_pMixer->mf_nGetNumMasterOutputs();
}

#endif    //SETMIXER

