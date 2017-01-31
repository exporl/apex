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

#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"
#ifndef Q_CC_MSVC
#include "iasiothiscallresolver.h"
#endif

#include "win32_asiowrapper.h"
#include "audioformat.h"
#include "utils/dataconversion.h"
//#include "utils/timers.h"
#include "utils/stringexception.h"
#include "appcore/threads/thread.h"
#include "appcore/threads/criticalsection.h"
#include <iostream>

#include "appcore/threads/thread.h"

using namespace utils;
using namespace dataconversion;
using namespace streamapp;
using namespace appcore;

#ifdef _MSC_VER
#pragma warning ( disable : 4018 ) //signed / unsigned mismatch
#pragma warning ( disable : 4244 ) //conversion ASIOSampleRate to unsigned long
#pragma warning ( disable : 4127 ) //conitional expression is constant
#endif

//taken and modified from the ASIO2 SDK host.cpp
enum eMaxAsioChannels
{
    kMaxInputChannels   = 32,
    kMaxOutputChannels  = 32
                      };

//FIXME this needs some cleanup
typedef struct DriverInfo
{
    ASIODriverInfo      driverInfo;
    long                inputChannels;
    long                outputChannels;
    long                minSize;
    long                maxSize;
    long                preferredSize;
    long                granularity;
    ASIOSampleRate      sampleRate;
    bool                postOutput;
    long                inputLatency;
    long                outputLatency;
    long                inputBuffers;
    long                outputBuffers;
    ASIOBufferInfo*     bufferInfos;
    ASIOChannelInfo*    channelInfos;
    double              nanoSeconds;
    double              samples;
    double              tcSamples;
    ASIOTime            tInfo;
    unsigned long       sysRefTime;
    bool                stopped;

    void mp_CreateInfos()
    {
        mp_DestroyInfos();
        bufferInfos = new ASIOBufferInfo[ inputChannels + outputChannels ];
        channelInfos = new ASIOChannelInfo[ inputChannels + outputChannels ];
    }

    void mp_DestroyInfos()
    {
        delete bufferInfos;
        delete channelInfos;
        bufferInfos = 0;
        channelInfos = 0;
    }
}DriverInfo;

//!uninitialized, use with care
//!shared between ASIO SDK, AsioWrapper and AsioFormatReader/Writer
//!FIXME? doesn't allow for multiple soundcards but is not possible anyway?
namespace
{
    unsigned  m_nIChan;
    unsigned  m_nOChan;
    unsigned  m_nBufferSize;
    unsigned  m_nClearSize;
    AudioFormat::mt_eBitMode  m_eBitMode;
    bool      m_bBusy;
    bool      m_bWasInCallback;
    long      m_nCurrentBufIndex;
    Callback* m_pCallback;

    DriverInfo            asioDriverInfo;
    ASIOCallbacks         asioCallbacks;


    const appcore::CriticalSection sc_CallbackLock;

    /**
      * Clears all input and output buffers.
      * Must be called only when the driver is open.
      */
    void sf_ClearBuffers()
    {
        sc_CallbackLock.mf_Enter();
        //clear input and output buffers
        const unsigned nTotal = asioDriverInfo.inputChannels + asioDriverInfo.outputChannels;
        for( unsigned i = 0 ; i < nTotal ; ++i )
        {
            memset( asioDriverInfo.bufferInfos[ i ].buffers[ 0 ], 0, m_nClearSize );
            memset( asioDriverInfo.bufferInfos[ i ].buffers[ 1 ], 0, m_nClearSize );
        }
        sc_CallbackLock.mf_Leave();
    }

}

static ASIOError             create_asio_buffers     ( DriverInfo* asioDriverInfo, unsigned long nBufferSize  );
static long                  init_asio_static_data   ( DriverInfo* asioDriverInfo );
static long                  init_asio_samplerate    ( DriverInfo* asioDriverInfo, const unsigned long ac_lSampleRate );
static void                  bufferSwitch            ( long index, ASIOBool processNow                        );
static ASIOTime  *           bufferSwitchTimeInfo    ( ASIOTime  * timeInfo, long index, ASIOBool processNow  );

