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

#include "apexdata/device/devicesdata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/stimulus/stimulusparameters.h"

#include "connection/connection.h"

#include "device/controldevice.h"
#include "device/controllers.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "streamapp/appcore/threads/criticalsection.h"
#include "streamapp/appcore/threads/thread.h"
#include "streamapp/appcore/threads/waitableobject.h"

#include "streamapp/utils/vectorutils.h"

#include "wavstimulus/wavdevice.h"

#include "apexcontrol.h"
#include "apexmodule.h"
#include "datablock.h"
#include "filter.h"
#include "filtertypes.h"
#include "playmatrix.h"
#include "stimulus.h"
#include "stimulusoutput.h"

#include <QMessageBox>

#include <QtGlobal>

//from libdata
using namespace apex;
using namespace stimulus;

namespace
{
    //get mode
  StimulusOutput::mt_eMode f_eInitMode( const tDeviceMap& ac_pDevices )
  {
    tDeviceMap::size_type s = ac_pDevices.size();
    if( s == 1 )
    {
      if( ac_pDevices.begin().value()->CanSequence() )
        return StimulusOutput::singlesequence;
      else
        return StimulusOutput::normal; //or throw??
    }
    else
    {
      for( tDeviceMapCIt i = ac_pDevices.begin() ; i != ac_pDevices.end() ; ++i )
        if( !i.value()->CanSequence() )
          throw ApexStringException( "StimulusOutput: all devices must be able to load a sequence for this mode" );
    }
    return StimulusOutput::onlysequence;
  }

    //get offline
  bool f_bIsOffLine( const tDeviceMap& ac_Devices )
  {
    for( tDeviceMapCIt i = ac_Devices.begin() ; i != ac_Devices.end() ; ++i )
      if( i.value()->IsOffLine() )
        return true;
    return false;
  }
}

namespace apex
{

  class DeviceCheckThread : public appcore::IThread
  {
  public:
    DeviceCheckThread( const tDeviceMap& ac_DevicesToWaitFor ) :
        appcore::IThread( "DeviceCheckThread" ),
      mv_bFired( false ),
      mv_bChecking( false ),
      m_Devs( ac_DevicesToWaitFor )
    {}

    ~DeviceCheckThread()
    {}

    void mp_Run()
    {
      for( ;; )
      {
        mf_bWait( 100 );

        if( mf_bThreadShouldStop() )
          return;

        mc_Lock.mf_Enter();

        if( !mv_bFired && mv_bChecking )
        {
          /*tDeviceMapCIt it = m_Devs.begin();
          while( it != m_Devs.end() )
          {
            (*it).second->AllDone();
            ++it;
          }*/

            // end in reverse order of starting
          tDeviceMapCIt it = m_Devs.end();
          while( it != m_Devs.begin() )
          {
            --it;
            it.value()->AllDone();
          }

          mc_Waiter.mp_SignalObject();

          mv_bFired = true;
          mv_bChecking = false;
        }

        mc_Lock.mf_Leave();

      }
    }

    void mp_StartCheck()
    {
      mc_Lock.mf_Enter();
      mv_bChecking = true;
      mc_Lock.mf_Leave();
    }

    void mp_Reset()
    {
      mc_Lock.mf_Enter();
      mv_bFired = false;
      mc_Lock.mf_Leave();
    }

    const appcore::WaitableObject& mf_GetWaiter()
    { return mc_Waiter; }

  private:
    bool mv_bFired;
    bool mv_bChecking;
    const tDeviceMap& m_Devs;
    const appcore::WaitableObject mc_Waiter;
    const appcore::CriticalSection mc_Lock;
  };

}

StimulusOutput::StimulusOutput(ExperimentRunDelegate& p_rd) :
        ApexModule(p_rd),
  mc_eMode( f_eInitMode(m_rd.GetDevices() ) ),
  mc_bOffLine( f_bIsOffLine(m_rd.GetDevices() ) ),//instead of keeping everything in the parser where it doesn't belong
  m_pDevices(m_rd.GetDevices() ),
  m_pFilters(m_rd.GetFilters() ),
  m_DataBlocks(m_rd.GetDatablocks() ),
  m_pCurStim( 0 ),
  m_pCurDev( 0 ),
  m_pMaster( 0 ),
  mc_pWaitThread( new DeviceCheckThread(m_rd.GetDevices() ) ),
  m_bThreadRunning(true),
  m_pControllers(m_rd.GetControllers() )
{
  if( mc_eMode == singlesequence )
    m_pCurDev = m_pDevices.begin().value();
  SetMaster(m_rd.GetData().devicesData()->masterDevice() );

  LoadFilters();
  ConnectFilters();

    ResetParams();

    //start thread
  mc_pWaitThread->mp_Start( appcore::IThread::priority_verylow );
}

