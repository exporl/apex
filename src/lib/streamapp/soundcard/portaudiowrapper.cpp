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

#include "soundcard/portaudiowrapper.h"
#include "appcore/singleton.h"
#include "containers/matrix.h"
#include "utils/stringexception.h"
#include "utils/stringutils.h"
#include <iostream>

#include <portaudio.h>

#include <QDebug>

#include <iostream>

Q_LOGGING_CATEGORY(soundcard, "apex.soundcard")

using namespace utils;
using namespace streamapp;

#ifdef S_MAC
#define S_MACPA // define to open all cards with PaFloat
#endif

namespace streamapp
{

/**
 * PaUserData
 *   data passed to the PaUserCallback.
 *   Member of PortAudioWrapper, will be accessed by Reader/Writer to get ptr to
 *input/output.
 *********************************************************************************************
 */
struct PaUserData {
    const void *m_pInput;
    void *m_pOutput;
    unsigned m_nIChan;
    unsigned m_nOChan;
    unsigned long m_nBufferSize;
    unsigned long m_nSampleRate;
    Callback *m_pCallback;
};

/**
 * Portaudio callback.
 */
int PaCallback(const void *inputBuffer, void *outputBuffer,
               unsigned long framesPerBuffer,
               const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags,
               void *userData)
{
    Q_UNUSED(framesPerBuffer);
    PaUserData *p = (PaUserData *)userData;

    Q_ASSERT(p->m_nBufferSize == framesPerBuffer);
    p->m_pInput = inputBuffer;
    p->m_pOutput = outputBuffer;

    //! due to bug in PA driver we can get zero ptr when streaming has just
    //! started !!
    if (p->m_nIChan && inputBuffer == 0)
        std::cout << "Portaudio inbuffer missing - time::" +
                         toString(timeInfo->currentTime)
                  << std::endl;
    if (p->m_nOChan && outputBuffer == 0)
        std::cout << "Portaudio outbuffer missing - time::" +
                         toString(timeInfo->currentTime)
                  << std::endl;

    if (p->m_pCallback)
        p->m_pCallback->mf_Callback();

    return 0;
}

const unsigned sc_nNumBuffers = 4;

/**
 * PortAudioInitter
 *   initializes and terminates the library once.
 *   Manages driver infos.
 ************************************************ */
class PortAudioInitter : public appcore::Singleton<PortAudioInitter>
{
    friend class appcore::Singleton<PortAudioInitter>;

    /**
     * Constructor.
     */
    PortAudioInitter() throw() : mv_bError(false)
    {
        if (Pa_Initialize() != paNoError)
            mv_bError = true;
    }

    /**
     * Refresh driver infos and names.
     * @param a_sError set to error string if any
     */
    void mp_GetDriverInfos(QString &a_sError) throw()
    {
        const int nDevs = Pa_GetDeviceCount();
        if (!nDevs) {
            a_sError = "PortaudioWrapper: no soundcards found";
            return;
        }

        m_DriverInfos.clear();
        m_saDrivers.clear();
        for (int i = 0; i < nDevs; ++i) {
            const PaDeviceInfo *pCur = Pa_GetDeviceInfo(i);
            m_DriverInfos.push_back(pCur);
            m_saDrivers.push_back(pCur->name);
        }
    }

    /**
     * Make sure driver infos are here.
     */
    void mp_AssureDriverInfos()
    {
        if (m_saDrivers.empty()) {
            QString sDummy;
            mp_GetDriverInfos(sDummy);
        }
    }

public:
    typedef std::vector<const PaDeviceInfo *> tDrvrInfos;

    /**
     * Destructor.
     */
    virtual ~PortAudioInitter() throw()
    {
        Pa_Terminate();
    }

    /**
     * Check if portaudio is initialized.
     * @return true for error
     */
    bool mf_bSeriousError() const throw()
    {
        return mv_bError;
    }

