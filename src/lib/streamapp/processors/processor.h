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

/**
  * @file processor.h
  * Contains some processors used in streamapp.
  * Most are just for demo.
  */

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "../appcore/threads/locks.h"

#include "../audiosamplebuffer.h"
#include "../stream.h"

#include "../utils/checks.h"
#include "../utils/dataconversion.h"

#include <QtGlobal>

namespace streamapp
{

/**
  * IGainProcessor
  *   simple base class for processors that work
  *   by applying gain, where the gain is calculated
  *   for every sample.
  ************************************************** */
class IGainProcessor : public StreamProcessor
{
public:
    /**
      * Constructor.
      * @param ac_nChan #channels
      * @param ac_nSize #samples
      */
    IGainProcessor(const unsigned ac_nChan, const unsigned ac_nSize)
        : StreamProcessor(ac_nChan, ac_nSize), m_Result(ac_nChan, ac_nSize)
    {
        mp_Reset();
    }

    /**
      * Destructor.
      */
    virtual ~IGainProcessor()
    {
    }

    /**
      * Implementation of the StreamProcessor method.
      */
    INLINE const Stream &mf_DoProcessing(const Stream &ac_Output)
    {
        if (mv_bDone)
            return ac_Output;
        const unsigned nSamps = ac_Output.mf_nGetBufferSize();
        const unsigned nChan = ac_Output.mf_nGetChannelCount();
        for (unsigned i = 0; i < nSamps; ++i) {
            const double dVal(mf_dGetCurGain());
            ++mv_dSamples;
            for (unsigned j = 0; j < nChan; ++j)
                m_Result.mp_Set(j, i, dVal * ac_Output(j, i));
        }
        return m_Result;
    }

    /**
      * Called to reset parameters.
      */
    virtual void mp_Reset()
    {
        mv_bDone = false;
        mv_dSamples = 0.0;
    }

    /**
      * Called to get current gain value.
      * Implementations normally use current sample position
      * to calculate a new gain value. They can also set
      * mv_bDone to true, indicating no processing should
      * take place anymore.
      * @return gain to multiply samples of all channels with
      */
    virtual double mf_dGetCurGain() = 0;

protected:
    bool mv_bDone;
    StreamType mv_dSamples;
    StreamBuf m_Result;
};

class RunningAverage : public IStreamProcessor
{
public:
    RunningAverage(const unsigned ac_nChannels, const unsigned ac_nBufferSize)
        : m_Result(ac_nChannels, ac_nBufferSize),
          mv_dCount(0.0),
          mv_bReset(true)
    {
    }
    ~RunningAverage()
    {
    }

    const Stream &mf_DoProcessing(const Stream &ac_Source)
    {
        const unsigned nChan = m_Result.mf_nGetChannelCount();
        const unsigned nSize = m_Result.mf_nGetBufferSize();
        ++mv_dCount;
        Q_ASSERT(ac_Source.mf_nGetChannelCount() >= nChan);
        Q_ASSERT(ac_Source.mf_nGetBufferSize() >= nSize);

        if (mv_bReset) {
            for (unsigned i = 0; i < nChan; ++i)
                for (unsigned j = 0; j < nSize; ++j)
                    m_Result.mp_Set(i, j, ac_Source(i, j));

            mv_bReset = false;
        } else {
            for (unsigned i = 0; i < nChan; ++i) {
                for (unsigned j = 0; j < nSize; ++j) {
                    const StreamType dPrev = m_Result(i, j) * (mv_dCount - 1.0);
                    const StreamType dNewv =
                        (ac_Source(i, j) + dPrev) / mv_dCount;
                    m_Result.mp_Set(i, j, dNewv);
                }
            }
        }
        return m_Result;
    }

    void mp_Reset()
    {
        mv_bReset = true;
        mv_dCount = 0.0;
    }

    const Stream &mf_Result() const
    {
        return m_Result;
    }

private:
    StreamBuf m_Result;
    StreamType mv_dCount;
    bool mv_bReset;
};

/**
  * IOProcessor
  *   input is sent to processor, output is read from InputStream
  *   This is more efficient then using a StreamAppChannel or routing
  *   warning: no check for buffersize/channels match
  ******************************************************************* */
class IOProcessor : public IStreamProcessor
{
public:
    IOProcessor(IStreamProcessor *const ac_pProc, InputStream *const ac_pStream,
                const bool ac_bDeleteContent = false)
        : mc_bDeleteContent(ac_bDeleteContent),
          m_pProcessor(utils::PtrCheck(ac_pProc)),
          m_pOutput(utils::PtrCheck(ac_pStream))
    {
    }

    //! use with care: don't call getters until proc and stream are set!!
    IOProcessor(const bool ac_bDeleteContent = false)
        : mc_bDeleteContent(ac_bDeleteContent), m_pProcessor(0), m_pOutput(0)
    {
    }