StimulusOutput::~StimulusOutput( )
{
  UnLoadStimulus();
  CloseDevices();
  delete mc_pWaitThread;
    //these are copies, not created through factory, so delete them
  for( tDeviceMapCIt i = m_OffLineDevices.begin() ; i != m_OffLineDevices.end() ; ++i )
    delete i.value();
}

void StimulusOutput::CloseDevices() {
    if (m_bThreadRunning) {
        mc_pWaitThread->mp_Stop( 2000 );
        for( tDeviceMapCIt i = m_pDevices.begin() ;
             i != m_pDevices.end() ; ++i )
            if (i.value())
                i.value()->Finish();
    }
    m_bThreadRunning = false;
}

void StimulusOutput::SetMaster( const QString& ac_sID )
{
  if( !ac_sID.isEmpty() )
  {
    m_pMaster = m_rd.GetDevice( ac_sID );
    Q_ASSERT( m_pMaster );
  }
}

void StimulusOutput::StopDevices()
{
  if( mc_eMode == singlesequence )
  {
    m_pCurDev->StopAll();
  }
  else
  {
    tDeviceMapCIt it = m_pDevices.begin();
    while( it != m_pDevices.end() )
    {
      OutputDevice* p = it.value();
      p->StopAll();
      ++it;
    }
  }
}

void StimulusOutput::ResumeDevices()
{
  if( mc_eMode == singlesequence )
  {
    m_pCurDev->PlayAll();
  }
  else
  {
    tDeviceMapCIt it = m_pDevices.begin();
    while( it != m_pDevices.end() )
    {
      OutputDevice* p = it.value();
      if( p!= m_pMaster )
        p->PlayAll();
      ++it;
    }
    if( m_pMaster )
      m_pMaster->PlayAll();
  }
}

void StimulusOutput::PlayUntilDone()
{
    //play all in order
  tDeviceMapCIt it = m_pDevices.begin();
  while( it != m_pDevices.end() )
  {
    OutputDevice* p = it.value();
    if( p!= m_pMaster )
      p->PlayAll();
    ++it;
  }
  if( m_pMaster )
    m_pMaster->PlayAll();

    //wait for eof and remove datablocks
  it = m_pDevices.begin();
  while( it != m_pDevices.end() )
  {
    OutputDevice* p = it.value();
    p->AllDone();
    p->RemoveAll();
    ++it;
  }
}

void StimulusOutput::PlayAndWaitInThread()
{
  //start playing all in order
  tDeviceMapCIt it = m_pDevices.begin();
  while( it != m_pDevices.end() )
  {
      OutputDevice* p = it.value();
    if( p!= m_pMaster )
      p->PlayAll();
    ++it;
  }

  qCDebug(APEX_RS, "Calling PlayAndWaitInThread at time: %s", qPrintable(QString::number(QDateTime::currentMSecsSinceEpoch())));

  if( m_pMaster )
    m_pMaster->PlayAll();

  //signal thread to start checking
  mc_pWaitThread->mp_Reset();
  mc_pWaitThread->mp_StartCheck();
}

const appcore::WaitableObject& StimulusOutput::GetStimulusEnd()
{
  return mc_pWaitThread->mf_GetWaiter();
}

void StimulusOutput::PlayStimulus()
{
  if( !m_pCurStim )
    return;

  qCInfo(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg( metaObject()->className(), "Playing Stimulus " + m_pCurStim->GetID() )));

  const PlayMatrix* pCurMat = m_pCurStim->GetPlayMatrix();
  if( mc_eMode == normal && pCurMat )
  {
     /*const unsigned nSeqs = pCurMat->mf_nGetBufferSize();
     for( unsigned i = 0 ; i < nSeqs ; ++i )
     {
         //load one column of sims; first one is loaded in LoadStimulus
       if( i > 0 )
       {
         LoadOneSim( pCurMat, i );
         ConnectDataBlocks();
       }

         //play sims
       PlayAndWaitInThread();
     }*/
    Q_ASSERT( 0 );
  }
  else
  {
    PlayAndWaitInThread();
  }
}


