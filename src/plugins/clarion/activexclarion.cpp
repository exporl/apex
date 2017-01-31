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
 
#include "activexclarion.h"

#include <assert.h>
#include <containers/array.h>
#include <appcore/threads/thread.h>
#include <appcore/threads/waitableobject.h>
#include <_fromv3/_archs/win32/automation/safearray.h>
#include <_fromv3/_archs/win32/automation/olecontroller.h>
#include <_fromv3/_archs/win32/automation/dispatchinfo.h>
#include <_fromv3/_archs/win32/automation/eventhandler.h>
#include <QStringList>
#include <iostream>

using namespace clarion;
using namespace appcore;
using namespace streamapp;
using ::smn::win32::automation::EventHandler;
using ::smn::win32::automation::OleController;
using ::smn::win32::automation::DispatchInfoBuilder;

namespace clarion
{
  class WaitConnection : public appcore::IThread
  {
  public:
    WaitConnection( const WaitableObject& a_Object ) :
        IThread( "waitclarion" ),
      mc_Waiter( a_Object )
    {
    }

    void mp_Run()
    {
      if( mc_Waiter.mf_eWaitForSignal( 10000 ) == WaitableObject::wait_ok )
      {
        //TODO disable entire apex screen after call to connect()
        //then post event to main qt message loop here
        //when event got received: show 'connectd' dialog
        //and re-enable entire screen
        //else when wait fqailed: show 'not connected' and quit exp
        std::cout << "!!!!connected!!!!" << std::endl;
      }
    }

  private:
    const WaitableObject& mc_Waiter;
  };

  struct ActiveXStuff
  {
    typedef EventHandler< ActiveXStuff > mt_EvtHandler;

      /**
        * Constructor.
        */
    ActiveXStuff() :
      m_pStateEventHandler( 0 ),
      m_pStateEventToSignal( 0 ),
      m_pMeasurementEventToSignal( 0 )
    {
    }

      /**
        * Destructor.
        */
    ~ActiveXStuff()
    {
      delete m_pStateEventHandler;
    }


      /**
        * Open the activeX control and get event IDs.
        * @return true if ok
        */
    bool mp_bOpenBEDCS() throw()
    {
        //first get AppCTL instance, then get actual app from it
      if( m_BEDSAppCtl.mp_bCreateObject( "BEDCSCTL.CBEDCSAppCTL" ) )
      {
        if( m_BEDSAppCtl.mp_bGetProperty( "BEDCSApp", m_BEDCSApp.mf_GetObject()() ) )
        {
            //get event IDs
          if( DispatchInfoBuilder::smf_hGetConnectionPointIDs( m_BEDCSApp.mf_GetObject()(), m_EventIDs ) == S_OK )
          {
              //install handler for any of the event ID's;
              //they all seem to use the same event after all
            m_pStateEventHandler = new mt_EvtHandler( m_BEDCSApp.mf_GetObject(), m_EventIDs[ 5 ], *this, &ActiveXStuff::m_hEquipmentStateChanged );
            return !m_pStateEventHandler->mf_bError();
          }
        }
      }
      return false;
    }

      /**
        * Establish connection by setting BEDCS "Online".
        * @return true for success
        */
    bool mp_bConnectBEDCS() throw()
    {
      return m_BEDCSApp.mp_bSetProperty( "Online", true );
    }

      /**
        * Disconnect from device.
        * @return true if ok.
        */
    bool mp_bDisconnectBEDCS() throw()
    {
      return m_BEDCSApp.mp_bSetProperty( "Online", false );
    }

      /**
        * Load a file and get it's parameters.
        * @return true if ok.
        */
    bool mp_bLoadFile( const QString& ac_psFile ) throw()
    {
      if( m_BEDCSApp.mp_bInvokeMethod( "LoadExpFile", "a", ac_psFile.toAscii().constData() ) )
      {
        VARIANT v;
        if( !m_BEDCSApp.mp_bGetProperty( "ControlVars", v ) )
          return false;
        ::smn::win32::automation::gf_bSafeArrayToStringArray( v, m_Params );
          //if there are no parameters file didn't load..
        return !m_Params.empty();
      }
      return false;
    }

