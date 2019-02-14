/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/device/wavdevicedata.h"

#include "bertha/experimentdata.h"

#include "connection/connection.h"

#include "streamapp/appcore/deleter.h"

#include "streamapp/appcore/threads/locks.h"
#include "streamapp/appcore/threads/thread.h"
#include "streamapp/appcore/threads/waitableobject.h"

#include "streamapp/audioformatstream.h"
#include "streamapp/bufferedprocessor.h"

#include "streamapp/callback/callback.h"
#include "streamapp/callback/streamappcallbacks.h"

#include "streamapp/connections.h"

#include "streamapp/soundcard/bufferdropcheck.h"
#include "streamapp/soundcard/soundcardfactory.h"

#include "streamapp/utils/dataconversion.h"
#include "streamapp/utils/math.h"
#include "streamapp/utils/stringexception.h"
#include "streamapp/utils/stringutils.h"
#include "streamapp/utils/tracer.h"

#include "../gui/soundcardsdialog.h"

#include "berthabuffer.h"
#include "bufferdropcallback.h"
#include "device/soundcardsettings.h"
#include "mixercallback.h"
#include "soundcarddrivers.h"
#include "wavdeviceio.h"

#include "portaudio.h"

#include <QStringList>
#include <QTimer>

//#define PRINTBUFFER
using namespace appcore;
using namespace streamapp;
using namespace apex::stimulus;

//#define PRINTBUFFER

// limits the maximum number of inputs per block (over all channels)
const unsigned WavDeviceIO::sc_nMaxBusInputs = 24;

// const unsigned     WavDeviceIO::sc_nBufferWriteSize = 8192;
// //buffersize of stream system that fills buffer
// const unsigned     WavDeviceIO::sc_nMinBufferSize =
// WavDeviceIO::sc_nBufferWriteSize*2;
const QString WavDeviceIO::sc_sOutputName("_ApexOutput.wav");
const QString WavDeviceIO::sc_sOffLineOutputName("_ApexOffLineOutput.wav");

using namespace apex;

namespace apex
{
namespace stimulus
{

/**
 * WavDeviceBufferThread
 *   this thread takes care of filling the main buffer.
 *   It calls the main callback when notified (by the soundcard
 *   callback or by mp_StartRun(). The main callback is responsible
 *   for reading input files, processing and routing the signals.
 *   All stream objects from the main callback use the same buffer size,
 *   namely sc_nBufferWriteSize.
 *   Note that after a notification, the callback is put in a loop in order
 *   to fill the buffer as much as posiible.
 *   Apart from calling the main callback, the IEofCheck object passed in
 *   the constructor is also executed periodically. It's status is read by
 *   mf_bEofReached(). If the method returns true, all non-looping input
 *   is read. This doesn't mean however that the streaming can stop since
 *   there's still data left in the buffer; instead WavDeviceSoundcardCallback
 *   has the logic to check when all non-looping input is played by the card.
 ******************************************************************************
 */
class WavDeviceBufferThread : public IThread
{
public:
    /**
     * Constructor.
     * @param a_pCheck the object that checks for input being read
     * @param a_pBuffer the buffer
     */
    WavDeviceBufferThread(IEofCheck *a_pCheck, BufferedProcessing *a_pBuffer,
                          unsigned blockSize,
                          const QSharedPointer<BerthaBuffer> &berthaBuffer)
        : IThread("WavDeviceBufferThread"),
          mv_bEof(true),
          mv_bContinuous(false),
          m_pEofChecker(a_pCheck),
          m_pFillBufferCallback(0),
          m_pBuffer(a_pBuffer),
          m_blockSize(blockSize),
          berthaBuffer(berthaBuffer)
    {
        qCDebug(APEX_THREADS, "Constructing WavDeviceBufferThread");
        mp_Start();
    }

    /**
     * Destructor.
     */
    ~WavDeviceBufferThread()
    {
        qCDebug(APEX_THREADS, "Destroying WavDeviceBufferThread");
        mp_Stop(2000);
    }

    /**
o     * Implementation of the IThread method.
     */
    void mp_Run()
    {
        const unsigned nBlockSize = m_blockSize;
        while (!mf_bThreadShouldStop()) {
            mc_WriteLock.mf_Enter(); // lock "this object" (ie StartRun,
                                     // AbortRun, Stopped)
            if (m_pFillBufferCallback) {
                // only write as much as available, so we'll never trigger an
                // overrun
                while (m_pBuffer->mf_nGetNumFree() >= nBlockSize) {
                    if (!mv_bEof || mv_bContinuous) {
                        if (berthaBuffer) {
                            m_pBuffer->mf_GetOutputStream().Write(
                                berthaBuffer->processStream());
                        } else {
                            m_pFillBufferCallback->mf_Callback();
                        }
                    } else {
                        m_pFillBufferCallback = 0;
                        if (berthaBuffer)
                            berthaBuffer->stop();
                        break;
                    }

                    // must use a seperate lock for this, else the caller
                    // of endOfInputReached has to wait on the buffer being
                    // filled
                    mc_EofLock.mf_Enter();
                    if (berthaBuffer) {
                        mv_bEof = berthaBuffer->endOfInputReached();
                    } else {
                        m_pEofChecker->mf_Callback();
                        mv_bEof = m_pEofChecker->mf_bEof();
                    }
                    mc_EofLock.mf_Leave();
                }
            }
            mc_WriteLock.mf_Leave();
            mf_bWait(-1);
        }
    }

