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

#include "../../appcore/threads/criticalsection.h"
#include "../../appcore/threads/thread.h"

#include "../../audioformat.h"

#include "../../containers/matrix.h"

#include "../../utils/dataconversion.h"
#include "../../utils/stringexception.h"
#include "../../utils/vectorutils.h"

#include "osx_coreaudiowrapper.h"

#include <CoreAudio/AudioHardware.h>

#include <iostream>

using namespace utils;
using namespace appcore;
using namespace streamapp;
using namespace dataconversion;

namespace streamapp
{

/**
  * tCoreAudioUserData
  *   an instance of this struct gets passed
  *   to the CoreAudio callback.
  ****************************************** */
struct tCoreAudioUserData {
    /**
      * Constructor.
      */
    tCoreAudioUserData()
        : m_DevID(0),
          m_nMaxInputChan(0),
          m_nMaxOutputChan(0),
          m_pCallback(0),
          m_nBufferSize(0),
          m_nIChan(0),
          m_nOChan(0),
          m_nSampleRate(0),
          m_bRunning(false),
          m_pInput(0),
          m_pOutput(0)
    {
    }

    /**
      * Destructor.
      */
    ~tCoreAudioUserData()
    {
        mp_DeAllocateMemory();
    }

    /**
      * tStreamInfo
      *   info for sample streams passed to
      *   the CoreAudio callback.
      ************************************* */
    struct tStreamInfo {
        int sourceChannelNum;
        int m_nStreamNum;
        int m_nOffset;
        int m_nStride;
    };

    /**
      * Get all stream info.
      */
    void mp_FillChannelInfos()
    {
        if (m_nMaxInputChan && m_nMaxOutputChan)
            return;
        memset(m_InputInfo, 0, sizeof(m_InputInfo));
        memset(m_OutputInfo, 0, sizeof(m_OutputInfo));
        mp_FillChannelInfos(true);
        mp_FillChannelInfos(false);
    }

    /**
      * Get stream information.
      * We need this in the callback, since channels might
      * be interleaved etc, depending on driver model.
      * @param ac_bAsInput true for input, else output
      */
    void mp_FillChannelInfos(const bool ac_bAsInput)
    {
        UInt32 nSize;
        unsigned nChannels = 0;
        if (AudioDeviceGetPropertyInfo(m_DevID, 0, ac_bAsInput,
                                       kAudioDevicePropertyStreamConfiguration,
                                       &nSize, 0) == noErr) {
            AudioBufferList *const pBufList =
                (AudioBufferList *)::malloc(nSize);
            if (AudioDeviceGetProperty(m_DevID, 0, ac_bAsInput,
                                       kAudioDevicePropertyStreamConfiguration,
                                       &nSize, pBufList) == noErr) {
                const int nStreams = pBufList->mNumberBuffers;
                for (int i = 0; i < nStreams; ++i) {
                    const AudioBuffer &b = pBufList->mBuffers[i];
                    for (unsigned j = 0; j < b.mNumberChannels; ++j) {
                        if (ac_bAsInput) {
                            m_InputInfo[nChannels].m_nStreamNum = i;
                            m_InputInfo[nChannels].m_nOffset = j;
                            m_InputInfo[nChannels].m_nStride =
                                b.mNumberChannels;
                        } else {
                            m_OutputInfo[nChannels].m_nStreamNum = i;
                            m_OutputInfo[nChannels].m_nOffset = j;
                            m_OutputInfo[nChannels].m_nStride =
                                b.mNumberChannels;
                        }
                        ++nChannels;
                    }
                }
            }
            ::free(pBufList);
        }
        ac_bAsInput ? m_nMaxInputChan = nChannels
                    : m_nMaxOutputChan = nChannels;
    }

    /**
      * Reset specs.
      */
    void mp_Reset()
    {
        m_nIChan = 0;
        m_nOChan = 0;
        m_nMaxInputChan = 0;
        m_nMaxOutputChan = 0;
        m_nSampleRate = 0;
        m_nBufferSize = 0;
    }

    /**
      * Allocate sample memory.
      */
    void mp_AllocateMemory()
    {
        mp_DeAllocateMemory();
        if (m_nIChan)
            m_pInput = new mt_SampleBuf(m_nIChan, m_nBufferSize, true);
        if (m_nOChan)
            m_pOutput = new mt_SampleBuf(m_nOChan, m_nBufferSize, true);
    }

    /**
      * Deallocate sample memory.
      */
    void mp_DeAllocateMemory()
    {
        delete m_pInput;
        delete m_pOutput;
        m_pInput = 0;
        m_pOutput = 0;
    }

