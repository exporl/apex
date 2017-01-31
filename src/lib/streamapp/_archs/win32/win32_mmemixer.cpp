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
 
#include "win32_mmemixer.h"
#include "win32_mmemixerimpl.h"
#include "win32_headers.h"
#include "utils/stringexception.h"
#include "utils/stringutils.h"
#include "utils/vectorutils.h"

using namespace utils;
using namespace streamapp;
using namespace streamapp::mme;

MmeMixer::MmeMixer( const unsigned ac_nMixerNum ) :
  mc_pData( new MmeMixerData() ),
  m_pMasterOut( 0 ),
  m_pIns( 0 ),
  m_pOuts( 0 )
{
  mp_Init( ac_nMixerNum );
}

MmeMixer::MmeMixer(const std::string& ac_sMixerName ) :
  mc_pData( new MmeMixerData() )
{
  mp_Init( sf_nMixerNumFromName( ac_sMixerName ) );
}

MmeMixer::~MmeMixer()
{
  delete mc_pData;
  delete m_pMasterOut;
  delete m_pOuts;
  delete m_pIns;
}

void MmeMixer::mp_Init( const unsigned ac_nMixerNum )
{
    //open the mixer device
  const unsigned nMixers = sf_nGetNumMixers();
  if( !nMixers )
    throw( StringException( "MmeMixer: no mixers present" ) );
  if( ac_nMixerNum >= nMixers )
    throw( StringException( "MmeMixer: mixer ID " + toString( ac_nMixerNum ) + " is out of range" ) );
  std::string sErr;
  if( !mc_pData->mp_bOpen( ac_nMixerNum, sErr ) )
    throw( StringException( "MmeMixer: failed to open mixer with ID " + toString( ac_nMixerNum ) +" ; " + sErr ) );

    //get the controls;
    //these will automatically initialize to 0dB
    //so we don't have to do anything else
  m_pMasterOut  = mc_pData->mf_pGetMasterOutput();
  m_pOuts       = mc_pData->mf_pGetSlaveOutputs();
  m_pIns        = mc_pData->mf_pGetSlaveInputs();
  m_pMasterIn   = (MmeVolumeControl*) m_pIns->mf_GetItem( 0 );

    //add
  m_Picker.mp_AddElements( m_pIns );
  m_Picker.mp_AddElements( m_pOuts );
}

INLINE unsigned MmeMixer::sf_nGetNumMixers()
{
  return mixerGetNumDevs();
}

unsigned MmeMixer::sf_nMixerNumFromName( const std::string& ac_sName )
{
  const int nMIxerNum = f_nFindElement( sf_saGetMixerNames(), ac_sName );
  if( nMIxerNum == -1 )
    throw( StringException( "MmeMixer: " + ac_sName + " doesn't exist" ) );
  else
    return (unsigned) nMIxerNum;
}

tStringVector MmeMixer::sf_saGetMixerNames()
{
  tStringVector ret;
  MIXERCAPS     caps;

  const unsigned nMixers = sf_nGetNumMixers();
  for( unsigned i = 0 ; i < nMixers ; ++i )
    if( !mixerGetDevCaps( i, &caps, sizeof(MIXERCAPS) ) )
      ret.push_back( caps.szPname );

  return ret;
}

IMixer::mt_Range MmeMixer::mf_tGetRange() const
{
  IMixer::mt_Range ret;

  ret.m_dMaximum = 0.0;
  ret.m_dMinimum = sc_dMixerMin;

  return ret;
}

void MmeMixer::mp_Initialize()
{
  const unsigned nChan = mf_nGetNumHardwareInputs();
  const unsigned nOChan = mf_nGetNumHardwareOutputs();
  for( unsigned i = 0 ; i < nChan ; ++i )
    for( unsigned j = 0 ; j < nOChan ; ++j )
      mp_SetGain( -150.0, i, j );
}

INLINE void MmeMixer::mp_SetMasterInputGain( const GainType ac_dGainIndB )
{
  const unsigned nIns = mf_nGetNumMasterInputs();
  for( unsigned i = 0 ; i < nIns ; ++i )
    m_pMasterIn->mp_SetValue( ac_dGainIndB, i );
}

INLINE void MmeMixer::mp_SetMasterInputGain( const GainType ac_dGainIndB, const unsigned ac_nChannel )
{
  assert( ac_nChannel < mf_nGetNumHardwareInputs() );
  m_pMasterIn->mp_SetValue( ac_dGainIndB, ac_nChannel );
}

