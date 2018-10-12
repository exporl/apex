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

/**
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/containers/circularlogic.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_CIRCULARLOGIC_H__
#define __STR_CIRCULARLOGIC_H__

#include "../threads/locks.h"

#include "defines.h"
#include "pointertraits.h"

#include <string.h>

S_NAMESPACE_BEGIN

namespace containers
{

#ifdef S_SPEC_PRINT
/**
  * tCircularLogicSpecPrinter
  *   prints values of all pointers etc
  *   for inspecting the class while debugging.
  ********************************************* */
template <class tType, bool tIsPtr, class tCriticalSection>
struct tCircularLogicSpecPrinter;
#endif

/**
  * tCircularLogicBase
  *   helper for tCircularLogic.
  *   Defines the actual pointer types used.
  *   @param tType the type pointed to
  ****************************************** */
template <class tType>
struct tCircularLogicBase {
    typedef types::tPointerTraits<tType> tTraits;
    typedef typename tTraits::mt_IsPointer tIsPointer;
    typedef typename tTraits::tIntPtr tIntPtr;
    typedef typename tTraits::tExtPtr tExtPtr;
    typedef typename tTraits::tExtCPtr tExtCPtr;
};

/**
  * CircularLogic
  *   class providing all logic needed to work with circular buffers;
  *   takes care of moving the read and write pointers around and also
  *   has read and write facilities using memcpy or a custom implementation.
  *   Works both for pointers and pointers to pointers since it uses
  *   tPointerTraits internally.
  *   Applications include buffering of data and delay lines.
  *
  *   The criticalsection is only locked and unlocked for the logic,
  *   not during read and write operations; this has two reasons:
  *
  *   - the class is only thread-safe if used with one reader thread
  *     and one writer thread. Reading or writing from multiple threads
  *     shouldn't be needed anyway: normally one high priority reader
  *     thread only copies mem (=fast), while a lower priority thread
  *     takes care of filling the buffer using eg hardware io (=slow).
  *     It doesn't make much sense having eg two threads for reading from
  *     disk, that won't get much faster as disk io is serial, so the first
  *     thread will always have to wait for the other one anyway.
  *
  *   - the fact the read and write operations do not block the buffer is
  *     the whole point of using a buffer: suppose we'd lock upon entering
  *     write, then the read would have to wait until the slow write is
  *     done, and that's exactly what we want to avoid.
  ************************************************************************** */
template <class tType, class tCriticalSection = threads::CriticalSection>
class CircularLogic
{
public:
    typedef threads::TLock<tCriticalSection> mt_Lock;
    typedef tCircularLogicBase<tType> mt_Parent;

    typedef typename mt_Parent::tIntPtr tIPtr;
    typedef typename mt_Parent::tExtPtr tPtr;
    typedef typename mt_Parent::tExtCPtr tCPtr;
    typedef typename mt_Parent::tIsPointer tIsPointer;

    /**
      * Constructor.
      * @param a_pBegin pointer to the buffer's start
      * @param ac_nLength the buffer's length
      */
    CircularLogic(tPtr a_pBegin, const size_type ac_nLength)
        : m_pBegin(a_pBegin),
          m_pEnd(a_pBegin + ac_nLength),
          m_pRead(a_pBegin),
          m_pWrite(a_pBegin),
          m_pTempR(a_pBegin),
          m_pTempW(a_pBegin),
          m_nLength(0),
          mc_nSize(ac_nLength),
          mc_Lock()
    {
        Q_ASSERT(a_pBegin);
        Q_ASSERT(ac_nLength);
        s_static_assert((tIsPointer::value == false),
                        YouGotTheWrongConstructor);
    }

    /**
      * Constructor for use with pointer to pointer.
      * @param a_pBegin pointer to the buffer's start
      * @param ac_nLength all buffer's length
      * @param ac_nChannels number of buffers pointed to
      */
    CircularLogic(tPtr a_pBegin, const size_type ac_nLength,
                  const size_type ac_nChannels)
        : m_pBegin(a_pBegin, ac_nChannels),
          m_pEnd(a_pBegin, ac_nChannels),
          m_pRead(a_pBegin, ac_nChannels),
          m_pWrite(a_pBegin, ac_nChannels),
          m_pTempR(a_pBegin, ac_nChannels),
          m_pTempW(a_pBegin, ac_nChannels),
          m_nLength(0),
          mc_nSize(ac_nLength),
          mc_Lock()
    {
        m_pEnd += ac_nLength;
        s_static_assert((tIsPointer::value == true), YouGotTheWrongConstructor);
    }