    static const unsigned smc_nMaxNumChan = 32;
    typedef MatrixStorage<float> mt_SampleBuf;

    AudioDeviceID m_DevID;

    tStreamInfo m_InputInfo[smc_nMaxNumChan];
    tStreamInfo m_OutputInfo[smc_nMaxNumChan];
    unsigned m_nMaxInputChan;
    unsigned m_nMaxOutputChan;

    Callback *m_pCallback;
    unsigned m_nBufferSize;
    unsigned m_nIChan;
    unsigned m_nOChan;
    unsigned m_nSampleRate;
    bool m_bRunning;
    bool m_bClearBuffers;
    mt_SampleBuf *m_pInput;
    mt_SampleBuf *m_pOutput;

    const CriticalSection mc_Lock;
};

OSStatus sf_CoreAudioCB(AudioDeviceID, const AudioTimeStamp *,
                        const AudioBufferList *a_pInputData,
                        const AudioTimeStamp *, AudioBufferList *a_pOutputData,
                        const AudioTimeStamp *, void *a_pData)
{
    tCoreAudioUserData *p = (tCoreAudioUserData *)a_pData;
    p->mc_Lock.mf_Enter();
    if (p->m_pCallback) {
        // copy input
        for (unsigned i = 0; i < p->m_nIChan; ++i) {
            const tCoreAudioUserData::tStreamInfo &tInfo = p->m_InputInfo[i];
            float *pDest = p->m_pInput->mf_pGetArray()[i];
            const float *pSrc =
                ((const float *)a_pInputData->mBuffers[tInfo.m_nStreamNum]
                     .mData) +
                tInfo.m_nOffset;
            const int nStride = tInfo.m_nStride;
            for (unsigned j = 0; j < p->m_nBufferSize; ++j) {
                *pDest++ = *pSrc;
                pSrc += nStride;
            }
        }

        // process
        p->m_pCallback->mf_Callback();

        // copy output
        for (unsigned i = 0; i < p->m_nOChan; ++i) {
            const tCoreAudioUserData::tStreamInfo &tInfo = p->m_OutputInfo[i];
            const float *pSrc = p->m_pOutput->mf_pGetArray()[i];
            float *pDest =
                ((float *)a_pOutputData->mBuffers[tInfo.m_nStreamNum].mData) +
                tInfo.m_nOffset;
            const int nStride = tInfo.m_nStride;
            for (unsigned j = 0; j < p->m_nBufferSize; ++j) {
                *pDest = *pSrc++;
                pDest += nStride;
            }
        }
    }
    p->mc_Lock.mf_Leave();
    return noErr;
}

/**
  * CoreAudioFormatReader
  *   implements AudioFormatReader for CoreAudioWrapper.
  ****************************************************** */
class CoreAudioFormatReader : public AudioFormatReader
{
public:
    CoreAudioFormatReader(tCoreAudioUserData &a_Parent) : m_Source(a_Parent)
    {
    }

    ~CoreAudioFormatReader()
    {
    }

    unsigned mf_nChannels() const
    {
        return m_Source.m_nIChan;
    }
    unsigned long mf_lSampleRate() const
    {
        return m_Source.m_nSampleRate;
    }
    AudioFormat::mt_eBitMode mf_eBitMode() const
    {
        return MSBint32;
    }

    unsigned long Read(void **a_pBuf, const unsigned ac_nSamples)
    {
        if (ac_nSamples != m_Source.m_nBufferSize)
            return 0; // too bad..
        int **pSrc = (int **)a_pBuf;
        float **pDest = m_Source.m_pInput->mf_pGetArray();
        for (unsigned i = 0; i < m_Source.m_nIChan; ++i)
            for (unsigned j = 0; j < m_Source.m_nBufferSize; ++j)
                pSrc[i][j] = (int)(pDest[i][j] * sc_f32BitMinMax);
        return ac_nSamples;
    }

private:
    tCoreAudioUserData &m_Source;

    CoreAudioFormatReader(const CoreAudioFormatReader &);
    CoreAudioFormatReader &operator=(const CoreAudioFormatReader &);
};

/**
  * CoreAudioFormatWriter
  *   implements AudioFormatWriter for CoreAudioWrapper.
  ****************************************************** */
class CoreAudioFormatWriter : public AudioFormatWriter
{
public:
    CoreAudioFormatWriter(tCoreAudioUserData &a_Parent) : m_Source(a_Parent)
    {
    }
    ~CoreAudioFormatWriter()
    {
    }

    unsigned mf_nChannels() const
    {
        return m_Source.m_nOChan;
    }
    unsigned long mf_lSampleRate() const
    {
        return m_Source.m_nSampleRate;
    }
    AudioFormat::mt_eBitMode mf_eBitMode() const
    {
        return MSBint32;
    }