void StimulusOutput::LoadStimulus( const QString& ac_Stimulus, const bool p_restoreParams )
{
    Stimulus* s = m_rd.GetStimulus( ac_Stimulus );
    LoadStimulus(s,p_restoreParams);
}

void StimulusOutput::LoadStimulus( Stimulus* ac_Stimulus, const bool p_restoreParams )
{
#ifdef SHOWSLOTS
  qCDebug(APEX_RS, "SLOT StimulusOutput::LoadStimulus " + ac_Stimulus);
#endif
  if( !m_pCurStim )
  {
    m_pCurStim = ac_Stimulus;

   if (p_restoreParams)
        ResetParams();
    HandleParams();


      //load datablocks
    const PlayMatrix* pCurMat = m_pCurStim->GetPlayMatrix();

    if( pCurMat  && pCurMat->mf_nGetBufferSize() > 0 && pCurMat->mf_nGetChannelCount() > 0)
    {
      if( mc_eMode == singlesequence || mc_eMode == onlysequence )  //FIXME make enum or'able
        LoadAll( pCurMat );
      else if( mc_eMode == normal )
        LoadOneSim( pCurMat, 0 );

        //connect IO
      ConnectDataBlocks();
    }

    PrepareClients();

      //do offline calculations and add result to online device
//     if( mc_bOffLine )
//       DoOffLine();
  }
  else
  {
    qCDebug(APEX_RS, "StimulusOutput::LoadStimulus called before unloading!" );
    QMessageBox::warning(0, "error", "StimulusOutput::LoadStimulus called before unloading!", "OK");
//    throw( 0 ); //leave this here to enforce proper control behaviour!
  }
}

  //this will unload the whole
void StimulusOutput::UnLoadStimulus()
{
  if( m_pCurStim )
  {
    if( mc_eMode == singlesequence )
    {
      m_pCurDev->RemoveAll();
      m_pCurStim = 0;
    }
    else
    {
      tDeviceMapCIt it = m_pDevices.begin();
      while( it != m_pDevices.end() )
      {
          OutputDevice* p = it.value();
        if( p!= m_pMaster )
          p->RemoveAll();
        ++it;
      }
      if( m_pMaster )
        m_pMaster->RemoveAll();
      m_pCurStim = 0;
    }
  }
}

  //!currently only wavdevice connections have effect
void StimulusOutput::ConnectDataBlocks()
{
//  const ExperimentData& exp = ApexControl::Get().GetCurrentExperiment();
  const ExperimentRunDelegate&      d = /*ApexControl::Get().GetCurrentExperimentRunDelegate()*/m_rd;
  const tConnectionsMap&            c = d.GetConnections();
  const tQStringVectorMap& curDBlcoks = m_pCurStim->GetDeviceDataBlocks();

  for( tConnectionsMapCIt it = c.begin() ; it != c.end() ; ++it )
  {
    const tConnections& Cur = it.value();

    OutputDevice* pDev = m_rd.GetDevice(it.key());
    const tQStringVector& curDB = ( *curDBlcoks.find( pDev->GetID() ) ).second;

    if( pDev->IsOffLine() )
      pDev = m_OffLineDevices.value(it.key());

    tConnections::size_type c_nItems = Cur.size();
    for( tConnections::size_type i = 0 ; i < c_nItems ; ++i )
    {
        //add it if it's from a datablock for this device
      const tConnection& cur = Cur[ i ];
      if( utils::f_bHasElement( curDB, cur.m_sFromID ) )
        pDev->AddConnection( cur );
    }
  }
}

void StimulusOutput::ConnectFilters()   //fixme first loop filters, then connections?
{
//  const ExperimentData& exp = ApexControl::Get().GetCurrentExperiment();
  const ExperimentRunDelegate&      d = /*ApexControl::Get().GetCurrentExperimentRunDelegate()*/m_rd;
  const tConnectionsMap&            c = d.GetConnections();

  for( tConnectionsMapCIt it = c.begin() ; it != c.end() ; ++it )
  {
    const tConnections& Cur = it.value();
    OutputDevice* pDev = m_rd.GetDevice(it.key());

    if( pDev->IsOffLine() )
      pDev = m_OffLineDevices.value(it.key());

    tConnections::size_type c_nItems = Cur.size();
    for( tConnections::size_type i = 0 ; i < c_nItems ; ++i )
    {
        //add it if it's from/to a filter or from filter to device, for this device only
        //FIXME make everything per device? eg <device> <dblocks/> <filters/> </device>
      const tConnection& cur = Cur[ i ];
      Filter* pF = m_rd.GetFilter( cur.m_sFromID );
      Filter* pT = m_rd.GetFilter( cur.m_sToID );
      if( pF &&  pT )
        pDev->AddConnection( cur );
      else if( pF && cur.m_sToID == pDev->GetID() )
        pDev->AddConnection( cur );
    }
  }
}