INLINE void MmeMixer::mp_SetMasterOutputGain( const GainType ac_dGainIndB )
{
  const unsigned nOuts = mf_nGetNumMasterOutputs();
  for( unsigned i = 0 ; i < nOuts ; ++i )
    m_pMasterOut->mp_SetValue( ac_dGainIndB, i );
}

INLINE void MmeMixer::mp_SetMasterOutputGain( const GainType ac_dGainIndB, const unsigned ac_nChannel )
{
  assert( ac_nChannel < mf_nGetNumHardwareOutputs() );
  m_pMasterOut->mp_SetValue( ac_dGainIndB, ac_nChannel );
}

INLINE unsigned MmeMixer::mf_nGetNumMasterInputs() const
{
  return m_pMasterIn->mf_nGetNumChannels();
}

INLINE unsigned MmeMixer::mf_nGetNumMasterOutputs() const
{
  return m_pMasterOut->mf_nGetNumChannels();
}

INLINE unsigned MmeMixer::mf_nGetNumHardwareInputs() const
{
  return m_pIns->mf_nGetNumChannels();
}

INLINE unsigned MmeMixer::mf_nGetNumSoftwareInputs() const
{
  return m_pOuts->mf_nGetNumChannels();
}

INLINE unsigned MmeMixer::mf_nGetNumHardwareOutputs() const
{
  return mf_nGetNumMasterOutputs();
}

INLINE GainType MmeMixer::mf_dGetMasterInputGain( const unsigned ac_nChannel ) const
{
  assert( ac_nChannel < mf_nGetNumMasterInputs() );
  return m_pMasterIn->mf_dGetValue( ac_nChannel );
}

INLINE GainType MmeMixer::mf_dGetMasterOutputGain( const unsigned ac_nChannel ) const
{
  assert( ac_nChannel < mf_nGetNumHardwareOutputs() );
  return m_pMasterOut->mf_dGetValue( ac_nChannel );
}

INLINE void MmeMixer::mp_SetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  assert( ac_nInputChannel < mf_nGetNumSoftwareInputs() + mf_nGetNumHardwareInputs() );
  assert( ac_nOutputChannel < mf_nGetNumHardwareOutputs() );
  IMixerElement* p = m_Picker.mf_pGetElement( ac_nInputChannel );
  p->mp_SetValue( ac_dGainIndB, ac_nOutputChannel );
}

INLINE GainType MmeMixer::mf_dGetGain( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) const
{
  assert( ac_nInputChannel < mf_nGetNumSoftwareInputs() + mf_nGetNumHardwareInputs() );
  assert( ac_nOutputChannel < mf_nGetNumHardwareOutputs() );
  const IMixerElement* p = m_Picker.mf_pGetElement( ac_nInputChannel );
  return p->mf_dGetValue( ac_nOutputChannel );
}

INLINE void MmeMixer::mp_Mute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  mp_SetGain( sc_dMixerMin, ac_nInputChannel, ac_nOutputChannel );
}

INLINE void MmeMixer::mp_UnMute( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  mp_SetGain( ac_dGainIndB, ac_nInputChannel, ac_nOutputChannel );
}


INLINE const std::string& MmeMixer::mf_sGetMasterInputName( const unsigned /*ac_nChannel*/ ) const
{
  return sc_sNull;
}

INLINE const std::string& MmeMixer::mf_sGetMasterOutputName( const unsigned ac_nChannel ) const
{
  assert( ac_nChannel < mf_nGetNumHardwareOutputs() );
  return m_pMasterOut->mf_sGetName( ac_nChannel );
}

INLINE const std::string& MmeMixer::mf_sGetInputName( const unsigned ac_nChannel ) const
{
  assert( ac_nChannel < mf_nGetNumSoftwareInputs() + mf_nGetNumHardwareInputs() );
  const IMixerElement* p = m_Picker.mf_pGetElement( ac_nChannel );
  return p->mf_sGetName( ac_nChannel );
}

INLINE const std::string& MmeMixer::mf_sGetOutputName( const unsigned ac_nChannel ) const
{
  return mf_sGetMasterOutputName( ac_nChannel );
}

INLINE void MmeMixer::mp_SelectMasterInput( const unsigned ac_nInputChannel )
{
  assert( ac_nInputChannel < mf_nGetNumHardwareInputs() );
  MixerElementPicker::mt_SearchElement p = m_Picker.mf_GetElement( ac_nInputChannel );
  MmeInputControls* pp = (MmeInputControls*) p.mc_Elements;
  pp->mp_SelectInputForMux( p.mc_ElementIndex );
}
