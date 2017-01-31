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
 
#include "rmemixer.h"
#include "utils/stringutils.h"
#include "utils/stringexception.h"
#include "utils/dataconversion.h"

using namespace utils;
using namespace streamapp;
using namespace streamapp::rme;

  //!these are implemented in the platform-specific code
bool f_bRmeIsAvailable();
const IRmeMixerInfo* f_RmeCreate();
void f_RmeMute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );
void f_RmeUnMute( const GainType ac_dGain, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );
void f_RmeSetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );
GainType f_dRmeGetGain( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel );

RmeMixer::RmeMixer() :
  mc_pMixer( 0 )
{
  if( !sf_bIsAvailable() )
    throw( StringException( "RmeMixer:: not available!" ) );
  mc_pMixer = f_RmeCreate();
}

RmeMixer::~RmeMixer()
{
}

void RmeMixer::ThrowIfNotExist() const
{
  if( !mc_pMixer )
    throw( StringException( "RmeMixer: unavailable or not initialized properly." ) );
}

bool RmeMixer::sf_bIsAvailable()
{
  return f_bRmeIsAvailable();
}

IMixer::mt_Range RmeMixer::mf_tGetRange() const
{
  ThrowIfNotExist();
  static IMixer::mt_Range ret;
  ret.m_dMinimum = sc_dMixerMin;
  ret.m_dMaximum = 3.0; //??
  return ret;
}

void RmeMixer::mp_Initialize()
{
  ThrowIfNotExist();
  const unsigned nHWIn = mf_nGetNumHardwareInputs();
  const unsigned nSWIn = mf_nGetNumSoftwareInputs();
  const unsigned nHWOut = mf_nGetNumHardwareOutputs();
  for( unsigned i = 0 ; i < nHWIn ; ++i )
  {
    mp_SetGain( 0.0, i, i );
    mp_Mute( i, i );
  }
  for( unsigned i = 0 ; i < nHWOut ; ++i )
  {
    mp_SetGain( 0.0, nSWIn + i, i );
    mp_UnMute( 0.0, nSWIn + i, i );
  }
}

void RmeMixer::mp_SetMasterInputGain( const GainType /*ac_dGainIndB*/ )
{
  throw( StringException( "RmeMixer::mp_SetMasterInputGain: gain cannot be set" ) );
}

void RmeMixer::mp_SetMasterInputGain( const GainType /*ac_dGainIndB*/, const unsigned /*ac_nChannel*/ )
{
  throw( StringException( "RmeMixer::mp_SetMasterInputGain: gain cannot be set" ) );
}

void RmeMixer::mp_SetMasterOutputGain( const GainType /*ac_dGainIndB*/ )
{
  ThrowIfNotExist();
  //...
}

void RmeMixer::mp_SetMasterOutputGain( const GainType /*ac_dGainIndB*/, const unsigned /*ac_nChannel*/ )
{
  ThrowIfNotExist();
  //f_RmeSetGain( ac_dGainIndB, 63, 63 + ac_nChannel );
}

GainType RmeMixer::mf_dGetMasterInputGain() const
{
  ThrowIfNotExist();
  return 0.0;
}

GainType RmeMixer::mf_dGetMasterInputGain( const unsigned /*ac_nChannel*/ ) const
{
  ThrowIfNotExist();
  return 0.0;
}

GainType RmeMixer::mf_dGetMasterOutputGain() const
{
  ThrowIfNotExist();
  return 0.0;
}

GainType RmeMixer::mf_dGetMasterOutputGain( const unsigned /*ac_nChannel*/ ) const
{
  ThrowIfNotExist();
  return 0.0;
}

void RmeMixer::mp_SetGain( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  ThrowIfNotExist();
  f_RmeSetGain( ac_dGainIndB, ac_nInputChannel, ac_nOutputChannel );
}

GainType RmeMixer::mf_dGetGain( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel ) const
{
  ThrowIfNotExist();
  return f_dRmeGetGain( ac_nInputChannel, ac_nOutputChannel );
}

void RmeMixer::mp_Mute( const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  ThrowIfNotExist();
  f_RmeMute( ac_nInputChannel, ac_nOutputChannel );
}

