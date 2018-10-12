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
  * [ file ] core/containers/pointertraits.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_POINTERTRAITS_H__
#define __STR_POINTERTRAITS_H__

#include "../mpl/if.h"

#include "../typetraits/checks.h"
#include "../typetraits/removepointer.h"

#include "pointertopointer.h"

#include <string.h>

S_NAMESPACE_BEGIN

namespace types
{

/**
  * tPointerTraits
  *   used to select straight pointer or PointerToPointer at compile time.
  *   Use this for classes with pointer logic that has to work for both
  *   pointer and pointer to pointer, so you can write all logic as if
  *   it were all straight pointers without having to bother with anything
  *   else then the logic, hence without having to duplicate all code.
  *
  *   Use the static smf_MemCpy functions instead of memcpy if data has
  *   to be copied, they'll take care of copying all channels.
  *
  *   Only withdrawal to this approach is that you might want to keep
  *   extra pointers as class members instead of having local temporary
  *   objects in functions, since PointerToPointer's constructor has to
  *   allocate memory.
  ************************************************************************ */
template <class tType>
struct tPointerTraits {
    typedef tType *mt_Pointer;
    typedef types::PointerToPointer<tType> mt_Pointer2Pointer;
    typedef types::tIsPointer<tType> mt_IsPointer;

    //! tIntPtr is for internal use
    typedef typename mpl::tIf<mt_IsPointer::value, mt_Pointer2Pointer,
                              mt_Pointer>::type tIntPtr;

    //! tExtPtr is for exposed use: arguments and return values
    typedef tType *tExtPtr;
    typedef const tType *tExtCPtr;

    /**
      * Memcopy for normal pointers.
      * @param ac_pSrc source
      * @param a_pDest destination
      * @param ac_nElements number of elements (NOT bytes)
      */
    s_finline static void smf_MemCpy(mt_Pointer &a_pDest, tExtCPtr ac_pSrc,
                                     const size_type ac_nElements)
    {
        memcpy(a_pDest, ac_pSrc, sizeof(tType) * ac_nElements);
    }

    /**
      * Memcopy for PointerToPointer.
      * Copies all a_pDest's channels
      * @param a_pDest destination
      * @param ac_pSrc source pointer to pointer
      * @param ac_nElements number of elements (NOT bytes)
      */
    s_finline static void smf_MemCpy(mt_Pointer2Pointer &a_pDest,
                                     tExtCPtr ac_pSrc,
                                     const size_type ac_nElements)
    {
        typedef typename types::tRemovePointer<tType>::type tPointedTo;
        s_static_assert(!types::tIsPointer<tPointedTo>::value,
                        PointerToPOinterToPOinterIsNotAllowed);
        const size_type nBytes = sizeof(tPointedTo) * ac_nElements;
        const size_type nChannels = a_pDest.mf_nGetNumChannels();
        for (size_type i = 0; i < nChannels; ++i)
            memcpy(a_pDest[i], ac_pSrc[i], nBytes);
    }
};
}

S_NAMESPACE_END

#endif //#ifndef __STR_POINTERTRAITS_H__
