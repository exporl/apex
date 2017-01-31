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

#include "connection/connection.h"

#include "device/mixer/imixer.h"
#include "device/mixer/mixerfactory.h"
#include "device/mixer/mixerparameters.h"

#include "device/wavdevicedata.h"

#include "error/errorevent.h"

#include "filter/wavgenerator.h"

#include "bufferdropcallback.h"
#include "exceptions.h"
#include "mixercallback.h"
#include "seqstream.h"
#include "soundcarddialog.h"
#include "streamgenerator.h"
#include "wavdatablock.h"
#include "wavdevice.h"
#include "wavfilter.h"

#include <appcore/qt_utils.h>

#include <soundcard/bufferdropcheck.h>
#include <soundcard/soundcardfactory.h>

#include <utils/stringexception.h>
#include <utils/stringutils.h>

#include "services/mainconfigfileparser.h"
#include "stimulus/wav/soundcarddrivers.h"

#include <bufferedprocessor.h>
#include <connections.h>
#include <silentreader.h>
#include <streamappfactory.h>

#include <QDebug>
#include <QMessageBox>

#include <iostream>

using namespace apex;
using namespace apex::stimulus;
using namespace apex::XMLKeys;
using namespace streamapp;

//!local definitions
namespace
{

typedef std::pair<std::string, ApexSeqStream*> tSeqPair;
typedef tSeqMap::const_iterator tSeqCIt;
typedef tSeqMap::iterator tSeqIt;
typedef std::pair<std::string, StreamGenerator*> tGenPair;
typedef tGenMap::const_iterator tGenCIt;
typedef tGenMap::iterator tGenIt;

void sf_ShowCardError( const WavDeviceIO::mt_eOpenStatus ac_eErr, const std::string& ac_sAddError )
{
    QString sErr( WavDeviceIO::sf_sGetErrorString( ac_eErr ) );
    if ( !ac_sAddError.empty() )
        sErr += "\n\nadditional soundcard info: " + QString( ac_sAddError.c_str() );
    QMessageBox::critical (NULL, "Error", sErr);
}

void sf_CheckDriverType(const QString& ac_sDriver, const gt_eDeviceType ac_eType)
{
    const QString sType(stimulus::fSndEnumToString< QString >(ac_eType));
    if (sType == ac_sDriver)
        throw ApexStringException("Driver \'" + sType + "\' not supported on this platform");
}

}

namespace XMLize
{
    QString f_PutBetweenStartBrackets( const QString& ac_Src, const bool ac_bEOF = true )
    {
        if ( ac_bEOF )
            return QString( "<" + ac_Src + ">\n" );
        return QString( "<" + ac_Src + ">" );
    }

    QString f_PutBetweenEndBrackets( const QString& ac_Src, const bool ac_bEOF = true )
    {
        if ( ac_bEOF )
            return QString( "</" + ac_Src + ">\n" );
        return QString( "</" + ac_Src + ">" );
    }

    QString f_SingleTag( const QString& ac_Src )
    {
        return QString( "  <" + ac_Src + "/>" );
    }

    const QString sc_sEndTag( "\"" );
    const QString sc_sEqualTag( "=\"" );
    const QString sc_sClippedTag( "clipped channel" );
    const QString sc_sBufferDropTag( "buffer underruns" );
}


