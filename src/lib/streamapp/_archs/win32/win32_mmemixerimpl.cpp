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
 
#include "win32_mmemixerimpl.h"
#include "win32_headers.h"
#include "utils/stringexception.h"
#include "utils/dataconversion.h"

#include <cstdio>

using namespace utils;
using namespace streamapp;
using namespace streamapp::mme;

namespace
{
  void sf_PrintError( const std::string& ac_sSource, MMRESULT a_hrError, std::string& a_sTarget )
  {
    static char cErr[ 128 ];
    sprintf( cErr, "%s returned %ld\n", ac_sSource.data(), a_hrError );
    a_sTarget = cErr;
  }

  std::string         s_Error;
  MIXERCONTROLDETAILS s_MixerCtrlDetails;
}

/************************************************************************************************************************/

struct streamapp::mme::tMixerLine
{
  tMixerLine() :
    m_pSourceLines( 0 )
  {
    m_Controls.pamxctrl = 0;
  }

  ~tMixerLine()
  {
    delete m_Controls.pamxctrl;
    delete m_pSourceLines;
  }

  MIXERLINE           m_MixerLine;      //line
  MIXERLINECONTROLS   m_Controls;       //controls
  MIXERLINE*          m_pSourceLines;   //sources
};

/************************************************************************************************************************/

struct streamapp::mme::tMixer
{
  tMixer() :
    m_hMixer( 0 )
  {
  }

  ~tMixer()
  {
    if( m_hMixer)
      mixerClose( m_hMixer );
  }

  HMIXER    m_hMixer;     //mixer
  MIXERCAPS m_hMixerCaps; //info
};

/************************************************************************************************************************/

struct streamapp::mme::tMixerControl
{
  tMixerControl( HMIXER& a_Mixer, MIXERCONTROL& a_Control, const unsigned ac_nChannels ) :
    m_nChannels( ac_nChannels ),
    m_dRange( a_Control.Bounds.dwMaximum - a_Control.Bounds.dwMinimum ),
    m_Mixer( a_Mixer ),
    m_Control( a_Control ),
    m_Details( 0 )
  {
    if( ac_nChannels )
      m_Details = new MIXERCONTROLDETAILS_UNSIGNED[ ac_nChannels ];
  }

  ~tMixerControl()
  {
    delete [] m_Details;
  }

  void mp_SetValue( const GainType ac_dGainInDb, const unsigned ac_nChannel )
  {
    if( ac_nChannel >= m_nChannels )
      //throw StringException( "tMixerControl::mp_SetValue channel out of range" );
      //ignore this; when iterating the matrix it will look square
      return;

    mp_Update();

      //set the new value
    const GainType dNVal = dataconversion::gf_dBtoLinear( ac_dGainInDb ) * m_dRange;
    m_Details[ ac_nChannel ].dwValue = s_min( (DWORD) m_Control.Bounds.dwMinimum + (DWORD) dNVal, m_Control.Bounds.dwMaximum );

    MMRESULT Result = mixerSetControlDetails( (HMIXEROBJ)m_Mixer, &s_MixerCtrlDetails, MIXER_SETCONTROLDETAILSF_VALUE );
    if( Result )
    {
      sf_PrintError( "mixerSetControlDetails", Result, s_Error );
      throw StringException( s_Error );
    }
  }

  GainType mf_dGetValue( const unsigned ac_nChannel )
  {
    if( ac_nChannel >= m_nChannels )
      throw StringException( "tMixerControl::mf_dGetValue channel out of range" );

    mp_Update();

    const DWORD dwVal = m_Details[ ac_nChannel ].dwValue;
    return dataconversion::gf_LinearTodB( (GainType)dwVal / m_dRange );
  }