void StimulusOutput::LoadAll( const PlayMatrix* ac_pMat )
{
    //split up the matrix in submatrices per device
    //and send them out
  tDeviceMapCIt itB = m_pDevices.begin();
  tDeviceMapCIt itE = m_pDevices.end();
  for( ; itB != itE ; ++itB )
  {

//#define PRINTPLAYMATRIX
#ifdef PRINTPLAYMATRIX
      PlayMatrixCreator::sf_DoDisplay( ac_pMat );
#endif

    PlayMatrix* toPlay = PlayMatrixCreator::sf_pCreateSubMatrix( ac_pMat,
            m_DataBlocks, itB.key() );

#ifdef PRINTPLAYMATRIX
    qCDebug(APEX_RS, "After sf_pCreateSubMatrix, for device %s",
          qPrintable(itB.key()));
    PlayMatrixCreator::sf_DoDisplay( toPlay );
#endif

    OutputDevice* p = itB.value();
    if( p->IsOffLine() )
      p = m_OffLineDevices.value( itB.key() );
    p->SetSequence( PlayMatrixCreator::sf_pConvert( toPlay, m_DataBlocks ) );
    delete toPlay;
  }
}

void StimulusOutput::LoadFilters()
{
  tFilterMap::const_iterator it = m_pFilters.begin();
  while( it != m_pFilters.end() )
  {
    const QString& sOutput = it.value()->GetDevice();
    tDeviceMapCIt itDev = m_pDevices.find( sOutput );
    Q_ASSERT( itDev != m_pDevices.end() );

    OutputDevice* p = itDev.value();
    if( p->IsOffLine() )                                 //switch to offline device, unless filter is contineuous
      //if( !(*it).second->HasParameter( "continuous" ) )
        p = m_OffLineDevices.value( sOutput );
    Q_ASSERT( p );

    p->AddFilter( *(it.value()) );
    ++it;
  }
}

void StimulusOutput::LoadOneSim( const PlayMatrix* a_cpMat, const unsigned ac_nSim )
{
  const unsigned nSims = a_cpMat->mf_nGetChannelCount();

  for( unsigned j = 0 ; j < nSims ; ++j )
  {
    const QString& sDBlock = a_cpMat->operator() ( j , ac_nSim );
    //ignore empty matrix space
    if( !sDBlock.isEmpty() )
    {
//      const DataBlock*  pDBlock = ApexControl::Get().GetDatablock( sDBlock );
        const DataBlock*  pDBlock = m_rd.GetDatablock( sDBlock );
      Q_ASSERT( pDBlock );
      const QString&    sDevice = pDBlock->GetDevice();
      tDeviceMapCIt it = m_pDevices.find( sDevice );
      Q_ASSERT( it != m_pDevices.end() && "unknown device" );
      it.value()->AddInput( *pDBlock );
    }
  }
}

// void StimulusOutput::DoOffLine()
// {
//   for( tDeviceMapCIt i = m_OffLineDevices.begin() ; i != m_OffLineDevices.end() ; ++i )
//   {
//     DataBlock* p = i.value()->CreateOffLine();          //offline
//     OutputDevice* pDev = m_pDevices.value( i.key() );  //online
//     pDev->AddInput( *p );
//     if( pDev->CanConnect() )
//     {
//       tConnection cur;
//       cur.m_sFromID = p->GetID();
//       cur.m_sToID = pDev->GetID();
//       const unsigned nChannels = p->GetParameters().m_nChannels;
//
//       for( unsigned i = 0 ; i < nChannels ; ++i )
//       {
//         cur.m_nFromChannel = i;
//         cur.m_nToChannel = i;
//         pDev->AddConnection( cur );
//       }
//     }
//     delete p;
//   }
// }

