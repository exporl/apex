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
 
#ifndef __SOCKETCONNFACTORYIMP_H__
#define __SOCKETCONNFACTORYIMP_H__

#include "socketconn.h"
#include "socketconnfactory.h"
#include "containers/ownedarray.h"
#include "appcore/threads/locks.h"
#include "appcore/threads/thread.h"
#include "utils/tracer.h"
#include "utils/stringutils.h"

using namespace appcore;
using namespace streamapp;

namespace network
{

    /**
      * SocketConnectionFactory
      *   a default implementation for ISocketConnectionFactory.
      *   Keeps track of connection's state, a disconnected one
      *   is removed from the list. Polls state in a low-priority
      *   thread.
      *********************************************************** */
  class SocketConnectionFactory : public ISocketConnectionFactory, private IThread
  {
  public:
      /**
        * Constructor.
        */
    SocketConnectionFactory() :
        IThread( "ESockMgr" ),
      m_nMagicID( 0x00000666 ),
      m_bNegotiate( true )
    {
      IThread::mp_Start( IThread::priority_low );
    }

      /**
        * Destructor.
        * Deletes all active sockets.
        */
    virtual ~SocketConnectionFactory()
    {
      m_Connections.mp_RemoveAllItems();
      IThread::mp_Stop( 10000 );
    }

      /**
        * The concrete factories only have to implement this
        * method and return a new'd connection.
        * The arguments must be passed to the new connection.
        * @param ac_nMagicID the MagicID to use
        * @param ac_bNegotiate whether or not to negotiate
        * @return new connection, will be deleted after it disconnects
        */
    virtual ISocketConnection* mf_pCreateConnectionImp( const unsigned ac_nMagicID, const bool ac_bNegotiate ) = 0;

      /**
        * Called right before a connection is deleted.
        * Used to notify the concrete factory that a connection is disconnected.
        * @param a_pConnection the connection that will be deleted after this call.
        */
    virtual void mf_AboutToRemove( ISocketConnection* a_pConnection ) = 0;

      /**
        * Set the ID to use for new connections.
        * If negotiate is false, all connections get this id.
        * Else, the id is incremented for every new connection.
        * @param ac_nMagicID the id
        */
    void mp_SetMagicID( const unsigned ac_nMagicID = 0x00000666, const bool ac_bNegotiate = true )
    {
      const Lock L( mc_Lock );
      m_nMagicID = ac_nMagicID;
      m_bNegotiate = ac_bNegotiate;
    }

      /**
        * Get the number of currently active connections.
        * @return the number
        */
    INLINE unsigned mf_nGetNumRegistered() const
    {
      return m_Connections.mf_nGetNumItems();
    }

  protected:
      /**
        * Obtain a lock.
        * Can be used by implementations if they need to do something
        * that must not be done while a connection is mader or removed.
        */
    INLINE void mf_ObtainLock()
    {
      mc_Lock.mf_Enter();
    }

      /**
        * Release the lock from mf_ObtainLock().
        */
    INLINE void mf_ReleaseLock()
    {
      mc_Lock.mf_Leave();
    }

  private:
    typedef OwnedArray<ISocketConnection> mt_ConnList;

    mt_ConnList                       m_Connections;
    unsigned                          m_nMagicID;
    bool                              m_bNegotiate;
    const CriticalSection             mc_Lock;

      /**
        * Implementation of the ISocketConnectionFactory method.
        */
    ISocketConnection* mf_pCreateConnection( ISocket* const ac_pSocketToInitWith )
    {
      const Lock L( mc_Lock );
      ISocketConnection* pNew = mf_pCreateConnectionImp( m_nMagicID, m_bNegotiate );
      pNew->mp_InitWithSocket( ac_pSocketToInitWith );
      if( m_bNegotiate )
        ++m_nMagicID;
      m_Connections.mp_AddItem( pNew );
      DBG( "#connected = " + toString( m_Connections.mf_nGetNumItems() ) )
      return pNew;
    }

      /**
        * Implementation of the IThread method.
        */
    void mp_Run()
    {
      while( !IThread::mf_bThreadShouldStop() )
      {
        mc_Lock.mf_Enter();
        for( unsigned i = 0 ; i < m_Connections.mf_nGetNumItems() ; ++i )
        {
          ISocketConnection* pCur = m_Connections( i );
          if( !pCur->mf_bIsConnected() )
          {
            mf_AboutToRemove( pCur );
            m_Connections.mp_RemoveItemAt( i );
            DBG( "#connected = " + toString( m_Connections.mf_nGetNumItems() ) )
          }
        }
        mc_Lock.mf_Leave();
        IThread::mf_bWait( 500 );
      }
    }
  };

}

#endif //#ifndef __SOCKETCONNFACTORYIMP_H__