    /**
     * Start filling the buffer.
     * @param a_pMainCallback callback responsible for filling
     * @param ac_bContinuous if true, filling continues after eof is reached
     */
    void mp_StartRun(Callback *a_pMainCallback, const bool ac_bContinuous)
    {
        const Lock L(mc_WriteLock);
        mv_bEof = false;
        mv_bContinuous = ac_bContinuous;
        m_pFillBufferCallback = a_pMainCallback;
        mf_NotifyWait();
    }

    /**
     * Stop filling.
     */
    void mp_AbortRun()
    {
        const Lock L(mc_WriteLock);
        m_pFillBufferCallback = 0;
        mv_bEof = true;
    }

    /**
     * Check if we're stopped;
     * this is the case if mp_AbortRun() wasn't called yet,
     * or when eof was reached and not filling continuously.
     * @return
     */
    s_finline bool mf_bStopped() const
    {
        const Lock L(mc_WriteLock);
        return m_pFillBufferCallback == 0;
    }

    /**
     * Get the eof status.
     * @return true if inpt files read completely
     */
    s_finline bool mf_bEofReached() const
    {
        const Lock L(mc_EofLock);
        if (berthaBuffer)
            return berthaBuffer->endOfInputReached();
        else
            return m_pEofChecker->mf_bEof();
    }

    /**
     * Acquire eofcheck lock.
     */
    s_finline void mf_EofCheckLock() const throw()
    {
        mc_EofLock.mf_Enter();
    }

    /**
     * Release eofcheck lock.
     */
    s_finline void mf_EofCheckUnLock() const throw()
    {
        mc_EofLock.mf_Leave();
    }

private:
    bool mv_bEof;
    bool mv_bContinuous;
    IEofCheck *m_pEofChecker;
    Callback *m_pFillBufferCallback;
    BufferedProcessing *m_pBuffer;
    unsigned m_blockSize;
    QSharedPointer<BerthaBuffer> berthaBuffer;

    const CriticalSection mc_WriteLock;
    const CriticalSection mc_EofLock;
};

/************************************************************************************************************/

/**
 * WavDeviceSoundcardCallback
 *   this is the actual soundcard callback, ie it's called
 *   periodically by the soundcard. In turn it calls the
 *   a_pMainCallback set in the constructor, which is responsible
 *   for reading from the buffer and checking for buffer underruns.
 *   The buffersize from this callback is the same as the soundcard's.
 *   Upon every call, there are two other things happening:
 *   - the WavDeviceBufferThread is notified so it will try to add samples
 *     to the buffer.
 *   - WavDeviceBufferThread's mf_bEofReached is called to check if all input
 *     files were read. If this is the case, mv_nLeftBeforeEof is set to the
 *     current number of samples in the buffer: this amount still has to be
 *     passed to the soundcard to make sure everything is played.
 *     mv_nLeftBeforeEof is the decremented after each read, and when it's
 *     zero (which means all input was effectively played), the WaitObject
 *     is triggered to indicate everything is done. If at this point the
 *     mv_bContinuous was set, the main callback will still be called.
 **************************************************************************** */
class WavDeviceSoundcardCallback : public Callback
{
public:
    /**
     * Constructor.
     * @param a_pMainCallback callback that copies samples to soundcard buffers
     * @param a_pThread buffer thread
     * @param a_pBuffer buffer
     */
    WavDeviceSoundcardCallback(Callback *a_pMainCallback,
                               WavDeviceBufferThread *a_pThread,
                               BufferedProcessing *a_pBuffer)
        : mv_bSignaled(false),
          mv_bContinuous(false),
          mv_nLeftBeforeEof(0),
          m_pCopyBufferToSoundcardCallback(a_pMainCallback),
          m_pBuffer(a_pBuffer),
          m_pBufferThread(a_pThread)
    {
        qCDebug(APEX_THREADS, "Constructing WavDeviceSoundcardCallback");
    }

    /**
     * Destructor.
     */
    ~WavDeviceSoundcardCallback()
    {
        qCDebug(APEX_THREADS, "Destroying WavDeviceSoundcardCallback");
    }