    /**
     * Get all drivernames.
     * @param a_sError set to error if any
     * @return string array
     */
    tStringVector mf_GetDriverNames(QString &a_sError) throw()
    {
        if (m_saDrivers.empty())
            mp_GetDriverInfos(a_sError);
        return m_saDrivers;
    }

    /**
     * Get all driver infos.
     * @return PaDeviceInfo array
     */
    const tDrvrInfos &mf_GetDriverInfos() throw()
    {
        mp_AssureDriverInfos();
        return m_DriverInfos;
    }

    /**
     * Get a PaDeviceIndex from a drivername.
     * @param ac_sDriverName drivername ("default" works here)
     * @return input device index or paNoDevice
     */
    PaDeviceIndex mf_nGetInputDeviceID(const QString &ac_sDriverName) throw()
    {
        if (ac_sDriverName == sc_sDefault) {
#ifdef _WIN32
            PaHostApiTypeId preferredHostApiTypeIds[] = {
                paWASAPI, paWDMKS, paDirectSound,
                paMME,    paASIO,  paInDevelopment};

            PaHostApiIndex numberOfInstalledHostApis = Pa_GetHostApiCount();

            for (PaHostApiIndex preferredHostApiIndex = 0;
                 preferredHostApiTypeIds[preferredHostApiIndex] !=
                 paInDevelopment;
                 preferredHostApiIndex++) {
                for (PaHostApiIndex installedHostApiIndex = 0;
                     installedHostApiIndex < numberOfInstalledHostApis;
                     installedHostApiIndex++) {
                    if (preferredHostApiTypeIds[preferredHostApiIndex] ==
                        Pa_GetHostApiInfo(installedHostApiIndex)->type)
                        return Pa_GetHostApiInfo(installedHostApiIndex)
                            ->defaultInputDevice;
                }
            }
#endif
            return Pa_GetDefaultInputDevice();
        }

        mp_AssureDriverInfos();

        tStringVector::size_type nNames = m_saDrivers.size();
        for (tStringVector::size_type i = 0; i < nNames; ++i) {
            if (ac_sDriverName.toStdString() == m_saDrivers.at(i) &&
                m_DriverInfos.at(i)->maxInputChannels)
                return (PaDeviceIndex)i;
        }
        return paNoDevice;
    }

    /**
     * Get a PaDeviceIndex from a drivername.
     * @param ac_sDriverName drivername ("default" works here)
     * @return output device index or paNoDevice
     */
    PaDeviceIndex mf_nGetOutputDeviceID(const QString &cardName) throw()
    {
        mp_AssureDriverInfos();

        qCDebug(soundcard) << "PortaudioWrapper::mf_nGetOutputDeviceID, card="
                           << cardName;
        bool isInt;
        cardName.toInt(&isInt);

        if (isInt) {
            qCDebug(soundcard)
                << "PortaudioWrapper::mf_nGetOutputDeviceID: returning numeric";
            return static_cast<PaDeviceIndex>(cardName.toInt());
        } else if (cardName == sc_sDefault) {
            /*
            #ifdef _WIN32
                        // on windows if the cardname is default, check which of
            the hostapi's default output devices supports 44.1kHz
                        PaHostApiTypeId preferredHostApiTypeIds[] = {paWASAPI,
            paWDMKS, paDirectSound, paMME, paInDevelopment};
                        for (PaHostApiIndex preferredHostApiIndex = 0;
            preferredHostApiTypeIds[preferredHostApiIndex] != paInDevelopment;
            preferredHostApiIndex++) {
                            PaHostApiIndex hostApiIndex =
            Pa_HostApiTypeIdToHostApiIndex(preferredHostApiTypeIds[preferredHostApiIndex]);
                            if (hostApiIndex < 0)
                                continue;

                            PaDeviceIndex devindex =
            Pa_GetHostApiInfo(hostApiIndex)->defaultOutputDevice;
                            const PaDeviceInfo *dinfo =
            Pa_GetDeviceInfo(devindex);
                            PaStreamParameters out;
                            out.channelCount = dinfo->maxOutputChannels;
                            out.device = devindex;
                            out.hostApiSpecificStreamInfo = 0;
                            out.sampleFormat = paInt32;
                            out.suggestedLatency = 0.0;

                            // Check if 44.1kHz is supported, if it is, the
            device is suitable
                            if (devindex != paNoDevice && dinfo &&
            dinfo->maxOutputChannels > 0
                                && Pa_IsFormatSupported(0, &out, 44100) ==
            paFormatIsSupported) {
                                return devindex;
                            }
                        }
            #endif
            */
            return Pa_GetDefaultOutputDevice();
        }

        mp_AssureDriverInfos();

        tStringVector::size_type nNames = m_saDrivers.size();
        for (tStringVector::size_type i = 0; i < nNames; ++i) {
            std::cout << "    checking against driver " << m_saDrivers.at(i);
            std::cout << " which has max "
                      << m_DriverInfos.at(i)->maxOutputChannels;
            std::cout << " output channels" << std::endl;
            if ("portaudio" == m_saDrivers.at(i) &&
                m_DriverInfos.at(i)->maxOutputChannels)
                return (PaDeviceIndex)i;
        }

        return paNoDevice;
    }