  void mp_Update()
  {
      //fill the struct
    s_MixerCtrlDetails.cbStruct       = sizeof( MIXERCONTROLDETAILS );
    s_MixerCtrlDetails.dwControlID    = m_Control.dwControlID;
    s_MixerCtrlDetails.cChannels      = m_nChannels;
    s_MixerCtrlDetails.cMultipleItems = m_Control.cMultipleItems;
    s_MixerCtrlDetails.cbDetails      = sizeof( MIXERCONTROLDETAILS_UNSIGNED );
    s_MixerCtrlDetails.paDetails      = m_Details;

      //update it
    MMRESULT Result = mixerGetControlDetails( (HMIXEROBJ)m_Mixer, &s_MixerCtrlDetails, MIXER_GETCONTROLDETAILSF_VALUE );
    if( Result )
    {
      sf_PrintError( "mixerGetControlDetails", Result, s_Error );
      throw StringException( s_Error );
    }
  }

  unsigned                      m_nChannels;
  GainType                        m_dRange;
  HMIXER&                       m_Mixer;
  MIXERCONTROL                  m_Control;  //keep copy, caller is not required to keep the control
  MIXERCONTROLDETAILS_UNSIGNED* m_Details;

private:
  tMixerControl( const tMixerControl& ) s_no_throw;
  tMixerControl& operator = ( const tMixerControl& ) s_no_throw;
};

/************************************************************************************************************************/

struct streamapp::mme::tMixerMultiplexer
{
  tMixerMultiplexer( HMIXER& a_Mixer, MIXERCONTROL& a_Control, const unsigned ac_nChannels ) :
    m_nChannels( ac_nChannels ),
    m_Mixer( a_Mixer ),
    m_Control( a_Control ),
    m_Details( 0 )
  {
    if( ac_nChannels )
      m_Details = new MIXERCONTROLDETAILS_BOOLEAN[ ac_nChannels ];
  }

  ~tMixerMultiplexer()
  {
    delete [] m_Details;
  }

  void mp_Select( const unsigned ac_nChannel )
  {
    if( ac_nChannel >= m_nChannels )
      throw StringException( "tMixerControl::mp_SetValue channel out of range" );

    mp_Update();

      //set the new value
    for( unsigned i = 0 ; i < m_nChannels ; ++i )
    {
      if( i == ac_nChannel )
        m_Details[ i ].fValue = 1;
      else
        m_Details[ i ].fValue = 0;
    }

    MMRESULT Result = mixerSetControlDetails( (HMIXEROBJ)m_Mixer, &s_MixerCtrlDetails, MIXER_SETCONTROLDETAILSF_VALUE );
    if( Result )
    {
      sf_PrintError( "mixerSetControlDetails", Result, s_Error );
      throw StringException( s_Error );
    }
  }

  unsigned mf_nGetSelected()
  {
    mp_Update();

    for( unsigned i = 0 ; i < m_nChannels ; ++i )
    {
      if( m_Details[ i ].fValue == 1 )
        return i;
    }
    throw StringException( "tMixerMultiplexer::mf_nGetSelected : no selection??" ); //can never be reached
  }

  void mp_Update()
  {
      //fill the struct
    s_MixerCtrlDetails.cbStruct       = sizeof( MIXERCONTROLDETAILS );
    s_MixerCtrlDetails.dwControlID    = m_Control.dwControlID;
    s_MixerCtrlDetails.cChannels      = 1;
    s_MixerCtrlDetails.cMultipleItems = m_Control.cMultipleItems;
    s_MixerCtrlDetails.cbDetails      = sizeof( MIXERCONTROLDETAILS_BOOLEAN );
    s_MixerCtrlDetails.paDetails      = m_Details;

      //update it
    MMRESULT Result = mixerGetControlDetails( (HMIXEROBJ)m_Mixer, &s_MixerCtrlDetails, MIXER_GETCONTROLDETAILSF_VALUE );
    if( Result )
    {
      sf_PrintError( "mixerGetControlDetails", Result, s_Error );
      throw StringException( s_Error );
    }
  }

