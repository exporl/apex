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
 
#include "connectionrundelegatecreator.h"
#include "exceptions.h"
#include "stimulus/datablock.h"
#include "stimulus/outputdevice.h"
#include "stimulus/filter.h"
#include <map>
#include <iostream>

using namespace apex;
using namespace apex::stimulus;

namespace apex { namespace stimulus { namespace details
{

  void f_ThrowNotEnoughOutput( const QString& ac_sID )
  {
    throw( ApexStringException( "Connection: " + ac_sID + " does not have enough outputchannels" ) );
  }

  void f_ThrowNotEnoughInput( const QString& ac_sID )
  {
    throw( ApexStringException( "Connection: " + ac_sID + " does not have enough inputchannels" ) );
  }

  void f_ThrowNoneConnected( const QString& ac_sID )
  {
    throw( ApexStringException( "Connection: " + ac_sID + " is not connected at all" ) );
  }

  void f_ThrowNotExisting( const QString& ac_sID )
  {
    throw( ApexStringException( "Connection: " + ac_sID + " doesn't exist" ) );
  }

  void f_ThrowNotFromDevice( const QString& ac_sID )
  {
    throw( ApexStringException( "Connection: " + ac_sID + " doesn't belong to the device to connect" ) );
  }

  void f_ThrowNoDefaultPossible()
  {
    throw( ApexStringException( "Connection: default connections not possible when filter(s) present" ) );
  }

  void f_ReportNotAllInputConnected( StatusReporter& a_Logger, const QString& ac_sID )
  {
    a_Logger.addWarning( "ConnectionsFactory", "not every inputchannel of " + ac_sID + " is connected" );
  }

  void f_ReportNotAllOutputConnected( StatusReporter& a_Logger, const QString& ac_sID )
  {
    a_Logger.addWarning( "ConnectionsFactory", "not every outputchannel of " + ac_sID + " is connected" );
  }


    /**
      * ConnectionChecker
      *   checks whether item with given id is completely connected.
      *   This is the case if all channels are connected.
      ************************************************************** */
  class ConnectionChecker
  {
  public:
      /**
        * Constructor.
        * @param ac_Connections the vector of connections to check
        */
    ConnectionChecker( const tConnections& ac_Connections ) :
      m_Connections( ac_Connections )
    {}

      /**
        * destructor.
        */
    ~ConnectionChecker()
    {}

      /**
        * Used to see which channels are connected.
        */
    typedef std::map<unsigned, bool>  m_tConnected;
    typedef m_tConnected::iterator    m_tConnectedIt;

      /**
        * Sets all items in the map to false.
        * @param ac_nItems the number of items to check
        */
    void mp_InitCheckMap( const unsigned ac_nItems )
    {
      m_Map.clear();
      for( unsigned i = 0 ; i < ac_nItems ; ++i )
        m_Map[ i ] = false;
    }

      /**
        * Result for checking methods.
        */
    enum mt_eConnectState
    {
      mc_eNoneConnected,
      mc_eSomeConnected,
      mc_eAllConnected
    };

      /**
        * See if any are connected.
        * This is the case if at least one of the channels is connected.
        * @return false if not completely connected
        */
    mt_eConnectState mf_eCheck() const
    {
      unsigned nConnected = 0;
      const m_tConnected::size_type nToCheck = m_Map.size();
      for( m_tConnected::size_type i = 0 ; i < nToCheck ; ++i )
        if( m_Map[ i ] )
          ++nConnected;
      return nConnected == nToCheck ? mc_eAllConnected : ( nConnected == 0 ? mc_eNoneConnected : mc_eSomeConnected );
    }

      /**
        * Check if all "to" channels on the element with
        * the given ID have a connection assigned to them.
        * @param ac_sToID the ID to check
        * @param ac_bCheckAll true to use mf_bAllOk(), else mf_bAnyOk()
        * @return true if fully connected
        */
    mt_eConnectState mp_eCheckTo( const QString& ac_sToID )
    {
      for( tConnections::size_type i = 0  ; i < m_Connections.size() ; ++i )
      {
        const tConnection& cur = m_Connections[ i ];
        if( cur.m_nToChannel != (int) streamapp::sc_nInfinite )
          if( cur.m_sToID == ac_sToID )
            m_Map[ cur.m_nToChannel ] = true;
      }
      return mf_eCheck();
    }

      /**
        * Check if all "from" channels on the element with
        * the given ID have a connection assigned to them.
        * @param ac_sToID the ID to check
        * @param ac_bCheckAll true to use mf_bAllOk(), else mf_bAnyOk()
        * @return true if fully connected
        */
    mt_eConnectState mp_eCheckFrom( const QString& ac_sFromID )
    {
      for( tConnections::size_type i = 0  ; i < m_Connections.size() ; ++i )
      {
        const tConnection& cur = m_Connections[ i ];
        if( cur.m_nFromChannel != (int) streamapp::sc_nInfinite )
          if( cur.m_sFromID == ac_sFromID )
            m_Map[ cur.m_nFromChannel ] = true;
      }
      return mf_eCheck();
    }