    /**
      * Destructor.
      */
    ~CircularLogic()
    {
    }

    /**
      * Query size.
      * @return true if full
      */
    s_finline bool mf_bFull() const
    {
        const mt_Lock L(mc_Lock);
        return this->m_nLength == mf_nGetSize();
    }

    /**
      * Query size.
      * @return true if empty
      */
    s_finline bool mf_bEmpty() const
    {
        const mt_Lock L(mc_Lock);
        return this->m_nLength == 0;
    }

    /**
      * Query size.
      * @return total size, equals begin - end
      */
    s_finline size_type mf_nGetSize() const
    {
        return this->mc_nSize;
    }

    /**
      * Query size.
      * @return num elements avaliable for read
      */
    s_finline size_type mf_nGetMaxRead() const
    {
        const mt_Lock L(mc_Lock);
        return this->m_nLength;
    }

    /**
      * Query size.
      * @return num elements free for write
      */
    s_finline size_type mf_nGetMaxWrite() const
    {
        const mt_Lock L(mc_Lock);
        return mf_nGetSize() - this->m_nLength;
    }

    /**
      * Get the pointer to the start of the memory.
      * Can be used eg to delete afterwards
      * @return the pointer
      */
    s_finline tPtr mf_pGetMemory()
    {
        return m_pBegin;
    }

    /**
      * Set empty.
      */
    s_finline void mp_SetEmpty()
    {
        const mt_Lock L(mc_Lock);
        this->m_nLength = 0;
        this->m_pRead = this->m_pBegin;
        this->m_pWrite = this->m_pBegin;
    }

    /**
      * Set full.
      */
    s_finline void mp_SetFull()
    {
        const mt_Lock L(mc_Lock);
        this->m_nLength = this->mc_nSize;
        this->m_pRead = this->m_pBegin;
        this->m_pWrite = this->m_pBegin;
    }

    /**
      * Move write pointer forward as much
      * as possible without wrapping around.
      * This means the pointer is moved up to the write
      * pointer, or up to the end, whichever comes first.
      * @param a_nWritten number of elements moved forward
      */
    s_finline void mp_WritePiece(size_type &a_nWritten)
    {
        mp_WritePieceInt(a_nWritten, this->m_pTempW, this->mf_nGetSize());
    }

    /**
      * Move write pointer forward. Moves ac_nMaxToMove, until
      * m_pRead is reached, or until m_pEnd is reached.
      * @param a_nWritten number of elements moved forward
      * @param ac_nMaxToMove maximum moves
      */
    s_finline void mp_WritePiece(size_type &a_nWritten,
                                 const size_type ac_nMaxToMove)
    {
        mp_WritePieceInt(a_nWritten, this->m_pTempW, ac_nMaxToMove);
    }

    /**
      * Move write pointer forward as much
      * as possible without wrapping around and return
      * the pointer's previous position, at which one
      * can safely write data up to a_nWritten samples.
      * This means the pointer is moved up to the write
      * pointer, or up to the end, whichever comes first.
      * @param a_nWritten number of elements moved forward
      * @param a_pPreviousPos original write pointer pos
      */
    s_finline void mp_WritePiece(size_type &a_nWritten, tPtr &a_pPreviousPos)
    {
        mp_WritePiece(a_nWritten, a_pPreviousPos, mf_nGetSize());
    }

    /**
      * Move write pointer forward. Moves ac_nMaxToMove, until
      * m_pRead is reached, or until m_pEnd is reached.
      * @param a_nWritten number of elements moved forward
      * @param a_pPreviousPos original write pointer pos
      * @param ac_nMaxToMove maximum moves
      */
    s_finline void mp_WritePiece(size_type &a_nWritten, tPtr &a_pPreviousPos,
                                 const size_type ac_nMaxToMove)
    {
        mp_WritePieceInt(a_nWritten, this->m_pTempW, ac_nMaxToMove);
        a_pPreviousPos = this->m_pTempW;
    }

    /**
      * Move read pointer forward as much
      * as possible without wrapping around.
      * @param a_nRead number of elements moved forward
      */
    s_finline void mp_ReadPiece(size_type &a_nRead)
    {
        mp_ReadPieceInt(a_nRead, this->m_pTempR, this->mf_nGetSize());
    }