    ~IOProcessor()
    {
        const appcore::Lock L(mc_ProcLock);
        if (mc_bDeleteContent) {
            delete m_pProcessor;
            delete m_pOutput;
        }
    }

    const Stream &mf_DoProcessing(const Stream &ac_Input)
    {
        Q_ASSERT(m_pOutput && m_pProcessor);
        const appcore::Lock L(mc_ProcLock);
        m_pProcessor->mf_DoProcessing(ac_Input);
        return m_pOutput->Read();
    }

    unsigned mf_nGetBufferSize() const
    {
        Q_ASSERT(m_pOutput);
        return m_pOutput->mf_nGetBufferSize();
    }
    unsigned mf_nGetNumInputChannels() const
    {
        Q_ASSERT(m_pProcessor);
        return m_pProcessor->mf_nGetNumInputChannels();
    }
    unsigned mf_nGetNumOutputChannels() const
    {
        Q_ASSERT(m_pOutput);
        return m_pOutput->mf_nGetNumChannels();
    }

    void mp_ReplaceOutput(InputStream *const ac_pStream)
    {
        if (ac_pStream) {
            const appcore::Lock L(mc_ProcLock);
            if (mc_bDeleteContent)
                delete m_pOutput;
            m_pOutput = ac_pStream;
        }
    }

    void mp_ReplaceProcessor(IStreamProcessor *const ac_pProc)
    {
        if (ac_pProc) {
            const appcore::Lock L(mc_ProcLock);
            if (mc_bDeleteContent)
                delete m_pProcessor;
            m_pProcessor = ac_pProc;
        }
    }

    InputStream *mf_pGetOutput() const
    {
        return m_pOutput;
    }

    IStreamProcessor *mf_pGetProcessor() const
    {
        return m_pProcessor;
    }

private:
    const bool mc_bDeleteContent;
    IStreamProcessor *m_pProcessor;
    InputStream *m_pOutput;
    const appcore::CriticalSection mc_ProcLock;
};

/**
    * IOProcessor2
    *   input is sent to OutputStream, output is read from InputStream
    *   This is more efficient then using a StreamAppChannel or routing
    ******************************************************************* */
class IOProcessor2 : public IStreamProcessor
{
public:
    IOProcessor2(OutputStream *const ac_pOStream,
                 InputStream *const ac_pIStream,
                 const bool ac_bDeleteContent = false)
        : mc_bDeleteContent(ac_bDeleteContent),
          m_pRead(utils::PtrCheck(ac_pIStream)),
          m_pWrite(utils::PtrCheck(ac_pOStream))
    {
    }

    IOProcessor2(const bool ac_bDeleteContent = false)
        : mc_bDeleteContent(ac_bDeleteContent), m_pRead(0), m_pWrite(0)
    {
    }

    ~IOProcessor2()
    {
        const appcore::Lock L(mc_Lock);
        if (mc_bDeleteContent) {
            delete m_pRead;
            delete m_pWrite;
        }
    }

    const Stream &mf_DoProcessing(const Stream &ac_Input)
    {
        Q_ASSERT(m_pRead && m_pWrite);
        const appcore::Lock L(mc_Lock);
        m_pWrite->Write(ac_Input);
        return m_pRead->Read();
    }

    unsigned mf_nGetBufferSize() const
    {
        Q_ASSERT(m_pRead);
        return m_pRead->mf_nGetBufferSize();
    }
    unsigned mf_nGetNumInputChannels() const
    {
        Q_ASSERT(m_pWrite);
        return m_pWrite->mf_nGetNumChannels();
    }
    unsigned mf_nGetNumOutputChannels() const
    {
        Q_ASSERT(m_pRead);
        return m_pRead->mf_nGetNumChannels();
    }

    void mp_ReplaceReader(InputStream *const ac_pStream)
    {
        if (ac_pStream) {
            const appcore::Lock L(mc_Lock);
            if (mc_bDeleteContent)
                delete m_pRead;
            m_pRead = ac_pStream;
        }
    }

    void mp_ReplaceWriter(OutputStream *const ac_pProc)
    {
        if (ac_pProc) {
            const appcore::Lock L(mc_Lock);
            if (mc_bDeleteContent)
                delete m_pWrite;
            m_pWrite = ac_pProc;
        }
    }

    InputStream *mf_pGetOutput() const
    {
        return m_pRead;
    }

    OutputStream *mf_pGetInput() const
    {
        return m_pWrite;
    }

private:
    const bool mc_bDeleteContent;
    InputStream *m_pRead;
    OutputStream *m_pWrite;
    const appcore::CriticalSection mc_Lock;
};

/**
  * Multiplier
  *   used eg as filter in frequency domain
  ***************************************** */
class Multiplier : public StreamProcessor
{
public:
    Multiplier(const unsigned ac_nChannels, const unsigned ac_nBufferSize)
        : StreamProcessor(ac_nChannels, ac_nBufferSize),
          m_Coefficients(ac_nChannels, ac_nBufferSize),
          m_Result(ac_nChannels, ac_nBufferSize)
    {
    }
    ~Multiplier()
    {
    }