  unsigned                      m_nChannels;
  HMIXER&                       m_Mixer;
  MIXERCONTROL                  m_Control;  //keep copy, caller is not required to keep the control
  MIXERCONTROLDETAILS_BOOLEAN*  m_Details;

private:
  tMixerMultiplexer( const tMixerMultiplexer& ) s_no_throw;
  tMixerMultiplexer& operator = ( const tMixerMultiplexer& ) s_no_throw;
};

/************************************************************************************************************************/

MmeMixerData::MmeMixerData() :
  m_bOpen( false ),
  m_pMixer( new tMixer() ),
  m_pMixerLines( 0 )
{}

MmeMixerData::~MmeMixerData()
{
  delete m_pMixer;
  delete [] m_pMixerLines;
}

bool MmeMixerData::mp_bOpen( const unsigned ac_nMixerID, std::string& a_sError )
{
    //Get info about requested mixer device
  MMRESULT Result = mixerGetDevCaps( ac_nMixerID, &m_pMixer->m_hMixerCaps, sizeof(MIXERCAPS) );
  if( Result )
  {
    sf_PrintError( "mixerGetDevCaps", Result, a_sError );
    return false;
  }

    //Open the requested mixer device
  Result = mixerOpen( &m_pMixer->m_hMixer, ac_nMixerID, 0, 0, CALLBACK_NULL );
  if( Result )
  {
    sf_PrintError( "mixerOpen", Result, a_sError );
    return false;
  }

    //Mixer device seems to be ok, get it's lines to see if something can be done with it
  if( !m_pMixer->m_hMixerCaps.cDestinations )
  {
    a_sError = "there are no mixer lines";
    return false;
  }

  if( !mp_bGetLines( a_sError ) )
    return false;

  return m_bOpen = true;
}

bool MmeMixerData::mp_bGetLines( std::string& a_sError )
{
  m_pMixerLines = new tMixerLine[ m_pMixer->m_hMixerCaps.cDestinations ];
  bool bHasControls = false;
  bool bHasConnections = false;

  for( unsigned i = 0 ; i < m_pMixer->m_hMixerCaps.cDestinations ; ++i )
  {
    m_pMixerLines[ i ].m_MixerLine.cbStruct = sizeof(MIXERLINE);
    m_pMixerLines[ i ].m_MixerLine.dwDestination = i;

    MMRESULT Result = mixerGetLineInfo( (HMIXEROBJ)m_pMixer->m_hMixer, &m_pMixerLines[ i ].m_MixerLine, MIXER_GETLINEINFOF_DESTINATION );
    if( Result )
    {
      sf_PrintError( "mixerGetLineInfo", Result, a_sError );
      return false;
    }

      //are there controls?
    if( m_pMixerLines[ i ].m_MixerLine.cControls )
    {
      if( !mf_bGetLineControls( m_pMixerLines[ i ], a_sError ) )
        return false;
      bHasControls |= true;
    }

      //are there sources?
    if( m_pMixerLines[ i ].m_MixerLine.cConnections )
    {
      if( !mf_bGetSourceLines( m_pMixerLines[ i ], i, a_sError ) )
        return false;
      bHasConnections |= true;
    }
  }

  if( !bHasControls )
  {
    a_sError = "none of the mixerlines is controllable";
    return false;
  }
  if( !bHasConnections )
  {
    a_sError = "none of the mixerlines has a source connected to it";
    return false;
  }
  return true;
}

bool MmeMixerData::mf_bGetLineControls( tMixerLine& a_Line, std::string& a_sError ) const
{
  const unsigned nControls = a_Line.m_MixerLine.cControls;

  if( nControls )
  {
    a_Line.m_Controls.cControls   = nControls;
    a_Line.m_Controls.pamxctrl   = new MIXERCONTROL[ nControls ];
    a_Line.m_Controls.cbStruct   = sizeof( MIXERLINECONTROLS );
    a_Line.m_Controls.dwLineID   = a_Line.m_MixerLine.dwLineID;
    a_Line.m_Controls.cbmxctrl   = sizeof( MIXERCONTROL );

    MMRESULT Result = mixerGetLineControls( (HMIXEROBJ)m_pMixer->m_hMixer, &a_Line.m_Controls, MIXER_GETLINECONTROLSF_ALL );
    if( Result )
    {
      delete a_Line.m_Controls.pamxctrl;
      sf_PrintError( "mixerGetLineControls", Result, a_sError );
      return false;
    }
  }

  return true;
}

