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
  * [ file ] core/typetraits/checks.h
  * [ fmod ] header - mpl
  * <^>
  */

#ifndef __STR_TT_CHECKS_H__
#define __STR_TT_CHECKS_H__

#include "../mpl/macros.h"

S_NAMESPACE_BEGIN

// make v2 compatible
namespace mpl
{
template <bool b0, bool b1, bool b2 = false, bool b3 = false, bool b4 = false,
          bool b5 = false>
struct tOrs {
    enum { value = true };
};
}

namespace types
{

s_enum_const_decl2(tIsSame, tType1, tType2,
                   false) s_enum_const_spec2(tIsSame, tType1, tType1, true)

    s_enum_const_decl2(tIsNotSame, tType1, tType2,
                       true) s_enum_const_spec2(tIsNotSame, tType1, tType1,
                                                false)

        s_enum_const_decl2(tIsSameSize, tType1, tType2,
                           (sizeof(tType1) == sizeof(tType2)))

            s_enum_const_decl(tIsFloat, tType, false) s_enum_const_spec(
                tIsFloat, float,
                true) s_enum_const_spec(tIsFloat, double,
                                        true) s_enum_const_spec(tIsFloat,
                                                                long double,
                                                                true)

                s_enum_const_decl(tIsBool, tType,
                                  false) s_enum_const_spec(tIsBool, bool, true)

                    s_enum_const_decl(tIsChar, tType, false) s_enum_const_spec(
                        tIsChar, char,
                        true) s_enum_const_spec(tIsChar, unsigned char,
                                                true) s_enum_const_spec(tIsChar,
                                                                        wchar_t,
                                                                        true)

                        s_enum_const_decl(tIsUnsignedInteger, tType, false)
                            s_enum_const_spec(
                                tIsUnsignedInteger, unsigned int,
                                true) s_enum_const_spec(tIsUnsignedInteger,
                                                        unsigned char, true)
                                s_enum_const_spec(
                                    tIsUnsignedInteger, unsigned short,
                                    true) s_enum_const_spec(tIsUnsignedInteger,
                                                            unsigned long, true)

                                    s_enum_const_decl(
                                        tIsSignedInteger, tType,
                                        false) s_enum_const_spec(tIsSignedInteger,
                                                                 int, true)
                                        s_enum_const_spec(tIsSignedInteger,
                                                          char, true)
                                            s_enum_const_spec(tIsSignedInteger,
                                                              short, true)
                                                s_enum_const_spec(
                                                    tIsSignedInteger, long,
                                                    true)

                                                    s_enum_const_decl(tIsVoid,
                                                                      tType,
                                                                      false)
                                                        s_enum_const_spec(
                                                            tIsVoid, void, true)

                                                            s_enum_const_decl(
                                                                tIsPointer,
                                                                tType, false)
                                                                s_enum_const_declt(
                                                                    tIsPointer,
                                                                    tType,
                                                                    tType *,
                                                                    true)
    // s_enum_const_declt( tIsPointer, tType, tType(*)( ... ), true ) FIXME
    // pointer to func etc

    s_enum_const_decl(tIsReference, tType, false)
        s_enum_const_declt(tIsReference, tType, tType &, true)

            template <class tType>
            struct tIsInteger
    : public mpl::tOrs< // FIXME this is for use with the convertor stuff only?
          tIsSignedInteger<tType>::value, tIsUnsignedInteger<tType>::value> {
};

template <class tType>
struct tIsIntegral
    : public mpl::tOrs<tIsChar<tType>::value, tIsBool<tType>::value,
                       tIsSignedInteger<tType>::value,
                       tIsUnsignedInteger<tType>::value> {
};

template <class tType>
struct tIsArithmetic
    : public mpl::tOrs<tIsFloat<tType>::value, tIsIntegral<tType>::value> {
};

template <class tType>
struct tIsScalar
    : public mpl::tOrs<
          tIsArithmetic<tType>::value, tIsPointer<tType>::value
          //,tIsEnum< tType >::value how the hell do you figure this out?
          //,tIsPointerToMember< tType >::value
          > {
};

template <class tType>
struct tIsPOD
    : public mpl::tOrs<tIsScalar<tType>::value, tIsVoid<tType>::value> {
};

#define s_declare_pod(tType)                                                   \
    s_enum_const_spec(::smn::types::tIsPOD, tType, true)

template <class tBase, class tDerived>
class tBaseCheck
{
    typedef char tSmall;
    class tBig
    {
        tSmall m_Dummy[2];
    };
    static tSmall sf_Test(const tDerived &);
    static tBig sf_Test(...);
    static tBase sf_MakeBase();

public:
    enum { mc_eExists = sizeof(sf_Test(sf_MakeBase())) == sizeof(tSmall) };
};

#define s_is_supersubclass(tBase, tDerived)                                    \
    (::smn::types::tBaseCheck<const tDerived *, const tBase *>::mc_eExists &&  \
     !::smn::types::tIsSame<const tDerived *, const void *>::value)

#define s_inherits(tDerived, tBase)                                            \
    (s_is_supersubclass(tBase, tDerived) &&                                    \
     !::smn::types::tIsSame<const tDerived *, const tBase *>::value)
}

S_NAMESPACE_END

#endif //#ifndef __STR_TT_CHECKS_H__
