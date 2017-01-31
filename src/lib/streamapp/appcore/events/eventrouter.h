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

#ifndef __EVENTROUTER_H__
#define __EVENTROUTER_H__

#include "containers/ownedarray.h"

#include "events.h"

#include <QtGlobal>

#include <map>

using namespace streamapp;

namespace appcore
{

    /**
      * EventRouter
      *   Simple IEventRouter implementation.
      *   @see IEventRouter
      *************************************** */
  class EventRouter : public DefEventRouter
  {
  public:
      /**
        * Constructor.
        */
    EventRouter();

      /**
        * Destructor.
        */
    ~EventRouter();

      /**
        * Implementation of the DefEventRouter method.
        */
    void mp_DispatchEvent( const int& ac_nEventType, gt_EventData a_pData = 0 );

      /**
        * Implementation of the DefEventRouter method.
        */
    void mp_RegisterEventDispatcher( DefEventDispatcher* a_pListener, const int& ac_nEventType );

      /**
        * Implementation of the DefEventRouter method.
        */
    void mp_UnRegisterEventDispatcher( DefEventDispatcher* a_pListener, const int& ac_nEventType );

      /**
        * Implementation of the DefEventRouter method.
        */
    void mp_UnRegisterEventDispatcher( DefEventDispatcher* a_pListener );

      /**
        * Get the number of registered dispatchers.
        * @return the number
        */
    unsigned mf_nGetNumDispatchers() const;

  protected:
      /**
        * mt_DispatcherAndType
        *   container for a dispatcher,
        *   and the type it's registered for
        ************************************ */
    struct mt_DispatcherAndType
    {
      mt_DispatcherAndType( const int ac_eType, DefEventDispatcher* a_pDispatcher ) :
        mc_eType( ac_eType ),
        mc_pDispatcher( a_pDispatcher )
      {}
      int mc_eType;
      DefEventDispatcher* mc_pDispatcher;
    };

    OwnedArray<mt_DispatcherAndType> m_Dispatchers;
    //CriticalSection* const mc_pLock; FIXME needs a ReadWriteLock supporting recursivity and thread count
  };

    /**
      * AdvEventRouter
      *   a faster implementation: dispatchers are stored per type in a map.
      *   @see EventRouter
      *   @see IEventRouter
      ********************************************************************** */
  template<class tType = int>
  class AdvEventRouter : public IEventRouter<tType>
  {
  public:
      /**
        * Constructor.
        */
    AdvEventRouter()
    {}

      /**
        * Destructor.
        */
    ~AdvEventRouter()
    {
      mt_It it = m_Container.end();
      for( mt_It i = m_Container.begin() ; i != it ; ++i )
        delete (*i).second;
    }

      /**
        * Implementation of the IEventRouter method.
        */
    void mp_DispatchEvent( const tType& ac_eEventType, gt_EventData a_pData = 0 )
    {
      if( mf_bPreDispatchEvent( ac_eEventType, a_pData ) )
        return;

        //find the dispatchers for the event
      mt_It it = m_Container.find( ac_eEventType );
      if( it != m_Container.end() )
      {
        mt_Dispatchers* pDisp = (*it).second;
        const unsigned nDispatchers = pDisp->mf_nGetNumItems();
        unsigned nDispatched = 0;
        while( nDispatched < nDispatchers )
        {
          pDisp->mf_GetRItem( nDispatched )->mp_DispatchEvent( ac_eEventType, a_pData );
          ++nDispatched;
        }
      }
    }

      /**
        * Implementation of the IEventRouter method.
        */
    void mp_RegisterEventDispatcher( IEventDispatcher<tType>* a_pListener, const tType& ac_eEventType )
    {
      Q_ASSERT( a_pListener != this );
      mt_It it = m_Container.find( ac_eEventType );
      if( it == m_Container.end() )
      {
        m_Container[ ac_eEventType ] = new mt_Dispatchers( 3 );
        it = m_Container.find( ac_eEventType );
      }
      (*it).second->mp_AddItem( a_pListener );
    }

      /**
        * Implementation of the IEventRouter method.
        */
    void mp_UnRegisterEventDispatcher( IEventDispatcher<tType>* a_pListener, const tType& ac_eEventType )
    {
      mt_It it = m_Container.find( ac_eEventType );
      if( it != m_Container.end() )
      {
        mt_Dispatchers* pDisp = (*it).second;
        const unsigned nDispatchers = pDisp->mf_nGetNumItems();
        for( unsigned i = 0 ; i < nDispatchers ; ++i )
        {
          if( pDisp->mf_GetItem( i ) == a_pListener )
          {
            pDisp->mp_RemoveItemAt( i );
            break;
          }
        }
      }
    }

      /**
        * Implementation of the IEventRouter method.
        */
    void mp_UnRegisterEventDispatcher( IEventDispatcher<tType>* a_pListener )
    {
      mt_It it = m_Container.begin();
      while( it != m_Container.end() )
      {
        mt_Dispatchers* pDisp = (*it).second;
        for( unsigned i = 0 ; i < pDisp->mf_nGetNumItems() ; ++i )
        {
          if( pDisp->mf_GetItem( i ) == a_pListener )
            pDisp->mp_RemoveItemAt( i );
        }
        if( pDisp->mf_nGetNumItems() == 0 )
        {
          m_Container.erase( it );
          it = m_Container.begin(); //FIXMEEE there must be a faster way to do this, right?
        }
        ++it;
      }
    }

  protected:
    typedef DynamicArray<IEventDispatcher<tType>*> mt_Dispatchers;
    typedef std::map<tType,mt_Dispatchers*>   mt_Container;
    typedef typename mt_Container::iterator   mt_It;

    mt_Container m_Container;
  };

}

#endif //#ifndef __EVENTROUTER_H__
