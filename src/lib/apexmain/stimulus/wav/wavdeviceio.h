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

#ifndef __APEX_WAVDEVICEIO_H__
#define __APEX_WAVDEVICEIO_H__

//#define WRITEWAVOUTPUT

#include <string>

#include <QVector>

class QString;
class QStringList;

namespace streamapp
{
    class Channel;
    class EofCheck;
    class ISoundCard;
    class ConnectItem;
    class InputStream;
    class OutputStream;
    class BufferDropCheck;
    class IStreamProcessor;
    class StreamAppChannel;
    class ConnectionManager;
    class BufferedProcessing;
    class EofCheckWaitCallback;
    class AudioFormatWriterStream;
    class PositionableInputStream;
}

using namespace streamapp;

namespace apex
{

namespace data
{
    class WavDeviceData;
}

namespace stimulus
{

struct tConnection;
class ApexSeqStream;
class StreamGenerator;
class BufferDropCallback;
class ApexOutputCallback;
class ApexProcessorCallback;

class WavDeviceBufferThread;
class WavDeviceSoundcardCallback;


typedef QVector<double> tGains;

      /**
        * WavDeviceIO
        *   container for all WavDevice's IO related members.
        ***************************************************** */
    class WavDeviceIO
    {
    public:
        /**
          * Constructor.
          */
        WavDeviceIO(const data::WavDeviceData* const ac_Config );

        /**
          * Destructor.
          */
      virtual ~WavDeviceIO();

          /**
            * Status flag as returned by SetSoundCard.
            * @see WavDeviceFactory for descriptions.
            */
      enum mt_eOpenStatus
      {
        mc_eOK,
        mc_eDriverNotExist,
        mc_eBadInputChannel,
        mc_eBadOutputChannel,
        mc_eBadSampleRate,
        mc_eBadBufferSize,
        mc_eDefBufferSize,
        mc_eInitError,
        mc_eUnknownError
      };

        /**
          * Try to open the soundcard, and initialize everything.
          * This method only creates the soundcard and the
          * soundcard's OutputStream, then it calls mp_InitIO()
          * to setup callbacks and buffering etc.
          * @param ac_Config reference to the config details
          * @param ac_bTryDefaultBufferSize if true and the buffersize specified in ac_Config
          * is invalid for the soundcard, try to open the card with it's default buffersize.
          * Using this size will always succeed (unless the other parameters are wrong off course).
          * If the default size was used this way and the card was opened successfully,
          * mc_eDefBufferSize is returned.
          * @param a_sErr set to soundcard specific error code if opening fails
          * @return mc_eOk for success, else use mf_sGetErrorString()
          */
      mt_eOpenStatus mp_eSetSoundcard( const data::WavDeviceData& ac_Config, const bool ac_bTryDefaultBufferSize, std::string& a_sErr );

        /**
          * Get the default buffersize for the card that was
          * opened last with mp_eSetSoundcard(). Invalid if that
          * method hasn't been called.
          * @return preffered buffersize
          */
/*      unsigned mf_nGetDefaultBufferSize() const throw()
      {
        return m_nDefaultBufSize;
      }*/

        /**
          * Get a string describing the error.
          * @param ac_eReason the error code returned by mp_eSetSoundcard()
          * @return an error string, empty if mc_eOk was passed in
          */
      static QString sf_sGetErrorString( const mt_eOpenStatus& ac_eReason );

        /**
          * Get a list with all soundcard drivernames.
          * Prepends the deicetype (Asio or Portaudio).
          * @return a stringlist
          */
      static QStringList sf_saGetDriverNames();

        /**
          * Get the dirvername for the given config.
          * @see mf_saGetDriverNames
          * @param ac_Config the data
          * @return a string
          */
      static QString sf_sGetDriverName( const data::WavDeviceData& ac_Config );

        /**
          * Set the dirvername acquired by
          * sf_sGetDriverName() or sf_saGetDriverNames().
          * @param ac_sDriverName drivername
          * @param a_Config the data
          */
      static void sf_SetDriverName( const QString& ac_sDriverName, data::WavDeviceData& a_Config );


        /**
          * Add an inputstream to the ConnectionManager.
          * @param a_pItem the InputStream
          * @param ac_sID the ID
          * @param ac_bAddToEofCheck true if the item needs registering for checking EOF
          */
      void mp_AddConnectItem( PositionableInputStream* a_pItem, const std::string& ac_sID, const bool ac_bAddToEofCheck );

        /**
          * Add a processor to the ConnectionManager.
          * @param a_pItem the IStreamProcessor
          * @param ac_sID the ID
          */
      void mp_AddConnectItem( ConnectItem* a_pItem, const std::string& ac_sID );

        /**
          * Add a connection.
          * @param ac_Connection the connect info
          * @param ac_bMixToFile if true, connection is also routed to fileoutput, if any
          */
      void mp_AddConnection( const tConnection& ac_Connection, const bool ac_bMixToFile );