//these are not really functional, but required by the asioCallbacks structure FIXME
static void                  samRateChanged          ( ASIOSampleRate SampleRateRate                          );
static long                  asioMessages            ( long selector, long value, void* message, double* opt  );

namespace streamapp
{
    class AsioFormatReader : public AudioFormatReader
    {
    public:
        ~AsioFormatReader()
        {}

        //!implement AudioFormatReader
        unsigned mf_nChannels() const
        { return m_pSource->mf_nGetIChan(); }
        unsigned long mf_lSampleRate() const
        { return m_pSource->mf_lGetSampleRate(); }
        AudioFormat::mt_eBitMode mf_eBitMode() const
        { return m_pSource->mf_eGetBitMode(); }

        //!read doesn't succeed if ac_nSamples > buffersize of asiowrapper
        //!use a buffered reader for that
        unsigned long Read( void** a_pBuf, const unsigned ac_nSamples );

        //!an AsioFormatReader can only exist if the wrapper can create it
        friend class AsioWrapper;
    private:
        AsioFormatReader( const AsioWrapper* a_pSource );
        const AsioWrapper* m_pSource;
    };

    class AsioFormatWriter : public AudioFormatWriter
    {
    public:
        ~AsioFormatWriter()
        {}

        //!implement AudioFormatWriter
        unsigned mf_nChannels() const
        { return m_pSource->mf_nGetOChan(); }
        unsigned long mf_lSampleRate() const
        { return m_pSource->mf_lGetSampleRate(); }
        AudioFormat::mt_eBitMode mf_eBitMode() const
        { return m_pSource->mf_eGetBitMode(); }

        unsigned long Write( const void** a_pBuf, const unsigned ac_nSamples );

        //!an AsioFormatReader can only exist if the wrapper can create it
        friend class AsioWrapper;
    private:
        AsioFormatWriter( const AsioWrapper* a_pSource );
        const AsioWrapper* m_pSource;
    };
}

/*******************************************************************************************************/

AsioWrapper::AsioWrapper( const std::string&  ac_sDrvName ) :
        ISoundCard(),
        mv_bOpen( 0 )
{
    m_nIChan      = 0;
    m_nOChan      = 0;
    m_nBufferSize = 0;
    m_pCallback   = 0;
    m_bBusy       = false;

    bool loadAsioDriver( char *name );

    if( !loadAsioDriver( const_cast<char*> ( ac_sDrvName.data() ) ) )
        throw( StringException( "AsioWrapper unknown drivername :" + ac_sDrvName ) );
    if( ASIOInit( &asioDriverInfo.driverInfo ) != ASE_OK )
    {
        mv_sError = asioDriverInfo.driverInfo.errorMessage;
        if( mv_sError.empty() ) //empty message means no driver available
            mv_sError = asioDriverInfo.driverInfo.name;
        ASIOExit();
        throw( StringException( "AsioWrapper couldn't init ASIO : " + mv_sError ) );
    }
    if( init_asio_static_data( &asioDriverInfo ) != 0 )
        throw( StringException( "AsioWrapper couldn't initialize driver : " + ac_sDrvName ) );
}

AsioWrapper::~AsioWrapper()
{
    mp_bCloseDriver();
}