    /**
     * Get device info for input/output at once.
     * @param ac_iInIndex input device index
     * @param ac_iOutIndex oputput device index
     * @param a_nInChannels set to max #input channels
     * @param a_nOutChannels set to max #output channels
     * @param a_SampleRates filled with supported samplerates
     */
    void mf_GetDeviceInfo(const PaDeviceIndex ac_iInIndex,
                          const PaDeviceIndex ac_iOutIndex,
                          unsigned &a_nInChannels, unsigned &a_nOutChannels,
                          std::vector<unsigned long> &a_SampleRates)
    {
        mp_AssureDriverInfos();
        const PaDeviceInfo *pIn = 0;
        const PaDeviceInfo *pOut = 0;

        // get dev info and channels
        if (ac_iInIndex == paNoDevice ||
            (tDrvrInfos::size_type)ac_iInIndex >= m_DriverInfos.size()) {
            a_nInChannels = 0;
        } else {
            pIn = m_DriverInfos[ac_iInIndex];
            a_nInChannels = pIn->maxInputChannels;
        }
        if (ac_iOutIndex == paNoDevice ||
            (tDrvrInfos::size_type)ac_iOutIndex >= m_DriverInfos.size()) {
            a_nOutChannels = 0;
        } else {
            pOut = m_DriverInfos[ac_iOutIndex];
            a_nOutChannels = pOut->maxOutputChannels;
        }

        // get samplerates
        // PaStreamParameters in;
        PaStreamParameters out;
        /*in.channelCount = a_nInChannels;
          in.device = ac_iInIndex;
          in.hostApiSpecificStreamInfo = 0;
          in.sampleFormat = paInt32;
          in.suggestedLatency = pIn ? pIn->defaultLowInputLatency : 0.0;*/
        out.channelCount = a_nOutChannels;
        out.device = ac_iOutIndex;
        out.hostApiSpecificStreamInfo = 0;
        out.sampleFormat = paInt32;
        out.suggestedLatency = pIn ? pIn->defaultLowOutputLatency : 0.0;

        const unsigned nRatesToTry = 9;
        const unsigned long RatesToTry[] = {8000,  12000, 16000, 24000, 32000,
                                            44100, 48000, 96000, 192000};
        for (unsigned i = 0; i < nRatesToTry; ++i)
            if (Pa_IsFormatSupported(0, &out, RatesToTry[i]) ==
                paFormatIsSupported)
                a_SampleRates.push_back(RatesToTry[i]);
    }

private:
    bool mv_bError;
    tDrvrInfos m_DriverInfos;
    tStringVector m_saDrivers;
};

/**
 * PortAudioReader
 ***************** */
class PortAudioReader : public AudioFormatReader
{
public:
    ~PortAudioReader()
    {
    }