WavDevice::WavDevice( data::WavDeviceData* p_data):
        OutputDevice( p_data ),
        data( p_data ),
        mv_bOffLine( false ),
        m_SilenceBefore(0),
        m_IO(p_data)
{
    // Get driver/card name
     //try validate drivername from device with mainconfig's name entries
    QString driver(data->driverString());      //asio/portaudio
    QString card(data->cardName());
    bool    bRme = false;                           //set to true if it's an Rme, needed for getting mixer

    if (!driver.isEmpty() && !card.isEmpty()) {
        const stimulus::SndDriversMap* drvrs       = MainConfigFileParser::Get().GetSoundCardDrivers();
        stimulus::SndDriversMap::const_iterator it = drvrs->find(card);

       /* qDebug("Looking for %s in:", qPrintable(card));
        for( stimulus::SndDriversMap::const_iterator i = drvrs->begin(); i!=drvrs->end(); ++i) {
            qDebug("(%s, %s)", qPrintable( (*i).first));
        }*/

        if (it != drvrs->end()) {       // Card ID found?
            stimulus::t_SndDrivers*   drvr = (*it).second;
            stimulus::t_SndDriverInfo* drv = 0;

            for (unsigned i = 0 ; i < drvr->mf_nGetNumItems() ; ++i) {
                if (drvr->mf_GetItem(i)->m_sDevType == driver) {
                    drv = drvr->mf_GetItem(i);
                    break;
                }
            }

            if (!drv) {
                throw ApexStringException("Driver " + driver + " does not exist. Check your mainconfig.xml file.");
            } else {
                driver = drv->m_sDevType;
                if (card == "RME")
                    bRme = true;
                m_card = drv->m_sDriverName;
                m_driver = drv->m_sDevType;
            }
        } else {        // use given string as card name
            qDebug("Card name not found in apexconfig, using literal string");
            m_card=card;
            m_driver=driver;

            //throw ApexStringException(QString(tr("Card name %1 not found in mainconfig file or in the system.").arg(card)));
        }
    }


    //parse non-default params
    QString sDriver = m_driver;
    if (! sDriver.isEmpty()) {

        //see if we support this driver
#if !defined( WIN32 ) && !defined( __APPLE__ )
        sf_CheckDriverType(sDriver, ASIO);
#endif
#if defined( WIN32 )
        sf_CheckDriverType(sDriver, JACK);
#endif

    }


    std::string sErr;
    WavDeviceIO::mt_eOpenStatus nOk = m_IO.mp_eSetSoundcard( *p_data, true, sErr );
    if( nOk == WavDeviceIO::mc_eDefBufferSize )
    {
        //issue a warning; TODO get this to ErrorLogger somehow
      nOk = WavDeviceIO::mc_eOK;  //fix this else an error gets thrown below

      ErrorHandler::Get().addWarning("WavDevice",
                        tr("The buffersize specified in the experiment file could not be used.\nUsing the systems default buffersize."));


      // FIXME: try to open again with correct buffersize??
      //QMessageBox::warning( 0, "WavDevice", "Using default buffersize" );
    }
    else if ( nOk != WavDeviceIO::mc_eOK )
    {
        sf_ShowCardError( nOk, sErr );
        const QStringList list( m_IO.sf_saGetDriverNames() );
        QString sTemp( m_IO.sf_sGetDriverName( *p_data ) );

        //loop until cancelled or SoundCard set
        for ( ; ; )
        {
            SoundcardDialog dlg( sTemp, list );
            if ( dlg.exec() == QDialog::Accepted )
            {
                sTemp = dlg.GetSelectedItem();
                m_IO.sf_SetDriverName( sTemp, *p_data );
                nOk = m_IO.mp_eSetSoundcard( *p_data, true, sErr );
                if( nOk != WavDeviceIO::mc_eOK )
                {
                  if( nOk == WavDeviceIO::mc_eDefBufferSize )
                  {
                    nOk = WavDeviceIO::mc_eOK;
                    ErrorHandler::Get().addWarning("WavDevice",
                                      tr("The buffersize specified in the experiment file could not be used.\nUsing the systems default buffersize."));
                    //QMessageBox::warning( 0, "WavDevice", "Using default buffersize" );
                    // FIXME: try to open again with correct buffersize??
                    break;
                  }
                  sf_ShowCardError( nOk, sErr );
                  continue;
                }
                break;
            }
            else
            {
                break;
            }
        }
    }

    if ( nOk == WavDeviceIO::mc_eOK )
    {
        /*    DeviceParameters& par = ModParameters();
            par.SetParameter( "channels", QString::number( data->nChannels ) );
            par.SetParameter( "buffersize", QString::number( data->nBufSize ) );
            par.SetParameter( "samplerate", QString::number( data->nFs ) );*/

        // FIXME
    }
    else
    {
        throw ApexStringException("Couldn't open soundcard, check your parameters" );
    }

#ifdef SETMIXER
    if ( p_data->valueByType( "setcardmixer" ) == "true" )
    {

        //FIXME:
        QString card = m_card;
        bool bRme = false;
        if ( card == "RME" )
            bRme = true;

        //open a mixerdevice; since opening the device sets all gains,
        //we immedeately destroy it again as it is not needed anymore
        QString sType;
        if ( bRme )
            sType = "RME";


        data::MixerParameters mixparam;
        // TODO: set some values of mixparam
        device::IMixer* mix = MixerFactory::sf_pInstance()->mf_pCreateMixer( &mixparam );
        delete (mix);


    }
#endif

//     //check offline
//     const QString& sOffLine = data->GetValueByType( "mode" ).toString();
//     if ( sOffLine == "offline" )
//         mv_bOffLine = true;

    mv_bNeedsRestore = true;
}