void RmeMixer::mp_UnMute( const GainType ac_dGainIndB, const unsigned ac_nInputChannel, const unsigned ac_nOutputChannel )
{
  ThrowIfNotExist();
  f_RmeUnMute( ac_dGainIndB, ac_nInputChannel, ac_nOutputChannel );
}

unsigned RmeMixer::mf_nGetNumHardwareInputs() const
{
  ThrowIfNotExist();
  return mc_pMixer->mc_nHardwareInputs;
}

unsigned RmeMixer::mf_nGetNumSoftwareInputs() const
{
  ThrowIfNotExist();
  return mc_pMixer->mc_nSoftwareInputs;
}

unsigned RmeMixer::mf_nGetNumHardwareOutputs() const
{
  ThrowIfNotExist();
  return mc_pMixer->mc_nOutputs;
}

const std::string& RmeMixer::mf_sGetMasterInputName( const unsigned /*ac_nChannel*/ ) const
{
  throw( StringException( "RmeMixer doesn't have master inputs!" ) );
}

const std::string& RmeMixer::mf_sGetMasterOutputName( const unsigned /*ac_nChannel*/ ) const
{
  ThrowIfNotExist();
  return sc_sNull;
}

const std::string& RmeMixer::mf_sGetInputName( const unsigned ac_nChannel ) const
{
  ThrowIfNotExist();
  return mc_pMixer->mc_pInputNames->at( ac_nChannel );
}

const std::string& RmeMixer::mf_sGetOutputName( const unsigned ac_nChannel ) const
{
  ThrowIfNotExist();
  return mc_pMixer->mc_pInputNames->at( ac_nChannel );
}

/******************************************************************************************************/

short streamapp::rme::gf_nConvertGain( const GainType ac_dGainInDb )
{
  return (short) ( pow( 10.0, ac_dGainInDb / 20.0 ) * 32768.0 );
}

GainType streamapp::rme::gf_dConvertGain( const short ac_nGainInLin )
{
  return 20.0 * log10( (GainType) ac_nGainInLin / 32768.0 );
}

/******************************************************************************************************/

namespace
{
  const unsigned short gc_nMultifaceChannels    = 18;
  const unsigned short gc_nMultifaceIns         = 36;
  const unsigned short gc_nMultifaceOuts        = 20;
  const unsigned short gc_nMultifaceRowSkipOne  = 8;
  const unsigned short gc_nMultifaceRowSkipTwo  = 10;
  const unsigned short gc_nMultifaceColSkipOne  = 8;
  const unsigned short gc_nMultifaceColSkipTwo  = 10;
  const unsigned short gc_nUnknownThingie       = 9;
}

MultiFace::MultiFace()
{
  mc_sIdentifier  = "Single Multiface";
  mc_nHardwareInputs = gc_nMultifaceChannels;
  mc_nSoftwareInputs = gc_nMultifaceChannels;
  mc_nOutputs     = gc_nMultifaceOuts;
  mc_pInputNames  = new MultifaceInputChannelNames();
  mc_pOutputNames = new MultifaceOutputChannelNames();
  mc_pChannelCC   = new MultifaceChannelCC();
  mc_pUnknownCC   = new MultifaceUnknownCC();
}

MultiFace::~MultiFace()
{
  delete mc_pInputNames;
  delete mc_pOutputNames;
  delete mc_pChannelCC;
  delete mc_pUnknownCC;
}

MultifaceInputChannelNames::MultifaceInputChannelNames()
{
  tStringVector::reserve( gc_nMultifaceIns );
  mp_Init();
}

MultifaceInputChannelNames::~MultifaceInputChannelNames()
{}

void MultifaceInputChannelNames::mp_Init()
{
  for( unsigned i = 0 ; i < gc_nMultifaceChannels ; ++i )
    tStringVector::push_back( "In " + toString( i + 1 ) );
  for( unsigned i = 0 ; i < gc_nMultifaceChannels ; ++i )
    tStringVector::push_back( "Out " + toString( i + 1 ) );
}

MultifaceOutputChannelNames::MultifaceOutputChannelNames()
{
  tStringVector::reserve( gc_nMultifaceOuts );
  mp_Init();
}

MultifaceOutputChannelNames::~MultifaceOutputChannelNames()
{}