      /**
        * Check if we have a parameter.
        * @param ac_sParam param name
        * @param a_pIndex set to index if non-zero
        */
    bool mf_bHasParameter( const QString& ac_sParam, int* a_pIndex )
    {
      const int iWhere = m_Params.indexOf( ac_sParam );
      if( iWhere >= 0 )
      {
        if( a_pIndex )
          *a_pIndex = iWhere;
        return true;
      }
      return false;
    }

      /**
        * Get a parameter's value.
        * @param ac_sParam param name
        * @param a_sParamValue set value
        * @return true if found
        */
    bool mf_bGetParameter( const QString& ac_sParam, QString& a_sParamValue )
    {
      int iWhere;
      if( mf_bHasParameter( ac_sParam, &iWhere ) )
      {
        a_sParamValue = m_Params[ iWhere ];
        return true;
      }
      return false;
    }

      /**
        * Check if all parameters in ac_saParams are available
        * @param ac_saParams params to check for
        * @param a_sFirstNotFound set to first param not found
        * @return true if all found
        */
    bool mf_bCheckParameters( const QStringList& ac_saParams, QString& a_sFirstNotFound )
    {
        //get total number of params
      const int nMustFind = m_Params.size();
      const int nToCheck = ac_saParams.size();
      int nFound = 0;
      int nFoundIndex = -1;

        //make a boolean array to check which params are found in the xml
      ArrayStorage<bool> bFound( nMustFind );

        //walk the param list
      for( ; nFound < nToCheck ; ++nFound )
      {
        bFound( nFound ) = false;

          //check if param exists
        if( !mf_bHasParameter( ac_saParams[ nFound ], &nFoundIndex ) )
          return false;
        else
          bFound( nFoundIndex ) = true;
      }

        //do final check
      if( nFound != nMustFind )
      {
          //get the (first) one that wasn't found
        for( int i = 0 ; i < nMustFind ; ++i )
        {
          if( !bFound( i ) )
          {
            a_sFirstNotFound = ac_saParams[ i ];
            return false;
          }
        }
      }
      return true;
    }

      /**
        * Set a parameter's value.
        * @param ac_sParam param name
        * @param ac_sNewValue set value
        * @return true if set
        */
    bool mp_bSetParameter( const QString& ac_sParam, const QString& ac_sNewValue )
    {
      return m_BEDCSApp.mp_bInvokeMethod( "Let_ControlVarVal", "aa",
        ac_sParam.toAscii().constData(), ac_sNewValue.toAscii().constData() );
    }

      /**
        * Start play.
        * @return true if ok.
        */
    bool mp_bStart() throw()
    {
      return m_BEDCSApp.mp_bInvokeMethod( "MeasureNoSave" );
    }

      /**
        * Stop play.
        * @return true if ok.
        */
    bool mp_bStop() throw()
    {
      return m_BEDCSApp.mp_bInvokeMethod( "StopPlayMeasure" );
    }


      /**
        * Set the event to signal when the state changes to "Ready".
        * This is done by registering an eventhandler for the ready event,
        * and have it call back to our m_hEquipmentStateChanged method.
        * @param a_pEventToSignal a WaitableObject pointer
        */
    void mp_SetStateHandler( WaitableObject* a_pEventToSignal )
    {
      assert( m_pStateEventToSignal == 0 ); //we can't have two listeners
      assert( a_pEventToSignal );
      m_pStateEventToSignal = a_pEventToSignal;
    }

      /**
        * Remove state handler.
        * Must have been set first!
        */
    void mp_RemoveStateHandler()
    {
      m_pStateEventToSignal = 0;
    }

