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

//#define DEBUGL34          for debugging

#include "./l34datablock.h"
#include "./l34device.h"

#include "device/l34devicedata.h"

#include "services/mainconfigfileparser.h"
#include "services/paths.h"
#include "services/pluginloader.h"

#include "apexnresocketinterface.h"
#include "apextools.h"
#include "exceptions.h"

#include <appcore/threads/thread.h>

#include <assert.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <QCoreApplication>
#include <QPluginLoader>
#include <QString>

#ifdef DEBUG
#include <iostream>
#endif

namespace apex
{

namespace stimulus
{

const QString csStatusError("error");
const QString csStatusConnectionError("fault");

// for the L34 type
const QString csStatusUnknown("<code>0");
const QString csStatusStreaming("<code>1");
const QString csStatusFinished("<code>2");
const QString csStatusSafetyError("<code>3");
const QString csStatusCommsError("<code>4");
const QString csStatusStopped("<code>5");
const QString csStatusWaiting("<code>6");
const QString csStatusUnderflow("<code>7");

/*        const QString csStatusUnknown("unknown");
const QString csStatusStreaming("streaming");
const QString csStatusFinished("finished");
const QString csStatusSafetyError("3");
const QString csStatusCommsError("4");
const QString csStatusStopped("stopped");
const QString csStatusWaiting("waiting");
const QString csStatusUnderflow("7");*/


// for the nic type

L34Device::L34Device( const QString& ac_sHost,
    data::L34DeviceData* ac_params) :
    OutputDevice( ac_params ),        // FIXME: move everything to DeviceParameters object and pass to Device instead of new
    m_pDevice(NULL),
    m_host(ac_sHost),
    m_bPlaying( false ),
    m_bReady(false),
    m_pMap(NULL),
    m_iTriggerType(ac_params->triggerType()),
    m_nDeviceNr(ac_params->deviceId()),
    m_nPowerupCount(ac_params->powerupCount()),
    m_bPowerupSent(false),
    m_stimulusLength(0),
    m_volume(ac_params->volume())
{
#ifdef L34_INTERFACE_C
#error OBSOLETE
#endif

    QString qsdevice(ac_params->deviceType());

    qsdevice += "-" + ac_params->implantType();
    qsdevice += "-%1";
    qsdevice = qsdevice.arg(m_nDeviceNr);

    qDebug("Initializing %s", qPrintable(qsdevice));

    // load plugin
    LoadPlugin();

    try {
        m_pDevice->connect(qsdevice);
    }

    catch (...) {
        throw ApexStringException( "L34 unknown error" );
    }

    // Get and show status
    m_pDevice->send("<status/>");
    QString status = m_pDevice->receive();

    if ( status.contains(csStatusConnectionError) ) { // || status.find("idle")==QString::npos) {
        m_pDevice->disconnect();
#ifdef DEBUGL34
        std::cout << status <<endl;
#endif
        OutputDevice::m_bError = true;
        throw ApexStringException(status);
    }

    //   m_pDevice->send( "<logging type=\"console\">on</logging>" );
    m_pDevice->send( "<timeout>10</timeout>" );
    m_pDevice->send( "<stop/>" );       // reset device

    OutputDevice::m_bError = false;
}

void L34Device::LoadPlugin()
{
    try {
        ApexNreSocketCreatorInterface *creator(PluginLoader::Get()
                .createPluginCreator<ApexNreSocketCreatorInterface>
                (QLatin1String("l34plugin")));
        m_pDevice.reset(creator->create(m_host,m_nDeviceNr));
    } catch (const std::exception &e) {
        throw PluginNotFoundException (QString::fromLocal8Bit(e.what()));
    }
}

L34Device::~L34Device() {
    if (m_pMap)
        delete m_pMap;

    m_pDevice->send( "<stop/>" ); // reset
}

//TODO
void L34Device::AddFilter( Filter&    /*ac_Filter*/       )
{
    Q_ASSERT(0 && "L34: filtering not implemented");
    throw(0);
}

void L34Device::Stop( const QString&  ac_sDataBlockID,
    const int       ac_nFlags        )
{
    Q_UNUSED (ac_sDataBlockID);
    Q_UNUSED (ac_nFlags);
    m_pDevice->send("<stop/>");

    m_bPlaying = false;
}

void L34Device::StopAll(  )
{
    m_pDevice->send("<stop/>");

    m_bPlaying = false;
}

bool L34Device::Done( const QString&  ac_sDataBlockID  )
{
    Q_UNUSED (ac_sDataBlockID);
    Q_ASSERT(0 && "L34Device::Done Not implemented");
    return false;
}

void L34Device::RemoveAll()
{
    StopAll();
}




void L34Device::SetSequence  ( const DataBlockMatrix* ac_pSequence )
{
    Q_CHECK_PTR(ac_pSequence);

    qDebug("Sending <stop/>");
    m_pDevice->send("<stop/>");

    m_stimulusLength=0;

    const unsigned nSeq = ac_pSequence->mf_nGetBufferSize();
    const unsigned nPar = ac_pSequence->mf_nGetChannelCount();

    if (nSeq==0 && nPar==0) {
        qDebug("L34:  not playing, playmatrix empty");
        m_bReady=false;
        return;
    }

    Q_ASSERT(nPar<=1);      // we don't support parallell datablocks yet

    bool inputsFound=false;
    //add..
    for( unsigned j = 0 ; j < nPar ; ++j ) {
        for( unsigned i = 0 ; i < nSeq ; ++i ) {
            //L34DataBlock* pCur = dynamic_cast<L34DataBlock*>( ac_pSequence->operator()( j, i ) );
            // not correct??
            L34DataBlock* pCur = (L34DataBlock*)( ac_pSequence->operator()( j, i ) );
            if( pCur ) {
                AddInput(*pCur);
                inputsFound=true;
            } else {
            }
        }
    }

    m_bReady=inputsFound;
}




void L34Device::showStatus()
{
#ifdef DEBUGL34
    m_pDevice->send("<status/>");
    QString status = m_pDevice->receive();

    qDebug(status.c_str() );
#endif
}


// CPP
void L34Device::AddInput( const DataBlock& ac_DataBlock )
{
    //send to device
    assert( ac_DataBlock.GetDevice() == GetID() );

    if (!m_bPowerupSent && m_nPowerupCount > 0)
        SendPowerup();

    const L34XMLData d(  ((L34DataBlock&)ac_DataBlock).GetMappedData(m_pMap, m_volume) );

    QString sendbuffer;     // we have to put everything in one string first,
    // so the XML parsing works. (everything we send must be valid XML)

    QTime time;
    time.restart();
    for (L34XMLData::const_iterator it = d.begin(); it!=d.end(); ++it) {
        sendbuffer +=  *it ;
    }

    qDebug("** Time for creating sendbuffer: %s", qPrintable(QString::number(time.elapsed())));

    time.restart();
#ifdef DEBUGL34
    std::cout << m_pDevice->send( sendbuffer ) << std::endl;
#else
    m_stimulusLength+=d.GetLength();
    m_pDevice->send( sendbuffer );
#endif
    qDebug("** Time in m_pDevice->send(): %u", time.elapsed());

    showStatus();
}

void L34Device::PlayAll( void )
{
    //! 0 = play and wait for end + remove all
    //! 1 = play
    //! 2 = manual + remove all

    if (! m_bReady) {
        qDebug("L34Device: not ready, not starting play");
        return;
    }

    if(  m_bPlaying == false ) {

        m_pDevice->send( "<end/>" );        // make sure the last frame is sent

        QString startcommand;
        switch (m_iTriggerType) {
        case data::TRIGGER_NONE:
            startcommand = "<start>immediate</start>" ;
            break;
        case data::TRIGGER_IN:
            startcommand =  "<start>external</start>" ;
            break;
        case data::TRIGGER_OUT:
            startcommand =  "<start>bilateral</start>" ;
            break;
        default:
            Q_ASSERT("Invalid trigger type");
            throw(0);
        }

        qDebug("Sending %s", qPrintable(startcommand));
        m_pDevice->send(startcommand);
        showStatus();

        m_pDevice->send("<status/>");
        QString status = m_pDevice->receive();

        if ( m_iTriggerType== data::TRIGGER_IN ) {
            bool wresult = waitUntil( csStatusWaiting, 5000 );
            m_bPlaying=true;
            if (!wresult) {
                m_bPlaying=false;
                m_pDevice->send("<stop/>");
                QMessageBox::critical(0, "Timeout",  QString("Timeout waiting for 'Waiting' status\nCheck the trigger switch on the POD and restart the experiment."), QMessageBox::Abort, QMessageBox::NoButton );
                //                                    throw ApexStringException(QString("Streaming timeout passed"));
                /*m_pDevice->disconnect();
                m_pDevice->connect("l34-1-cic3");
                m_bPlaying=false;
                m_pDevice->send("<stop/>");*/
            }

        } else {        // not trigger IN
            if (! status.contains(csStatusStreaming) && !waitUntil( csStatusStreaming, 5000 )) {
                m_bPlaying=false;
                m_pDevice->send("<stop/>");
                QMessageBox::critical(0, "Timeout",  QString("Timeout waiting for 'Streaming' status\nCheck your stimuli and the hardware."), QMessageBox::Abort, QMessageBox::NoButton );
                throw ApexStringException(QString("Streaming timeout passed"));
            } else if (! status.contains(csStatusStreaming) && ! status.contains(csStatusStopped)) {
                qDebug("Unexpected status of device %d after sending start command: %s", m_nDeviceNr, qPrintable (status));
            } else {
                m_bPlaying=true;
            }
        }

        m_timeSinceStart.restart();
    } else {
        Q_ASSERT("error: playall called while playing==false");
    }

    m_bReady=false;
    m_bPowerupSent=false;
}



/*  Polls the hardware-system until the requested state is observed.
*   Or until timeout ms are gone by
*   returns true if the requested state has been observed
*   false if timeout has passed by
*   timeout==0 -> no timeout
/
*/
//void waitUntil( ClientSocketPtr& client, const QString& STATE )
// TODO: add timeout
bool L34Device::waitUntil( const QString& STATE1,const QString& STATE2, int timeout=0 )
{
#ifdef DEBUGL34
    qDebug("L34-" + QString::number(m_nDeviceNr) + ": Wait until " + STATE1.c_str() + " or " + STATE2.c_str() + " timeout " + QString::number(timeout) );
#endif
    Q_ASSERT(m_pDevice.get());

    QTime timer;
    if (timeout) {
        timer.start();
    }


    QString data;
    data.reserve( 512 );
    m_pDevice->send( "<status/>" );
    data = m_pDevice->receive( );
#ifdef DEBUGL34
    qDebug(QString("Device %1: ").arg(m_nDeviceNr) + data.c_str());
#endif

    while ( ! data.contains( STATE1 ) && ! data.contains( STATE2 )) {
        if ( data.contains( "error" ) ) {

            if ( data.contains( "parity" ) ) {
                qDebug("FIXME: warn user that the trigger switch is in the wrong direction");
            }

            throw ApexStringException( QString("L34: error string received ") +data );
        }
        qDebug("%s", qPrintable(data));


        if ( timeout && timer.elapsed() > timeout)
            return false;
        appcore::IThread::sf_Sleep( 250 );

        //      qDebug("Polling again...");

        m_pDevice->send( "<status/>" );
        //      qDebug("Receiving");
        data = m_pDevice->receive( );
        //      qDebug("End receive");
#ifdef DEBUGL34
        qDebug(QString("Device %1: ").arg(m_nDeviceNr) + data.c_str());
#endif
    }

    return true;
}

bool L34Device::waitUntil( const QString& STATE, int timeout=0 )
{
    return waitUntil (STATE, "<a a=k>", timeout);       // second parameter is invalid XML, so won't appear
}





bool L34Device::AllDone( )
{

    qDebug("L34Device::AllDone( )");

    if (m_bPlaying==false)
        return true;


    if (m_stimulusLength) {         // try to not poll while we're sure the stimulus hasn't finished (polling is extremely blocking)
        int msSinceStart =  m_timeSinceStart.elapsed();
        long sleeptime = m_stimulusLength/1000 - msSinceStart;
        if (sleeptime>0) {
            qDebug("Waiting %d ms for stimulation to end (total stimulus duration=%d ms)",(int) sleeptime, (int) m_stimulusLength/1000);
            appcore::IThread::sf_Sleep(sleeptime ); // 10ms safety margin
            qDebug("End wait");
        } else {
            //appcore::IThread::sf_Sleep(10 );  // 10ms safety margin
        }

    }
    bool wResult = waitUntil("finished", "stopped",60000);       // TODO: set timeout to length of stimulus
    //bool wResult=true;
    if (!wResult) {
        if (m_iTriggerType==data::TRIGGER_IN) {
            qDebug("Timeout waiting for 'Stopped' status\nTrigger did probably not occur.");
            QMessageBox::critical(0, "Timeout",  QString("Timeout waiting for 'finished/stopped' status\nCheck that the soundcard is set as master device."), QMessageBox::Abort, QMessageBox::NoButton );
        } else
            qDebug("Timeout waiting for 'Stopped' status\nCheck all connections.");
    }

    m_pDevice->send("<stop/>"); // reset

    qDebug("leaving L34Device::AllDone( )");

    m_stimulusLength=0;
    m_bPlaying=false;
    m_bReady=false;
    return true;
}

void L34Device::SetMap (data::ApexMap* p_map)
{
//    qDebug("L34Device::SetMap, map size: %s", qPrintable(QString::number((unsigned)p_map->size())));
    Q_ASSERT(p_map);
    m_pMap = p_map;
}

void L34Device::SendPowerup(  )
{
    Q_ASSERT(m_nPowerupCount > 0);

    const float powerup_period = 70;            // +- 14000pps

    QTime time;
    time.restart();

    qDebug("Number of powerup frames: %s", qPrintable(QString::number(m_nPowerupCount)));

    QString data(QString(NIC_SEQUENCE_OPEN) +  "<stimulus repeat='%1'><parameters>"
        "<pw>25</pw><pg>8</pg>"
        "<ae>0</ae>"
        "<re>0</re>"
        "<cl>0</cl>"
        "<t>" + QString::number(powerup_period,'g',15) + "</t>"
        "</parameters></stimulus>" + QString(NIC_SEQUENCE_END));
    data=data.arg(m_nPowerupCount);
    m_pDevice->send( data );

#ifdef DEBUGL34
    qDebug("** Time in SendPowerup: "+  QString::number(time.elapsed()));
    showStatus();
#endif

    m_stimulusLength += long (m_nPowerupCount * powerup_period);

    m_bPowerupSent=true;
}


QString L34Device::GetEndXML() const
{
    return QString();
}

bool L34Device::SetParameter ( const QString& type, const int channel, const QVariant& value )
{
    Q_UNUSED (channel);

    qDebug("L34Device::SetParameter %s=%s", qPrintable(type), qPrintable(value.toString()));
    if (type=="volume") {

        bool ok;
        m_volume =  value.toDouble(&ok);
        Q_ASSERT(ok);
        //Q_ASSERT(m_iVolume<=100);
        qDebug("L34Device: set volume to %s", qPrintable(QString::number(m_volume)));
        if (m_volume<0)
            throw ApexStringException(
                    tr("Attempting to set volume to a negative value: %1")
                    .arg(m_volume));
        Q_ASSERT(m_volume>=0);

        if (m_volume>100)
            m_volume=100;
        if (m_volume<0)
            m_volume=0;

        mv_bNeedsRestore=true;

        return true;
    } else {
        return false;
    }
}

void L34Device::RestoreParametersImpl()
{
    m_volume =  (dynamic_cast<data::L34DeviceData*>(  m_pParameters  ) )->volume();
}



void L34Device::Reset()
{
    m_volume=100;
}

void L34Device::Prepare()
{
    // TODO: channel mapping here
}

void L34Device::SetSilenceBefore(double)
{
    throw ApexStringException(tr("Error: silence before the first"
            " stimulus is not implemented for L34Device"));
}

}

}