    unsigned mf_nChannels() const
    {
        return m_Host.mf_nGetIChan();
    }
    unsigned long mf_lSampleRate() const
    {
        return m_Host.mf_lGetSampleRate();
    }
    AudioFormat::mt_eBitMode mf_eBitMode() const
    {
        return AudioFormat::MSBint32;
    }

    unsigned long Read(void **a_pBuf, const unsigned ac_nSamples)
    {
        Q_ASSERT(ac_nSamples == m_Host.mf_nGetBufferSize());
        Q_ASSERT(a_pBuf);

        int **pBuf = (int **)a_pBuf;

#ifndef S_MACPA
        int *in = (int *)m_Host.m_pUserData->m_pInput;
#else
        float *in = (float *)m_Host.m_pUserData->m_pInput;
#endif

        if (in != 0) {
            const unsigned nSize = m_Samples.mf_nGetBufferSize();

            if (m_Samples.mf_nGetChannelCount() == 2) // de-interleave
            {
                for (unsigned i = 0; i < nSize; ++i) {
                    pBuf[0][i] =
#ifdef S_MACPA
                        (int)(in[2 * i] * sc_f32BitMinMax);
#else
                        in[2 * i];
#endif
                    pBuf[1][i] =
#ifdef S_MACPA
                        (int)(in[2 * i + 1] * sc_f32BitMinMax);
#else
                        in[2 * i + 1];
#endif
                }
            } else
#ifndef S_MACPA
                memcpy(pBuf[0], in, nSize * sizeof(int));
#else
            {
                for (unsigned i = 0; i < nSize; ++i)
                    pBuf[0][i] = (int)(in[i] * sc_f32BitMinMax);
            }
#endif
            return ac_nSamples;
        }
        return ac_nSamples;
    }

    friend class PortAudioWrapper;

private:
    PortAudioReader(const PortAudioWrapper &a_Host)
        : m_Host(a_Host),
          m_Samples(a_Host.mf_nGetIChan(), a_Host.mf_nGetBufferSize())
    {
    }
    const PortAudioWrapper &m_Host;
    MatrixStorage<int> m_Samples;

    PortAudioReader(const PortAudioReader &);
    PortAudioReader &operator=(const PortAudioReader &);
};

/**
 * PortAudioWriter
 ***************** */
class PortAudioWriter : public AudioFormatWriter
{
public:
    ~PortAudioWriter()
    {
    }

    unsigned mf_nChannels() const
    {
        return m_Host.mf_nGetOChan();
    }
    unsigned long mf_lSampleRate() const
    {
        return m_Host.mf_lGetSampleRate();
    }
    AudioFormat::mt_eBitMode mf_eBitMode() const
    {
        return AudioFormat::MSBint32;
    }

    unsigned long Write(const void **a_pBuf, const unsigned ac_nSamples)
    {
        Q_ASSERT(ac_nSamples == m_Host.mf_nGetBufferSize());
        Q_ASSERT(a_pBuf);

#ifndef S_MACPA
        int *out = (int *)m_Host.m_pUserData->m_pOutput;
#else
        float *out = (float *)m_Host.m_pUserData->m_pOutput;
#endif

        int **pBuf = (int **)a_pBuf;

        if (out != 0) {
            if (m_Host.mf_nGetOChan() == 2) {
                for (unsigned i = 0; i < ac_nSamples; ++i) {
                    out[2 * i] =
#ifdef S_MACPA
                        (float)(pBuf[0][i]) / sc_f32BitMinMax;
#else
                        pBuf[0][i];
#endif
                    out[2 * i + 1] =
#ifdef S_MACPA
                        (float)(pBuf[1][i]) / sc_f32BitMinMax;
#else
                        pBuf[1][i];
#endif
                }
            } else

#ifndef S_MACPA
            {
                memcpy(out, pBuf[0], ac_nSamples * sizeof(int));
            }
#else
                for (unsigned i = 0; i < ac_nSamples; ++i)
                    out[i] = (float)(pBuf[0][i]) / sc_f32BitMinMax;
        }
#endif
        }

