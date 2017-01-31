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
  * [ file ] core/typetraits/removepointer.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_REMOVEPOINTER_H__
#define __STR_REMOVEPOINTER_H__

#include "core/_main.h"

S_NAMESPACE_BEGIN

namespace types
{

  template< class T >
  struct tRemovePointer
  {
    typedef T type;

    s_static_assert_inclass( 0, NotAPointer )
  };

  template< class T >
  struct tRemovePointer< T* >
  {
    typedef T type;
  };

  template< class T >
  struct tRemovePointer< const T* >
  {
    typedef T type;
  };

  template< class T >
  struct tRemovePointer< T* const >
  {
    typedef T type;
  };

  template< class T >
  struct tRemovePointer< volatile T* >
  {
    typedef T type;
  };

  template< class T >
  struct tRemovePointer< volatile T* const >
  {
    typedef T type;
  };

}

S_NAMESPACE_END

#endif //#ifndef __STR_REMOVEPOINTER_H__