bool MmeMixerData::mf_bGetSourceLines( tMixerLine& a_Line, const unsigned ac_nDestinationLine, std::string& a_sError ) const
{
  const unsigned nConnections = a_Line.m_MixerLine.cConnections;

  if( nConnections )
  {
    a_Line.m_pSourceLines = new MIXERLINE[ nConnections ];

    for( unsigned i = 0 ; i < nConnections ; ++i )
    {
      a_Line.m_pSourceLines[ i ].cbStruct = sizeof( MIXERLINE );
      a_Line.m_pSourceLines[ i ].dwDestination = ac_nDestinationLine;
      a_Line.m_pSourceLines[ i ].dwSource = i;

      MMRESULT Result = mixerGetLineInfo( (HMIXEROBJ)m_pMixer->m_hMixer, &a_Line.m_pSourceLines[ i ], MIXER_GETLINEINFOF_SOURCE );
      if( Result )
      {
        delete a_Line.m_pSourceLines;
        sf_PrintError( "mixerGetLineInfo", Result, a_sError );
        return false;
      }
    }
  }

  return true;
}

MmeVolumeControl* MmeMixerData::mf_pGetMasterOutput() const
{
  if( !m_bOpen )
    throw StringException( "MmeMixerData::mf_pGetMasterOutput: must be open to get volumecontrol" );

    //check for the first WAVEOUT encountered FIXME can there be more then one master??
  for( unsigned i = 0 ; i < m_pMixer->m_hMixerCaps.cDestinations ; ++i )
  {
    if( m_pMixerLines[ i ].m_MixerLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS )
      return new MmeVolumeControl( *m_pMixer, m_pMixerLines[ i ] );
  }

  throw StringException( "MmeMixerData::mf_pGetMasterOutput: no master output found??" );
}

MmeOutputControls* MmeMixerData::mf_pGetSlaveOutputs() const
{
  if( !m_bOpen )
    throw StringException( "MmeMixerData::mf_pGetSlaveOutputs: must be open to get volumecontrol" );

    //get number of lines
  const unsigned nSlaves = m_pMixerLines[ 0 ].m_MixerLine.cConnections;
  MmeOutputControls* pRet = new MmeOutputControls( nSlaves );

    //for every line, get it's controls and construct an MmeVolumeControl for it
  for( unsigned i = 0 ; i < nSlaves ; ++i )
  {
    tMixerLine theLine;
    theLine.m_MixerLine = m_pMixerLines[ 0 ].m_pSourceLines[ i ];

    if( !mf_bGetLineControls( theLine, s_Error ) )
      throw StringException( s_Error );

    if( theLine.m_MixerLine.cControls )
      pRet->mp_AddItem( new MmeVolumeControl( *m_pMixer, theLine ) );
  }

  return pRet;
}