WavDevice::~WavDevice()
{}

/*OutputDevice* WavDevice::CreateOffLineCopy()
{
    //    WavDevice* pRet = new WavDevice( mc_sID2, data->nChannels, data->nFs, data->nBufSize, Device::m_pParameters );
    WavDevice* pRet = new WavDevice( data );    // FIXME

    //do this here so the copy can immedeately call CreateOffLine
    pRet->mv_nOffLine = 0;
    pRet->mv_sOffLine = m_IO.sc_sOffLineOutputName;
    pRet->m_IO.m_pSoundcardWriterStream = (AudioFormatWriterStream*) StreamAppFactory::sf_pInstance()->mf_pWriterStream( m_IO.sc_sOffLineOutputName, data->numberOfChannels(), data->sampleRate(), data->bufferSize() );
    pRet->m_IO.m_pMainOutput = new ApexOutputCallback( pRet->m_IO.m_pSoundcardWriterStream, WavDeviceIO::sc_nMaxBusInputs );
    pRet->m_IO.m_pConnMan->mp_RegisterItem( OutputDevice::GetID().toAscii().data(), pRet->m_IO.m_pMainOutput );

    return pRet;
}*/

void WavDevice::AddFilter( Filter& ac_Filter )
{
    assert( ac_Filter.GetDevice() == OutputDevice::GetID() );
    const std::string sID( ac_Filter.GetID().toAscii().data() );

    WavFilter* pFilter = & dynamic_cast< WavFilter& >( ac_Filter );

    if ( !pFilter->mf_bIsRealFilter() )
    {
        //it's an inputstream
        StreamGenerator* pS = ((WavGenerator*) pFilter)->GetStreamGen();

        m_Generators.insert( tGenPair( sID, pS ) );
        if ( pS->mf_bContinuous() )
        {
            m_IO.mp_SetContinuous( true );
            if ( mv_bOffLine ) //cant't have this (solution is moving the cont filter to the online device in stimoutput)
                throw( ApexStringException( "WavDevice::AddFilter: offline mode is not supported with continuous filters" ) );
        }
        m_IO.mp_AddConnectItem( pS, sID, false );
    }
    else
    {
        IStreamProcessor* pP = ((WavFilter*) pFilter)->GetStrProc();
        m_IO.mp_AddConnectItem( new ApexProcessorCallback( pP, WavDeviceIO::sc_nMaxBusInputs ), sID );
    }
    if ( pFilter->mf_bWantsToKnowStreamLength() )
        m_InformFilters.push_back( pFilter );
}

void WavDevice::AddInput( const DataBlock& ac_DataBlock )
{
    assert( ac_DataBlock.GetDevice() == GetID() );
//#define PRINTWAVDEVICE
#ifdef PRINTWAVDEVICE

    const std::string sID( ac_DataBlock.GetID().toAscii().data() );
    std::cout << "WavDevice: AddInput " + sID << std::endl;
#endif

    PosAudioFormatStream* p = ((WavDataBlock*) &ac_DataBlock )->
            GetWavStream(data->blockSize(), data->sampleRate());
    ApexSeqStream* pS = new ApexSeqStream( p, true );

    mp_AddSeqStream( pS, ac_DataBlock.GetID() );
}

