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

#include "linux_jackwrapper.h"
#ifndef S_WIN32
  #include <jack/jack.h>
#else
  #ifndef jack_client_t
    #define jack_client_t unsigned
  #endif
  #ifndef jack_port_t
    #define jack_port_t unsigned
  #endif
  #ifndef jack_nframes_t
    #define jack_nframes_t unsigned
  #endif

  const char* JACK_DEFAULT_AUDIO_TYPE = "whatever";

  enum JackPortFlags
  {
    JackPortIsInput = 0x1,
    JackPortIsOutput = 0x2,
    JackPortIsPhysical = 0x4,
    JackPortCanMonitor = 0x8,
    JackPortIsTerminal = 0x10
  };

  jack_client_t* jack_client_new( const char* )
  {
    return 0;
  }
  int jack_client_close( jack_client_t* )
  {
    return 0;
  }
  int jack_activate( jack_client_t* )
  {
    return 0;
  }
  int jack_deactivate( jack_client_t* )
  {
    return 0;
  }
  jack_nframes_t jack_get_buffer_size( jack_client_t* )
  {
    return 0;
  }
  jack_nframes_t jack_get_sample_rate( jack_client_t* )
  {
    return 0;
  }
  const char** jack_get_ports( jack_client_t*, const char* , const char*, unsigned long )
  {
    return 0;
  }
  jack_port_t* jack_port_register( jack_client_t*, const char* , const char*, unsigned long, unsigned long )
  {
    return 0;
  }
  int jack_connect( jack_client_t*, const char*, const char* )
  {
    return 0;
  }
  int jack_port_disconnect( jack_client_t*, jack_port_t* )
  {
    return 0;
  }
  int jack_port_unregister( jack_client_t*, jack_port_t* )
  {
    return 0;
  }
  void* jack_port_get_buffer( jack_client_t*, jack_port_t )
  {
    return 0;
  }
  int jack_set_error_function( void* )
  {
    return 0;
  }
  int jack_on_shutdown( jack_client_t*, void*, void* )
  {
    return 0;
  }
  int jack_set_process_callback( jack_client_t*, void*, void* )
  {
    return 0;
  }
  const char* jack_port_name( jack_port_t* )
  {
    return 0;
  }
#endif

#include "audioformat.h"
#include "utils/vectorutils.h"
#include "utils/dataconversion.h"
#include "utils/stringexception.h"
#include "appcore/threads/thread.h"
#include "appcore/threads/criticalsection.h"
#include <iostream>
#include <cstring>
#include <stdio.h>

//#define TEST_JACK_OUTPUT

using namespace utils;
using namespace appcore;
using namespace streamapp;
using namespace dataconversion;

namespace streamapp
{

    /**
      * tJackUserData
      *   an instance of this struct gets passed
      *   to the Jack callback.
      ****************************************** */
  struct tJackUserData
  {
    tJackUserData() :
      m_pClient( 0 ),
      m_pInputPorts( 0 ),
      m_pOutputPorts( 0 ),
      m_pCallback( 0 ),
      m_nBufferSize( 0 ),
      m_nIChan( 0 ),
      m_nOChan( 0 ),
      m_nSampleRate( 0 ),
      m_bRunning( false ),
      m_pInput( 0 ),
      m_pOutput( 0 )
    {
    }

