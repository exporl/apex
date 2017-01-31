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
 
  /**
    * @file events.h
    * Declares a generic communication pattern.
    *
    * By defining a set of interfaces that each have the same generic methods,
    * it is very easy to write classes controlling these interfaces without having
    * to know anything about the actual data passed between them.
    *
    *   template<class tType>
    *   void mp_DispatchEvent( const tType& ac_tEventType, void* a_pData = 0 );
    *
    * void* is used for the event data since this is the most generic one can get,
    * and it removes the need for a template on the data since it can be anything.
    * Also, using a template would seriously reduce the reusability, since all elements
    * need to have the same type.
    * A template for the opcode however makes more sense, the opcode cannot just be anything,
    * as it is used the same way everywhere throughout an app, namely to see what kind of event it is.
    * The opcode is preferably a small datasize that is fast to check and without casting.
    * It still leaves the choice free between eg enum, string or smaller types.
    *
    * Classes using mp_DispatchEvent first check the ac_tEventType to see what the event is about,
    * then they cast a_pData to the appropriate type. This off course requires all classes to use the same
    * types, so the programmer should create a header file containing all ac_tEventType and their corresponding
    * datatype, and document this. Once this is done, everything is set up to use events throughout the application.
    */

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "defines.h"

  /**
    * namespace containing classes to build an application
    ****************************************************** */
namespace appcore
{

    /**
      * The event data type.
      */
  typedef void* gt_EventData;

    /**
      * IEventDispatcher
      *   interface for dispatching events.
      ************************************* */
  template< class tType = int >
  class IEventDispatcher
  {
  protected:
      /**
        * Protected constructor.
        */
    IEventDispatcher()
    {}

  public:
      /**
        * Destructor.
        */
    virtual ~IEventDispatcher()
    {}

      /**
        * Dispatch an event.
        * @param ac_tEventType the event code
        * @param a_pData the event data
        */
    virtual void mp_DispatchEvent( const tType& ac_tEventType, gt_EventData a_pData = 0 ) = 0;
  };

     /**
      * IEventPoster
      *   base class for posting events to a single dispatcher.
      ********************************************************* */
  template< class tType = int >
  class IEventPoster
  {
  public:
      /**
        * Constructor.
        */
    IEventPoster() :
      mv_pListener( 0 )
    {}

      /**
        * Destructor.
        */
    virtual ~IEventPoster()
    {}

      /**
        * Set the dispatcher to receive events.
        * @param a_pListener the dispatcher
        */
    virtual void mp_InstallEventDispatcher( IEventDispatcher<tType>* a_pListener )
    { mv_pListener = a_pListener; }

      /**
        * Get the current dispatcher.
        * @return const pointer (dispatching must happen through mp_InstallEventDispatcher)
        */
    const IEventDispatcher<tType>* mf_pGetListener() const
    { return mv_pListener; }

  protected:
      /**
        * Dispatch an event to our listener, if any.
        * @param ac_tEventType the event code
        * @param a_pData the event data
        */
    virtual void mp_DispatchEventToListener( const tType& ac_tEventType, gt_EventData a_pData = 0 )
    {
      if( mv_pListener )
        mv_pListener->mp_DispatchEvent( ac_tEventType, a_pData );
    }

      /**
        * Our dispatcher.
        */
    IEventDispatcher<tType>* mv_pListener;
  };

    /**
      * IEventCommunicator
      *   combines dispatching and posting.
      ************************************* */
  template< class tType = int >
  class IEventCommunicator : public IEventPoster<tType>, public IEventDispatcher<tType>
  {
  protected:
      /**
        * Protected constructor.
        */
    IEventCommunicator()
    {}

  public:
      /**
        * Destructor.
        */
    virtual ~IEventCommunicator()
    {}

      /**
        * Dispatch an event.
        * Implementation for IEventDispatcher.
        * @param ac_tEventType the event code
        * @param a_pData the event data
        */
    virtual void mp_DispatchEvent( const tType& ac_tEventType, gt_EventData a_pData = 0 ) = 0;

      /**
        * Set the dispatcher to receive events.
        * @param a_pListener the dispatcher
        */
    void mp_InstallEventDispatcher( IEventDispatcher<tType>* a_pListener )
    { IEventPoster<tType>::mp_InstallEventDispatcher( a_pListener ); }

  protected:
      /**
        * Dispatch an event to our listener, if any.
        * @param ac_tEventType the event code
        * @param a_pData the event data
        */
    void mp_DispatchEventToListener( const tType& ac_tEventType, gt_EventData a_pData = 0 )
    { IEventPoster<tType>::mp_DispatchEventToListener( ac_tEventType, a_pData ); }
  };

    /**
      * IEventRouter
      *   interface for dispatching to multiple listeners.
      **************************************************** */
  template< class tType = int >
  class IEventRouter : public IEventDispatcher<tType>
  {
  protected:
      /**
        * Protected constructor.
        */
    IEventRouter()
    {}

  public:
      /**
        * Destructor.
        */
    virtual ~IEventRouter()
    {}