    unsigned long Write(const void **a_pBuf, const unsigned ac_nSamples)
    {
        (void)a_pBuf;
        if (ac_nSamples != m_Source.m_nBufferSize)
            return 0; // too bad..
#ifndef TEST_JACK_OUTPUT
        int **pSrc = (int **)a_pBuf;
        float **pDest = m_Source.m_pOutput->mf_pGetArray();
        for (unsigned i = 0; i < m_Source.m_nOChan; ++i)
            for (unsigned j = 0; j < m_Source.m_nBufferSize; ++j)
                pDest[i][j] = (float)(pSrc[i][j]) / sc_f32BitMinMax;
#endif
        return ac_nSamples;
    }

private:
    tCoreAudioUserData &m_Source;

    CoreAudioFormatWriter(const CoreAudioFormatWriter &);
    CoreAudioFormatWriter &operator=(const CoreAudioFormatWriter &);
};
}

/*******************************************************************************************************/

tCoreAudioPersistentData CoreAudioWrapper::sm_Data;

CoreAudioWrapper::CoreAudioWrapper(const std::string &ac_sDrvName)
    : ISoundCard(), m_pData(0), mv_bOpen(0)
{
    if (ac_sDrvName == sc_sDefault) {
        UInt32 nSize = sizeof(m_pData->m_DevID);
        if (AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
                                     &nSize, &m_pData->m_DevID) != noErr)
            throw(utils::StringException(
                "CoreAudioWrapper: couldn't get default device ID"));
    } else {
        std::string sDummy;
        sf_saGetDriverNames(sDummy);
        bool bGotIt = false;
        const tStringVector::size_type nDevs = sm_Data.m_Names.size();
        if (!nDevs)
            throw(utils::StringException(
                "CoreAudioWrapper: no audio devices found"));
        for (tStringVector::size_type i = 0; i < nDevs; ++i) {
            if (sm_Data.m_Names[i] == ac_sDrvName) {
                bGotIt = true;
                m_pData->m_DevID = sm_Data.m_IDs[i];
                break;
            }
        }
        if (!bGotIt)
            throw(utils::StringException(
                "CoreAudioWrapper: couldn't find driver for " + ac_sDrvName));
    }
    m_pData = new tCoreAudioUserData();
}

CoreAudioWrapper::~CoreAudioWrapper()
{
    mp_bCloseDriver();
    delete m_pData;
}

tSoundCardInfo CoreAudioWrapper::mf_GetInfo() const
{
    tSoundCardInfo Ret;

    m_pData->mp_Reset();

    Float64 sr;
    UInt32 nSize = sizeof(sr);
    if (AudioDeviceGetProperty(m_pData->m_DevID, 0, false,
                               kAudioDevicePropertyNominalSampleRate, &nSize,
                               &sr) == noErr) {
        Ret.m_SampleRates.push_back((unsigned long)sr);
        m_pData->m_nSampleRate = (unsigned long)sr;
    }

    UInt32 bs;
    nSize = sizeof(bs);
    if (AudioDeviceGetProperty(m_pData->m_DevID, 0, false,
                               kAudioDevicePropertyBufferFrameSize, &nSize,
                               &bs) == noErr) {
        Ret.m_BufferSizes.push_back(bs);
        m_pData->m_nBufferSize = bs;
    }

    if (AudioDeviceGetPropertyInfo(
            m_pData->m_DevID, 0, false,
            kAudioDevicePropertyUsesVariableBufferFrameSizes, &nSize,
            0) == noErr)
        std::cout << "CoreAudioWrapper: warning buffersize might change and "
                     "this is not implemented"
                  << std::endl;

    m_pData->mp_FillChannelInfos();
    Ret.m_nMaxInputChannels = m_pData->m_nMaxInputChan;
    Ret.m_nMaxOutputChannels = m_pData->m_nMaxOutputChan;

    return Ret;
}