      /**
        * Try to register out ports and allocate
        * memory as required.
        * @return true for success
        */
    bool mp_bMakePorts()
    {
      if( m_nIChan )
      {
        m_pInputPorts = new jack_port_t* [ m_nIChan ];
        m_pInput = new float*[ m_nIChan ];
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
          m_pInputPorts[ i ] = 0;
          m_pInput[ i ] = 0;
        }
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
          char portname[ 10 ];
          ::sprintf( portname, "in%d", i );
          jack_port_t* p = jack_port_register( m_pClient, portname, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, m_nBufferSize );
          if( p )
            m_pInputPorts[ i ] = p;
          else
            return false;
        }
      }
      else
      {
        m_pInputPorts = 0;
        m_pInput = 0;
      }
      if( m_nOChan )
      {
        m_pOutputPorts = new jack_port_t* [ m_nOChan ];
        m_pOutput = new float*[ m_nOChan ];
        for( unsigned i = 0 ; i < m_nOChan ; ++i )
        {
          m_pOutputPorts[ i ] = 0;
          m_pOutput[ i ] = 0;
        }
        for( unsigned i = 0 ; i < m_nOChan ; ++i )
        {
          char portname[ 10 ];
          ::sprintf( portname, "out%d", i );
          jack_port_t* p = jack_port_register( m_pClient, portname, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, m_nBufferSize );
          if( p )
            m_pOutputPorts[ i ] = p;
          else
            return false;
        }
      }
      else
      {
        m_pOutputPorts = 0;
        m_pOutput = 0;
      }
      return true;
    }

      /**
        * Disconnect and unregister ports,
        * clean up memory.
        */
    void mp_ClosePorts()
    {
      if( m_pInputPorts )
      {
        for( unsigned i = 0 ; i < m_nIChan ; ++i )
        {
          jack_port_disconnect( m_pClient, m_pInputPorts[ i ] );
          jack_port_unregister( m_pClient, m_pInputPorts[ i ] );
        }
        delete [] m_pInputPorts;
        delete [] m_pInput;
        m_pInput = 0;
        m_pInputPorts = 0;
      }
      if( m_pOutputPorts )
      {
        for( unsigned i = 0 ; i < m_nOChan ; ++i )
        {
          jack_port_disconnect( m_pClient, m_pOutputPorts[ i ] );
          jack_port_unregister( m_pClient, m_pOutputPorts[ i ] );
        }
        delete [] m_pOutputPorts;
        delete [] m_pOutput;
        m_pOutput = 0;
        m_pInputPorts = 0;
      }
    }

    jack_client_t*  m_pClient;
    jack_port_t**   m_pInputPorts;
    jack_port_t**   m_pOutputPorts;
    Callback*       m_pCallback;
    unsigned        m_nBufferSize;
    unsigned        m_nIChan;
    unsigned        m_nOChan;
    unsigned        m_nSampleRate;
    bool            m_bRunning;
    bool            m_bClearBuffers;
    float**         m_pInput;
    float**         m_pOutput;
  };


    /**
      * JackFormatReader
      *   implements AudioFormatReader for JackWrapper.
      ************************************************* */
  class JackFormatReader : public AudioFormatReader
  {
  public:
    JackFormatReader( tJackUserData& a_Parent ) :
      m_Source( a_Parent )
    {
    }

    ~JackFormatReader()
    {
    }

    unsigned mf_nChannels() const
    { return m_Source.m_nIChan; }
    unsigned long mf_lSampleRate() const
    { return m_Source.m_nSampleRate; }
    AudioFormat::mt_eBitMode mf_eBitMode() const
    { return MSBint32; }

    unsigned long Read( void** a_pBuf, const unsigned ac_nSamples )
    {
      if( ac_nSamples != m_Source.m_nBufferSize )
        return 0; //too bad..
      int** pSrc = (int**) a_pBuf;
      for( unsigned i = 0 ; i < m_Source.m_nIChan ; ++i )
        for( unsigned j = 0 ; j < m_Source.m_nBufferSize ; ++j )
          pSrc[ i ][ j ] =  (int) ( m_Source.m_pInput[ i ][ j ] * sc_f32BitMinMax );
      return ac_nSamples;
    }

  private:
    tJackUserData& m_Source;

    JackFormatReader( const JackFormatReader& );
    JackFormatReader& operator = ( const JackFormatReader& );
  };

    /**
      * JackFormatWriter
      *   implements AudioFormatWriter for JackWrapper.
      ************************************************* */
  class JackFormatWriter : public AudioFormatWriter
  {
  public:
    JackFormatWriter( tJackUserData& a_Parent ) :
      m_Source( a_Parent )
    {
    }
    ~JackFormatWriter()
    {
    }

    unsigned mf_nChannels() const
    { return m_Source.m_nOChan; }
    unsigned long mf_lSampleRate() const
    { return m_Source.m_nSampleRate; }
    AudioFormat::mt_eBitMode mf_eBitMode() const
    { return MSBint32; }

    unsigned long Write( const void** a_pBuf, const unsigned ac_nSamples )
    {
      (void) a_pBuf;
      if( ac_nSamples != m_Source.m_nBufferSize )
        return 0; //too bad..
#ifndef TEST_JACK_OUTPUT
      int** pSrc = (int**) a_pBuf;
      for( unsigned i = 0 ; i < m_Source.m_nOChan ; ++i )
        for( unsigned j = 0 ; j < m_Source.m_nBufferSize ; ++j )
          m_Source.m_pOutput[ i ][ j ] =  (float) ( pSrc[ i ][ j ] ) / sc_f32BitMinMax;
#endif
      return ac_nSamples;
    }

  private:
    tJackUserData& m_Source;

    JackFormatWriter( const JackFormatWriter& );
    JackFormatWriter& operator = ( const JackFormatWriter& );
  };


    /**
      * Error callback.
      */
  void sf_JackErrorCB( const char* ac_pMsg )
  {
    std::cout << ac_pMsg << std::endl;
  }

    /**
      * Shutdown callback.
      */
  void sf_JackShutsUsDownCB( void* )
  {
    std::cout << "Jack shuts us down" <<  std::endl;
  }

    /**
      * Process callback.
      */
  int sf_JackCB( jack_nframes_t nFrames, void* arg )
  {
#ifdef TEST_JACK_OUTPUT
      //setup for a nice reese
    static float lf = 0.f;
    static float rf = 0.f;
#endif
    tJackUserData* p = (tJackUserData*) arg;
    if( p->m_bRunning )
    {
      p->m_nBufferSize = nFrames;
      if( p->m_pInput )
      {
        for( unsigned i = 0 ; i < p->m_nOChan ; ++i )
          p->m_pInput[ i ] = (float*) jack_port_get_buffer( p->m_pInputPorts[ i ], nFrames );
      }
      if( p->m_pOutput )
      {
        for( unsigned i = 0 ; i < p->m_nOChan ; ++i )
          p->m_pOutput[ i ] = (float*) jack_port_get_buffer( p->m_pOutputPorts[ i ], nFrames );
#ifdef TEST_JACK_OUTPUT
        for( unsigned i = 0 ; i < nFrames ; ++i )
        {
          p->m_pOutput[ 0 ][ i ] = lf;
          p->m_pOutput[ 1 ][ i ] = rf;
          lf += 0.005f;
          rf += 0.004f;
          if( lf >= 1.0f )
            lf = -1.0f;
          if( rf >= 1.0f )
            rf = -0.5f;
        }
#endif
      }
      if( p->m_pCallback )
        p->m_pCallback->mf_Callback();
    }
    else
    {
        //clear buffers; do this all the time to make sure
        //no garbage can exist
      for( unsigned i = 0 ; i < p->m_nOChan ; ++i )
      {
        float* pDest = (float*) jack_port_get_buffer( p->m_pOutputPorts[ i ], nFrames );
        ::memset( pDest, 0, nFrames * sizeof( float ) );
      }
    }
    return 0;
  }

    /**
      * Build drivernames array.
      * Ports are specified as "drivername:portname:channelname",
      * we only want the "drivername:portname" part.
      * Edit: appeareantly it can be "drivername:portname_channelname"
      * as well..
      * @param ac_pcJackNames array returned by jack_get_ports
      * @param a_Dest array to append unique names to
      */
  void sf_AddDrvrNames( const char** ac_pcJackNames, tStringVector& a_Dest )
  {
    if( ac_pcJackNames )
    {
      const char* cSep = ":";
      const char* cOtherSep = "_";
      unsigned i = 0;
      while( ac_pcJackNames[ i ] != 0 )
      {
          //get the part before first ":"
        const char* pEnd = ::strstr( ac_pcJackNames[ i ], cSep );
        if( pEnd )
        {
            //get the part before second ":"
          const char* pDrvrEnd = ::strstr( pEnd + 1, cSep );
            //or maybe before second "_"
          if( !pDrvrEnd )
            pDrvrEnd = ::strstr( pEnd + 1, cOtherSep );
          if( pDrvrEnd )
          {
            const ptrdiff_t nNameLen = pDrvrEnd - ac_pcJackNames[ i ];
            char* pAdd = new char[ nNameLen + 1 ];
            ::memcpy( pAdd, ac_pcJackNames[ i ], nNameLen );
            pAdd[ nNameLen ] = 0;
            const std::string sAdd( pAdd );
            delete[] pAdd;
            if( !f_bHasElement( a_Dest, sAdd ) )
              a_Dest.push_back( sAdd );
          }
          ++i;
        }
      }
      free( ac_pcJackNames );
    }
  }

    /**
      * Compare first part of each name in ac_pcJackNames
      * with ac_sDrvName, and add to a_Dest if equal.
      * @param ac_pcJackNames array returned by jack_get_ports
      * @param ac_sDrvName name acquired by sf_AddDrvrNames
      * @param a_Dest array to add to
      */
  void sf_GetDriverNames( const char** ac_pcJackNames, const std::string& ac_sDrvName, tStringVector& a_Dest )
  {
    if( ac_pcJackNames )
    {
      const char* cSep = ":";
      const char* cOtherSep = "_";
      unsigned i = 0;
      while( ac_pcJackNames[ i ] != 0 )
      {
          //get the part before first ":"
        const char* pEnd = ::strstr( ac_pcJackNames[ i ], cSep );
        if( pEnd )
        {
            //get the part before second ":"
          const char* pDrvrEnd = ::strstr( pEnd + 1, cSep );
          if( !pDrvrEnd )
            pDrvrEnd = ::strstr( pEnd + 1, cOtherSep );
          if( pDrvrEnd )
          {
            const ptrdiff_t nNameLen = pDrvrEnd - ac_pcJackNames[ i ];
            char* pAdd = new char[ nNameLen + 1 ];
            ::memcpy( pAdd, ac_pcJackNames[ i ], nNameLen );
            pAdd[ nNameLen ] = 0;
            if( ac_sDrvName == pAdd )
              a_Dest.push_back( ac_pcJackNames[ i ] );
          }
          ++i;
        }
      }
      free( ac_pcJackNames );
    }
  }

  const char* sc_psJackServerErr = "JackWrapper: couldn't connect to Jack server";
}