tSoundCardInfo AsioWrapper::mf_GetInfo() const
{
    tSoundCardInfo Ret;

    Ret.m_nMaxInputChannels   = (unsigned) asioDriverInfo.inputChannels;
    Ret.m_nMaxOutputChannels  = (unsigned) asioDriverInfo.outputChannels;
    Ret.m_nDefaultBufferSize  = (unsigned) asioDriverInfo.preferredSize;

    //get available sizes, between min and max, in steps of powers of 2
    if( asioDriverInfo.granularity == 0 )
    {
        Ret.m_BufferSizes.push_back( asioDriverInfo.preferredSize );
    }
    else if( asioDriverInfo.granularity == -1 )  //powers of 2 ??
    {
        long temp = asioDriverInfo.minSize;
        while( temp <= asioDriverInfo.maxSize )
        {
            Ret.m_BufferSizes.push_back( temp );
            temp += temp;    //this is just an estimation, how to find out all rates?
        }
    } else if ( asioDriverInfo.granularity >= 0 ) {
        long temp = asioDriverInfo.minSize;
        while( temp <= asioDriverInfo.maxSize )
        {
            Ret.m_BufferSizes.push_back( temp );
            temp += asioDriverInfo.granularity;
        }
    } else {
        qFatal("Unknown granularity %li", asioDriverInfo.granularity);
    }

    //test common samplerates
    BaseSampleRates rates;
    for( BaseSampleRates::size_type i = 0 ; i < rates.size() ; ++i )
    {
        const double base = (double) rates.at( i );
        double mult = 1;
        ASIOSampleRate cur = base * mult;
        while( cur < rates.sc_dMaxSampleRate )
        {
            if( ASIOCanSampleRate( cur ) == ASE_OK )
                Ret.m_SampleRates.push_back( (unsigned long) cur );
            ++mult;
            cur = base * mult;
        }
    }

    return Ret;
}

tStringVector AsioWrapper::sf_saGetDriverNames( std::string& a_sError )
{
    tStringVector ret;

    AsioDrivers drv;
#ifdef S_WIN32
    long nDevs        = drv.asioGetNumDev();
#else
    long nDevs        = drv.getNumFragments();
#endif

    if( !nDevs )
    {
        a_sError = "AsioWrapper: no ASIO soundcards exist";
        return ret;
    }

    char** pDevNames  = new char*[ nDevs ];
    for( long i = 0 ; i < nDevs ; ++i )
        pDevNames[ i ] = new char[ 32 ];

    nDevs = drv.getDriverNames( pDevNames, nDevs );
    for( long i = 0 ; i < nDevs ; ++i )
        ret.push_back( std::string( pDevNames[ i ] ) );

    for( long i = 0 ; i < nDevs ; ++i )
        delete pDevNames[ i ];
    delete pDevNames;

    return ret;
}

bool AsioWrapper::mp_bCloseDriver( )
{
    bool bSuccess = true;
    if( mv_bOpen )
    {
        bSuccess = mp_bStop(); //make sure we're done

        if( ASIODisposeBuffers() != ASE_OK )
            bSuccess &= false;

        m_nIChan      = 0;
        m_nOChan      = 0;
        m_nBufferSize = 0;
        m_pCallback   = 0;

        mv_bOpen = false;
    }

    if( ASIOExit() != ASE_OK )
        bSuccess &= false;

    if( !bSuccess )
        mv_sError = "Failed mf_bCloseDriver!!";

    asioDriverInfo.mp_DestroyInfos();
    return bSuccess;
}

unsigned AsioWrapper::mf_nGetBufferSize( ) const
{
    return m_nBufferSize;
}

unsigned AsioWrapper::mf_nGetIChan( ) const
{
    return m_nIChan;
}

unsigned AsioWrapper::mf_nGetOChan( ) const
{
    return m_nOChan;
}

unsigned long AsioWrapper::mf_lGetSampleRate( ) const
{
    return (unsigned long) asioDriverInfo.sampleRate;
}

unsigned long AsioWrapper::mf_lGetEstimatedLatency() const
{
    //at least twice the buffersize
    const double dLat = ( ((double) mf_nGetBufferSize() * 2.0 + 128.0) / (double) mf_lGetSampleRate() ) * 1000.0;
    return (unsigned long) dLat;                                 //|-> this is at least enough to cover professional cards
}

AudioFormat::mt_eBitMode AsioWrapper::mf_eGetBitMode() const
{
    return m_eBitMode;
}

void AsioWrapper::mp_ClearIOBuffers()
{
    sf_ClearBuffers();
}

#ifdef _MSC_VER
#pragma warning ( disable : 4706 )
#endif