void MultifaceOutputChannelNames::mp_Init()
{
  for( unsigned i = 0 ; i < gc_nMultifaceRowSkipOne ; ++i )
    tStringVector::push_back( "AN " + toString( i + 1 ) );
  for( unsigned i = 0 ; i < gc_nMultifaceRowSkipOne ; ++i )
    tStringVector::push_back( "A " + toString( i + 1 ) );
  tStringVector::push_back( "SP.L" );
  tStringVector::push_back( "SP.R" );
  tStringVector::push_back( "PH.L" );
  tStringVector::push_back( "PH.R" );
}

MultifaceChannelCC::MultifaceChannelCC() :
  gt_RmeChannelCC( gc_nMultifaceIns, gc_nMultifaceOuts, true )
{
  mp_Init();
}

MultifaceChannelCC::~MultifaceChannelCC()
{}

void MultifaceChannelCC::mp_Init()
{
  for( short j = 0 ; j < gc_nMultifaceOuts ; ++j )
  {
    if( j < gc_nMultifaceColSkipOne )
    {
      for( short i = 0 ; i < gc_nMultifaceColSkipOne ; ++i )
        mp_InitColumn( i, i * 0x34 );
    }
    else if( ( j >= gc_nMultifaceColSkipOne ) && ( j < gc_nMultifaceColSkipTwo ) )
    {
      const short nOffsetOne = gc_nMultifaceColSkipOne * 0x34;

      for( short i = 0 ; i < gc_nMultifaceColSkipTwo ; ++i )
        mp_InitColumn( i + gc_nMultifaceColSkipOne, nOffsetOne + i * 0x34 );
    }
  }
}

void MultifaceChannelCC::mp_InitColumn( const unsigned ac_nColumn, const short ac_nOffset )
{
  for( short i = 0 ; i < gc_nMultifaceRowSkipOne ; ++i )
    mp_Set( i, ac_nColumn , i + ac_nOffset );

  const short nOffsetOne = 0x0010 + ac_nOffset;

  for( short i = 0 ; i < gc_nMultifaceRowSkipTwo ; ++i )
    mp_Set( gc_nMultifaceRowSkipOne + i, ac_nColumn, nOffsetOne + i );

  const short nOffsetTwo = 0x001a + ac_nOffset;

  for( short i = 0 ; i < gc_nMultifaceRowSkipOne ; ++i )
    mp_Set( gc_nMultifaceChannels + i, ac_nColumn, nOffsetTwo + i );

  const short nOffsetThree = 0x002a + ac_nOffset;

  for( short i = 0 ; i < gc_nMultifaceRowSkipTwo ; ++i )
    mp_Set( gc_nMultifaceChannels + gc_nMultifaceRowSkipOne + i, ac_nColumn, nOffsetThree + i );
}

MultifaceUnknownCC::MultifaceUnknownCC() :
  gt_RmeUnknownCC( gc_nMultifaceIns, gc_nMultifaceOuts, true )
{
  mp_Init();
}

MultifaceUnknownCC::~MultifaceUnknownCC()
{}

void MultifaceUnknownCC::mp_Init()
{
  for( unsigned j = 0 ; j < gc_nMultifaceIns ; ++j )
    for( unsigned i = 0 ; i < gc_nMultifaceOuts ; ++i )
      mp_Set( j, i, 0x00010084 );

  unsigned x0 = 0;
  unsigned y0 = 0;
  unsigned x1 = 1;
  unsigned y1 = 1;

  int nStart = 0x00AC0204;

  for( unsigned j = 0 ; j < gc_nUnknownThingie ; ++j )
  {
    int nThing = ( j + 1 ) * 0x01E0;

    mp_Set( x0, y0, nStart );
    mp_Set( x1, y0, nStart );
    mp_Set( x0, y1, nStart + nThing );
    mp_Set( x1, y1, nStart + nThing );

    mp_Set( gc_nMultifaceChannels + x0, y0, nStart + 0x30C0 );
    mp_Set( gc_nMultifaceChannels + x1, y0, nStart + 0x30C0 );
    mp_Set( gc_nMultifaceChannels + x0, y1, nStart + 0x30C0 + nThing );
    mp_Set( gc_nMultifaceChannels + x1, y1, nStart + 0x30C0 + nThing );

    x0 += 2;
    y0 += 2;
    x1 += 2;
    y1 += 2;
  }

}

/******************************************************************************************************/