/*******************************************************************************************************/

JackWrapper::JackWrapper( const std::string& ac_sDrvName ) :
    ISoundCard(),
  mc_sDriver( ac_sDrvName ),
  m_pData( new tJackUserData() ),
  mv_bOpen( 0 )
{
  //FIXME multiple cards!!
  m_pData->m_pClient = jack_client_open( "apex", JackNullOption, NULL );
  if( !m_pData->m_pClient )
  {
    delete m_pData;
    throw( utils::StringException( sc_psJackServerErr ) );
  }
    //build drivernames arrays
  sf_GetDriverNames( jack_get_ports( m_pData->m_pClient, 0, 0, JackPortIsPhysical | JackPortIsInput ), ac_sDrvName, m_JackPortsIn );
  sf_GetDriverNames( jack_get_ports( m_pData->m_pClient, 0, 0, JackPortIsPhysical | JackPortIsOutput ), ac_sDrvName, m_JackPortsOut );
  if( m_JackPortsIn.empty() && m_JackPortsIn.empty() )
  {
    delete m_pData;
    throw( utils::StringException( "JackWrapper: driver " + ac_sDrvName + " not found" ) );
  }
}

JackWrapper::~JackWrapper()
{
  mp_bCloseDriver();
  jack_client_close( m_pData->m_pClient );
  delete m_pData;
}

