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

#ifndef __STR_MULTISTREAM_CPP__
#define __STR_MULTISTREAM_CPP__

#include "multistream.h"

#include <QtGlobal>

using namespace streamapp;

template <bool tOwned>
MultiInputStream<tOwned>::MultiInputStream() : m_pRead(0), m_pResult(0)
{
}

template <bool tOwned>
MultiInputStream<tOwned>::~MultiInputStream()
{
    delete m_pRead;
    delete m_pResult;
}

template <bool tOwned>
bool MultiInputStream<tOwned>::mf_bIsEndOfStream() const
{
    unsigned nStreamsToEnd = this->mf_nGetNumItems();
    bool bEOF = true;

    for (unsigned i = 0; i < nStreamsToEnd; ++i) {
        if (!this->mf_GetItem(i)->mf_bIsEndOfStream())
            bEOF &= false;
    }
    return bEOF;
}

template <bool tOwned>
const Stream &MultiInputStream<tOwned>::Read()
{
    unsigned nStrToRead = this->mf_nGetNumItems();
    unsigned nChanread = 0;

    for (unsigned i = 0; i < nStrToRead; ++i) {
        const Stream &curStr = this->mf_GetItem(i)->Read();

        for (unsigned j = 0; j < curStr.mf_nGetChannelCount(); ++j)
            m_pRead->mp_Set(j + nChanread, curStr.mf_pGetArray()[j]);

        nChanread += curStr.mf_nGetChannelCount();
    }

    return *m_pResult;
}

template <bool tOwned>
void MultiInputStream<tOwned>::mp_UpdateMemory()
{
    if (!m_pRead || (m_pRead->mf_nGetBufferSize() != this->mv_nChan)) {
        delete m_pRead;
        delete m_pResult;
        m_pRead = new StreamChanPtr(this->mv_nChan);
        m_pResult =
            new Stream(m_pRead->mf_pGetArray(), this->mv_nChan, this->mv_nSize);
    }
}

/********************************************************************************************************/

template <bool tOwned>
MultiPosInputStream<tOwned>::MultiPosInputStream() : m_pRead(0), m_pResult(0)
{
}

template <bool tOwned>
MultiPosInputStream<tOwned>::~MultiPosInputStream()
{
    delete m_pRead;
    delete m_pResult;
}

template <bool tOwned>
void MultiPosInputStream<tOwned>::mp_AddItem(
    PositionableInputStream *const ac_pItem)
{
    DynStreamList<PositionableInputStream, tOwned>::mp_AddItem(ac_pItem);
    if (this->mf_nGetNumItems())
        Q_ASSERT(ac_pItem->mf_lSamplesLeft() ==
                 this->mf_GetItem(0)->mf_lSamplesLeft());
}

template <bool tOwned>
bool MultiPosInputStream<tOwned>::mf_bIsEndOfStream() const
{
    const unsigned &nStreamsToEnd = this->mf_nGetNumItems();
    bool bEOF = true;

    for (unsigned i = 0; i < nStreamsToEnd; ++i)
        bEOF &= this->mf_GetItem(i)->mf_bIsEndOfStream();

    return bEOF;
}

template <bool tOwned>
const Stream &MultiPosInputStream<tOwned>::Read()
{
    unsigned nStrToRead = this->mf_nGetNumItems();
    unsigned nChanread = 0;

    for (unsigned i = 0; i < nStrToRead; ++i) {
        const Stream &curStr = this->mf_GetItem(i)->Read();

        for (unsigned j = 0; j < curStr.mf_nGetChannelCount(); ++j)
            m_pRead->mp_Set(j + nChanread, curStr.mf_pGetArray()[j]);

        nChanread += curStr.mf_nGetChannelCount();
    }

    return *m_pResult;
}

template <bool tOwned>
void MultiPosInputStream<tOwned>::mp_UpdateMemory()
{
    if (!m_pRead || (m_pRead->mf_nGetBufferSize() != this->mv_nChan)) {
        delete m_pRead;
        delete m_pResult;
        m_pRead = new StreamChanPtr(this->mv_nChan);
        m_pResult =
            new Stream(m_pRead->mf_pGetArray(), this->mv_nChan, this->mv_nSize);
    }
}

template <bool tOwned>
unsigned long MultiPosInputStream<tOwned>::mf_lSampleRate() const
{
    if (this->mf_nGetNumItems() != 0)
        return this->mf_GetItem(0)->mf_lSampleRate();
    return 0L;
}