void WavDevice::mp_AddSeqStream( ApexSeqStream* pStream, const QString& ac_sID )
{
    const std::string sID( ac_sID.toAscii().data() );
    m_InputStreams.insert( tSeqPair( sID, pStream ) );
    m_IO.mp_AddConnectItem( pStream, sID, true );
}

bool WavDevice::AddConnection( const tConnection& ac_Connection )
{
    m_IO.mp_AddConnection( ac_Connection, ac_Connection.m_sToID == OutputDevice::GetID() );

//    qDebug("WavDevice::AddConnection");

    //keep as param if stored
    if ( !ac_Connection.m_sFromChannelID.isEmpty() || !ac_Connection.m_sToChannelID.isEmpty() )
        m_NamedConnections.push_back( ac_Connection );

    return true;
}

void WavDevice::SetSequence( const DataBlockMatrix* ac_pSequence )
{
    const unsigned nSeq = ac_pSequence->mf_nGetBufferSize();
    const unsigned nPar = ac_pSequence->mf_nGetChannelCount();

    //add..
    //qDebug("DatablockMatrix: ");
    for ( unsigned j = 0 ; j < nPar ; ++j )
    {
        for ( unsigned i = 0 ; i < nSeq ; ++i )
        {
            DataBlock* pCur = ac_pSequence->operator()( j, i );
            if ( pCur )
            {
                if ( !m_IO.m_pConnMan->mf_bIsRegistered( sq( pCur->GetID() ) ) )
                    AddInput( *pCur );
//                qDebug("%s", qPrintable(pCur->GetID()));
            }
//            else { qDebug("**"); }
        }
    }

    //see if we have to add an extra silence
    ApexSeqStream* pZeroPad = 0;
    if ( data->valueByType( gc_sPadZero).toUInt() )
    {
        //create inputstream with nZeroPads * buffersize samples to add after longest row
        const unsigned nZeroPads = data->valueByType( gc_sPadZero ).toUInt();
        SilentReader* p = new SilentReader( 1, data->sampleRate(), (unsigned long) (nZeroPads * data->blockSize()));
        PosAudioFormatStream* ps = new PosAudioFormatStream( p, data->blockSize(), true, true );
        pZeroPad = new ApexSeqStream( ps, true );
        //add it
        mp_AddSeqStream( pZeroPad, gc_sPadZero );
    }

    unsigned long startOffset = 0;          // the actual stimulus will begin after offset
    if (m_SilenceBefore)
    {
        startOffset=(unsigned long) m_SilenceBefore*data->sampleRate();

        qDebug() << "Adding extra silence of " + QString::number(m_SilenceBefore) + "s";

    }

    //this will contain the total length of the entire matrix
    unsigned long nLongestRowLength = 0;

    //setup sequencing per row
    for ( unsigned j = 0 ; j < nPar ; ++j )
    {
        for ( unsigned i = 0 ; i < nSeq ; ++i )
        {
            DataBlock* pCur = ac_pSequence->operator()( j, i );
            if ( pCur )
            {
                const QString& c_sCur( ac_pSequence->operator()( j, i )->GetID() );
                if ( !c_sCur.isEmpty() )
                {
                    const std::string sCur( sq( c_sCur ) );
                    tSeqCIt itCur = m_InputStreams.find( sCur );
                    if ( i == 0 )
                    {
                        assert( itCur != m_InputStreams.end() );
                        (*itCur).second->mp_SetStartAt( startOffset );  //first element always starts at position 0

                        if ( (*itCur).second->mf_lTotalSamples() > nLongestRowLength )
                            nLongestRowLength = (*itCur).second->mf_lTotalSamples();
                    }
                    else
                    {
                        DataBlock* d =0;
                        for (unsigned q=0; q<=j; ++q)
                        {
                            d = ac_pSequence->operator()( j-q, i - 1 );
                            if (d)
                            {
                                const QString qsPre = d->GetID();
                                const std::string sPre( sq(qsPre) );
                                tSeqCIt itPre = m_InputStreams.find( sPre );
                                if ( itCur != m_InputStreams.end() && itPre != m_InputStreams.end() )
                                {
                                    unsigned long nOff = (*itPre).second->mf_lTotalSamples();
                                    (*itCur).second->mp_SetStartAt( nOff ); //start right after the previous item

                                    if ( (*itCur).second->mf_lTotalSamples() > nLongestRowLength )
                                        nLongestRowLength = (*itCur).second->mf_lTotalSamples();
                                }
                                else
                                {
                                    assert( 0 && "cannot get length; no wavdatablock" );
                                }
                                break;
                            }
                        }
                        Q_ASSERT ( d && "Can't find any prevous datablock");
                    }

                }
            }
        }
    }

    //set stream length for filters needing it
    const tWavFilters::size_type nFilters = m_InformFilters.size();
    for ( tWavFilters::size_type i = 0 ; i < nFilters ; ++i )
        m_InformFilters[ i ]->mp_SetStreamLength( nLongestRowLength );

    //connect extra silence to soundcard output
    if ( pZeroPad )
    {
        //start as last
        pZeroPad->mp_SetStartAt( nLongestRowLength );
        tConnection con;
        con.m_nFromChannel = 0;
        con.m_nToChannel = 0;
        con.m_sFromID = gc_sPadZero;
        con.m_sToID = OutputDevice::GetID();
        AddConnection( con );
    }

    /*    if (m_SilenceBefore) {

        }*/

    delete ac_pSequence;
}