    /**
      * Move read pointer forward. Moves ac_nMaxToMove, until
      * m_pWrite is reached, or until m_pEnd is reached.
      * @param a_nRead number of elements moved forward
      * @param ac_nMaxToRead maximum moves
      */
    s_finline void mp_ReadPiece(size_type &a_nRead,
                                const size_type ac_nMaxToMove)
    {
        mp_ReadPieceInt(a_nRead, this->m_pTempR, ac_nMaxToMove);
    }

    /**
      * Move read pointer forward as much
      * as possible without wrapping around and return
      * the previous position.
      * @param a_nRead number of elements moved forward
      * @param a_pPreviousPos original read pointer pos
      */
    s_finline void mp_ReadPiece(size_type &a_nRead, tCPtr &a_pPreviousPos)
    {
        mp_ReadPiece(a_nRead, a_pPreviousPos, mf_nGetSize());
    }

    /**
      * Move read pointer forward. Moves ac_nMaxToMove, until
      * m_pWrite is reached, or until m_pEnd is reached.
      * @param a_nRead number of elements moved forward
      * @param a_pPreviousPos original read pointer pos
      * @param ac_nMaxToRead maximum moves
      */
    s_finline void mp_ReadPiece(size_type &a_nRead, tCPtr &a_pPreviousPos,
                                const size_type ac_nMaxToMove)
    {
        mp_ReadPieceInt(a_nRead, this->m_pTempR, ac_nMaxToMove);
        a_pPreviousPos = this->m_pTempR;
    }

    /**
      * Copy ac_nMaxToWrite elements from ac_pSource into the buffer.
      * Basically does while( writepiece ) memcpy( ... );
      * @see mp_WritePiece
      * @param ac_pSource the data to copy
      * @param a_nWritten the number of elemets copied
      * @param ac_nMaxToWrite the number of elements to copy
      */
    void mp_Write(tCPtr ac_pSource, size_type &a_nWritten,
                  const size_type ac_nMaxToWrite)
    {
        a_nWritten = 0;
        size_type nPiece = 1;
        this->m_pTempW = const_cast<tType *>(ac_pSource);
        while (nPiece && a_nWritten < ac_nMaxToWrite) {
            nPiece = mf_nGetPiece(this->m_pWrite, this->m_pRead,
                                  ac_nMaxToWrite - a_nWritten, false);
            if (nPiece) {
                mt_Parent::tTraits::smf_MemCpy(this->m_pWrite, this->m_pTempW,
                                               nPiece);
                a_nWritten += nPiece;
                this->m_pTempW += nPiece;
                mp_Move(this->m_pWrite, nPiece, false);
            }
        }
    }

    /**
      * Copy ac_nMaxToRead elements from the buffer into a_pDest.
      * Basically does while( readpiece ) memcpy( ... );
      * @see mp_ReadPiece
      * @param a_pDest the data destination
      * @param a_nRead the number of elemets copied
      * @param ac_nMaxToRead the number of elements to copy
      */
    void mp_Read(tPtr a_pDest, size_type &a_nRead,
                 const size_type ac_nMaxToRead)
    {
        a_nRead = 0;
        size_type nPiece = 1;
        this->m_pTempR = a_pDest;
        while (nPiece && a_nRead < ac_nMaxToRead) {
            nPiece = mf_nGetPiece(this->m_pRead, this->m_pWrite,
                                  ac_nMaxToRead - a_nRead, true);
            if (nPiece) {
                mt_Parent::tTraits::smf_MemCpy(this->m_pTempR, this->m_pRead,
                                               nPiece);
                a_nRead += nPiece;
                this->m_pTempR += nPiece;
                mp_Move(this->m_pRead, nPiece, true);
            }
        }
    }

    /**
      * Dummy read action.
      * Move pointer forward up to end or ac_nMaxToRead.
      * @param a_nRead amount moved
      * @param ac_nMaxToRead max amount to move
      */
    void mp_Read(size_type &a_nRead, const size_type ac_nMaxToRead)
    {
        a_nRead = 0;
        size_type nPiece = 1;
        while (nPiece && a_nRead < ac_nMaxToRead) {
            nPiece = mf_nGetPiece(this->m_pRead, this->m_pWrite,
                                  ac_nMaxToRead - a_nRead, true);
            if (nPiece) {
                a_nRead += nPiece;
                mp_Move(this->m_pRead, nPiece, true);
            }
        }
    }