        /**
          * Remove a connection.
          * @param ac_Connection the connect info
          */
      void mp_RemoveConnection( const tConnection& ac_Connection );

        /**
          * Remove all connection s for an item.
          * @param ac_sID the connect item's ID
          */
      void mp_RemoveConnection( const std::string& ac_sID );

        /**
          * Remove connection and add again with
          * another channel number.
          * @param ac_Connection connect info touse and change
          * @param ac_nNewChannel new channel
          * @param ac_bIsFromConnection true for from, false for to connection
          */
      void mp_RewireConnection( tConnection& a_Connection, const unsigned ac_nNewChannel, const bool ac_bIsFromConnection );

        /**
          * Check if all inputs (wavefiles) are connected.
          * If this is not the case, playing will not start,
          * since it will never end anyway.
          * @return true if all connected
          */
      bool mf_bAllInputsConnected() const throw();

        /**
          * Remove all inputs that are registered
          * with the eof checker.
          * @see mp_AddConnectItem()
          */
      void mp_RemoveCheckedInputs();

        /**
          * Stop an item from playing.
          * @param ac_sID the ID
          */
      void mp_PausePlay( const std::string& ac_sID );


        /**
          * Start the soundcard.
          */
      void mp_Start();

        /**
          * Stop the soundcard.
          */
      void mp_Stop();

      /**
       * Stop the soundcard no matter what (also if continuous == true)
       */
      void mp_Finish();

        /**
          * Set continuous mode.
          * @param ac_bContinuous true for continuous mode: main callback
          * will keep on running despite all input files being read
          */
      void mp_SetContinuous( const bool ac_bContinuous )
      {
        mv_bContinuous = ac_bContinuous;
      }

        /**
          * Wait until all files are played.
          */
      void mf_WaitUntilDone() const;


        /**
          * Set the output's gain.
          * Sets the gain of the final stage,
          * ie the soundcard and eventually the output file.
          * @param ac_dGain gain in dB
          */
      void mp_SetOutputGain( const double ac_dGain );

        /**
          * Set the output's gain.
          * Sets the gain of the final stage,
          * ie the soundcard and eventually the output file.
          * @param ac_dGain gain in dB
          * @param ac_nChannel channel number
          */
      void mp_SetOutputGain( const double ac_dGain, const unsigned ac_nChannel );

      /**
       * Reset all parameters (gain for every channel) to the default (0dB)
       */
      virtual void Reset();

      /**
       * Send all parameters to the matrix mixer
       */
      virtual void Prepare();

      static const unsigned     sc_nMaxBusInputs;
      static const std::string  sc_sOutputName;
      static const std::string  sc_sOffLineOutputName;

        /**
          * The size, in samples per channel, of the stream
          * objects used to fill the buffer. On the other side,
          * there's the soundcard buffersize that's used to empty
          * the buffer. This must be lower then sc_nBufferWriteSize,
          * else there's no point in using a buffer.
          */
//      static const unsigned     sc_nBufferWriteSize;
//      static const unsigned     sc_nMinBufferSize;      //! minimal buffer size, is set to 2*sc_nBufferWriteSize

      bool                        mv_bContinuous;
      bool                        mv_bConnectError;
      EofCheck*                   m_pEofCheck;
      ConnectionManager*          m_pConnMan;
      ApexOutputCallback*         m_pMainOutput;
      ISoundCard*                 m_pCard;
      //unsigned                    m_nDefaultBufSize;
      unsigned                    m_soundcardBufferSize;        //! buffer size that the soundcard uses, is not necessarily the same as the buffersize in wavdevicedata
      BufferDropCheck*            m_pSoundcardBufferDropCheck;
      BufferDropCallback*         m_pSoundcardbufferDroppedCallback;
      BufferDropCallback*         m_pBigBufferDroppedCallback;
      AudioFormatWriterStream*    m_pSoundcardWriterStream; //only keep this for getting clipped samples..
      BufferedProcessing*         m_pBuffer;
#ifdef WRITEWAVOUTPUT
      OutputStream*               m_pDbgFileStream;
      StreamAppChannel*           m_pSoundcardWriterCallback;
#else
      Channel*                    m_pSoundcardWriterCallback;
#endif
      WavDeviceBufferThread*      m_pBufferThread;
      WavDeviceSoundcardCallback* m_pSoundcardCallback;

      const data::WavDeviceData&  mc_Config;

    private:
        /**
          * Init callbacks, buffer, file output.
          * @param ac_Config reference to the config details
          */
      void mp_InitIO( );

        /**
          * Delete everything that was not new'd in the ctor.
          */
      void mp_DeInitIO();

      /**
       * Cache for gains
       * Every time a gain parameter is set, this cache is updated
       * On prepare, the cache is sent to the actual matrixmixer (streamapp)
       * The vector is indexed by channel number
       */
      tGains m_gains;
    };

  }
}

#endif //#ifndef __APEX_WAVDEVICEIO_H__