        return ac_nSamples;
    }

    friend class PortAudioWrapper;

private:
    PortAudioWriter(const PortAudioWrapper &a_Host) : m_Host(a_Host)
    {
    }
    const PortAudioWrapper &m_Host;

    PortAudioWriter(const PortAudioWriter &);
    PortAudioWriter &operator=(const PortAudioWriter &);
};
}

/***************************************************************************************************/

tStringVector PortAudioWrapper::sf_saGetDriverNames(QString &a_sError)
{
    return PortAudioInitter::sf_pInstance()->mf_GetDriverNames(a_sError);
}

tStringVector PortAudioWrapper::sf_saGetDriverNames(std::string &a_sError)
{
    QString error;
    tStringVector v(sf_saGetDriverNames(error));
    a_sError = error.toStdString();
    return v;
}

PortAudioWrapper::PortAudioWrapper(const QString &cardName)
    : m_pPaStream(0),
      m_pUserData(new PaUserData()),
      mv_bOpen(false),
      mv_nBuffers(sc_nNumBuffers)
{
    PortAudioInitter *pInst = PortAudioInitter::sf_pInstance();
    if (pInst->mf_bSeriousError()) {
        std::cout << "Portaudio: failed initializing: there is no audio "
                     "hardware, or it's busy"
                  << std::endl;
        throw StringException("Portaudio: failed initializing: there is no "
                              "audio hardware, or it's busy");
    }

    m_iInputID =
        PortAudioInitter::sf_pInstance()->mf_nGetInputDeviceID(cardName);
    m_iOutputID =
        PortAudioInitter::sf_pInstance()->mf_nGetOutputDeviceID(cardName);

    /*if (m_iInputID == paNoDevice && m_iOutputID == paNoDevice) {
      QString sDummy;
      if (PortAudioInitter::sf_pInstance()->mf_GetDriverNames(sDummy).empty())
      throw StringException("Portaudio: no soundcards found");
      else
      throw StringException(QString("Portaudio: couldn't open driver:" +
      cardName));
      }*/
}

PortAudioWrapper::~PortAudioWrapper()
{
    mp_bCloseDriver();
    delete m_pUserData;
}

tSoundCardInfo PortAudioWrapper::mf_GetInfo() const
{
    tSoundCardInfo Ret;

    std::cout << "in: " << m_iInputID << " out: " << m_iOutputID << std::endl;
    PortAudioInitter::sf_pInstance()->mf_GetDeviceInfo(
        m_iInputID, m_iOutputID, Ret.m_nMaxInputChannels,
        Ret.m_nMaxOutputChannels, Ret.m_SampleRates);

    // pa supports any buffersize since it does it's own buffering, so just push
    // back common values
    // but beware of latency!
    for (unsigned i = 64; i <= 16384; i *= 2)
        Ret.m_BufferSizes.push_back(i);
    // use sth sensible
    Ret.m_nDefaultBufferSize = 4096;

    return Ret;
}