      /**
        * Set the event to signal when the measurement is done.
        * @param a_pEventToSignal a CEvent pointer
        */
    void mp_SetDoneHandler( WaitableObject* a_pEventToSignal )
    {
      assert( m_pMeasurementEventToSignal == 0 ); //we can't have two listeners
      assert( a_pEventToSignal );
      m_pMeasurementEventToSignal = a_pEventToSignal;
    }

      /**
        * Remove measurement done handler.
        * Must have been set first!
        */
    void mp_RemoveDoneHandler()
    {
      m_pMeasurementEventToSignal = 0;
    }

      /**
        * The function that gets called when an event is fired changes.
        */
    HRESULT m_hEquipmentStateChanged( mt_EvtHandler*, DISPID a_DispIDMember, REFIID, LCID,
                                      WORD, DISPPARAMS* a_pParams, VARIANT*, EXCEPINFO*, UINT* )
    {
      if( a_DispIDMember == 5 )
      {
        std::cout << "got equipmentstatechanged event" << std::endl;
        if( m_pStateEventToSignal && a_pParams->cArgs == 1 )
        {
          VARIANTARG& arg = a_pParams->rgvarg[ 0 ];
          if( arg.vt == VT_I4 )
          {
            long lValue = V_I4( &arg );
            if( lValue == 2 ) // 2 = ready
            {
              m_pStateEventToSignal->mp_SignalObject();
              std::cout << "equipmentstatechanged == Ready!" << std::endl;
            }
          }
        }
      }
      else if( a_DispIDMember == 3 )
      {
        std::cout << "got donemeasurement event" << std::endl;
        if( m_pMeasurementEventToSignal )
          m_pMeasurementEventToSignal->mp_SignalObject();
      }
      else if( a_DispIDMember == 6 )
      {
        std::cout << "got parameterchanged event" << std::endl;
      }
      else
      {
        std::cout << "unknown event: " << a_DispIDMember << std::endl;
      }
      return S_OK;
    }

  private:
    OleController m_BEDSAppCtl;
    OleController m_BEDCSApp;
    DispatchInfoBuilder::mt_CLSIDs m_EventIDs;
    mt_EvtHandler* m_pStateEventHandler;
    WaitableObject* m_pStateEventToSignal;
    WaitableObject* m_pMeasurementEventToSignal;
    QStringList m_Params;
  };
}


/********************************************************************************/


ActiveXClarionWrapper::ActiveXClarionWrapper() :
  m_pWaitThread( 0 ),
  m_pBEDCSConnectedEvent( new WaitableObject() ),
  m_pEndOfStimulusEvent( new WaitableObject() ),
  m_pStuff( new ActiveXStuff() )
{
  m_pWaitThread = new WaitConnection( *m_pBEDCSConnectedEvent );
}

ActiveXClarionWrapper::~ActiveXClarionWrapper()
{
  mp_bDisConnect();
  delete m_pWaitThread;
  delete m_pBEDCSConnectedEvent;
  delete m_pEndOfStimulusEvent;
  delete m_pStuff;
}

bool ActiveXClarionWrapper::mp_bConnect()
{
  if( mv_eStatus != mc_eInit )
  {
    strcpy( m_cError, "already connected" );
    return false;
  }
  bool bRet = false;

    //open ActiveX
  if( m_pStuff->mp_bOpenBEDCS() )
  {
      //install "online" evt handler
    m_pStuff->mp_SetStateHandler( m_pBEDCSConnectedEvent );
    m_pWaitThread->mp_Start();

      //set "Online"; BEDCS will start searching the device
    if( m_pStuff->mp_bConnectBEDCS() )
    {
      mv_eStatus = mc_eConnected; //TODO remove this
      bRet = true;                //
    }
    else
    {
      bRet = false;
      strcpy( m_cError, "couldn't connect to BEDCS device" );
    }
  }
  else
  {
    bRet = false;
    strcpy( m_cError, "couldn't open BEDCS ActiveX control" );
  }

  return bRet;
}

