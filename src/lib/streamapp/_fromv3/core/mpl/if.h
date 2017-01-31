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

/**
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/mpl/if.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_IF_H__
#define __STR_IF_H__

#include "../_main.h"

S_NAMESPACE_BEGIN

  /**
    * namespace with metaprogamming stuff.
    * @note weird gcc behaviour: it can't resolve
    * names in templates properly unless they're qualified with @c mpl::
    */
namespace mpl
{

  template< bool tVal, class tTrueType, class tFalseType >
  struct tIf
  {
    typedef tTrueType type;
  };

  template< class tTrueType, class tFalseType >
  struct tIf< false, tTrueType, tFalseType >
  {
    typedef tFalseType type;
  };


  template< bool tVal, class tTrueType, class tFalseType >
  struct tIfTyped
  {
    typedef typename tTrueType::type type;
  };

  template< class tTrueType, class tFalseType >
  struct tIfTyped< false, tTrueType, tFalseType >
  {
    typedef typename tFalseType::type type;
  };

}

S_NAMESPACE_END

#endif //#ifndef __STR_IF_H__