void StimulusOutput::RestoreParams(  ) {
     tDeviceMapCIt itB = m_pDevices.begin();
  tDeviceMapCIt itE = m_pDevices.end();
  for( ; itB != itE ; ++itB )
    itB.value()->RestoreParameters();

  /*tFilterMapCIt itBf = m_pFilters.begin();
  tFilterMapCIt itEf = m_pFilters.end();
  for( ; itBf != itEf ; ++itBf )
    (*itBf).second->RestoreParameters();*/
}

void StimulusOutput::ResetParams() {
     // ask each filter to reset its parameters
    for(tFilterMapCIt itBf = m_pFilters.begin();itBf != m_pFilters.end() ; ++itBf ) {
            itBf.value()->Reset();
        itBf.value()->RestoreParameters();
    }


    for (tDeviceMapCIt it=m_pDevices.begin(); it!=m_pDevices.end(); ++it) {
        it.value()->Reset();
        it.value()->RestoreParameters();
    }

    for (device::tControllerMap::const_iterator it=m_pControllers.begin(); it!=m_pControllers.end(); ++it) {
        it.value()->Reset();
        it.value()->RestoreParameters();
    }
}


void StimulusOutput::HandleParams()
{
    SendParametersToClients();
}


void StimulusOutput::SendParametersToClients() {
    // ask each filter to parse its parameters
    for(tFilterMapCIt itBf = m_pFilters.begin();itBf != m_pFilters.end() ; ++itBf ) {
        itBf.value()->SetParameters(m_rd.GetParameterManager());
    }


    for (tDeviceMapCIt it=m_pDevices.begin(); it!=m_pDevices.end(); ++it) {
        it.value()->SetParameters(*m_rd.GetParameterManager());
    }

    for (device::tControllerMap::const_iterator it=m_pControllers.begin(); it!=m_pControllers.end(); ++it) {
        it.value()->SetParameters(*m_rd.GetParameterManager());
    }

}

void StimulusOutput::PrepareClients() {
    // ask each filter to parse its parameters
    for(tFilterMapCIt itBf = m_pFilters.begin();itBf != m_pFilters.end() ; ++itBf ) {
        itBf.value()->Prepare();
    }


    for (tDeviceMapCIt it=m_pDevices.begin(); it!=m_pDevices.end(); ++it) {
        it.value()->Prepare();
    }

    for (device::tControllerMap::const_iterator it=m_pControllers.begin(); it!=m_pControllers.end(); ++it) {
        it.value()->Prepare();
    }
}

void StimulusOutput::setSilenceBeforeNextStimulus(double seconds)
{
    for (tDeviceMap::const_iterator dev = m_pDevices.begin();
         dev != m_pDevices.end();
         ++dev)
    {
        (*dev)->SetSilenceBefore(seconds);
    }
}

bool StimulusOutput::HandleParam( const QString& ac_sID, const QVariant& ac_sValue )
{
  qCDebug(APEX_RS, "StimulusOutput::HandleParam: %s to value %s", qPrintable (ac_sID), qPrintable (ac_sValue.toString()));

  data::Parameter name( m_rd.GetParameterManager()->parameter(ac_sID) );

  bool result=false;

  for(tFilterMapCIt itBf = m_pFilters.begin();itBf != m_pFilters.end() ; ++itBf ) {
        result |= itBf.value()->SetParameter(name, ac_sValue);
  }


  for (tDeviceMapCIt it=m_pDevices.begin(); it!=m_pDevices.end(); ++it) {
      result |= it.value()->SetParameter(name, ac_sValue);
  }

  for (device::tControllerMap::const_iterator it=m_pControllers.begin(); it!=m_pControllers.end(); ++it) {
      result |= it.value()->SetParameter(name, ac_sValue);
  }


  return result;  // FIXME
}

QString StimulusOutput::GetResultXML( ) const
{
  QString result = QString("<output>\n" );

    //query all devices
  for( tDeviceMapCIt it = m_pDevices.begin() ; it != m_pDevices.end() ; ++it )
    result += it.value()->GetResultXML();

  result += "</output>";

  return result;
}

QString StimulusOutput::GetEndXML() const
{
  QString result;

    //query all devices
  for( tDeviceMapCIt it = m_pDevices.begin() ; it != m_pDevices.end(); ++it )
    result += it.value()->GetEndXML();

  if( !result.isEmpty() )
    result = "<output>\n" + result + "</output>";

  return result;
}