void WavDevice::RemoveAll()
{
      //[ stijn 29/11/2008 ] moved this before the loop that
      //deletes the inputs: else WavDeviceIO:m_pEofCheck
      //tries to access items that are already deleted..
    m_IO.mp_RemoveCheckedInputs();

    //get rid of inputstreams
    tSeqIt itB = m_InputStreams.begin();
    tSeqCIt itE = m_InputStreams.end();
    while ( itB != itE )
    {
        const std::string& sCur( (*itB).first );
        //remove connections
        m_IO.mp_RemoveConnection( sCur );
        mp_RemoveNamedConnection( QString( sCur.data() ) );
        //delete the stream
        ApexSeqStream* pCur = (*itB).second;
        delete pCur;
        ++itB;
    }
    m_InputStreams.clear();

    //pause non-continuous generators
    tGenIt itGB = m_Generators.begin();
    tGenCIt itGE = m_Generators.end();
    while ( itGB != itGE )
    {
        const std::string& sCur( (*itGB).first );
        StreamGenerator* pCur = (*itGB).second;
        if ( !pCur->mf_bContinuous() || m_bContinuousModeEnabled==false )
            m_IO.mp_PausePlay( sCur );
        ++itGB;
    }

#ifdef PRINTCONNECTIONS
    PrintVector( m_IO.m_pConnMan->mf_saGetRoutes() );
#endif
}

void WavDevice::PlayAll()
{
#ifdef PRINTWAVDEVICE
    std::cout << "WavDevice: PlayAll " << std::endl;
#endif
#ifdef PRINTCONNECTIONS
    PrintVector( m_IO.m_pConnMan->mf_saGetRoutes() );
#endif
    m_IO.mp_Start();
}

bool WavDevice::AllDone()
{
// qDebug("Wavdevice::AllDone()");
    m_IO.mf_WaitUntilDone();
    m_IO.mp_Stop();

#ifdef PRINTWAVDEVICE
    std::cout<< "WavDevice: AllDone "<<std::endl;
#endif

//  qDebug("Wavdevice::Leaving AllDone()");
    m_SilenceBefore=0;

    // check for clipping
    CheckClipping();

    return true;
}

void WavDevice::CheckClipping() {
    const streamapp::tUnsignedBoolMap& zork = m_IO.m_pSoundcardWriterStream->mf_GetClippedChannels();
    streamapp::tUnsignedBoolMapCIt itE = zork.end();
    for ( streamapp::tUnsignedBoolMapCIt it = zork.begin() ; it != itE ; ++it )
    {
        if ( (*it).second ) {
            xmlresults.append( XMLize::f_SingleTag( XMLize::sc_sClippedTag + XMLize::sc_sEqualTag + qn( (*it).first ) + XMLize::sc_sEndTag ) );     //<clipped channel ="0"/>

            QApplication::postEvent( &ApexControl::Get(),
                            new ErrorEvent(
                                    StatusItem

(StatusItem

::ERROR
, "WavDevice",
                                    "Soundcard output clipped")
                                        ) );
        }
    }
    xmlresults.append( XMLize::f_SingleTag( XMLize::sc_sBufferDropTag + XMLize::sc_sEqualTag + qn( m_IO.m_pSoundcardbufferDroppedCallback->mf_nGetNumDrops() ) + XMLize::sc_sEndTag ) );  //<buffer underruns="0"/>

    m_IO.m_pSoundcardWriterStream->mp_ResetClipped();
}