    /**
     * Implementation of the Callback method.
     * @note there are two possible buffer underrun conditions:
     * - first one if the soundcard callback takes longer then one soundcard
     *   buffer period (triggered by BufferDropCheck in
     * m_pCopyBufferToSoundcardCallback).
     *   To get around this, BufferedProcessing is used, so the only thing
     *   the soundcard callback does is moving RAM around, which is pretty fast.
     * - second one if trying to read more samples from the big buffer than it
     *   actually contains (triggered by the InputStream from a_pBuffer; this
     *   InputStream is used by m_pCopyBufferToSoundcardCallback). To get around
     * this, playing
     *   only starts when the buffer is full, and afterwards the buffer is kept
     *   as full as possible by trying to fill it at a higher rate then it gets
     *   read from.
     */
    void mf_Callback()
    {
        const Lock L(mc_Lock);
        if (!mv_bSignaled) {
            if (m_pBufferThread->mf_bEofReached()) {
                if (mv_nLeftBeforeEof == 0)
                    mv_nLeftBeforeEof = m_pBuffer->mf_nGetNumInBuffer();
                if (mv_nLeftBeforeEof > 0) {
                    m_pCopyBufferToSoundcardCallback
                        ->mf_Callback(); // as long as there's data in the
                                         // buffer, continue delivering it to
                                         // soundcard
                    m_pBufferThread->mf_NotifyWait(); // also continue
                                                      // signaling: in the
                                                      // "continuous" case,
                                                      // bufferthread just keeps
                                                      // working
#ifdef PRINTBUFFER
                    DBGPF("callback < %d (thread at eof)",
                          m_pBuffer->mf_nGetNumInBuffer());
#endif
                } else {
                    // we got called back by soundcard callback, but there
                    // are no more samples left; last buffer is being played
                    // again
                    DBG("WavdeviceIO: warning got called back after eof, "
                        "soundcard didn't stop in time");
                }

                mv_nLeftBeforeEof -= m_pBuffer->mf_nGetOutputBufferSize();

                // all done..
                if (mv_nLeftBeforeEof == 0) {
                    mc_EofReached.mp_SignalObject(); // all finite file data has
                                                     // been played, so signal
                    mv_bSignaled = true;
#ifdef PRINTBUFFER
                    DBG("callback: eof");
#endif
                }
            } else {
                m_pCopyBufferToSoundcardCallback->mf_Callback(); // this will
                                                                 // trigger
                                                                 // underrun
                                                                 // callback if
                                                                 // needed..
                m_pBufferThread->mf_NotifyWait();
#ifdef PRINTBUFFER
                DBGPF("callback < %d (thread running)",
                      m_pBuffer->mf_nGetNumInBuffer());
#endif
            }
        } else if (mv_bContinuous) {
            m_pCopyBufferToSoundcardCallback
                ->mf_Callback(); // just continue running
            m_pBufferThread->mf_NotifyWait();
#ifdef PRINTBUFFER
            DBGPF("callback < %d (continuous)",
                  m_pBuffer->mf_nGetNumInBuffer());
#endif
        }
    }

    /**
     * Reset status flags.
     * @param ac_bContinuous true if callback should be called after eof
     */
    void mp_Start(const bool ac_bContinuous)
    {
        const Lock L(mc_Lock);
        mv_bSignaled = false;
        mv_bContinuous = ac_bContinuous;
        mv_nLeftBeforeEof = 0;
        mc_EofReached.mp_ResetObject();
    }

    /**
     * Get the waiter that will be signalled when
     * eof is reached.
     * @return a const ref
     */
    const WaitableObject &mf_GetWaiter() const
    {
        return mc_EofReached;
    }

private:
    bool mv_bSignaled;
    bool mv_bContinuous;
    unsigned mv_nLeftBeforeEof;
    Callback *m_pCopyBufferToSoundcardCallback;
    BufferedProcessing *m_pBuffer;
    WavDeviceBufferThread *m_pBufferThread;
    const WaitableObject mc_EofReached;
    const CriticalSection mc_Lock;
};
}
}

/************************************************************************************************************/

apex::stimulus::WavDeviceIO::WavDeviceIO(
    const data::WavDeviceData *const ac_Config,
    const bertha::ExperimentData &data, bool useBertha)
    : mv_bContinuous(false),
      mv_bConnectError(true),
      m_pEofCheck(new EofCheck()),
      m_pConnMan(new ConnectionManager()),
      m_pMainOutput(0),
      m_pCard(0),
      //  m_nDefaultBufSize                 (0),
      m_soundcardBufferSize(0),
      m_blockSize(0),
      m_pSoundcardBufferDropCheck(new BufferDropCheck(false)),
      m_pSoundcardbufferDroppedCallback(new BufferDropCallback("soundcard")),
      m_pBigBufferDroppedCallback(new BufferDropCallback("buffer")),
      m_pSoundcardWriterStream(0),
      m_pBuffer(0),
      m_pSoundcardWriterCallback(new Channel(false)),
      m_pBufferThread(0),
      m_pSoundcardCallback(0),
      mc_Config(*ac_Config),
      berthaData(data),
      useBertha(useBertha)
{
    m_gains.resize(mc_Config.numberOfChannels());
}

