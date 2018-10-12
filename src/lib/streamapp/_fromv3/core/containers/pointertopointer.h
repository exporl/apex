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
  * [ file ] core/containers/pointertopointer.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_POINTER_H__
#define __STR_POINTER_H__

#include "../_main.h"

#include <QtGlobal>

S_NAMESPACE_BEGIN

namespace types
{

/**
  * PointerToPointer
  *   purpose of this class is to be able to perform
  *   all kinds of pointer trickery on a pointer to a pointer,
  *   as if it was a plain pointer. It's only meant for 'block-style'
  *   pointers, ie pointers to arrays of equal length where all
  *   element operations always propagate over all dimesnsions.
  *   (like arrays with audio channels, each channel has an equal
  *   number of samples, always.)
  *
  *   All arithmetic and comparatory operations are basically done
  *   this way:
  *   - if the pointer content has to be changed, the operation
  *     is put in a for loop.
  *   - if a comparision is made, the fisrt element is compared;
  *     this is always safe, assuming that only the class itself
  *     may access the internal pointer.
  *
  *   @note use only for arithmetic and comparision on the pointers
  *   itself, this class doesn't have anything to do with the elemnts
  *   pointed to, this is not a safe container!!
  *   @see tPointerTraits to put this class to use
  ******************************************************************* */
template <class tType>
class PointerToPointer
{
public:
    /**
      * Constructor.
      * Allocates memory (using new) for ac_nPointedTo pointers.
      * @param a_pPointer the pointer
      * @param ac_nPointedTo the number of pointers pointed to
      */
    PointerToPointer(tType *a_pPointer, const size_type ac_nPointedTo)
        : m_pPointer(new tType[ac_nPointedTo]), m_nPointedTo(ac_nPointedTo)
    {
        Q_ASSERT(a_pPointer && ac_nPointedTo);
        for (size_type i = 0; i < ac_nPointedTo; ++i) {
            Q_ASSERT(a_pPointer[i]);
            m_pPointer[i] = a_pPointer[i];
        }
    }

    /**
      * Copy Constructor.
      * @param ac_Pointer object to copy
      */
    s_finline PointerToPointer(const PointerToPointer &ac_Pointer)
        : m_pPointer(ac_Pointer.m_pPointer),
          m_nPointedTo(ac_Pointer.m_nPointedTo)
    {
    }

    /**
      * Assignment operator.
      * @param a_pVal object to copy
      */
    s_finline PointerToPointer &operator=(const PointerToPointer &ac_Pointer)
    {
        if (&ac_Pointer != this) {
            if (m_nPointedTo != ac_Pointer.m_nPointedTo) {
                for (size_type i = 0; i < m_nPointedTo; ++i)
                    m_pPointer[i] = 0;
                delete[] m_pPointer;
                m_nPointedTo = ac_Pointer.m_nPointedTo;
                m_pPointer = new tType[m_nPointedTo];
            }
            for (size_type i = 0; i < m_nPointedTo; ++i)
                m_pPointer[i] = ac_Pointer.m_pPointer[i];
        }
        return *this;
    }

    /**
      * Assignment operator.
      * Better make sure ac_pPointer is ok!
      * @param ac_pPointer pointers to copy
      */
    s_finline PointerToPointer &operator=(const tType *ac_pPointer)
    {
        for (size_type i = 0; i < m_nPointedTo; ++i)
            m_pPointer[i] = ac_pPointer[i];
        return *this;
    }

    /**
      * Destructor.
      */
    ~PointerToPointer()
    {
        for (size_type i = 0; i < m_nPointedTo; ++i)
            m_pPointer[i] = 0;
        delete[] m_pPointer;
    }

    /**
      * Comparision operator.
      * @param ac_Pointer object to compare with
      * @return true if all pointers match
      */
    s_finline bool operator==(const PointerToPointer &ac_Pointer) const
    {
        return m_pPointer[0] == ac_Pointer.m_pPointer[0];
    }

    /**
      * Comparision operator.
      * @param ac_Pointer object to compare with
      * @return invert of operator ==
      */
    s_finline bool operator!=(const PointerToPointer &ac_Pointer) const
    {
        return !operator==(ac_Pointer);
    }

