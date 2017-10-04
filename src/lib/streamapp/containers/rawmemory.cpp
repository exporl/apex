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

#include "rawmemory.h"

#ifndef S_C6X
#include <cstdlib> //malloc
#else
#include <stdlib.h>
#endif
#include <string.h> //memmove

using namespace streamapp;

RawMemoryAccess::RawMemoryAccess() : m_pData(0), mv_nSize(0)
{
}

RawMemoryAccess::RawMemoryAccess(void *const ac_pMemory,
                                 const unsigned ac_nSize,
                                 const bool ac_bInitToZero)
    : m_pData((char *)ac_pMemory), mv_nSize(ac_nSize)
{
    if (ac_bInitToZero)
        mp_FillWith(0);
}

RawMemoryAccess::~RawMemoryAccess()
{
}

bool RawMemoryAccess::operator==(const RawMemoryAccess &ac_Rh)
{
    return (mv_nSize == ac_Rh.mf_nGetSize()) &&
           memcmp(m_pData, ac_Rh.mf_pGetMemory(), mv_nSize);
}

bool RawMemoryAccess::operator!=(const RawMemoryAccess &ac_Rh)
{
    return !operator==(ac_Rh);
}

void RawMemoryAccess::mp_CopyFrom(const void *const ac_pToCopy,
                                  const unsigned ac_nDestOffset,
                                  const unsigned ac_nSize)
{
    const unsigned nToCopy = s_min(mv_nSize - ac_nDestOffset, ac_nSize);
    memcpy(m_pData + ac_nDestOffset, ac_pToCopy, nToCopy);
}

void RawMemoryAccess::mf_CopyTo(void *const ac_pToCopy,
                                const unsigned ac_nSourceOffset,
                                const unsigned ac_nSize) const
{
    const unsigned nCanCopy = s_min(mv_nSize - ac_nSourceOffset, ac_nSize);
    const unsigned nDiff = ac_nSize - nCanCopy;
    memcpy(ac_pToCopy, m_pData + ac_nSourceOffset, nCanCopy);
    if (nDiff)
        memset(((char *)ac_pToCopy) + nCanCopy, 0, nDiff);
}

void RawMemoryAccess::mp_FillWith(const char ac_pChar)
{
    memset(m_pData, (int)ac_pChar, mv_nSize);
}

/******************************************************************************************/

namespace
{
char *f_Allocate(const unsigned ac_nInitialSize,
                 const bool ac_bInitToZero = false)
{
    if (ac_nInitialSize > 0) {
        if (ac_bInitToZero)
            return (char *)calloc(ac_nInitialSize, sizeof(char));
        else
            return (char *)malloc(ac_nInitialSize);
    } else {
        return 0;
    }
}
}

RawMemory::RawMemory() : RawMemoryAccess()
{
}

RawMemory::~RawMemory()
{
    if (m_pData)
        free(m_pData);
}

RawMemory::RawMemory(const unsigned ac_nInitialSize, const bool ac_bInitToZero)
{
    m_pData = f_Allocate(ac_nInitialSize, ac_bInitToZero);
    mv_nSize = ac_nInitialSize;
}

RawMemory::RawMemory(const RawMemoryAccess &ac_Rh)
{
    m_pData = f_Allocate(ac_Rh.mf_nGetSize());
    mv_nSize = ac_Rh.mf_nGetSize();
    mp_CopyFrom(ac_Rh.mf_pGetMemory(), 0, mv_nSize);
}

RawMemory::RawMemory(const void *const ac_ToCopy, const unsigned ac_nSize)
{
    m_pData = f_Allocate(ac_nSize);
    mv_nSize = ac_nSize;
    mp_CopyFrom(ac_ToCopy, 0, mv_nSize);
}

RawMemory &RawMemory::operator=(const RawMemoryAccess &ac_Rh)
{
    mp_Resize(ac_Rh.mf_nGetSize());
    mp_CopyFrom(ac_Rh.mf_pGetMemory(), 0, mv_nSize);
    return *this;
}

RawMemoryAccess RawMemory::mf_GetAccess(const unsigned ac_nOffset,
                                        const unsigned ac_nSize)
{
    if (ac_nOffset > mv_nSize)
        return RawMemoryAccess();
    const unsigned nSize = s_min(ac_nSize, mv_nSize - ac_nOffset);
    return RawMemoryAccess((m_pData + ac_nOffset), nSize);
}

void RawMemory::mp_Resize(const unsigned ac_nNewSize, const bool ac_bInitToZero)
{
    if (ac_nNewSize != mv_nSize) {
        if (ac_nNewSize == 0) {
            free(m_pData);
            m_pData = 0;
            mv_nSize = 0;
        } else {
            if (m_pData) {
                m_pData = (char *)realloc(m_pData, ac_nNewSize);

                if (ac_bInitToZero && ac_nNewSize > mv_nSize)
                    memset(m_pData + mv_nSize, 0, ac_nNewSize - mv_nSize);
            } else {
                m_pData = f_Allocate(ac_nNewSize, ac_bInitToZero);
            }
            mv_nSize = ac_nNewSize;
        }
    }
}

void RawMemory::mp_ResizeIfSmaller(const unsigned ac_nNewSize,
                                   const bool ac_bInitToZero)
{
    if (ac_nNewSize > mv_nSize)
        mp_Resize(ac_nNewSize, ac_bInitToZero);
}

void RawMemory::mp_RemovePart(const unsigned ac_nStartOffset,
                              const unsigned ac_nSize)
{
    if (ac_nStartOffset + ac_nSize > mv_nSize) {
        mp_Resize(ac_nStartOffset);
    } else {
        memmove(m_pData + ac_nStartOffset, m_pData + ac_nStartOffset + ac_nSize,
                mv_nSize - (ac_nStartOffset + ac_nSize));
        mp_Resize(mv_nSize - ac_nSize);
    }
}

void RawMemory::mp_ShiftDown(const unsigned ac_nStartOffset,
                             const unsigned ac_nSize,
                             const bool ac_bClear /*= true*/)
{
    const unsigned nToDo = s_min(ac_nSize, mv_nSize - ac_nStartOffset);
    memmove(m_pData, m_pData + ac_nStartOffset, nToDo);
    if (ac_bClear)
        memset(m_pData + nToDo, 0, mv_nSize - nToDo);
}

void RawMemory::mp_Append(const void *const ac_pToAppend,
                          const unsigned ac_nSize)
{
    if (ac_nSize > 0) {
        const unsigned nOffset = mv_nSize;
        mp_Resize(mv_nSize + ac_nSize);
        mp_CopyFrom(ac_pToAppend, nOffset, ac_nSize);
    }
}