WavDeviceIO::~WavDeviceIO()
{
    mp_DeInitIO();
    delete m_pEofCheck;
    // delete what's still here (items added in WavDevice::AddFilter)
    m_pConnMan->mp_DeleteRegisteredItems();
    delete m_pConnMan;
    delete m_pSoundcardBufferDropCheck;
    delete m_pSoundcardbufferDroppedCallback;
    delete m_pBigBufferDroppedCallback;
    delete m_pSoundcardWriterCallback;
}

WavDeviceIO::mt_eOpenStatus WavDeviceIO::mp_eSetSoundcard(
    const apex::data::WavDeviceData &ac_Config, const QString &driver,
    const QString &card, const bool ac_bTryDefaultBufferSize, QString &a_sErr)
{
    QString useDriver(driver);
    if (driver.isEmpty()) {
        useDriver = ac_Config.driverString();
    }

    if (m_pCard)
        Q_ASSERT(0 && "soundcard switching not implemented");

    mt_eOpenStatus eRet = mc_eOK;

    try {
        // create driver
        qCDebug(APEX_RS, "Card name: %s, Driver name: %s", qPrintable(card),
                qPrintable(useDriver));
        m_pCard = SoundCardFactory::CreateSoundCard(
            card, stimulus::fSndStringToEnum(useDriver), a_sErr);
        if (!m_pCard) {
            qCDebug(APEX_RS, "Could not create sound card");
            eRet = mc_eDriverNotExist;
            goto carderror;
        }

        // check specs
        tSoundCardInfo info = m_pCard->mf_GetInfo();
        qCDebug(APEX_RS, "num of sample rates=%u",
                unsigned(info.m_SampleRates.size()));

        // qCDebug(APEX_RS, "nBufSizeUsed=%u", nBufSizeUsed);
        qCDebug(APEX_RS, "num of sample rates=%u",
                unsigned(info.m_SampleRates.size()));
        qCDebug(APEX_RS, "# channels: %i", ac_Config.numberOfChannels());
        if (!info.mf_bCanOutputChannels(ac_Config.numberOfChannels())) {
            eRet = mc_eBadOutputChannel;
            goto carderror;
        }
        if (!info.mf_bCanSampleRate(ac_Config.sampleRate())) {
            eRet = mc_eBadSampleRate;
            goto carderror;
        }

        long nBufSizeUsed = ac_Config.bufferSize();
        //    m_nDefaultBufSize = info.m_nDefaultBufferSize;
        if (ac_Config.bufferSize() != -1 &&
            !info.mf_bCanBufferSize(ac_Config.bufferSize()))
            qCDebug(APEX_RS, "Warning: requested soundcard buffer size (%d) "
                             "not suppored, to the default: %d",
                    ac_Config.bufferSize(), info.m_nDefaultBufferSize);

        if (ac_Config.bufferSize() == -1)
            nBufSizeUsed = info.m_nDefaultBufferSize;
        else if (!info.mf_bCanBufferSize(ac_Config.bufferSize())) {
            if (ac_bTryDefaultBufferSize) {
                nBufSizeUsed = info.m_nDefaultBufferSize;
                eRet = mc_eDefBufferSize;
            } else {
                eRet = mc_eBadBufferSize;
                goto carderror;
            }
        }

        qCDebug(APEX_RS, "Using sound driver buffer size %ld", nBufSizeUsed);
        m_soundcardBufferSize = nBufSizeUsed;

        // Set block size to the same if nothing is requested
        if (ac_Config.blockSize() == -1)
            m_blockSize = m_soundcardBufferSize;
        else
            m_blockSize = ac_Config.blockSize();

        qCDebug(APEX_RS, "Block size: %d", m_blockSize);

        // open driver
        if (!m_pCard->mp_bOpenDriver(0, ac_Config.numberOfChannels(),
                                     ac_Config.sampleRate(), nBufSizeUsed)) {
            eRet = mc_eUnknownError;
            goto carderror;
        }

        m_pSoundcardWriterStream = new AudioFormatWriterStream(
            m_pCard->mf_pCreateWriter(), nBufSizeUsed, true);

        // all ok!
        mp_InitIO();
        return eRet;
    } catch (utils::StringException &e) {
        if (a_sErr.isEmpty())
            a_sErr = QString::fromStdString(e.mc_sError);
        eRet = mc_eInitError;
    }

carderror:
    if (m_pCard) {
        a_sErr = QString::fromStdString(m_pCard->mf_sGetLastError());
        ZeroizingDeleter()(m_pCard);
    }
    return eRet;
}

