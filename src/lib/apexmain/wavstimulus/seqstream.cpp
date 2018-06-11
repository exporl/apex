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

#include "streamapp/utils/checks.h"

#include "seqstream.h"

using namespace apex;
using namespace apex::stimulus;
using namespace utils;

ApexSeqStream::ApexSeqStream(PosAudioFormatStream *a_pParent,
                             const bool ac_bDeleteParent)
    : mc_pParent(PtrCheck(a_pParent)),
      mc_bDeleteParent(ac_bDeleteParent),
      mv_nStartIndex(0),
      mv_lTotalSamples(a_pParent->mf_lSamplesLeft()),
      mv_lStartAt(0),
      mv_lCurrentPosition(0),
      mv_bParentIsPlaying(false),
      mv_bParentNeedsReset(false)
{
    a_pParent->mp_GetConvertor().mp_Clear();
    //  qCDebug(APEX_RS, "ApexSeqStream::ApexSeqStream: mv_lTotalSamples=%u",
    //  mv_lTotalSamples);
}

ApexSeqStream::~ApexSeqStream()
{
    if (mc_bDeleteParent)
        delete mc_pParent;
}

void ApexSeqStream::mp_SetStartAt(const unsigned long ac_lStartAt)
{
    if (m_Triggers.mf_nGetNumItems() == 0)
        mv_lStartAt = ac_lStartAt;
    else
        Q_ASSERT(ac_lStartAt >= mv_lTotalSamples);
    m_Triggers.mp_AddItem(ac_lStartAt);
    mv_lTotalSamples = ac_lStartAt + mc_pParent->mf_lTotalSamples();
}

const Stream &ApexSeqStream::Read()
{
    if (m_Triggers.mf_nGetNumItems() == 0) // nothing to do here..
        return mc_pParent->Read();

    mv_lCurrentPosition += mf_nGetBufferSize();
    if (!mv_bParentIsPlaying) {
        AudioFormatConvertor &rConv = mc_pParent->mp_GetConvertor();

        if (mv_bParentNeedsReset) {
            rConv.mp_Clear();
            mv_bParentNeedsReset = false;
        }

        if (mv_lCurrentPosition >= mv_lStartAt) {
            // manually get needed samples on offset
            unsigned nOffset = mf_nGetBufferSize() -
                               (unsigned)(mv_lCurrentPosition - mv_lStartAt);
            rConv.ReadFromSource(mc_pParent->mf_pGetReader(),
                                 mf_nGetBufferSize() - nOffset, nOffset);
            mv_bParentIsPlaying = true;
        }

        return rConv;
    } else {
        const Stream &ret = mc_pParent->Read();
        if (mc_pParent->mf_lSamplesLeft() == 0) {
            ++mv_nStartIndex;
            if (mv_nStartIndex < m_Triggers.mf_nGetNumItems()) {
                mc_pParent->mp_SeekPosition(0);
                mv_lStartAt = m_Triggers.mf_GetItem(mv_nStartIndex);
                if (mv_lCurrentPosition >= mv_lStartAt) {
                    AudioFormatConvertor &rConv = mc_pParent->mp_GetConvertor();
                    // manually get needed samples on offset
                    unsigned nOffset =
                        mf_nGetBufferSize() -
                        (unsigned)(mv_lCurrentPosition - mv_lStartAt);
                    rConv.ReadFromSource(mc_pParent->mf_pGetReader(),
                                         mf_nGetBufferSize() - nOffset,
                                         nOffset);
                    mv_bParentIsPlaying = true;
                } else {
                    mv_bParentIsPlaying = false;
                    mv_bParentNeedsReset = true;
                }
            }
        }
        return ret;
    }
}

unsigned long ApexSeqStream::mf_lSamplesLeft() const
{
    if (mv_lCurrentPosition < mv_lStartAt)
        return mv_lTotalSamples - mv_lCurrentPosition;
    return mc_pParent->mf_lSamplesLeft();
}

unsigned long ApexSeqStream::mf_lTotalSamples() const
{
    return mv_lTotalSamples;
}

unsigned long ApexSeqStream::mf_lCurrentPosition() const
{
    if (mv_lCurrentPosition < mv_lStartAt)
        return mv_lCurrentPosition;
    return mv_lStartAt + mc_pParent->mf_lCurrentPosition();
}

void ApexSeqStream::mp_SeekPosition(const unsigned long ac_nPosition)
{
    Q_ASSERT(0 && "not implemented");
    if (ac_nPosition < mv_lStartAt) {
        mv_lCurrentPosition = ac_nPosition;
        mc_pParent->mp_SeekPosition(0);
        mv_bParentIsPlaying = false;
    } else
        mc_pParent->mp_SeekPosition(ac_nPosition - mv_lStartAt);
}