    /**
      * Read and serialize 2D content. Eg if buffering
      * 2 * 512 data, copies items into 1 * 1024 buffer
      * while making sure the first 512 are from channel 1,
      * and the rest from channel 2.
      * This is a seperate method as it's not so trivial if
      * the buffered data might be wrapping around at the end of the buffer,
      * and it's faster to do it here then having a seperate reader for it.
      *
      * @verbatim
        If this is in the buffer:
        [ x00 x00 x00 x00 ] m_pEnd [ x01 x01 x01 x01 ]
        [ x10 x10 x10 x10 ] m_pEnd [ x11 x11 x11 x11 ]
        then it has to be copied like this into a_pDest:
        1st
        [ x00 x00 x00 x00 ... ... ... ... x01 x01 x01 x01 ... ... ... ... ]
        2nd
        [ x10 x10 x10 x10 x01 x01 x01 x01 x11 x11 x11 x11 x01 x01 x01 x01 ]
      * @endverbatim
      *
      * @param a_pDest destination, better large enough
      * @param a_nRead receives number of elements read
      * @param ac_nMaxToRead max number of elements to read
      */
    void mp_Read2Dto1D(tType a_pDest, size_type &a_nRead,
                       const size_type ac_nMaxToRead)
    {
        s_static_assert((tIsPointer::value == true), UseOnlyFor2Dto1D);
        typedef typename types::tRemovePointer<tType>::type tPointedTo;

        a_nRead = 0;
        tType pBuf = a_pDest;
        size_type nPiece = 1;
        while (nPiece && a_nRead < ac_nMaxToRead) {
            nPiece = mf_nGetPiece(this->m_pRead, this->m_pWrite,
                                  ac_nMaxToRead - a_nRead, true);
            const unsigned nToCopy = sizeof(tPointedTo) * nPiece;
            const unsigned nChan = this->m_pRead.mf_nGetNumChannels();
            if (nPiece == ac_nMaxToRead || a_nRead == 0) {
                // copy first part or entirely
                // if it's first part, we still have to
                // keep space for the second part, so always
                // move pointer with ac_nMaxToRead
                a_nRead += nPiece;
                for (unsigned i = 0; i < nChan; ++i) {
                    memcpy(pBuf, this->m_pRead[i], nToCopy);
                    pBuf += ac_nMaxToRead;
                }
            } else {
                // copy rest at offset
                pBuf = a_pDest;
                for (unsigned i = 0; i < nChan; ++i) {
                    pBuf += a_nRead;
                    memcpy(pBuf, this->m_pRead[i], nToCopy);
                    pBuf += nPiece; // a_nRead + nPiece == ac_nMaxToRead here
                }
                a_nRead += nPiece;
            }
            mp_Move(this->m_pRead, nPiece, true);
        }
    }

    /**
      * Write data using another class; the class just needs
      * an operator( tPtr, size_type ) member where the
      * tPtr is the destination ans size_type the number of elements
      * to write. The operator gets called twice if the end was reached
      * before ac_nMaxToWrite was written, the first time with end - start
      * elements, the second time with ac_nMaxToWrite - ( end - start ).
      * @param a_Writer the class responsible for writing
      * @param a_nWritten receives number of elements written, will be less
      * then ac_nMaxToWrite if ac_nMaxToWrite > mf_nGetMaxWrite
      * @param ac_nMaxToWrite number of elements to write
      */
    template <class tOp>
    void mp_Write(tOp &a_Writer, size_type &a_nWritten,
                  const size_type ac_nMaxToWrite)
    {
        a_nWritten = 0;
        size_type nPiece = 1;
        while (nPiece && a_nWritten < ac_nMaxToWrite) {
            nPiece = mf_nGetPiece(this->m_pWrite, this->m_pRead,
                                  ac_nMaxToWrite - a_nWritten, false);
            if (nPiece) {
                a_Writer(this->m_pWrite, nPiece);
                a_nWritten += nPiece;
                mp_Move(this->m_pWrite, nPiece, false);
            }
        }
    }

