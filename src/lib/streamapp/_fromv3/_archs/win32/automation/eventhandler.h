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
 
#ifndef __STR_OLE_EVENTHANDLER_H__
#define __STR_OLE_EVENTHANDLER_H__

#include "idispatch.h"
#include <OCIdl.h>

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

      /**
        * EventHandler
        *   implements event handling through OLE connection points.
        *   Just wraps all the IConnectionPoint stuff: pass an IDispatch
        *   that fires events, the event ID you're interested in and
        *   an event target, and the target receives all events..
        *   @param tEventTarget type of event target class
        **************************************************************** */
    template< class tEventTarget >
    class EventHandler : public IDispatch
    {
    public:

        //!the invoke method tEventTarget must implement;
        //!parameters just like a normal Inkove call.
      typedef HRESULT (tEventTarget::*pfnInvoke) ( EventHandler< tEventTarget >*, DISPID, REFIID, LCID,
                                                   WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT* );

        //!the method tEventTarget can implement to get
        //!notified of connection changes
      typedef void (tEventTarget::*pfnStateChanged ) ( const bool, const CLSID& );

        /**
          * Constructor.
          * Use when connecting later on.
          * @param a_Target event target
          */
      EventHandler( tEventTarget& a_Target ) :
        mv_hResult( S_FALSE ),
        mv_nRefs( 1 ),
        m_Target( a_Target ),
        m_pfnTargetInvoke( 0 ),
        m_pfnTargetStateChange( 0 ),
        m_dwEventCookie( 0 ),
        m_EventID( ac_sSourceEventID )
      {
      }

        /**
          * Constructor.
          * Use mf_bError() afterwards to see
          * if connection succeeded.
          * @param a_pSource event source
          * @param ac_sSourceEventID event ID
          * @param a_Target event target
          * @param a_pfnTargetInvoke target invoke method pointer
          * @param a_pfnTargetStateChange target state change method pointer
          */
      EventHandler( IDispatch* a_pSource, const CLSID& ac_sSourceEventID,
                    tEventTarget& a_Target,
                    pfnInvoke a_pfnTargetInvoke, pfnStateChanged a_pfnTargetStateChange = 0 ) :
        mv_hResult( S_OK ),
        mv_nRefs( 1 ),
        m_Target( a_Target ),
        m_pfnTargetInvoke( a_pfnTargetInvoke ),
        m_pfnTargetStateChange( a_pfnTargetStateChange ),
        m_dwEventCookie( 0 ),
        m_EventID( ac_sSourceEventID )
      {
        mp_bOpenConnection( a_pSource );
      }

        /**
          * Destructor.
          */
      ~EventHandler()
      {
        mp_CloseConnection();
      }


        /**
          * Check if all is fine.
          * @return false if no errors
          */
      s_finline bool mf_bError() const s_no_throw
      {
        return mv_hResult != S_OK;
      }

        /**
          * Get the last error occurred
          * @return our HRESULT
          */
      s_finline HRESULT mf_hGetLastError() const s_no_throw
      {
        return mv_hResult;
      }

        /**
          * Open connection.
          * If a connection is present, it is closed first.
          * @param a_pSource event source
          * @param ac_sSourceEventID event ID
          * @param a_pfnTargetInvoke target invoke method pointer
          * @param a_pfnTargetStateChange target state change method pointer
          * @return true for success
          */
      bool mp_bOpenConnection( IDispatch* a_pSource, const CLSID& ac_sSourceEventID,
                               pfnInvoke a_pfnTargetInvoke, pfnStateChanged a_pfnTargetStateChange = 0 )
      {
        mp_CloseConnection();
        m_EventID = ac_sSourceEventID;
        m_pfnTargetInvoke = a_pfnTargetInvoke;
        m_pfnTargetStateChange = a_pfnTargetStateChange;
        return mp_bOpenConnection( a_pSource );
      }

        /**
          * Close connection.
          * Call this if you don't want to receive events anymore.
          */
      void mp_CloseConnection()
      {
        if( m_pConnectionPoint )
        {
          m_pConnectionPoint->Unadvise( m_dwEventCookie );
          m_dwEventCookie = 0;
          m_pConnectionPoint.mp_Close();
          if( m_pfnTargetStateChange )
            (m_Target.*m_pfnTargetStateChange)( false, m_EventID );
        }
      }


        /**
          * Implementation of the IDispatch method.
          */
      STDMETHOD_( ULONG, AddRef )()
      {
        InterlockedIncrement( &mv_nRefs );
        return mv_nRefs;
      }

        /**
          * Implementation of the IDispatch method.
          */
      STDMETHOD_( ULONG, Release )()
      {
        InterlockedDecrement( &mv_nRefs );
        if( mv_nRefs == 0 )
          delete this;
        return mv_nRefs;
      }

        /**
          * Implementation of the IDispatch method.
          * Returns pointer to this if riid matches
          * something we know.
          */
      STDMETHOD( QueryInterface )( REFIID riid, void** ppvObject )
      {
        if( riid == IID_IUnknown )
        {
          *ppvObject = (IUnknown*) this;
          AddRef();
          return S_OK;
        }
        if( riid == IID_IDispatch || riid == m_EventID )
        {
          *ppvObject = (IDispatch*) this;
          AddRef();
          return S_OK;
        }

        return E_NOINTERFACE;
      }

        /**
          * Implementation of the IDispatch method.
          */
      STDMETHOD( GetTypeInfoCount )( UINT* )
      {
        return E_NOTIMPL;
      }

        /**
          * Implementation of the IDispatch method.
          */
      STDMETHOD( GetTypeInfo )( UINT , LCID , ITypeInfo** )
      {
        return E_NOTIMPL;
      }

        /**
          * Implementation of the IDispatch method.
          */
      STDMETHOD( GetIDsOfNames )( REFIID, LPOLESTR*, UINT, LCID, DISPID* )
      {
        return E_NOTIMPL;
      }

        /**
          * Implementation of the IDispatch method.
          * This is the one thing we need off course;
          * Forwards call to target.
          */
      STDMETHOD( Invoke )( DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams,
                           VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr )
      {
        return (m_Target.*m_pfnTargetInvoke)( this, dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
      }

    protected :
      typedef tGetIUnknownWrapper< IConnectionPoint* >::type mt_ConnPoint;

      HRESULT           mv_hResult;
      LONG              mv_nRefs;
      tEventTarget&     m_Target;
      pfnInvoke         m_pfnTargetInvoke;
      pfnStateChanged   m_pfnTargetStateChange;
      CLSID             m_EventID;
      DWORD             m_dwEventCookie;
      mt_ConnPoint      m_pConnectionPoint;

      bool mp_bOpenConnection( IDispatch* a_pSource )
      {
          //QI this object itself for the IUnknown pointer which will be used
          //later to connect to the Connection Point of the source object.
        tGetIUnknownWrapper< IUnknown* >::type pIUnknown;
        mv_hResult = this->QueryInterface( IID_IUnknown, (void**) &pIUnknown() );
        if( !mf_bError() )
        {
            //QI the source for it's IConnectionPointContainer interface.
          tGetIUnknownWrapper< IConnectionPointContainer* >::type pContainer;
          mv_hResult = a_pSource->QueryInterface( IID_IConnectionPointContainer, (void**)&pContainer() );

            //get IConnectionpoint for our event
          if( !mf_bError() )
            mv_hResult = pContainer->FindConnectionPoint( m_EventID, &m_pConnectionPoint() );

            //connect and save cookie
          if( !mf_bError() )
            mv_hResult = m_pConnectionPoint->Advise( pIUnknown(), &m_dwEventCookie );

            //report connected
          bool bOk = !mf_bError();
          if( m_pfnTargetStateChange )
            (m_Target.*m_pfnTargetStateChange)( bOk, m_EventID );

          return bOk;
        }
        return false;
      }

      EventHandler( const EventHandler& ) s_no_throw;
      EventHandler& operator = ( const EventHandler& ) s_no_throw;
    };

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_OLE_EVENTHANDLER_H__