tStringVector CoreAudioWrapper::sf_saGetDriverNames(std::string &a_sError)
{
    if (!sm_Data.m_Names.empty())
        return sm_Data.m_Names;

    UInt32 nSize;
    if (AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &nSize,
                                     0) == noErr) {
        AudioDeviceID *const pDevs = (AudioDeviceID *)::malloc(nSize);
        if (AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &nSize,
                                     pDevs) == noErr) {
            const unsigned nDevs = nSize / sizeof(AudioDeviceID);
            for (unsigned i = 0; i < nDevs; ++i) {
                char name[1024];
                nSize = sizeof(name);
                if (AudioDeviceGetProperty(pDevs[i], 0, false,
                                           kAudioDevicePropertyDeviceName,
                                           &nSize, name) == noErr) {
                    sm_Data.m_Names.push_back(name);
                    sm_Data.m_IDs.push_back(pDevs[i]);
                } else {
                    a_sError = "CoreAudioWrapper: "
                               "kAudioDevicePropertyDeviceName failed";
                }
            }
            if (!nDevs)
                a_sError = "CoreAudioWrapper: no audio devices found";
        } else {
            a_sError = "CoreAudioWrapper: kAudioHardwarePropertyDevices failed";
        }
        ::free(pDevs);
    } else {
        a_sError =
            "CoreAudioWrapper: kAudioHardwarePropertyDevices info failed";
    }

    return sm_Data.m_Names;
}

bool CoreAudioWrapper::mp_bCloseDriver()
{
    bool bSuccess = true;
    if (mv_bOpen) {
        bSuccess = mp_bStop();
        mv_bOpen = false;
    }

    if (!bSuccess)
        mv_sError = "Failed mf_bCloseDriver!!";

    return bSuccess;
}

unsigned CoreAudioWrapper::mf_nGetBufferSize() const
{
    return m_pData->m_nBufferSize;
}

unsigned CoreAudioWrapper::mf_nGetIChan() const
{
    return m_pData->m_nIChan;
}

unsigned CoreAudioWrapper::mf_nGetOChan() const
{
    return m_pData->m_nOChan;
}

unsigned long CoreAudioWrapper::mf_lGetSampleRate() const
{
    return (unsigned long)m_pData->m_nSampleRate;
}

unsigned long CoreAudioWrapper::mf_lGetEstimatedLatency() const
{
    return (unsigned long)0;
}

AudioFormat::mt_eBitMode CoreAudioWrapper::mf_eGetBitMode() const
{
    return AudioFormat::MSBint32;
}

void CoreAudioWrapper::mp_ClearIOBuffers()
{
    // not necessary
}

bool CoreAudioWrapper::mp_bStart(Callback &a_Callback)
{
    if (mv_bOpen) {
        if (AudioDeviceAddIOProc(m_pData->m_DevID, sf_CoreAudioCB, m_pData) ==
            noErr) {
            if (AudioDeviceStart(m_pData->m_DevID, sf_CoreAudioCB) == noErr) {
                m_pData->mc_Lock.mf_Enter();
                m_pData->m_pCallback = &a_Callback;
                m_pData->m_bRunning = true;
                m_pData->mc_Lock.mf_Leave();
            } else {
                AudioDeviceRemoveIOProc(m_pData->m_DevID, sf_CoreAudioCB);
                return false;
            }
            return true;
        }
    }
    return false;
}

bool CoreAudioWrapper::mp_bStop(void)
{
    if (mv_bOpen) {
        m_pData->mc_Lock.mf_Enter();
        m_pData->m_bRunning = false;
        m_pData->m_pCallback = 0;
        m_pData->mc_Lock.mf_Leave();
        AudioDeviceStop(m_pData->m_DevID, sf_CoreAudioCB);
        AudioDeviceRemoveIOProc(m_pData->m_DevID, sf_CoreAudioCB);
        return true;
    }
    return false;
}

bool CoreAudioWrapper::mf_bIsRunning() const
{
    return false;
}

AudioFormatReader *CoreAudioWrapper::mf_pCreateReader() const
{
    if (mv_bOpen)
        return new CoreAudioFormatReader(*m_pData);
    return 0;
}

AudioFormatWriter *CoreAudioWrapper::mf_pCreateWriter() const
{
    if (mv_bOpen)
        return new CoreAudioFormatWriter(*m_pData);
    return 0;
}

bool CoreAudioWrapper::mp_bOpenDriver(const unsigned ac_nIChan,
                                      const unsigned ac_nOChan,
                                      const unsigned long ac_nFs,
                                      const unsigned ac_nBufferSize)
{
    mp_bStop();
    mv_bOpen = false;

    // specs are all fixed so just check against them..
    // but make sure they are available!
    mf_GetInfo();
    if (ac_nIChan > m_pData->m_nMaxInputChan)
        return false;
    if (ac_nOChan > m_pData->m_nMaxOutputChan)
        return false;
    if (ac_nFs != m_pData->m_nSampleRate)
        return false;
    if (ac_nBufferSize != m_pData->m_nBufferSize)
        return false;

    m_pData->m_nIChan = ac_nIChan;
    m_pData->m_nOChan = ac_nOChan;
    m_pData->mp_AllocateMemory();

    return mv_bOpen = true;
}