tSoundCardInfo JackWrapper::mf_GetInfo() const
{
  tSoundCardInfo Ret;

  Ret.m_nMaxInputChannels = (unsigned) m_JackPortsOut.size();
  Ret.m_nMaxOutputChannels = (unsigned) m_JackPortsIn.size();
  Ret.m_nDefaultBufferSize = jack_get_buffer_size( m_pData->m_pClient );
  Ret.m_BufferSizes.push_back( Ret.m_nDefaultBufferSize );
  Ret.m_SampleRates.push_back( jack_get_sample_rate( m_pData->m_pClient ) );

  return Ret;
}

tStringVector JackWrapper::sf_saGetDriverNames( std::string& a_sError )
{
  jack_set_error_function( sf_JackErrorCB );
  tStringVector ret;

  jack_client_t* pClient = jack_client_open( "getspecs", JackNullOption, NULL );
  if( !pClient )
  {
    a_sError = sc_psJackServerErr;
    return ret;
  }

  sf_AddDrvrNames( jack_get_ports( pClient, 0, 0, JackPortIsPhysical | JackPortIsInput ), ret );
  sf_AddDrvrNames( jack_get_ports( pClient, 0, 0, JackPortIsPhysical | JackPortIsOutput ), ret );

  jack_client_close( pClient );

  if( ret.empty() )
    a_sError = "JackWrapper: no active ports found on server";

  return ret;
}

