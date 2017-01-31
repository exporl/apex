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

#include "eofcheck.h"
using namespace streamapp;


EofCheck::EofCheck() :
  mv_nTotalSamples( 0 ),
  mv_nCurSamplesLeft( 0 ),
  mv_bFired( false ),
  m_pAllDoneCallback( 0 )
{
}

EofCheck::~EofCheck()
{
}

void EofCheck::mp_AddInputToCheck( PositionableInputStream& a_cpInput )
{
  m_List.mp_AddItem( &a_cpInput );
  mv_nTotalSamples = s_max( a_cpInput.mf_lSamplesLeft(), mv_nTotalSamples );
  mv_nCurSamplesLeft = mv_nTotalSamples;
  mp_ResetState();
}

void EofCheck::mp_RemoveAllInputs()
{
  m_List.mp_RemoveAllItems();
  mv_nTotalSamples = 0;
  mv_nCurSamplesLeft = 0;
}

void EofCheck::mp_SeekAll( const unsigned long ac_lPosition )
{
  unsigned nToCheck = m_List.mf_nGetNumItems();
  for( unsigned i = 0 ; i < nToCheck ; ++i )
  {
    m_List.mf_GetItem( i )->mp_SeekPosition( ac_lPosition );
    mv_nCurSamplesLeft = s_max( mv_nCurSamplesLeft, m_List.mf_GetItem( i )->mf_lSamplesLeft() );
  }
  mp_ResetState();
}

void EofCheck::mp_ResetState()
{
  mv_bFired = false;
}

void EofCheck::mp_InstallCallback( Callback& a_Callback )
{
  m_pAllDoneCallback = &a_Callback;
  mp_ResetState();
}

void EofCheck::mp_Callback()
{
  unsigned nToCheck = m_List.mf_nGetNumItems();
  mv_nCurSamplesLeft = 0;
  for( unsigned i = 0 ; i < nToCheck ; ++i )
  {
    const unsigned long lCur = m_List.mf_GetItem( i )->mf_lSamplesLeft();
    if( lCur != 0L )
    {
      mv_nCurSamplesLeft = s_max( mv_nCurSamplesLeft, lCur );
    }
    /*else
    {
    m_List.mp_RemoveItem( i );
    --nToCheck;
    } */
  }
  if( mv_nCurSamplesLeft == 0L && !mv_bFired )
  {
    if( m_pAllDoneCallback )
    {
      m_pAllDoneCallback->mf_Callback();
      //std::cout << "EOF" << std::endl;
    }
    mv_bFired = true;
  }
}

/*************************************************************************************************************/

SimpleEofCheck::SimpleEofCheck() :
  mv_bEof( false ),
  mv_bFired( false ),
  mv_bSingle( true ),
  m_pAllDoneCallback( 0 )
{
}

SimpleEofCheck::~SimpleEofCheck()
{
}

void SimpleEofCheck::mp_AddInputToCheck( InputStream& a_cpInput )
{
  m_List.mp_AddItem( &a_cpInput );
  mp_ResetState();
}

void SimpleEofCheck::mp_RemoveAllInputs()
{
  m_List.mp_RemoveAllItems();
}

void SimpleEofCheck::mp_ResetState()
{
  mv_bFired = false;
}

void SimpleEofCheck::mp_InstallCallback( Callback& a_Callback )
{
  m_pAllDoneCallback = &a_Callback;
  mp_ResetState();
}

void SimpleEofCheck::mp_Callback()
{
  unsigned nToCheck = m_List.mf_nGetNumItems();
  for( unsigned i = 0 ; i < nToCheck ; ++i )
  {
    if( !mv_bSingle )
    {
      mv_bEof = true;
      mv_bEof &= m_List.mf_GetItem( i )->mf_bIsEndOfStream();
    }
    else
    {
      mv_bEof = false;
      mv_bEof |= m_List.mf_GetItem( i )->mf_bIsEndOfStream();
    }
  }
  if( mv_bEof && !mv_bFired )
  {
    if( m_pAllDoneCallback )
    {
      m_pAllDoneCallback->mf_Callback();
      //std::cout << "EOF" << std::endl;
    }
    mv_bFired = true;
  }
}
