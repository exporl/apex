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
 
#include "multiproc.h"
#include "audiosamplebuffer.h"

using namespace streamapp;

SequentialProcessor::SequentialProcessor( const bool ac_bDeleteProcsWhenDeleted /* = true*/ ) :
    DynProcList( 4 ),
  mc_bDeleteContent( ac_bDeleteProcsWhenDeleted )
{
}
SequentialProcessor::~SequentialProcessor()
{
  if( mc_bDeleteContent )
    DynProcList::mp_RemoveAllItems();
}

void SequentialProcessor::mp_AddItem( IStreamProcessor* const ac_pItem )
{
  assert( ac_pItem );
  if( mf_nGetNumItems() != 0 )
  {
    assert( mf_GetItem( mf_nGetNumItems() - 1 )->mf_nGetNumOutputChannels() >= ac_pItem->mf_nGetNumInputChannels() ); //exception? voiD?
  }
  DynProcList::mp_AddItem( ac_pItem );
}


void SequentialProcessor::mp_RemoveItem( IStreamProcessor* const ac_pItem )
{
  DynProcList::mp_RemoveItem( ac_pItem );
  if( mc_bDeleteContent )
    delete ac_pItem;
  //m_Bypassed.mp_RemoveItem( true );
}

void SequentialProcessor::mp_RemoveAllItems()
{
  if( mc_bDeleteContent )
    DynProcList::mp_RemoveAllItems();
  else
    DynProcList::mp_Clear();
}

//get first processor's input
INLINE unsigned SequentialProcessor::mf_nGetNumInputChannels() const
{
  if( mf_nGetNumItems() != 0 )
    return mf_GetItem( 0 )->mf_nGetNumInputChannels();
  return 0; //!special case: no processing will happen, so return 0;
}

//get last processor's output
INLINE unsigned SequentialProcessor::mf_nGetNumOutputChannels() const
{
  if( mf_nGetNumItems() != 0 )
    return mf_GetItem( mf_nGetNumItems() - 1 )->mf_nGetNumOutputChannels();
  return 0; //...
}

const Stream& SequentialProcessor::mf_DoProcessing( const Stream& ac_Output )
{
  const unsigned nItems = DynProcList::mf_nGetNumItems();
  if( nItems == 0 )
    return ac_Output;
  else if( nItems == 1 )
    return mf_GetItem( 0 )->mf_DoProcessing( ac_Output );
  else
  {
    const Stream* pRet = &mf_GetItem( 0 )->mf_DoProcessing( ac_Output );
    for( unsigned i = 1 ; i < nItems ; ++i )
      pRet = &mf_GetItem( i )->mf_DoProcessing( *pRet );
    return *pRet;
  }
}

/************************************************************************************************************/

SlotProcessor::SlotProcessor( const unsigned ac_nSlots ) :
  mc_nSlots( ac_nSlots ),
  mv_dPreGain( 1.0 ),
  mv_dPostGain( 1.0 ),
  m_bDelete( ac_nSlots, false ),
  m_bBypass( ac_nSlots, false ),
  m_Items( ac_nSlots, true )
{

}
SlotProcessor::~SlotProcessor()
{

}

void SlotProcessor::mp_SetItem( const unsigned ac_nItem, IStreamProcessor* const ac_pItem,
                                const bool ac_bDeleteWhenDone, const bool ac_bBypassNow )
{
  m_Items.mp_Set( ac_nItem, ac_pItem );
  m_bBypass.mp_Set( ac_nItem, ac_bBypassNow );
  m_bDelete.mp_Set( ac_nItem, ac_bDeleteWhenDone );
}

void SlotProcessor::mp_RemoveItem( const unsigned ac_nItem )
{
  if( m_bDelete( ac_nItem ) )
    delete m_Items( ac_nItem );
  m_Items.mp_Set( ac_nItem, 0 );
}

void SlotProcessor::mp_RemoveAllItems()
{
  for( unsigned i = 0 ; i < mc_nSlots ; ++i )
  {
    if( m_Items( i ) )
    {
      if( m_bDelete( i ) )
        delete m_Items( i );
      m_Items.mp_Set( i, 0 );
    }
  }
}

//get first processor's input
unsigned SlotProcessor::mf_nGetNumInputChannels() const
{
  for( unsigned i = 0 ; i < mc_nSlots ; ++i )
  {
    if( m_Items( i ) )
      return m_Items( i )->mf_nGetNumInputChannels();
  }
  return 0;
}

//get last processor's output
unsigned SlotProcessor::mf_nGetNumOutputChannels() const
{
  for( unsigned i = mc_nSlots - 1 ; i == 0 ; --i )
  {
    if( m_Items( i ) )
      return m_Items( i )->mf_nGetNumOutputChannels();
  }
  return 0;
}

//!mf_DoProcessing
const Stream& SlotProcessor::mf_DoProcessing( const Stream& ac_Output )
{
  if( mv_dPreGain != 1.0 )
  {
    AudioSampleBuffer Buf( ac_Output );
    for( unsigned i = 0 ; i < ac_Output.mf_nGetChannelCount() ; ++i )
     Buf.mp_ApplyGain( i, mv_dPreGain );
  }

  const Stream* pRet = &ac_Output;
  for( unsigned i = 0 ; i < mc_nSlots ; ++i )
  {
    IStreamProcessor* pCur = m_Items( i );
    if( pCur && !m_bBypass( i ) )
      pRet = &pCur->mf_DoProcessing( *pRet );
  }

  if( mv_dPostGain != 1.0 )
  {
    AudioSampleBuffer Buf( *pRet );
    for( unsigned i = 0 ; i < Buf.mf_nGetChannelCount() ; ++i )
      Buf.mp_ApplyGain( i, mv_dPostGain );
  }

  return *pRet;
}
//void mf_DoProcessing( Stream& ac_Output ); TODO