void WavDeviceIO::mp_InitIO()
{
    // create the buffer
    const double dBufferSizeWanted =
        mc_Config.internalBufferSize() * (double)mc_Config.sampleRate();

    unsigned nBufferSize = math::gf_RoundToMultiple(
        dataconversion::roundDoubleToIntT<unsigned>(dBufferSizeWanted),
        m_blockSize, false);

    if (nBufferSize < 2 * m_blockSize)
        nBufferSize = 2 * m_blockSize;
    //#ifdef PRINTBUFFER
    qCDebug(APEX_RS, "Internal sound buffer size: %d samples", nBufferSize);
    //#endif
    m_pBuffer =
        new BufferedProcessing(m_blockSize, m_soundcardBufferSize,
                               mc_Config.numberOfChannels(), nBufferSize);
    m_pBuffer->mp_InstallBufferUnderrunCallback(m_pBigBufferDroppedCallback);

    // write connection's output to the buffer
    m_pMainOutput = new ApexOutputCallback(&m_pBuffer->mf_GetOutputStream(),
                                           sc_nMaxBusInputs, false);
    if (useBertha)
        berthaBuffer.reset(new BerthaBuffer(berthaData));
    else
        m_pConnMan->mp_RegisterItem(mc_Config.id().toLatin1().data(),
                                    m_pMainOutput);

    // setup the primary soundcard callback;
    // it just reads from the big buffer and
    // copies the sample to the soundcard's output buffers
    m_pSoundcardWriterCallback->mp_SetSink(m_pSoundcardWriterStream);
    m_pSoundcardWriterCallback->mp_SetSource(&m_pBuffer->mf_GetInputStream());

    // place the soundcard callback inside m_pSoundcardBufferDropCheck; it will
    // call m_pCardCallback
    // and check how long it takes; if that's too long, it calls
    // m_pSoundcardbufferDroppedCallback
    // which will in turn leave the user a message.
    m_pSoundcardBufferDropCheck->mp_InstallMainCallback(
        m_pSoundcardWriterCallback);
    m_pSoundcardBufferDropCheck->mp_InstallNoLockCallback(
        m_pSoundcardbufferDroppedCallback);
    m_pSoundcardBufferDropCheck->mp_SetMaxCallbackTime(
        (double)m_soundcardBufferSize / (double)mc_Config.sampleRate());
    m_pSoundcardbufferDroppedCallback->mp_ResetDrops();

    // create buffer thread, it will fill the buffer until it's full
    // or until no more samples are available
    m_pBufferThread = new WavDeviceBufferThread(m_pEofCheck, m_pBuffer,
                                                m_blockSize, berthaBuffer);

    // streaming now shouldn't stop when no more input samples
    // are available, because they are used to read ahead;
    // it should stop when no more input samples are available *and*
    // all remaining samples in the buffer are played.
    // That is what this callback takes care of.
    m_pSoundcardCallback = new WavDeviceSoundcardCallback(
        m_pSoundcardBufferDropCheck, m_pBufferThread, m_pBuffer);
}

void WavDeviceIO::mp_AddConnectItem(PositionableInputStream *a_pItem,
                                    const QString &ac_sID,
                                    const bool ac_bAddToEofCheck)
{
    m_pConnMan->mp_RegisterItem(ac_sID, a_pItem,
                                false); // register but do not play at this time
    if (ac_bAddToEofCheck) {
        if (m_pBufferThread) {
            //[ stijn 29/11/2008 ] this should solve crashes due to thread
            // calling m_pEofCheck->mf_Callback(),
            // while at the same time we're modifying the object here; Also se
            // mp_RemoveCheckedInputs()
            m_pBufferThread->mf_EofCheckLock();
            m_pEofCheck->mp_AddInputToCheck(
                (PositionableInputStream &)(*a_pItem));
            m_pBufferThread->mf_EofCheckUnLock();
        } else {
            m_pEofCheck->mp_AddInputToCheck(
                (PositionableInputStream &)(*a_pItem));
        }
    }
}

void WavDeviceIO::mp_AddConnectItem(ConnectItem *a_pItem, const QString &ac_sID)
{
    m_pConnMan->mp_RegisterItem(ac_sID, a_pItem);
}

void WavDeviceIO::mp_AddConnection(const tConnection &ac_Connection,
                                   const bool /*ac_bMixToFile */)
{
    if (ac_Connection.m_nFromChannel >= 0 && ac_Connection.m_nToChannel >= 0) {
        m_pConnMan->mp_Connect(ac_Connection.m_sFromID.toLatin1().data(),
                               ac_Connection.m_sToID.toLatin1().data(),
                               ac_Connection.m_nFromChannel,
                               ac_Connection.m_nToChannel);
    }
    mv_bConnectError = !m_pConnMan->mf_bAllInputsConnected();

#ifdef PRINTCONNECTIONS
    PrintVector(m_pConnMan->mf_saGetRoutes());
#endif
}