  private:
    mutable m_tConnected  m_Map;
    const tConnections&   m_Connections;
  };

}}}

ConnectionRunDelegateCreator::ConnectionRunDelegateCreator( tConnectionsMap& a_VectorToFill, const tDeviceMap& a_Devs, const tFilterMap& a_Filters, const tDataBlockMap& a_DBlocks ) :
  m_Devices( a_Devs ),
  m_Filters( a_Filters ),
  m_DBlocks( a_DBlocks ),
  m_Connections( a_VectorToFill )
{
}

bool ConnectionRunDelegateCreator::AddConnection (const data::ConnectionData& data)
{
    try {
        if (data.matchType()==data::MATCH_NAME) {
            if (data.fromId() != "_ALL_")
                mp_AddConnection (tConnection (data));
            else {
                bool first=true;
                for (tDataBlockMapCIt it = m_DBlocks.begin(); it != m_DBlocks.end(); ++it) {
                    tConnection temp (data);
                    temp.m_sFromID = it.key();
                    try {
                        mp_AddConnection (temp);
                    } catch (ApexStringException &e) {
                        if (first)
                            log().addWarning("Connections",
                                    tr("While connecting all datablocks:"));
                        first=false;
                        log().addWarning("Connections", e.what());
                    }
                }
            }
        } else if ( data.matchType()== data::MATCH_REGEXP
                    || data.matchType()== data::MATCH_WILDCARD) {

            QRegExp re(data.fromId());
            if (data.matchType()== data::MATCH_WILDCARD)
                re.setPatternSyntax(QRegExp::Wildcard);
                        
            for (tDataBlockMapCIt it = m_DBlocks.begin(); it != m_DBlocks.end(); ++it) {

                if (re.exactMatch(it.key())) {
                    tConnection temp (data);
                    temp.m_sFromID = it.key();
                
                    try {
                        mp_AddConnection (temp);
                    } catch (ApexStringException &e) {
                        log().addWarning("Connections",
                            tr("While connecting datablock %1, matched by %2")
                                            .arg(it.key()).arg(data.fromId()));
                                            log().addWarning("Connections", e.what());
                    }

                }
            }

        
        } else {
            
                qFatal("Invalid match type");
        }
    } catch (ApexStringException& e) {
        log().addError ("ConnectionsFactory", e.what());
        return false;
    }
    return true;
}

bool ConnectionRunDelegateCreator::mp_bMakeDefaultConnections()
{
  try
  {
    if( m_Filters.size() > 0 )
      details::f_ThrowNoDefaultPossible();

    for( tDataBlockMapCIt it = m_DBlocks.begin() ; it != m_DBlocks.end() ; ++it )
    {
      tConnection Cur;
      Cur.m_sFromID = it.key();
      Cur.m_sToID = it.value()->GetDevice();
//      qDebug("Making connection from %s to %s", qPrintable (Cur.m_sFromID), qPrintable (Cur.m_sToID));

      const unsigned nChannels = it.value()->GetParameters().nbChannels();

      for( unsigned i = 0 ; i < nChannels ; ++i )
      {
        Cur.m_nFromChannel = i;
        Cur.m_nToChannel = i;
        mp_AddConnection( Cur );
      }
    }
  }
  catch( ApexStringException& e )
  {
      log().addError( "ConnectionsFactory", e.what() );
    return false;
  }
  return true;
}