    Stream &mp_Coefficients()
    {
        return m_Coefficients;
    }

    const Stream &mf_DoProcessing(const Stream &ac_ToProcess)
    {
        StreamType **mul = m_Coefficients.mf_pGetArray();
        StreamType **src = ac_ToProcess.mf_pGetArray();
        StreamType **dst = m_Result.mf_pGetArray();

        for (unsigned i = 0; i < mc_nChan; ++i) {
            for (unsigned j = 0; j < mc_nSize; ++j) {
                dst[i][j] = mul[i][j] * src[i][j];
            }
        }

        return m_Result;
    }

    void mf_DoProcessReplacing(Stream &ac_ToProcess)
    {
        StreamType **mul = m_Coefficients.mf_pGetArray();
        StreamType **src = ac_ToProcess.mf_pGetArray();

        for (unsigned i = 0; i < mc_nChan; ++i) {
            for (unsigned j = 0; j < mc_nSize; ++j) {
                src[i][j] = mul[i][j] * src[i][j];
            }
        }
    }

private:
    StreamBuf m_Coefficients;
    StreamBuf m_Result;
};

/**
  * Subtractor
  *   subtracts lower channels from upper channels
  *   returns result which is half of the channels
  ************************************************ */
class Subtractor : public StreamProcessor
{
public:
    Subtractor(const unsigned ac_nChannels, const unsigned ac_nBufferSize)
        : StreamProcessor(ac_nChannels, ac_nBufferSize),
          m_Result(ac_nChannels / 2, ac_nBufferSize)
    {
        Q_ASSERT(ac_nChannels % 2 == 0); // only even!
    }
    ~Subtractor()
    {
    }

    const Stream &mf_DoProcessing(const Stream &ac_ToProcess)
    {
        StreamType **src = ac_ToProcess.mf_pGetArray();
        StreamType **dst = m_Result.mf_pGetArray();

        const unsigned nHalfChan = mc_nChan / 2;
        for (unsigned i = 0; i < nHalfChan; ++i) {
            for (unsigned j = 0; j < mc_nSize; ++j) {
                dst[i][j] = src[i + nHalfChan][j] - src[i][j];
            }
        }

        return m_Result;
    }

private:
    StreamBuf m_Result;
};

/**
  * Amplifier
  *   all gains in dB
  *   @note always does replacing process!!
  ***************************************** */
typedef ArrayStorage<StreamType> tGains;
class Amplifier : public StreamProcessor
{
public:
    Amplifier(const unsigned ac_nChan, const unsigned ac_nSize,
              const bool ac_bInit = true)
        : StreamProcessor(ac_nChan, ac_nSize), mv_dGains(ac_nChan)
    {
        if (ac_bInit)
            mp_SetGain(1.0);
    }

    virtual ~Amplifier()
    {
    }

    virtual const Stream &mf_DoProcessing(const Stream &ac_Output)
    {
        const appcore::Lock L(mc_Lock);
        AudioSampleBuffer b(ac_Output);
        for (unsigned i = 0; i < StreamProcessor::mf_nGetNumInputChannels();
             ++i)
            b.mp_ApplyGain(i, mv_dGains(i));
        return ac_Output;
    }

    void mp_SetGain(const unsigned ac_nChannel, const StreamType ac_dGain)
    {
        const appcore::Lock L(mc_Lock);
        mv_dGains.mp_Set(ac_nChannel, dataconversion::gf_dBtoLinear(ac_dGain));
    }

    void mp_SetGain(const StreamType ac_dGain)
    {
        const appcore::Lock L(mc_Lock);
        for (unsigned i = 0; i < mv_dGains.mf_nGetBufferSize(); ++i)
            mv_dGains.mp_Set(i, dataconversion::gf_dBtoLinear(ac_dGain));
    }

    void mp_SetGain(const ArrayAccess<StreamType> &ac_Gains)
    {
        const appcore::Lock L(mc_Lock);
        mv_dGains.mp_CopyFrom(ac_Gains);
    }

private:
    tGains mv_dGains;
    const appcore::CriticalSection mc_Lock;
};

/**
  * AmplifierNR
  *   same as above but not replacing
  *********************************** */
class AmplifierNR : public Amplifier
{
public:
    AmplifierNR(const unsigned ac_nChan, const unsigned ac_nSize,
                const bool ac_bInit = true)
        : Amplifier(ac_nChan, ac_nSize, ac_bInit), m_Buf(ac_nChan, ac_nSize)
    {
    }

    virtual ~AmplifierNR()
    {
    }

    virtual const Stream &mf_DoProcessing(const Stream &ac_Output)
    {
        m_Buf.mp_CopyFrom(ac_Output);
        return Amplifier::mf_DoProcessing(m_Buf);
    }

private:
    StreamBuf m_Buf;
};
}

#endif //#ifndef __PROCESSOR_H__