      /**
        * Dispatch an event.
        * Implementation for IEventDispatcher.
        * @param ac_tEventType the event code
        * @param a_pData the event data
        */
    virtual void mp_DispatchEvent( const tType& ac_tEventType, gt_EventData a_pData = 0 ) = 0;

      /**
        * Pre-Dispatch an event.
        * Implementations must call this method first in their mp_DispatchEvent method.
        * Custom classes can then derive from the implementation, and implement this method
        * to pre-translate messages. If this method returns true, this indicates the event was
        * dispatched in some way, and must not be routed forth anymore.
        * Example usage: suppose a server spawning dispatchers, which are all registered for
        * a TransmitEvent.If you want to broadcast a message, you would dispatch the TransmitEvent
        * to the router, and all dispatchers will receive it. Good. Now if you don't want to
        * broadcast it, but you want a single dispatcher to handle the event, you have a problem
        * since the router can't distinguish between the receivers. So you need some kind of
        * translator, preferrably without modifying any existing code.
        * Here's what you do: derive your own class from the router, you don't have to modify it,
        * and override the mp_PreDispatchEvent method. In this method you only do something if you
        * receive a TransmitEvent, and you make sure the data that comes with the TransmitEvent
        * contains the receiver id somehow.Then dispatch some QueryIDEvent event to all routers
        * registered for the TransmitEvent, and let them return their id. If one has the id you're
        * looking for, have it dispatch the TransmitEvent and return true.
        * If the data didn't contain an id, just return false and it's broadcast time again.
        * @note in order to be usefull, implementations should have protected members so that
        * this method can access the registered items..
        * @param ac_tEventType the event code
        * @param a_pData the event data
        * @return false if the event can be routed further, true to omit it
        */
    INLINE virtual bool mf_bPreDispatchEvent( const tType& /*ac_tEventType*/, gt_EventData /*a_pData*/ )
    {
      return false;
    }

      /**
        * Register a dispatcher for ac_tEventType.
        * If mp_DispatchEvent receives an ac_tEventType, all registered dispatchers
        * will be called with the event and the data.
        * @param ac_tEventType the event code
        * @param a_pListener the dispatcher
        */
    virtual void mp_RegisterEventDispatcher( IEventDispatcher<tType>* a_pListener, const tType& ac_tEventType ) = 0;

      /**
        * Unregister a dispatcher for ac_tEventType.
        * Doesn't do anything if the dispatcher isn't found,
        * or if it's not registered for the given type.
        * @param ac_tEventType the event code
        * @param a_pListener the dispatcher
        */
    virtual void mp_UnRegisterEventDispatcher( IEventDispatcher<tType>* a_pListener, const tType& ac_tEventType ) = 0;

      /**
        * Completely unregister a dispatcher.
        * Doesn't do anything if the dispatcher isn't found.
        * @param a_pListener the dispatcher
        */
    virtual void mp_UnRegisterEventDispatcher( IEventDispatcher<tType>* a_pListener ) = 0;
  };


    /**
      * The default IEventDispatcher.
      ******************************* */
  typedef IEventDispatcher<> DefEventDispatcher;

    /**
      * The default IEventPoster.
      *************************** */
  typedef IEventPoster<> DefEventPoster;

    /**
      * The default IEventCommunicator.
      ********************************* */
  typedef IEventCommunicator<> DefEventCommunicator;

    /**
      * The default IEventRouter.
      *************************** */
  typedef IEventRouter<> DefEventRouter;

    /**
      * tParamEvent
      *   generic event data sructure for parameters
      ********************************************** */
  struct tParamEvent
  {
      /**
        * Constructor.
        * @param ac_nIndex index
        * @param ac_dValue value
        * @param ac_nOpCode extra op
        * @param a_pData extra data
        */
    tParamEvent( const unsigned ac_nIndex, const double ac_dValue, const int ac_nOpCode = 0, void* a_pData = 0 ) :
      m_nParamIndex( ac_nIndex ),
      m_dParamValue( ac_dValue ),
      m_nParamOpcode( ac_nOpCode ),
      m_pParamData( a_pData )
    {}

      /**
        * Destructor.
        */
    tParamEvent() :
      m_nParamIndex( 0 ),
      m_dParamValue( 0.0 ),
      m_nParamOpcode( 0 ),
      m_pParamData( 0 )
    {}

      /**
        * Used by eg Queue.
        * @return a new'd tParamEvent
        */
    static tParamEvent* tParamEventAllocator()
    {
      return new tParamEvent();
    }

      /**
        * Used by eg Queue.
        * @param a_pIn the event to copy
        * @param a_pOut the a_pIn copy
        */
    static void tParamEventDuplicator( const tParamEvent* a_pIn, tParamEvent* const a_pOut )
    {
      a_pOut->m_dParamValue   = a_pIn->m_dParamValue;
      a_pOut->m_nParamOpcode  = a_pIn->m_nParamOpcode;
      a_pOut->m_nParamIndex   = a_pIn->m_nParamIndex;
      a_pOut->m_pParamData    = a_pIn->m_pParamData;
    }

    unsigned  m_nParamIndex;
    double    m_dParamValue;
    int       m_nParamOpcode;
    void*     m_pParamData;
  };

}

#endif //#ifndef __EVENTS_H__