void WavDevice::StopAll()
{
    m_IO.mp_Stop();
    m_SilenceBefore=0;
}

void WavDevice::Finish()
{
    m_IO.mp_Finish();
    m_SilenceBefore=0;
}

bool WavDevice::HasParameter( const QString& ac_ParamID )
{
    if ( OutputDevice::HasParameter( ac_ParamID ) )
        return true;
    return mf_bIsNamedConnection( ac_ParamID );
}


bool WavDevice::mf_bIsNamedConnection( const QString& ac_sID ) const
{
    const tConnections::size_type c_nSize = m_NamedConnections.size();
    for ( tConnections::size_type i = 0 ; i < c_nSize ; ++i )
    {
        const tConnection& cur = m_NamedConnections[ i ];
        if ( QString::compare( cur.m_sFromChannelID, ac_sID ) == 0 )
            return true;
        else if ( QString::compare( cur.m_sToChannelID, ac_sID ) == 0 )
            return true;
    }
    return false;
}

void WavDevice::mp_RemoveNamedConnection( const QString& ac_sFromID )
{
    tConnections::iterator it = m_NamedConnections.begin();
    while ( it != m_NamedConnections.end() )
    {
        const tConnection& cur = *it;
        if ( QString::compare( cur.m_sFromID, ac_sFromID ) == 0 )
        {
            m_NamedConnections.erase( it );
            it = m_NamedConnections.begin();
        }
        else
            ++it;
    }
}

/*void WavDevice::SetParameters ( ParameterManager* pm )
    {
        IApexDevice::SetParameters(pm);

        // also set parameters for connections
        tParameterValueMap params = pm->GetParametersForOwner ( m_NamedConnections.GetID() );

        for ( tParameterValueMap::const_iterator it=params.begin(); it!=params.end(); ++it )
        {
            qDebug() << "WavDevice: Setting parameter " + it.key().GetType() + " to value " + it.value().toString();
            SetParameter ( it.key().GetType(), it.key().GetChannel(), it.value() );
        }
    }*/


bool WavDevice::SetParameter ( const QString& type, const int channel, const QVariant& value )
{
/*    qDebug(qPrintable("WavDevice::SetParameter " + type +
            "channel " + QString::number(channel) +
            " to value " + value.toString()));*/

    if (type=="gain")
    {
        if ( channel==-1 )
            m_IO.mp_SetOutputGain( value.toDouble() );
        else
            m_IO.mp_SetOutputGain( value.toDouble(), channel );
    }
   else if (type.startsWith("connection"))
    {
        
        // Get default parameter value:
/*        data::ParameterName p(
                            m_pParameters->GetParameterByType(type.mid(11,-1)));
        Q_ASSERT(!p.GetType().isEmpty());
        qDebug("Original parameter value: %d", p.GetDefaultValue().toInt());*/
        m_connection_param_cache[type]=value;
    } else {
        return false;
    }

    return true; // FIXME
}

/**
 * Read connections parameters from cache and change connections
 * appropriately. Cache is cleared afterwards.
 */