MmeInputControls* MmeMixerData::mf_pGetSlaveInputs() const
{
  if( !m_bOpen )
    throw StringException( "MmeMixerData::mf_pGetSlaveInputs: must be open to get volumecontrol" );

    //find the multiplexer
  tMixerMultiplexer* pMux = 0;
  for( unsigned i = 0 ; i < m_pMixerLines[ 1 ].m_Controls.cControls ; ++i )
  {
    if( m_pMixerLines[ 1 ].m_Controls.pamxctrl[ i ].dwControlType == MIXERCONTROL_CONTROLTYPE_MUX )
    {
      pMux = new tMixerMultiplexer( m_pMixer->m_hMixer, m_pMixerLines[ 1 ].m_Controls.pamxctrl[ i ], m_pMixerLines[ 1 ].m_MixerLine.cConnections );
      break;
    }
  }
  if( !pMux )
    throw StringException( "MmeMixerData::mf_pGetSlaveInputs: couldn't find the multiplexer" );


    //get number of lines
  const unsigned nSlaves = m_pMixerLines[ 1 ].m_MixerLine.cConnections;
  MmeInputControls* pRet = new MmeInputControls( nSlaves, pMux );

    //for every line, get it's controls and construct an MmeVolumeControl for it
  for( unsigned i = 0 ; i < nSlaves ; ++i )
  {
    tMixerLine theLine;
    theLine.m_MixerLine = m_pMixerLines[ 1 ].m_pSourceLines[ i ];

    if( !mf_bGetLineControls( theLine, s_Error ) )
      throw StringException( s_Error );

    if( theLine.m_MixerLine.cControls )
      pRet->mp_AddItem( new MmeVolumeControl( *m_pMixer, theLine ) );
  }

  return pRet;
}

/************************************************************************************************************************/

MmeVolumeControl::MmeVolumeControl( tMixer& a_Mixer, tMixerLine& a_Line, const bool ac_bInit ) :
  m_Mixer( a_Mixer ),
  m_pControl( 0 ),
  mc_sName( a_Line.m_MixerLine.szName )
{
    //find the volume control
  for( unsigned i = 0 ; i < a_Line.m_Controls.cControls ; ++i )
  {
    if( a_Line.m_Controls.pamxctrl[ i ].dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME )
    {
      m_pControl = new tMixerControl( a_Mixer.m_hMixer, a_Line.m_Controls.pamxctrl[ i ], a_Line.m_MixerLine.cChannels );
      break;
    }
  }
  if( !m_pControl )
    throw StringException( "MmeVolumeControl: no volume control found for this mixerline??" );
  if( ac_bInit )
  {
    for( unsigned i = 0 ; i < m_pControl->m_nChannels ; ++i )
      mp_SetValue( 0.0, i );
  }
}

MmeVolumeControl::~MmeVolumeControl()
{
}

INLINE void MmeVolumeControl::mp_SetValue( const GainType ac_dGainInDb, const unsigned ac_nChannel )
{
  m_pControl->mp_SetValue( ac_dGainInDb, ac_nChannel );
}

INLINE GainType MmeVolumeControl::mf_dGetValue( const unsigned ac_nChannel ) const
{
  return m_pControl->mf_dGetValue( ac_nChannel );
}

INLINE unsigned MmeVolumeControl::mf_nGetNumChannels() const
{
  return m_pControl->m_nChannels;
}

INLINE const std::string& MmeVolumeControl::mf_sGetName( const unsigned /*ac_nChannel*/ ) const
{
  return mc_sName;
}

/************************************************************************************************************************/

MmeOutputControls::MmeOutputControls( const unsigned ac_nControls ) :
  IMixerElements( ac_nControls )
{
}

MmeOutputControls::~MmeOutputControls()
{
  IMixerElements::mp_RemoveAllItems();
}

/************************************************************************************************************************/

MmeInputControls::MmeInputControls( const unsigned ac_nControls, tMixerMultiplexer* a_pMultiPlexer ) :
  IMixerElements( ac_nControls ),
  m_pMux( a_pMultiPlexer )
{
}

MmeInputControls::~MmeInputControls()
{
  IMixerElements::mp_RemoveAllItems();
}

void MmeInputControls::mp_SelectInputForMux( const unsigned ac_nInput )
{
  if( ac_nInput >= tIMixerElements::mf_nGetNumItems() )
    throw StringException( "MmeInputControls::mp_SelectInputForMux: channel out of range" );

    //CHECKME muxer order is reversed??
  const unsigned nItems = tIMixerElements::mf_nGetNumItems() - 1;
  m_pMux->mp_Select( nItems - ac_nInput );
}

INLINE unsigned MmeInputControls::mf_nGetSelectedInput() const
{
  return m_pMux->mf_nGetSelected();
}
