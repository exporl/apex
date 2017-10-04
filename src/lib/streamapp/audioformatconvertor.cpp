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

#include "callback/callback.h"

#include "utils/dataconversion.h"

#include "audioformatconvertor.h"
#include "audiosamplebuffer.h"
#include "audiosamplebuffer.h"

#include <QtGlobal>

using namespace streamapp;
using namespace dataconversion;

namespace
{

INLINE bool f_bIsFloating(const AudioFormat::mt_eBitMode ac_eMode)
{
    return (ac_eMode == AudioFormat::MSBfloat32) ? true : false;
}

void f_InitUnsignedBoolMap(tUnsignedBoolMap &a_Map, const unsigned ac_nChannels)
{
    a_Map.clear();
    for (unsigned i = 0; i < ac_nChannels; ++i)
        a_Map[i] = false;
}

void f_ClearUnsignedBoolMap(tUnsignedBoolMap &a_Map)
{
    tUnsignedBoolMapCIt itE = a_Map.end();
    for (tUnsignedBoolMapIt it = a_Map.begin(); it != itE; ++it)
        (*it).second = false;
}
}

AudioFormatConvertor::AudioFormatConvertor(const unsigned ac_nChannels,
                                           const unsigned ac_nSamples)
    : Stream(f_InitMem<double>(ac_nChannels, ac_nSamples, true), ac_nChannels,
             ac_nSamples),
      m_WorkHere(ac_nChannels, ac_nSamples),
      mv_lWrittenSoFar(0L),
      mv_bCalledBack(false),
      mv_pCallback(0),
      mv_dGain(1.0)
{
    f_InitUnsignedBoolMap(m_Clipped, ac_nChannels);
}

AudioFormatConvertor::~AudioFormatConvertor()
{
    for (unsigned i = 0; i < Stream::mf_nGetChannelCount(); ++i)
        delete[] Stream::mc_pSamplesArray[i];
    delete[] Stream::mc_pSamplesArray;
}

unsigned
AudioFormatConvertor::ReadFromSource(AudioFormatReader *const a_cpSource,
                                     const unsigned ac_nSamples,
                                     const unsigned ac_nSourceOffset)
{
    unsigned nRead = 0;
    const unsigned nChannels = a_cpSource->mf_nChannels();
    const AudioFormat::mt_eBitMode &eMode = a_cpSource->mf_eBitMode();

    Q_ASSERT(nChannels <= Stream::mf_nGetChannelCount());
    Q_ASSERT(ac_nSamples <= Stream::mf_nGetBufferSize());

    if (eMode == AudioFormat::MSBfloat64) {
        double **dest = Stream::mf_pGetArray();

        // read
        nRead = (const unsigned)a_cpSource->Read(
            (void **)(dest + ac_nSourceOffset), ac_nSamples);
        const unsigned nToDo = ac_nSourceOffset + nRead;

        if (mv_dGain != 1.0) {
            AudioSampleBuffer calc(mf_pGetArray(), mf_nGetChannelCount(),
                                   nRead);
            calc.mp_ApplyGain(mv_dGain);
        }

        if (nRead < ac_nSamples) {
            for (unsigned i = 0; i < nChannels; ++i)
                memset(&dest[i][nToDo], 0,
                       (ac_nSamples - nRead) * sizeof(double));
        }
    } else {
        const bool bFloating = f_bIsFloating(eMode);
        int **src = m_WorkHere.mf_pGetArray();
        StreamType **dest = Stream::mf_pGetArray();

        // read
        nRead = (const unsigned)a_cpSource->Read((void **)src, ac_nSamples);
        const unsigned nToDo = ac_nSourceOffset + nRead;

        // convert
        if (bFloating) // src is 32bit float
        {
            if (mv_dGain == 1.0) {
                for (unsigned i = 0; i < nChannels; ++i) {
                    for (unsigned j = ac_nSourceOffset; j < nToDo; ++j)
                        dest[i][j] =
                            ToDouble(*(float *)(&src[i][j - ac_nSourceOffset]));
                }
            } else {
                for (unsigned i = 0; i < nChannels; ++i) {
                    for (unsigned j = ac_nSourceOffset; j < nToDo; ++j)
                        dest[i][j] =
                            mv_dGain *
                            ToDouble(*(float *)(&src[i][j - ac_nSourceOffset]));
                }
            }
        } else // src is 32bit int
        {
            const StreamType dMult = mv_dGain / sc_d32BitMinMax;
            for (unsigned i = 0; i < nChannels; ++i) {
                for (unsigned j = ac_nSourceOffset; j < nToDo; ++j)
                    dest[i][j] = ToDouble(src[i][j - ac_nSourceOffset]) * dMult;
            }
        }

        if (nRead < ac_nSamples) {
            for (unsigned i = 0; i < nChannels; ++i)
                memset(&dest[i][nToDo], 0,
                       (ac_nSamples - nRead) * sizeof(double));
        }
    }

    // finito
    return nRead;
}

unsigned
AudioFormatConvertor::WriteToSource(AudioFormatWriter *const a_cpSource,
                                    const unsigned ac_nSamples)
{
    const unsigned nChannels = a_cpSource->mf_nChannels();
    Q_ASSERT(nChannels <= Stream::mf_nGetChannelCount());
    Q_ASSERT(ac_nSamples <= Stream::mf_nGetBufferSize());
    const bool bFloating = f_bIsFloating(a_cpSource->mf_eBitMode());
    StreamType **src = Stream::mf_pGetArray();
    int **dest = m_WorkHere.mf_pGetArray();

    // convert
    for (unsigned i = 0; i < nChannels; ++i) {
        for (unsigned j = 0; j < ac_nSamples; ++j) {
            mv_bCalledBack = false;
            if (f_dClipNormalized(src[i][j])) {
                m_Clipped[i] = true;
                if (mv_pCallback && !mv_bCalledBack)
                    mv_pCallback->mf_Callback();
                mv_bCalledBack = true;
            }
            if (bFloating)
                *(float *)(&dest[i][j]) = src[i][j] * sc_f32BitMinMax;
            else
                dest[i][j] = roundDoubleToInt(src[i][j] * sc_d32BitMinMax);
        }
    }

    mv_lWrittenSoFar += ac_nSamples;

    // write
    const void **isrc = (const void **)m_WorkHere.mf_pGetArray();
    return (unsigned)a_cpSource->Write(isrc, ac_nSamples);
}

const tUnsignedBoolMap &AudioFormatConvertor::mf_GetClippedChannels() const
{
    return m_Clipped;
}

void AudioFormatConvertor::mp_ResetClipped()
{
    f_ClearUnsignedBoolMap(m_Clipped);
}

void AudioFormatConvertor::mp_SetClipCallback(Callback *a_pCallback)
{
    mv_pCallback = a_pCallback;
}