void WavDeviceIO::mp_RemoveConnection(const tConnection &ac_Connection)
{
    m_pConnMan->mp_RemoveConnection(ac_Connection.m_sFromID.toLatin1().data(),
                                    ac_Connection.m_sToID.toLatin1().data(),
                                    ac_Connection.m_nFromChannel,
                                    ac_Connection.m_nToChannel);
    mv_bConnectError = !m_pConnMan->mf_bAllInputsConnected();
#ifdef PRINTCONNECTIONS
    PrintVector(m_pConnMan->mf_saGetRoutes());
#endif
}

void WavDeviceIO::mp_RemoveConnection(const QString &ac_sID)
{
    m_pConnMan->mp_UnregisterItem(ac_sID);
    mv_bConnectError = !m_pConnMan->mf_bAllInputsConnected();
#ifdef PRINTCONNECTIONS
    PrintVector(m_pConnMan->mf_saGetRoutes());
#endif
}

void WavDeviceIO::mp_RewireConnection(tConnection &a_Connection,
                                      const int ac_nNewChannel,
                                      const bool ac_bIsFromConnection)
{
    mp_RemoveConnection(a_Connection);
    if (ac_bIsFromConnection)
        a_Connection.m_nFromChannel = ac_nNewChannel;
    else
        a_Connection.m_nToChannel = ac_nNewChannel;
    if (ac_nNewChannel != (int)sc_nInfinite)
        mp_AddConnection(a_Connection, false);
    mv_bConnectError = !m_pConnMan->mf_bAllInputsConnected();
}

bool WavDeviceIO::mf_bAllInputsConnected() const throw()
{
    return !mv_bConnectError;
}

void WavDeviceIO::mp_RemoveCheckedInputs()
{
    if (m_pBufferThread) {
        m_pBufferThread->mf_EofCheckLock();
        m_pEofCheck->mp_RemoveAllInputs();
        m_pBufferThread->mf_EofCheckUnLock();
    } else {
        m_pEofCheck->mp_RemoveAllInputs();
    }
}

void WavDeviceIO::mp_PausePlay(const QString &ac_sID)
{
    m_pConnMan->mp_PlayItem(ac_sID, false);
}

void WavDeviceIO::mp_Start()
{
    if (!useBertha && mv_bConnectError)
        return;

    if (m_pCard) {
        m_pSoundcardCallback->mp_Start(mv_bContinuous);
        if (useBertha)
            berthaBuffer->start();
        else
            m_pConnMan->mp_PlayAllItems();

        if (!mv_bContinuous)
            m_pBuffer->mp_Reset();
        m_pBufferThread->mp_StartRun(m_pConnMan,
                                     mv_bContinuous); // start buffer thead
        // make sure buffer is filled
        if (!mv_bContinuous) {
            while (m_pBuffer->mf_nGetNumInBuffer() <
                       m_pBuffer->mf_nGetBufferSize() &&
                   !m_pBufferThread->mf_bStopped()) // wait while buffer thread
                                                    // fills the buffer
                IThread::sf_Sleep(10);
        } else if (!m_pCard->mf_bIsRunning()) {
            while (m_pBuffer->mf_nGetNumInBuffer() <
                   m_pBuffer->mf_nGetBufferSize())
                IThread::sf_Sleep(
                    10); // wait while buffer thread fills the buffer
        }

        QTimer::singleShot(1000 * ((double)(m_pBuffer->mf_nGetNumInBuffer() +
                                            m_pCard->mf_nGetBufferSize()) /
                                   (double)mc_Config.sampleRate()),
                           Qt::PreciseTimer, this, SIGNAL(stimulusStarted()));
        m_pCard->mp_bStart(*m_pSoundcardCallback); // start play
    }
}

void WavDeviceIO::mp_Stop()
{
    if (useBertha && mv_bContinuous)
        berthaBuffer->runPermanentLeafNodesOnly();
    if (m_pCard && !mv_bContinuous)
        mp_Finish();
}

void WavDeviceIO::mp_Finish()
{
#ifdef PRINTBUFFER
    DBG("WavDeviceIO: finishing");
#endif
    if (m_pCard && (useBertha || !mv_bConnectError)) {
        m_pCard->mp_bStop();
        // m_pCard->mp_bCloseDriver();
        m_pBufferThread->mp_AbortRun();
    }

    if (useBertha) {
        m_pBuffer->mp_Reset();
        berthaBuffer->release();
    }
}

void WavDeviceIO::mf_WaitUntilDone() const
{
    if (mv_bConnectError && !useBertha)
        return;
    m_pSoundcardCallback->mf_GetWaiter().mf_eWaitForSignal();
}

void WavDeviceIO::mp_SetOutputGain(const double ac_dGain)
{
    qCDebug(APEX_RS, "Wavdevice set gain of all channels to %f", ac_dGain);
    for (int i = 0; i < m_gains.size(); ++i) {
        m_gains[i] = ac_dGain;
    }
}