bool AsioWrapper::mp_bStart( Callback& a_Callback )
{
    if( mv_bOpen )
    {
        sc_CallbackLock.mf_Enter();
        m_pCallback = &a_Callback;
        sc_CallbackLock.mf_Leave();
        if( ASIOStart() == ASE_OK )
            return !(asioDriverInfo.stopped = false );
        else
            return false;
    }
    return false;
}

bool AsioWrapper::mp_bStop( void )
{
    if( mv_bOpen )
    {
        sc_CallbackLock.mf_Enter();
        m_pCallback = 0;
        sc_CallbackLock.mf_Leave();
        IThread::sf_Sleep( 2000 * mf_nGetBufferSize() / mf_lGetSampleRate() );
        if( ASIOStop() != ASE_OK )
            return false;
        return asioDriverInfo.stopped = true;
    }
    return false;
}

bool AsioWrapper::mf_bIsRunning() const
{
    return !asioDriverInfo.stopped;
}

AudioFormatReader* AsioWrapper::mf_pCreateReader() const
{
    if( mv_bOpen )
        return new AsioFormatReader( this );
    return 0;
}

AudioFormatWriter* AsioWrapper::mf_pCreateWriter() const
{
    if( mv_bOpen )
        return new AsioFormatWriter( this );
    return 0;
}

AsioFormatReader::AsioFormatReader( const AsioWrapper* a_pSource ) :
        m_pSource( a_pSource )
{
}

unsigned long AsioFormatReader::Read( void** a_pBuf, const unsigned ac_nSamples )
{
    if( ac_nSamples != m_nBufferSize )
        return 0l;

    int** pBuf = (int**) a_pBuf;

    if( m_eBitMode == AudioFormat::MSBint16 )
    {
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
            const short* src = (short*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
                pBuf[ i ][ j ] = src[ j ] << 16;
        }
    }
    else if( m_eBitMode == AudioFormat::MSBint24 )
    {
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
            const char* src = (char*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
            {
                pBuf[ i ][ j ] = littleEndian24Bit( src ) << 8;
                src += 3;
            }
        }
    }
    else if( m_eBitMode == AudioFormat::MSBint32 )
    {
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
            const int* src = (int*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
                pBuf[ i ][ j ] = src[ j ];
        }
    }
    else if( m_eBitMode == AudioFormat::MSBfloat32 )
    {
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
            const float* src = (float*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
                pBuf[ i ][ j ] = *(int*) (float*)&src[ j ];
        }
    }
    else
        return 0l;

    return ac_nSamples;
}

AsioFormatWriter::AsioFormatWriter( const AsioWrapper* a_pSource ) :
        m_pSource( a_pSource )
{
}