    /**
      * Comparision operator.
      * @param a_pVal pointers to compare with
      * @return true if this < a_pVal
      */
    s_finline bool operator<(const PointerToPointer &a_pVal) const
    {
        return m_pPointer[0] < a_pVal.m_pPointer[0];
    }

    /**
      * Comparision operator.
      * @param a_pVal pointers to compare with
      * @return true if this <= a_pVal
      */
    s_finline bool operator<=(const PointerToPointer &a_pVal) const
    {
        return m_pPointer[0] <= a_pVal.m_pPointer[0];
    }

    /**
      * Comparision operator.
      * @param a_pVal pointers to compare with
      * @return true if this > a_pVal
      */
    s_finline bool operator>(const PointerToPointer &a_pVal) const
    {
        return m_pPointer[0] > a_pVal.m_pPointer[0];
    }

    /**
      * Comparision operator.
      * @param a_pVal pointers to compare with
      * @return true if this < a_pVal
      */
    s_finline bool operator>=(const PointerToPointer &a_pVal) const
    {
        return m_pPointer[0] >= a_pVal.m_pPointer[0];
    }

    /**
      * Arithmetic operator.
      * @param ac_nVal value to add
      * @return reference to this
      */
    s_finline PointerToPointer &operator+(const size_type ac_nVal)
    {
        for (size_type i = 0; i < m_nPointedTo; ++i)
            m_pPointer[i] += ac_nVal;
        return *this;
    }

    /**
      * Arithmetic operator.
      * @param ac_nVal value to subtract
      * @return reference to this
      */
    s_finline PointerToPointer &operator-(const size_type ac_nVal)
    {
        for (size_type i = 0; i < m_nPointedTo; ++i)
            m_pPointer[i] -= ac_nVal;
        return *this;
    }

    /**
      * Arithmetic operator.
      * @param ac_nVal value to subtract
      * @return reference to this
      */
    s_finline size_type operator-(const PointerToPointer &ac_Pointer) const
    {
        return (size_type)(m_pPointer[0] - ac_Pointer.m_pPointer[0]);
    }

    /**
      * Arithmetic operator.
      * @param ac_nVal value to add
      * @return reference to this
      */
    s_finline PointerToPointer &operator+=(const size_type ac_nVal)
    {
        return operator+(ac_nVal);
    }

    /**
      * Arithmetic operator.
      * @param ac_nVal value to subtract
      * @return reference to this
      */
    s_finline PointerToPointer &operator-=(const size_type ac_nVal)
    {
        return operator-(ac_nVal);
    }

    /**
      * Arithmetic operator.
      * Increase pointers.
      * @return reference to this
      */
    s_finline PointerToPointer &operator++()
    {
        for (size_type i = 0; i < m_nPointedTo; ++i)
            ++m_pPointer[i];
        return *this;
    }

    /**
      * Arithmetic operator.
      * Decrease pointers.
      * @return reference to this
      */
    s_finline PointerToPointer &operator--()
    {
        for (size_type i = 0; i < m_nPointedTo; ++i)
            --m_pPointer[i];
        return *this;
    }

    /**
      * Get the number of pointers pointed to
      * @return number of 'channels'
      */
    s_finline const size_type &mf_nGetNumChannels() const
    {
        return m_nPointedTo;
    }

    /**
      * Set the number of channels pointed to.
      * Needless to say, but only use this if you know
      * very, very good what you are doing.
      */
    s_finline void mp_SetNumChannels(const size_type ac_nChannels)
    {
        m_nPointedTo = ac_nChannels;
    }

    /**
      * Get the underlying pointer.
      * @return a tType* (as tType is already a pointer)
      */
    s_finline operator tType *()
    {
        return m_pPointer;
    }

    /**
      * Get the underlying pointer as const.
      * @return a tType* (as tType is already a pointer)
      */
    s_finline operator const tType *() const
    {
        return m_pPointer;
    }

    s_track_mem_leaks(PointerToPointer)

        private : tType *m_pPointer;
    size_type m_nPointedTo;
};
}

S_NAMESPACE_END

#endif //#ifndef __STR_POINTER_H__