bool JackWrapper::mp_bCloseDriver()
{
  bool bSuccess = true;
  if( mv_bOpen )
  {
    bSuccess = mp_bStop(); //make sure we're done

    m_pData->mp_ClosePorts();

    m_pData->m_nIChan      = 0;
    m_pData->m_nOChan      = 0;
    m_pData->m_nBufferSize = 0;
    m_pData->m_pCallback   = 0;

    mv_bOpen = false;
  }

  if( !bSuccess )
    mv_sError = "Failed mf_bCloseDriver!!";

  return bSuccess;
}

unsigned JackWrapper::mf_nGetBufferSize( ) const
{
  return m_pData->m_nBufferSize;
}

unsigned JackWrapper::mf_nGetIChan( ) const
{
  return m_pData->m_nIChan;
}

unsigned JackWrapper::mf_nGetOChan( ) const
{
  return m_pData->m_nOChan;
}

unsigned long JackWrapper::mf_lGetSampleRate( ) const
{
  return (unsigned long) m_pData->m_nSampleRate;
}

unsigned long JackWrapper::mf_lGetEstimatedLatency() const
{
  return (unsigned long) 0;
}

AudioFormat::mt_eBitMode JackWrapper::mf_eGetBitMode() const
{
  return AudioFormat::MSBint32;
}

void JackWrapper::mp_ClearIOBuffers()
{
  //not necessary
}

bool JackWrapper::mp_bStart( Callback& a_Callback )
{
  if( mv_bOpen )
  {
    //sc_CallbackLock.mf_Enter();
    m_pData->m_pCallback = &a_Callback;
    m_pData->m_bRunning = true;
    //sc_CallbackLock.mf_Leave();
  }
  return false;
}

bool JackWrapper::mp_bStop( void )
{
  if( mv_bOpen )
  {
    //sc_CallbackLock.mf_Enter();
    m_pData->m_bRunning = false;
    m_pData->m_pCallback = 0;
    //sc_CallbackLock.mf_Leave();
  }
  return false;
}

bool JackWrapper::mf_bIsRunning() const
{
  return false;
}

AudioFormatReader* JackWrapper::mf_pCreateReader() const
{
  if( mv_bOpen )
    return new JackFormatReader( *m_pData );
  return 0;
}

AudioFormatWriter* JackWrapper::mf_pCreateWriter() const
{
  if( mv_bOpen )
    return new JackFormatWriter( *m_pData );
  return 0;
}

bool JackWrapper::mp_bOpenDriver(  const unsigned      ac_nIChan,
                                   const unsigned      ac_nOChan,
                                   const unsigned long ac_nFs,
                                   const unsigned      ac_nBufferSize )
{
  mp_bCloseDriver();
  m_pData->m_nIChan = ac_nIChan;
  m_pData->m_nOChan = ac_nOChan;
  m_pData->m_nBufferSize = ac_nBufferSize;
  m_pData->m_nSampleRate = ac_nFs;

  jack_on_shutdown( m_pData->m_pClient, sf_JackShutsUsDownCB, 0 );

  if( jack_set_process_callback( m_pData->m_pClient, sf_JackCB, m_pData ) )
  {
    mv_sError = "JackWrapper:: cannot set process callback";
    return false;
  }

  if( jack_activate( m_pData->m_pClient ) )
  {
    mv_sError = "JackWrapper:: cannot activate client";
    return false;
  }

  if( !m_pData->mp_bMakePorts() )
  {
    mv_sError = "JackWrapper:: failed registering ports";
    return false;
  }

  for( unsigned i = 0 ; i < m_pData->m_nIChan ; ++i )
  {
    if( jack_connect( m_pData->m_pClient, m_JackPortsIn.at( i ).c_str(), jack_port_name( m_pData->m_pInputPorts[ i ] ) ) )
      return false;
  }

  for( unsigned i = 0 ; i < m_pData->m_nOChan ; ++i )
  {
    const char* iname = jack_port_name( m_pData->m_pOutputPorts[ i ] );
    const char* oname = m_JackPortsIn.at( i ).c_str();
    int nRes = jack_connect( m_pData->m_pClient, iname, oname );
    if( nRes )
    {
      mv_sError = "couldn't connect Jack port " + m_JackPortsIn.at( i );
      std::cout << "no connect" << std::endl;
      return false;
    }
  }

  return mv_bOpen = true;
}