unsigned long AsioFormatWriter::Write( const void** a_pBuf, const unsigned ac_nSamples )
{
    if( ac_nSamples != m_nBufferSize )
        return 0l;

    const unsigned nOchanOffset = asioDriverInfo.inputChannels;
    const unsigned nTodo        = nOchanOffset + m_nOChan;

    int** pBuf = (int**) a_pBuf;

    if( m_eBitMode == AudioFormat::MSBint16 )
    {
        for( unsigned i = nOchanOffset ; i < nTodo ; ++i )
        {
            short* src = (short*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
                src[ j ] = pBuf[ i - nOchanOffset ][ j ] >> 16;
        }
    }
    else if( m_eBitMode == AudioFormat::MSBint24 )
    {
        for( unsigned i = nOchanOffset ; i < nTodo ; ++i )
        {
            char* src = (char*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
            {
                littleEndian24BitToChars( pBuf[ i - nOchanOffset ][ j ] >> 8, src );
                src += 3;
            }
        }
    }
    else if( m_eBitMode == AudioFormat::MSBint32 )
    {
        for( unsigned i = nOchanOffset ; i < nTodo ; ++i )
        {
            int* src = (int*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
                src[ j ] = pBuf[ i - nOchanOffset ][ j ];
        }
    }
    else if( m_eBitMode == AudioFormat::MSBfloat32 )
    {
        for( unsigned i = nOchanOffset ; i < nTodo ; ++i )
        {
            float* src = (float*) asioDriverInfo.bufferInfos[ i ].buffers[ m_nCurrentBufIndex ];
            for( unsigned j = 0 ; j < m_nBufferSize ; ++j )
                src[ j ] = *(float*)&pBuf[ i - nOchanOffset ][ j ];
        }
    }
    else
        return 0l;

    return ac_nSamples;
}

void bufferSwitch( long index, ASIOBool processNow )
{
    ASIOTime  timeInfo;
    memset (&timeInfo, 0, sizeof (timeInfo));

    //get the time stamp of the buffer
    if(ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime) == ASE_OK)
        timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
    bufferSwitchTimeInfo( &timeInfo, index, processNow );
}

// conversion from 64 bit ASIOSample/ASIOTimeStamp to double float
#if NATIVE_INT64
#define ASIO64toDouble(a)  (a)
#else
const double twoRaisedTo32 = 4294967296.;
#define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif

/*unsigned long get_sys_reference_time()
{  // get the system reference time
#if WINDOWS
  return timeGetTime();
#elif MAC
  static const double twoRaisedTo32 = 4294967296.;
  UnsignedWide ys;
  Microseconds(&ys);
  double r = ((double)ys.hi * twoRaisedTo32 + (double)ys.lo);
  return (unsigned long)(r / 1000.);
#endif
}*/


//static SimpleProfiling prof2( "Callback" );

ASIOTime* bufferSwitchTimeInfo( ASIOTime *timeInfo, long index, ASIOBool /*processNow*/ )
{
    m_bBusy = true;
    m_bWasInCallback = true;

    sc_CallbackLock.mf_Enter();

    //prof1.Stop();

    //static Timer2 timerken;
    //static Timer2 timerken2;

    //static processedSamples = 0;

    /*if( processNow == ASIOFalse )
    OutputDebugString( "\n ****** aaargh\n" );
  if( m_nCurrentBufIndex == index )
    OutputDebugString( "\n ****** aaargh2\n" );*/

    // store the timeInfo for later use
    asioDriverInfo.tInfo = *timeInfo;

    // get the time stamp of the buffer, not necessary if no
    // synchronization to other media is required
    /*if (timeInfo->timeInfo.flags & kSystemTimeValid)
    asioDriverInfo.nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
  else
    asioDriverInfo.nanoSeconds = 0;

  if (timeInfo->timeInfo.flags & kSamplePositionValid)
    asioDriverInfo.samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
  else
    asioDriverInfo.samples = 0;

  if (timeInfo->timeCode.flags & kTcValid)
    asioDriverInfo.tcSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
  else
    asioDriverInfo.tcSamples = 0;  */

    // get the system reference time
    //asioDriverInfo.sysRefTime = get_sys_reference_time();


    //static double last_samples = 0;
    //static unsigned long last_sysref = get_sys_reference_time();
    //static double last_sysref = timerken2.Elapsed();
    //char tmp[128];
    //sprintf (tmp, "diff: %d / %d ms / %d ms / %d samples                 \n", asioDriverInfo.sysRefTime - (long)(asioDriverInfo.nanoSeconds / 1000000.0), asioDriverInfo.sysRefTime, (long)(asioDriverInfo.nanoSeconds / 1000000.0), (long)(asioDriverInfo.samples - last_samples));



    //set current index for reader/writer
    m_nCurrentBufIndex = index;

    //call back
    //timerken.Reset();
    if( m_pCallback && !asioDriverInfo.stopped )
    {
        //prof2.Start();
        m_pCallback->mf_Callback();
        //prof2.Stop();
    }
    else
    {
        sf_ClearBuffers();
    }

    //sprintf( tmp, "diff: %f elapsed: %f\n", timerken2.Elapsed() - last_sysref, timerken.Elapsed() );
    //OutputDebugString( tmp );
    //last_samples = asioDriverInfo.samples;
    //last_sysref = timerken2.Elapsed();

    //prof1.Start();

    m_bBusy = false;

    sc_CallbackLock.mf_Leave();

    return 0L;
};

bool AsioWrapper::mp_bOpenDriver(  const unsigned      ac_nIChan,
                                   const unsigned      ac_nOChan,
                                   const unsigned long ac_nFs,
                                   const unsigned      ac_nBufferSize )
{
    mv_sError = "";
    mv_sWarning = "";

    if( (long) ac_nBufferSize != asioDriverInfo.preferredSize )
        mv_sWarning = "Buffersize doesn't match Preferred Size";

    m_nBufferSize                = ac_nBufferSize;
    asioDriverInfo.preferredSize = ac_nBufferSize;
    asioDriverInfo.maxSize       = ac_nBufferSize;
    asioDriverInfo.minSize       = ac_nBufferSize;
    asioDriverInfo.granularity   = 0;
    asioDriverInfo.stopped       = false;

    if( init_asio_samplerate( &asioDriverInfo, ac_nFs ) == 0 )
    {
        asioCallbacks.bufferSwitch         = &bufferSwitch;
        asioCallbacks.sampleRateDidChange  = &samRateChanged;
        asioCallbacks.asioMessage          = &asioMessages;
        asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;

        /*ASIOGetChannels()
    ASIOGetBufferSize()
    ASIOCanSampleRate()
    ASIOGetSampleRate()
    ASIOGetClockSources()
    ASIOGetChannelInfo()
    ASIOSetSampleRate()
    ASIOSetClockSource()
    ASIOGetLatencies()*/

        //first check max # channels
        if( ac_nIChan <= (unsigned) asioDriverInfo.inputChannels && ac_nOChan <= (unsigned) asioDriverInfo.outputChannels )
        {
            //set to actual values to use and create buffers for all channels
            m_nIChan      = ac_nIChan;
            m_nOChan      = ac_nOChan;

            ASIOError bError = create_asio_buffers( &asioDriverInfo, ac_nBufferSize );
            ASIOSampleType p = asioDriverInfo.channelInfos[ 0 ].type;     //FIXME check if these can change? for ADAT etc maybe?

            switch( p )
            {
            case ASIOSTInt16LSB   : m_eBitMode = AudioFormat::MSBint16; break;
            case ASIOSTInt24LSB   : m_eBitMode = AudioFormat::MSBint24; break;
            case ASIOSTInt32LSB   : m_eBitMode = AudioFormat::MSBint32; break;
            case ASIOSTFloat32LSB : m_eBitMode = AudioFormat::MSBfloat32; break;
            default               : bError     = ASE_InvalidMode; break;
        }

            int size = 0;
            if( m_eBitMode == AudioFormat::MSBint16 )
                size = 2;
            else if( m_eBitMode == AudioFormat::MSBint24 )
                size = 3;
            else if( m_eBitMode == AudioFormat::MSBint32 || m_eBitMode == AudioFormat::MSBfloat32 )
                size = 4;

            m_nClearSize = size * ac_nBufferSize;

            //!do a little test to see ASIO can run
            if( bError == ASE_OK )
            {
                m_bWasInCallback = false;
                m_pCallback = 0;
                std::cout << "Starting ASIO test.." << std::endl;
                if( ASIOStart() != ASE_OK )
                {
                    mv_sError = "Failed to start Asio test";
                    return false;
                }
                else
                    IThread::sf_Sleep( 4 * m_nBufferSize * 1000 / ac_nFs ); //enough time to zero buffers in callback

                asioDriverInfo.stopped = true;
                ASIOStop();
                if( !m_bWasInCallback )
                {
                    mv_sError = "Failed to start ASIO streaming";
                    return false;
                }
                std::cout << "->Passed ASIO test" << std::endl;
            }
            else if( bError == ASE_InvalidMode )
            {
                mv_sError = "This bitmode is not supported";
                return false;
            }
            else
            {
                mv_sError = "Failed to create buffers";
                return false;
            }
        }
        else
        {
            mv_sError = "Failed number of channels";
            return false;
        }
    }
    else //if( init_asio_static_data( &asioDriverInfo, ac_nFs ) == 0 )
    {
        mv_sError = "init_asio_samplerate Failure!!!!";
        return false;
    }

    return mv_bOpen = true;
}

void samRateChanged( ASIOSampleRate /*sRate*/ )
{
}

long asioMessages( long selector, long value, void* /*message*/, double* /*opt*/ )
{
    switch (selector)
    {
    case kAsioSelectorSupported:
        if (value == kAsioResetRequest
            || value == kAsioEngineVersion
            || value == kAsioResyncRequest
            || value == kAsioLatenciesChanged
            || value == kAsioSupportsInputMonitor)
            return 1;
        break;

    case kAsioBufferSizeChange:
        break;

    case kAsioResetRequest:
        /*if (currentASIODev != 0)
        currentASIODev->resetRequest();*/

        return 1;

    case kAsioResyncRequest:
        /*if (currentASIODev != 0)
        currentASIODev->resyncRequest();*/

        return 1;

    case kAsioLatenciesChanged:
        return 1;

    case kAsioEngineVersion:
        return 2;

    case kAsioSupportsTimeInfo:
    case kAsioSupportsTimeCode:
        return 0;
    }

    return 0;
}

ASIOError create_asio_buffers( DriverInfo* asioDriverInfo, unsigned long nBufferSize )
{
    asioDriverInfo->mp_CreateInfos();
    ASIOBufferInfo *info          = asioDriverInfo->bufferInfos;

    asioDriverInfo->inputBuffers  = asioDriverInfo->inputChannels;
    asioDriverInfo->outputBuffers = asioDriverInfo->outputChannels;

    for( unsigned i = 0 ; i < (unsigned) asioDriverInfo->inputBuffers ; i++, info++ )
    {
        info->isInput    = ASIOTrue;
        info->channelNum = i;
        info->buffers[0] = info->buffers[1] = 0;
    }

    for( unsigned i = 0 ; i <  (unsigned) asioDriverInfo->outputBuffers ; i++, info++ )
    {
        info->isInput    = ASIOFalse;
        info->channelNum = i;
        info->buffers[0] = info->buffers[1] = 0;
    }

    ASIOError result   = ASIOCreateBuffers( asioDriverInfo->bufferInfos,
                                            asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers,
                                            nBufferSize,
                                            &asioCallbacks );
    if( result == ASE_OK )
    {
        for( int i = 0 ; i < ( asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers ) ; i++ )
        {
            asioDriverInfo->channelInfos[i].channel = asioDriverInfo->bufferInfos[i].channelNum;
            asioDriverInfo->channelInfos[i].isInput = asioDriverInfo->bufferInfos[i].isInput;
            result = ASIOGetChannelInfo(&asioDriverInfo->channelInfos[i]);
            if (result != ASE_OK)
                break;
        }
    }

    return result;
}

long init_asio_static_data( DriverInfo *asioDriverInfo )
{
    if( ASIOGetChannels( &asioDriverInfo->inputChannels, &asioDriverInfo->outputChannels ) == ASE_OK )
    {
        if( ASIOGetBufferSize(& asioDriverInfo->minSize, &asioDriverInfo->maxSize, &asioDriverInfo->preferredSize, &asioDriverInfo->granularity ) == ASE_OK )
        {
            if( ASIOGetSampleRate( &asioDriverInfo->sampleRate ) == ASE_OK )
            {
                return 0;
            }
            return -3;
        }
        return -2;
    }
    return -1;
}

long init_asio_samplerate( DriverInfo *asioDriverInfo, const unsigned long ac_nFs )
{
    if( ASIOSetSampleRate( ac_nFs ) == ASE_OK) {
        if( ASIOGetSampleRate( &asioDriverInfo->sampleRate ) == ASE_OK )
        {
        }
        else
        {
            return -5;
        }
    }
    else
        return -5;

    if( ASIOOutputReady() == ASE_OK )
        asioDriverInfo->postOutput = true;
    else
        asioDriverInfo->postOutput = false;

    return 0;
}