template <bool tOwned>
unsigned long MultiPosInputStream<tOwned>::mf_lSamplesLeft() const
{
    if (this->mf_nGetNumItems())
        return this->mf_GetItem(0)->mf_lSamplesLeft();
    return 0l;
}

template <bool tOwned>
unsigned long MultiPosInputStream<tOwned>::mf_lTotalSamples() const
{
    if (this->mf_nGetNumItems())
        return this->mf_GetItem(0)->mf_lTotalSamples();
    return 0l;
}

template <bool tOwned>
unsigned long MultiPosInputStream<tOwned>::mf_lCurrentPosition() const
{
    if (this->mf_nGetNumItems())
        return this->mf_GetItem(0)->mf_lCurrentPosition();
    return 0l;
}

template <bool tOwned>
void MultiPosInputStream<tOwned>::mp_SeekPosition(
    const unsigned long ac_nPosition)
{
    const unsigned nStrToRead = this->mf_nGetNumItems();
    for (unsigned i = 0; i < nStrToRead; ++i)
        this->mf_GetItem(i)->mp_SeekPosition(ac_nPosition);
}

template <bool tOwned>
void MultiPosInputStream<tOwned>::mp_SetNumLoops(const unsigned ac_nLoops)
{
    const unsigned nStrToRead = this->mf_nGetNumItems();
    for (unsigned i = 0; i < nStrToRead; ++i)
        this->mf_GetItem(i)->mp_SetNumLoops(ac_nLoops);
}

template <bool tOwned>
unsigned MultiPosInputStream<tOwned>::mf_nGetNumLoops() const
{
    if (this->mf_nGetNumItems())
        return this->mf_GetItem(0)->mf_nGetNumLoops();
    return 0;
}

template <bool tOwned>
unsigned MultiPosInputStream<tOwned>::mf_nGetNumLooped() const
{
    if (this->mf_nGetNumItems())
        return this->mf_GetItem(0)->mf_nGetNumLooped();
    return 0;
}

template <bool tOwned>
bool MultiPosInputStream<tOwned>::mf_bIsLooping() const
{
    if (this->mf_nGetNumItems())
        return this->mf_GetItem(0)->mf_bIsLooping();
    return false;
}

/********************************************************************************************************/

template <bool tOwned>
MultiOutputStream<tOwned>::MultiOutputStream(const bool ac_bSerialMode)
    : mc_bSerialMode(ac_bSerialMode)
{
}

template <bool tOwned>
MultiOutputStream<tOwned>::~MultiOutputStream()
{
}

template <bool tOwned>
bool MultiOutputStream<tOwned>::mf_bIsEndOfStream() const
{
    const unsigned &nStreamsToEnd = this->mf_nGetNumItems();
    bool bEOF = true;

    for (unsigned i = 0; i < nStreamsToEnd; ++i)
        bEOF &= this->mf_GetItem(i)->mf_bIsEndOfStream();

    return bEOF;
}

template <bool tOwned>
void MultiOutputStream<tOwned>::Write(const Stream &ac_ToWrite)
{
    const unsigned &nStrToWrite = this->mf_nGetNumItems();

    if (mc_bSerialMode) {
        for (unsigned i = 0; i < nStrToWrite; ++i)
            this->mf_GetItem(i)->Write(ac_ToWrite);
    } else {
        const unsigned nChanCanWrite = ac_ToWrite.mf_nGetChannelCount();
        const unsigned nChanShouldWrite = this->mv_nChan;

        const unsigned nChanToWrite = (nChanCanWrite >= nChanShouldWrite)
                                          ? nChanShouldWrite
                                          : nChanCanWrite;

        // try to write all output streams..
        for (unsigned i = 0, nChan = 0; i < nStrToWrite && nChan < nChanToWrite;
             ++i) {
            const unsigned c_nCurNumOchan =
                this->mf_GetItem(i)->mf_nGetNumChannels();

            //..or until there are not enough channels left to write
            if (c_nCurNumOchan + nChan > nChanToWrite)
                break;

            StreamChanPtr write(c_nCurNumOchan);
            for (unsigned j = 0; j < c_nCurNumOchan; ++j)
                write.mp_Set(j, ac_ToWrite.mf_pGetArray()[j + nChan]);

            const Stream curStr(write.mf_pGetArray(), c_nCurNumOchan,
                                this->mv_nSize);

            this->mf_GetItem(i)->Write(curStr);
            nChan += c_nCurNumOchan;
        }
    }
}

#endif //#ifndef __STR_MULTISTREAM_CPP__