bool PortAudioWrapper::mp_bOpenDriver(const unsigned ac_nIChan,
                                      const unsigned ac_nOChan,
                                      const unsigned long ac_nFs,
                                      const unsigned ac_nSize)
{
    if ((ac_nIChan > 2) || (ac_nOChan > 2) || mv_bOpen)
        return false;

    PaStream **str = (PaStream **)&m_pPaStream;

    PaStreamParameters in;
    PaStreamParameters out;
    PaStreamParameters *pIn = &in;
    PaStreamParameters *pOut = &out;
    const PortAudioInitter::tDrvrInfos &infos(
        PortAudioInitter::sf_pInstance()->mf_GetDriverInfos());

    if (ac_nIChan == 0 || m_iInputID == paNoDevice) {
        pIn = 0;
    } else {
        in.channelCount = ac_nIChan;
        in.device = m_iInputID;
        in.hostApiSpecificStreamInfo = 0;
        in.sampleFormat =
#ifdef S_MACPA
            paFloat32;
#else
            paInt32;
#endif
        in.suggestedLatency = infos[m_iInputID]->defaultLowInputLatency;
    }
    if (ac_nOChan == 0 || m_iOutputID == paNoDevice) {
        pOut = 0;
    } else {
        out.channelCount = ac_nOChan;
        out.device = m_iOutputID;
        out.hostApiSpecificStreamInfo = 0;
        out.sampleFormat =
#ifdef S_MACPA
            paFloat32;
#else
            paInt32;
#endif
        out.suggestedLatency = infos[m_iOutputID]->defaultLowOutputLatency;
    }

    const PaStreamFlags flags = paClipOff | paDitherOff;
    PaError err = Pa_OpenStream(str, pIn, pOut, ac_nFs, ac_nSize, flags,
                                PaCallback, m_pUserData);
    if (err == paNoError) {
        m_pUserData->m_nIChan = ac_nIChan;
        m_pUserData->m_nOChan = ac_nOChan;
        m_pUserData->m_nBufferSize = ac_nSize;
        m_pUserData->m_nSampleRate = ac_nFs;
        m_pUserData->m_pOutput = 0;
        m_pUserData->m_pInput = 0;

        return mv_bOpen = true;
    } else {
        mv_sError = Pa_GetErrorText(err);
        return false;
    }
}

bool PortAudioWrapper::mp_bCloseDriver()
{
    if (mv_bOpen) {
        Pa_CloseStream((PaStream *)m_pPaStream);
        m_pUserData->m_nIChan = 0;
        m_pUserData->m_nOChan = 0;
        m_pUserData->m_nBufferSize = 0;
        m_pUserData->m_nSampleRate = 0l;
        mv_bOpen = false;
        return true;
    }
    return false;
}

bool PortAudioWrapper::mp_bStart(Callback &a_CallbackToUse)
{
    if (mv_bOpen && !mf_bIsRunning()) {
        m_pUserData->m_pCallback = &a_CallbackToUse;
        PaError err = Pa_StartStream(m_pPaStream);
        if (err == paNoError)
            return true;
        else
            mv_sError = Pa_GetErrorText(err);
    }
    return false;
}

bool PortAudioWrapper::mp_bStop()
{
    if (mv_bOpen) {
        PaError err = Pa_StopStream(m_pPaStream);
        if (err == paNoError)
            return true;
        else
            mv_sError = Pa_GetErrorText(err);
    }
    return false;
}

bool PortAudioWrapper::mf_bIsRunning() const
{
    return Pa_IsStreamActive(m_pPaStream) == 1;
}

AudioFormatReader *PortAudioWrapper::mf_pCreateReader() const
{
    if (mv_bOpen)
        return new PortAudioReader(*this);
    return 0;
}

AudioFormatWriter *PortAudioWrapper::mf_pCreateWriter() const
{
    if (mv_bOpen)
        return new PortAudioWriter(*this);
    return 0;
}

unsigned PortAudioWrapper::mf_nGetIChan() const
{
    return m_pUserData->m_nIChan;
}

unsigned PortAudioWrapper::mf_nGetOChan() const
{
    return m_pUserData->m_nOChan;
}

unsigned PortAudioWrapper::mf_nGetBufferSize() const
{
    return m_pUserData->m_nBufferSize;
}

unsigned long PortAudioWrapper::mf_lGetSampleRate() const
{
    return m_pUserData->m_nSampleRate;
}

unsigned long PortAudioWrapper::mf_lGetEstimatedLatency() const
{
    const PaStreamInfo *p = Pa_GetStreamInfo(m_pPaStream);
    if (p)
        return (unsigned long)((p->inputLatency + p->outputLatency) * 1000.0);
    return 0;
}
