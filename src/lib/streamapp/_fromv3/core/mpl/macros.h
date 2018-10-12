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
  * [ file ] core/mpl/macros.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_MPL_MACROS_H__
#define __STR_MPL_MACROS_H__

#include "../_mainnb.h"

#define s_enum_const_decl(tName, tType, tVal)                                  \
    template <class tType>                                                     \
    struct tName {                                                             \
        enum { value = tVal };                                                 \
    };

#define s_enum_const_spec(tName, tType, tVal)                                  \
    template <>                                                                \
    struct tName<tType> {                                                      \
        enum { value = tVal };                                                 \
    };

#define s_enum_const_declt(tName, tType1, tType2, tVal)                        \
    template <class tType1>                                                    \
    struct tName<tType2> {                                                     \
        enum { value = tVal };                                                 \
    };

#define s_enum_const_decl2(tName, tType1, tType2, tVal)                        \
    template <class tType1, class tType2>                                      \
    struct tName {                                                             \
        enum { value = tVal };                                                 \
    };

#define s_enum_const_spec2(tName, tType1, tType2, tVal)                        \
    template <class tType1>                                                    \
    struct tName<tType1, tType2> {                                             \
        enum { value = tVal };                                                 \
    };

#define s_static_integral_const_decl(tName, tType, tVal, tValType)             \
    template <class tType>                                                     \
    struct tName {                                                             \
        static const tValType value = tVal;                                    \
        operator const tValType &() const s_no_throw                           \
        {                                                                      \
            return this->value;                                                \
        }                                                                      \
    };

#define s_static_integral_const_spec(tName, tType, tVal, tValType)             \
    template <>                                                                \
    struct tName<tType> {                                                      \
        static const tValType value = tVal;                                    \
        operator const tValType &() const s_no_throw                           \
        {                                                                      \
            return this->value;                                                \
        }                                                                      \
    };

#endif //#ifndef __STR_MPL_MACROS_H__