    /**
      * Read data using another class; the class just needs
      * an operator( tCPtr, size_type ) member where the
      * tPtr will contain the source data and size_type is the number
      * of elements to read. The operator gets called twice if the end was
      * reached before ac_nMaxToRead was read.
      * @param a_Reader a tOp instance
      * @param a_nRead receives number of elements read
      * @param ac_nMaxToRead number of elements to read
      */
    template <class tOp>
    void mp_Read(tOp &a_Reader, size_type &a_nRead,
                 const size_type ac_nMaxToRead)
    {
        a_nRead = 0;
        size_type nPiece = 1;
        while (nPiece && a_nRead < ac_nMaxToRead) {
            nPiece = mf_nGetPiece(this->m_pRead, this->m_pWrite,
                                  ac_nMaxToRead - a_nRead, true);
            if (nPiece) {
                a_Reader(this->m_pRead, nPiece);
                a_nRead += nPiece;
                mp_Move(this->m_pRead, nPiece, true);
            }
        }
    }

    /**
      * Move read pointer forward until ac_tSearch is encountered.
      * If not found, move as far as possible and return false.
      * @param ac_tSearch item to look for
      * @param a_nRead number of items read
      * @return true if ac_tSearch was found
      */
    bool mp_bReadUpTo(const tType &ac_tSearch, size_type &a_nRead)
    {
        a_nRead = 0;
        size_type nPiece = 1;
        while (nPiece) {
            nPiece = mf_nGetPiece(this->m_pRead, this->m_pWrite,
                                  mc_nSize - a_nRead, true);
            size_type nWhere = 0;
            while (nWhere < nPiece) {
                if (this->m_pRead[nWhere] == ac_tSearch)
                    break;
                ++nWhere;
            }
            a_nRead += nWhere;
            mp_Move(this->m_pRead, nWhere, true);
            if (nWhere != nPiece)
                return true;
        }
        return false;
    }

    /**
      * Change the number of items pointed to by the
      * internal pointers. Attention to the pointer
      * magic n00bs: USE ONLY when you know what
      * you're doing, ie after figuring out, and understanding,
      * what every single function of this class does, in detail.
      * Otherwise you'll screw it up in no time.
      * @param ac_nChannels new # channels, better be less
      * then the number set in the constructor.
      */
    void mp_ChangeNumChannels(const size_type ac_nChannels)
    {
        s_static_assert((tIsPointer::value == true), YouGotTheWrongType);
        m_pBegin.mp_SetNumChannels(ac_nChannels);
        m_pEnd.mp_SetNumChannels(ac_nChannels);
        m_pRead.mp_SetNumChannels(ac_nChannels);
        m_pWrite.mp_SetNumChannels(ac_nChannels);
        m_pTempR.mp_SetNumChannels(ac_nChannels);
        m_pTempW.mp_SetNumChannels(ac_nChannels);
    }

#ifdef S_SPEC_PRINT
    friend struct tCircularLogicSpecPrinter<tType, tIsPointer::value,
                                            tCriticalSection>;

    /**
      * Debug output.
      */
    s_finline void mf_PrintSpecs() const s_no_throw
    {
        tCircularLogicSpecPrinter<tType, tIsPointer::value, tCriticalSection>()(
            *this);
    }

    s_spec_printer(CircularLogic, mf_PrintSpecs)
#endif

        s_track_mem_leaks(CircularLogic)

            private :
        /**
          * Move write pointer forward. Moves ac_nMaxToMove, until
          * m_pRead is reached, or until m_pEnd is reached.
          * @param a_nWritten number of elements moved forward
          * @param a_pPreviousPos original write pointer pos
          * @param ac_nMaxToMove maximum moves
          */
        void mp_WritePieceInt(size_type &a_nWritten, tIPtr &a_pPreviousPos,
                              const size_type ac_nMaxToMove)
    {
        a_pPreviousPos = this->m_pWrite;
        a_nWritten =
            mf_nGetPiece(this->m_pWrite, this->m_pRead, ac_nMaxToMove, false);
        mp_Move(this->m_pWrite, a_nWritten, false);
    }

    /**
      * Move write pointer forward. Moves ac_nMaxToMove, until
      * m_pWrite is reached, or until m_pEnd is reached.
      * @param a_nRead number of elements moved forward
      * @param a_pPreviousPos original read pointer pos
      * @param ac_nMaxToRead maximum moves
      */
    void mp_ReadPieceInt(size_type &a_nRead, tIPtr &a_pPreviousPos,
                         const size_type ac_nMaxToMove)
    {
        a_pPreviousPos = this->m_pRead;
        a_nRead =
            mf_nGetPiece(this->m_pRead, this->m_pWrite, ac_nMaxToMove, true);
        mp_Move(this->m_pRead, a_nRead, true);
    }