void WavDeviceIO::mp_SetOutputGain(const double ac_dGain,
                                   const unsigned ac_nChannel)
{
    qCDebug(APEX_RS, "Wavdevice set gain of channel %d to %f", ac_nChannel,
            ac_dGain);
    Q_ASSERT(ac_nChannel < mc_Config.numberOfChannels());
    m_gains[ac_nChannel] = ac_dGain;
}

void WavDeviceIO::Prepare()
{
    Q_ASSERT((unsigned)m_gains.size() == mc_Config.numberOfChannels());
    Q_ASSERT(m_pMainOutput);

    for (int i = 0; i < m_gains.size(); ++i) {
        if (useBertha) {
            berthaBuffer->setGain(i, m_gains[i]);
        } else {
            // streamapp considers values <-150 as mute
            if (m_gains[i] <= -150)
                m_pMainOutput->mp_SetGain(-149.99999999, i);
            else
                m_pMainOutput->mp_SetGain(m_gains[i], i);
        }
    }
}

void WavDeviceIO::Reset()
{
    Q_ASSERT((unsigned)m_gains.size() == mc_Config.numberOfChannels());

    for (int i = 0; i < m_gains.size(); ++i) {
        m_pMainOutput->mp_SetGain(0, i);
        m_gains[i] = 0;
    }
}

void WavDeviceIO::showSoundcardDialog(unsigned sampleRate)
{
    SoundcardSettings scs;
    SoundcardsDialog d;
    WavDeviceIO::addPortaudioDevicesToGui(&d, sampleRate);
    d.displayHostApis();
    if (scs.hasData())
        d.setSelection(scs.hostApi(), scs.device());
    d.exec();
    if (d.result() == QDialog::Accepted) {
        scs.saveDevice(d.hostApiName(), d.cardName());
    }
}

void WavDeviceIO::addPortaudioDevicesToGui(SoundcardsDialog *soundcardsDialog,
                                           unsigned sampleRate)
{

    if (Pa_Initialize())
        return;

    PaHostApiIndex numberOfHostApis = Pa_GetHostApiCount();
    PaHostApiIndex numberOfDevices;
    PaDeviceIndex deviceIndex;
    const PaDeviceInfo *deviceInfo;
    for (PaHostApiIndex hostApiIndex = 0; hostApiIndex < numberOfHostApis;
         hostApiIndex++) {
        QString hostApiName(
            QString::fromLocal8Bit(Pa_GetHostApiInfo(hostApiIndex)->name));
        numberOfDevices = Pa_GetHostApiInfo(hostApiIndex)->deviceCount;
        for (int hostApiDeviceIndex = 0; hostApiDeviceIndex < numberOfDevices;
             hostApiDeviceIndex++) {
            deviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(
                hostApiIndex, hostApiDeviceIndex);
            deviceInfo = Pa_GetDeviceInfo(deviceIndex);

            PaStreamParameters out;
            out.device = deviceIndex;
            out.channelCount = deviceInfo->maxOutputChannels;
            out.hostApiSpecificStreamInfo = 0;
            out.sampleFormat = paInt32;
            out.suggestedLatency = 0.0;

            // Check if any of the most frequently used sample rates are
            // supported, if it is, the device is suitable
            // or if sampleRate is not 0, check if that sampleRate is supported
            if (sampleRate != 0 && deviceIndex != paNoDevice && deviceInfo &&
                deviceInfo->maxOutputChannels > 0 &&
                Pa_IsFormatSupported(0, &out, sampleRate) ==
                    paFormatIsSupported) {
                soundcardsDialog->registerDevice(hostApiName, deviceIndex,
                                                 deviceInfo->name,
                                                 deviceInfo->maxOutputChannels);
            } else if (deviceIndex != paNoDevice && deviceInfo &&
                       deviceInfo->maxOutputChannels > 0 &&
                       (Pa_IsFormatSupported(0, &out, 32000) ==
                            paFormatIsSupported ||
                        Pa_IsFormatSupported(0, &out, 44100) ==
                            paFormatIsSupported ||
                        Pa_IsFormatSupported(0, &out, 48000) ==
                            paFormatIsSupported ||
                        Pa_IsFormatSupported(0, &out, 96000) ==
                            paFormatIsSupported)) {
                soundcardsDialog->registerDevice(hostApiName, deviceIndex,
                                                 deviceInfo->name,
                                                 deviceInfo->maxOutputChannels);
            }
        }
    }
    Pa_Terminate();
}