void WavDevice::SetConnectionParams() {

    // The type of a connection parameter is set to "connection-<ID>"
    //find the connection subject to change
    //TODO ConnectionManager::mp_Rewire


    for (QMap<QString,QVariant>::const_iterator it=m_connection_param_cache.begin();
            it!=m_connection_param_cache.end(); ++it) {

        QString type(it.key());
        QVariant value(it.value());


        QString connectionID(type.mid(11,-1));
        //qDebug(qPrintable("WavDevice::SetParameter: Setting parameter for connection with id " + connectionID));

        bool bRet = false;
        const tConnections::size_type c_nSize = m_NamedConnections.size();
        for ( tConnections::size_type i = 0 ; i < c_nSize ; ++i )
        {
            tConnection& cur = m_NamedConnections[ i ];
            if ( QString::compare( cur.m_sFromChannelID, connectionID ) == 0 )
            {
                const unsigned nNewFromChannel = (unsigned) value.toInt();

                qDebug("Reconnecting %s from channel %d to channel %d",
                       qPrintable(cur.m_sFromChannelID),
                                  cur.m_nFromChannel, nNewFromChannel);
                
                if ( nNewFromChannel != cur.m_nFromChannel ) //don't change if it's the same
                    m_IO.mp_RewireConnection( cur, nNewFromChannel, true );
                bRet = true;
            }
            else if ( QString::compare( cur.m_sToChannelID, connectionID ) == 0 )
            {
                const unsigned nNewToChannel = (unsigned) value.toInt();

                qDebug("Reconnecting %s from channel %d to channel %d",
                       qPrintable(cur.m_sToChannelID),
                                  cur.m_nToChannel, nNewToChannel);
                
                if ( nNewToChannel != cur.m_nToChannel )
                    m_IO.mp_RewireConnection( cur, nNewToChannel, false );
                bRet = true;
            }
        }


    }

      //now check if we can run
    if( !m_IO.mf_bAllInputsConnected() )
    {
      QApplication::postEvent( &ApexControl::Get(), new ErrorEvent( StatusItem

 (
          StatusItem

::ERROR
,
          "WavDevice",
          "Some datablocks scheduled to play are not connected."
        "Check your connections (that may be changed by a parameter)") ) );
    }

    m_connection_param_cache.clear();
}




//only soundcard output is checked for clipping, and this is *always* the first
//stream
QString WavDevice::GetResultXML() const
{
    QString res( "<device id=\"" + GetID() +"\">\n");

    res.append( xmlresults.join("\n"));
    xmlresults.clear();

    res.append( "\n</device>\n" );                                 //</wavdevice>

    return res;
}

bool WavDevice::GetInfo( const unsigned ac_nType, void* a_pInfo ) const
{
    if ( ac_nType != IApexDevice::mc_eClipping )
        return false;

    streamapp::tUnsignedBoolMap* p = (streamapp::tUnsignedBoolMap*) a_pInfo;

    //just copy all samples
    const streamapp::tUnsignedBoolMap& zork = m_IO.m_pSoundcardWriterStream->mf_GetClippedChannels();
    streamapp::tUnsignedBoolMapCIt itE = zork.end();
    for ( streamapp::tUnsignedBoolMapCIt it = zork.begin() ; it != itE ; ++it )
    {
        p->operator [] ( (*it).first ) = (*it).second;
    }

    m_IO.m_pSoundcardWriterStream->mp_ResetClipped();

    return true;
}

// DataBlock* WavDevice::CreateOffLine()
// {
//     //run
//     PlayAll();
// 
//     //close the outputfile by replacing it with a new one
//     ++mv_nOffLine;
//     std::string temp = toString( mv_nOffLine ) + m_IO.sc_sOffLineOutputName;
//     AudioFormatWriter* pRepl = StreamAppFactory::sf_pInstance()->mf_pWriter( temp, data->GetNumberOfChannels(), data->GetSampleRate() );
//     m_IO.m_pSoundcardWriterStream->mp_ReplaceWriter( pRepl, true );
// 
//     //create the datablock with the closed outputfile
//     data::DatablockData p;
//     p.m_nChannels = data->GetNumberOfChannels();
//     p.m_nLoops = 1;
//     p.m_dGain = 0;
//     p.SetID(GetID());
//     QUrl url( mv_sOffLine.data() );
//     WavDataBlock* pRet = new WavDataBlock(p, url, 0);
// 
//     //set filename to new name
//     mv_sOffLine = temp;
// 
//     return pRet;
// }

void WavDevice::Reset()
{
    m_IO.Reset();
}

void WavDevice::Prepare()
{
    SetConnectionParams();
    m_IO.Prepare();
}