    /**
      * Find out how much we can move in one go
      * without wrapping from end to begin.
      * This method uses the lock.
      * @param a_pStartAt source pointer
      * @param ac_pStopAt stop pointer
      * @param ac_nRequested max num elements requested
      * @param ac_bIsReadPtr true if it's m_pRead
      * @return number of available elements
      */
    size_type mf_nGetPiece(const tIPtr &ac_pStartAt, const tIPtr &ac_pStopAt,
                           const size_type ac_nRequested,
                           const bool ac_bIsReadPtr)
    {
        size_type nMoved = 0;
        const mt_Lock L(mc_Lock);
        if (ac_pStopAt > ac_pStartAt) {
            const size_type nDiff =
                (size_type)(ac_pStopAt - ac_pStartAt); // avoid calculating
                                                       // twice in s_min.. FIXME
                                                       // template
            nMoved = s_min(ac_nRequested, nDiff);
        } else if (ac_pStopAt < ac_pStartAt) {
            const size_type nDiff = (size_type)(this->m_pEnd - ac_pStartAt);
            nMoved = s_min(ac_nRequested, nDiff);
        } else if ((this->m_nLength && ac_bIsReadPtr) ||
                   (!this->m_nLength && !ac_bIsReadPtr)) {
            const size_type nDiff = (size_type)(this->m_pEnd - ac_pStartAt);
            nMoved = s_min(ac_nRequested, nDiff);
        }
        return nMoved;
    }

    /**
      * Move the given pointer.
      * Must only be used after using mf_nGetPiece()
      * to find out how much it can be moved..
      * This method uses the lock.
      * @param a_pToMove pointer to move
      * @param ac_nToMove how much it should be moved
      * @param ac_bIsReadPtr true if it's m_pRead
      */
    void mp_Move(tIPtr &a_pToMove, const size_type ac_nToMove,
                 const bool ac_bIsReadPtr)
    {
        const mt_Lock L(mc_Lock);
        a_pToMove += ac_nToMove;
        if (a_pToMove == this->m_pEnd)
            a_pToMove = this->m_pBegin;
        if (ac_bIsReadPtr)
            this->m_nLength -= ac_nToMove;
        else
            this->m_nLength += ac_nToMove;
    }

    tIPtr m_pBegin;
    tIPtr m_pEnd;
    tIPtr m_pRead;
    tIPtr m_pWrite;
    tIPtr m_pTempR;
    tIPtr m_pTempW;
    size_type m_nLength;
    const size_type mc_nSize;
    const tCriticalSection mc_Lock;

    CircularLogic(const CircularLogic &);
    CircularLogic &operator=(const CircularLogic &);
};

#ifdef S_SPEC_PRINT
template <class tType, bool tIsPtr, class tCriticalSection>
struct tCircularLogicSpecPrinter {
    void operator()(const CircularLogic<tType, tCriticalSection> &ac_ToPrint)
        const s_no_throw
    {
        s_Dbgpf("begin  : 0x%08x", ac_ToPrint.m_pBegin);
        s_Dbgpf("end    : 0x%08x", ac_ToPrint.m_pEnd);
        s_Dbgpf("read   : 0x%08x", ac_ToPrint.m_pRead);
        s_Dbgpf("write  : 0x%08x", ac_ToPrint.m_pWrite);
        s_Dbgpf("diff   : %d", ac_ToPrint.m_pWrite - ac_ToPrint.m_pRead);
        s_Dbgpf("length : %d", ac_ToPrint.m_nLength);
    }
};

template <class tType, class tCriticalSection>
struct tCircularLogicSpecPrinter<tType, true, tCriticalSection> {
    void operator()(const CircularLogic<tType, tCriticalSection> &ac_ToPrint)
        const s_no_throw
    {
        s_Dbgpf("begin  : 0x%08x", ac_ToPrint.m_pBegin[0]);
        s_Dbgpf("end    : 0x%08x", ac_ToPrint.m_pEnd[0]);
        s_Dbgpf("read   : 0x%08x", ac_ToPrint.m_pRead[0]);
        s_Dbgpf("write  : 0x%08x", ac_ToPrint.m_pWrite[0]);
        s_Dbgpf("diff   : %d", ac_ToPrint.m_pWrite[0] - ac_ToPrint.m_pRead[0]);
        s_Dbgpf("length : %d", ac_ToPrint.m_nLength);
    }
};
#endif
}

S_NAMESPACE_END

#endif //#ifndef __STR_CIRCULARLOGIC_H__