int WavDeviceIO::deviceNameToPortaudioId(const QString &hostName,
                                         const QString &devName)
{
    // Code inspired on audacity code

    if (Pa_Initialize())
        return -1;

    int deviceId = -1;

    PaHostApiIndex hostCnt = Pa_GetHostApiCount();
    PaHostApiIndex hostNum;
    for (hostNum = 0; hostNum < hostCnt; hostNum++) {
        const PaHostApiInfo *hinfo = Pa_GetHostApiInfo(hostNum);
        if (hinfo && hinfo->name == hostName) {
            for (PaDeviceIndex hostDevice = 0; hostDevice < hinfo->deviceCount;
                 hostDevice++) {
                PaDeviceIndex deviceNum =
                    Pa_HostApiDeviceIndexToDeviceIndex(hostNum, hostDevice);

                const PaDeviceInfo *dinfo = Pa_GetDeviceInfo(deviceNum);
                if (dinfo && dinfo->name == devName &&
                    dinfo->maxOutputChannels > 0) {
                    // this device name matches the stored one, and works.
                    // So we say this is the answer and return it
                    deviceId = deviceNum;
                    break;
                }
            }
        }
    }

    Pa_Terminate();
    return deviceId;
}

void WavDeviceIO::mp_DeInitIO()
{
    ZeroizingDeleter()(m_pCard);
    ZeroizingDeleter()(m_pSoundcardWriterStream);
    ZeroizingDeleter()(m_pSoundcardCallback);
    ZeroizingDeleter()(m_pBuffer);
    ZeroizingDeleter()(m_pBufferThread);
}

QString
WavDeviceIO::sf_sGetErrorString(const WavDeviceIO::mt_eOpenStatus &ac_eReason)
{
    if (ac_eReason == mc_eOK)
        return QString();

    QString sErr("Soundcard: ");

    if (ac_eReason == mc_eBadBufferSize)
        sErr += "wrong buffersize. Check whether your soundcard's setting "
                "corresponds to the apex setting.";
    else if (ac_eReason == mc_eBadOutputChannel)
        sErr += "wrong number of channels.\nCheck whether another program is "
                "blocking the soundcard and\nthat the specified number of "
                "output channels is available on your soundcard.";
    else if (ac_eReason == mc_eBadSampleRate)
        sErr += "wrong samplerate.\nProbably your soundcard doesn't support "
                "the given samplerate.";
    else if (ac_eReason == mc_eInitError)
        sErr += "failed initilizing driver.\nThis usually means there is no "
                "soundcard, or it's already open.";
    else if (ac_eReason == mc_eDriverNotExist)
        sErr += "driver could not be opened.\nMake sure the driver exists on "
                "your platform and that it's services are running.";
    else
        sErr += "unspecified error";

    return sErr;
}

QString sf_sGetDrvrNamePrefix(const gt_eDeviceType ac_eType)
{
    return fSndEnumToString<QString>(ac_eType) + ".";
}

void sf_AppendDrvrNames(const gt_eDeviceType ac_eType, QStringList &a_List)
{
    QString sErr;
    tStringVector saCards(SoundCardFactory::GetDriverNames(ac_eType, sErr));
    const tStringVector::size_type nCards = saCards.size();
    if (nCards) {
        const QString sDrvrPrefixed(sf_sGetDrvrNamePrefix(ac_eType));
        for (tStringVector::size_type i = 0; i < saCards.size(); ++i) {
            QString temp(saCards.at(i).c_str());
            temp.insert(0, sDrvrPrefixed);
            a_List.push_front(temp);
        }
    }
}

QStringList WavDeviceIO::sf_saGetDriverNames()
{
    QStringList list;
    sf_AppendDrvrNames(ASIO, list);
    sf_AppendDrvrNames(PORTAUDIO, list);
    sf_AppendDrvrNames(COREAUDIO, list);
    sf_AppendDrvrNames(JACK, list);
    sf_AppendDrvrNames(DUMMY, list);
    sf_AppendDrvrNames(QTAUDIO, list);
    return list;
}

QString
WavDeviceIO::sf_sGetDriverName(const apex::data::WavDeviceData &ac_Config)
{
    QString sTemp(ac_Config.cardName());
    sTemp.insert(0, sf_sGetDrvrNamePrefix(
                        stimulus::fSndStringToEnum(ac_Config.driverString())));
    return sTemp;
}

void WavDeviceIO::sf_SetDriverName(const QString &ac_sDriverName,
                                   apex::data::WavDeviceData &a_Config)
{
    const int iPos = ac_sDriverName.indexOf(".");
    Q_ASSERT(iPos > 0 && "drivername not created by sf_sGetDriverName??");

    a_Config.setValueByType("driver", ac_sDriverName.left(iPos));
    a_Config.setValueByType(
        "card", ac_sDriverName.right(ac_sDriverName.length() - (iPos + 1)));

    /*a_Config.SetDeviceType(fSndStringToEnum(ac_sDriverName.left(iPos)));
      a_Config.SetParameterByType("driver",ac_sDriverName.right(ac_sDriverName.length()
      - (iPos + 1)));*/
}

int WavDeviceIO::GetBlockSize() const
{
    return m_blockSize;
}