bool ActiveXClarionWrapper::mp_bDisConnect()
{
  if( mv_eStatus == mc_eConnected || mv_eStatus == mc_eLoaded )
  {
    mp_bStop();
    m_pStuff->mp_RemoveStateHandler();
    m_pStuff->mp_bDisconnectBEDCS();

    strcpy( m_cError, "" );
    mv_eStatus = mc_eInit;
    return true;
  }
  strcpy( m_cError, "not connected" );
  return false;
}

bool ActiveXClarionWrapper::mp_bLoadFile( const QString& ac_pFileName )
{
  if( mv_eStatus == mc_eRunning )
  {
    strcpy( m_cError, "can't load while running" );
    return false;
  }

  if( m_pStuff->mp_bLoadFile( ac_pFileName ) )
  {
    mv_eStatus = mc_eLoaded;
    return true;
  }
  else
  {
    sprintf( m_cError, "failed to load %s", ac_pFileName.toAscii().data() );
    return false;
  }
}

bool ActiveXClarionWrapper::mp_bStart()
{
  if( mv_eStatus == mc_eLoaded )
  {
    m_pStuff->mp_SetDoneHandler( m_pEndOfStimulusEvent );
    if( m_pStuff->mp_bStart() )
    {
      mv_eStatus = mc_eRunning;
      return true;
    }
  }
  strcpy( m_cError, "can't play if not loaded, already running, or not connected" );
  return true;
}

bool ActiveXClarionWrapper::mp_bStop()
{
  if( mv_eStatus == mc_eRunning )
  {
    m_pStuff->mp_bStop();
    m_pStuff->mp_RemoveDoneHandler();
    mv_eStatus = mc_eLoaded;
    return true;
  }
  strcpy( m_cError, "not running" );
  return false;
}

bool ActiveXClarionWrapper::mf_bIsPlaying() const
{
  return mv_eStatus == mc_eRunning;
}

bool ActiveXClarionWrapper::mp_bWaitForStop()
{
  if( mv_eStatus == mc_eRunning )
  {
    m_pEndOfStimulusEvent->mf_eWaitForSignal();
    return mp_bStop();
  }
  strcpy( m_cError, "not running" );
  return false;
}

bool ActiveXClarionWrapper::mf_bHasParameter( const QString& ac_pParName, int* const ac_pIndex ) const
{
  if( mv_eStatus == mc_eLoaded )
  {
    if( !m_pStuff->mf_bHasParameter( ac_pParName, ac_pIndex ) )
    {
      sprintf( m_cError, "parameter %s doesn't exist", ac_pParName );
      return false;
    }
    return true;
  }
  strcpy( m_cError, "can't check parameter if not loaded" );
  return false;
}

bool ActiveXClarionWrapper::mf_bCheckParameters( const QStringList& ac_saParms ) const
{
  if( mv_eStatus == mc_eLoaded )
  {
    QString sNotFound;
    if( !m_pStuff->mf_bCheckParameters( ac_saParms, sNotFound ) )
    {
      const QString sMess( "parameter not found: " + sNotFound );
      strcpy( m_cError, sMess.toAscii().constData() );
      return false;
    }
    return true;
  }
  strcpy( m_cError, "can't check parameter if not loaded" );
  return false;
}

bool ActiveXClarionWrapper::mp_bSetParameter( const QString& ac_pPar, const QString& ac_pVal )
{
  if( mv_eStatus == mc_eLoaded )
  {
    if( !m_pStuff->mp_bSetParameter( ac_pPar, ac_pVal ) )
    {
      sprintf( m_cError, "parameter %s could not be set", ac_pPar );
      return false;
    }
    return true;
  }
  strcpy( m_cError, "not loaded or not connected" );
  return false;
}

/********************************************************************************************/

Q_EXPORT_PLUGIN2( clarionplugin, clarion::ClarionWrapperCreator )

ClarionWrapperCreator::~ClarionWrapperCreator()
{
}

QStringList ClarionWrapperCreator::availablePlugins() const
{
  return QStringList( "clarionplugin" );
}

IClarionWrapper *ClarionWrapperCreator::create () const
{
  return new ActiveXClarionWrapper();
}