const tConnection& ConnectionRunDelegateCreator::mf_IsValid( const tConnection& ac_Connection ) const
{
    //search from datablock/filter
  bool bFound = false;
  QString sDevice;

  tDataBlockMapCIt itF = m_DBlocks.find( ac_Connection.m_sFromID );
  if( itF != m_DBlocks.end() )
  {
    bFound = true;
    sDevice = itF.value()->GetDevice();
    if( ac_Connection.m_nFromChannel != (int) streamapp::sc_nInfinite )
        if( ac_Connection.m_nFromChannel >= (int) itF.value()->GetParameters().nbChannels())
        details::f_ThrowNotEnoughOutput( ac_Connection.m_sFromID );
  }

  tFilterMapCIt itFi;
  if( !bFound )
  {
    itFi = m_Filters.find( ac_Connection.m_sFromID );
    if( itFi != m_Filters.end() )
    {
      bFound = true;
      sDevice = itFi.value()->GetDevice();
      if( ac_Connection.m_nFromChannel != streamapp::sc_nInfinite )
          if( ac_Connection.m_nFromChannel >= (int) itFi.value()->GetParameters().numberOfChannels() )
          details::f_ThrowNotEnoughOutput( ac_Connection.m_sFromID );
    }
  }

  if( !bFound )
    details::f_ThrowNotExisting( ac_Connection.m_sFromID );

    //search to filter/device
  bFound = false;

  tDeviceMapCIt itD = m_Devices.find( ac_Connection.m_sToID );
  if( itD != m_Devices.end() )
  {
    bFound = true;

    if( itD.key() != sDevice )
            throw (ApexConnectionBetweenDifferentDevicesException("Attempted connection between " + sDevice + " and " + itD.key() + ", ignoring"));

    if( ac_Connection.m_nToChannel != streamapp::sc_nInfinite )
                  if( ac_Connection.m_nToChannel >= itD.value()->GetParameters().numberOfChannels() )
              details::f_ThrowNotEnoughInput( ac_Connection.m_sToID );
  }

  if( !bFound )
  {
    tFilterMapCIt itFi = m_Filters.find( ac_Connection.m_sToID );
    if( itFi != m_Filters.end() )
    {
      bFound = true;

      if( itFi.value()->GetDevice() != sDevice )
              throw (ApexConnectionBetweenDifferentDevicesException("Attempted connection between " + sDevice + " and " + itFi.value()->GetDevice() + ", ignoring"));

      if( ac_Connection.m_nToChannel != streamapp::sc_nInfinite )
        if( ac_Connection.m_nToChannel >= itFi.value()->GetParameters().numberOfChannels() )
          details::f_ThrowNotEnoughInput( ac_Connection.m_sToID );
    }
  }

  if( !bFound )
    details::f_ThrowNotExisting( ac_Connection.m_sToID );

  return ac_Connection;
}

void ConnectionRunDelegateCreator::mp_AddConnection( const tConnection& ac_Connection )
{
#ifdef PRINTCONNECTIONS
  qDebug("Adding connection from " + ac_Connection.m_sFromID + " to " + ac_Connection.m_sToID );
#endif

  tDataBlockMapCIt itD = m_DBlocks.find( ac_Connection.m_sFromID );
  if( itD != m_DBlocks.end() )
  {
    const QString& c_sDev( itD.value()->GetDevice() );
    try
    {
      m_Connections[ c_sDev ].push_back( mf_IsValid( ac_Connection ) );
    }
    catch( ApexConnectionBetweenDifferentDevicesException& e )
    {
        log().addWarning( "ConnectionsFactory", e.what() );
    }
    return;
  }

  tFilterMapCIt itF = m_Filters.find( ac_Connection.m_sFromID );
  if( itF != m_Filters.end() )
  {
    const QString& c_sDev( itF.value()->GetDevice() );
    try
    {
      m_Connections[ c_sDev ].push_back( mf_IsValid( ac_Connection ) );
    }
    catch( ApexConnectionBetweenDifferentDevicesException& e )
    {
        log().addWarning( "ConnectionsFactory", e.what() );
    }
    return;
  }
}

void ConnectionRunDelegateCreator::mf_ReportUnconnectedItems()
{
  for( tConnectionsMapCIt itDev = m_Connections.begin() ; itDev != m_Connections.end() ; ++itDev )
  {
    details::ConnectionChecker m_Checker( itDev.value() );
    const QString mc_sDevice( itDev.key() );

      //to device
    QString sToID( itDev.key() );
    unsigned nToConnect = (m_Devices.find( mc_sDevice ).value()->GetParameters().numberOfChannels());
    m_Checker.mp_InitCheckMap( nToConnect );
    if( m_Checker.mp_eCheckTo( sToID ) != details::ConnectionChecker::mc_eAllConnected )
      details::f_ReportNotAllInputConnected( log(), sToID );

      //to/from filter
    for( tFilterMapCIt it = m_Filters.begin() ; it != m_Filters.end() ; ++it )
    {
      sToID = it.key();
      nToConnect = it.value()->GetParameters().numberOfChannels();
      m_Checker.mp_InitCheckMap( nToConnect );
      if( !it.value()->GetParameters().hasParameter( "generator" ) )
      {
        if( m_Checker.mp_eCheckTo( sToID ) != details::ConnectionChecker::mc_eAllConnected )
          details::f_ReportNotAllInputConnected( log(), sToID );
      }
      if( m_Checker.mp_eCheckFrom( sToID ) != details::ConnectionChecker::mc_eAllConnected )
        details::f_ReportNotAllOutputConnected( log(), sToID );
    }

      //from datablock
    for( tDataBlockMapCIt it = m_DBlocks.begin() ; it != m_DBlocks.end() ; ++it )
    {
      sToID = it.key();
      nToConnect = it.value()->GetParameters().nbChannels();
      m_Checker.mp_InitCheckMap( nToConnect );
      details::ConnectionChecker::mt_eConnectState eRes = m_Checker.mp_eCheckFrom( sToID );
      /*if( eRes == details::ConnectionChecker::mc_eNoneConnected )
        details::f_ThrowNoneConnected( sToID );
      else*/ if( eRes == details::ConnectionChecker::mc_eSomeConnected )
        details::f_ReportNotAllOutputConnected( log(), sToID );
    }
  }
}
