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

#ifndef __CHECKS_H_
#define __CHECKS_H_

#include "stringexception.h"

namespace utils
{

/**
  * PtrCheck
  *   throws exception for a 0 pointer,
  *   else returns the pointer.
  *   @code
  *   try
  *   {
  *     tType* pMustBeNonZero = PtrCheck( pToCheck );
  *   }
  *   catch( StringException& e )
  *   {
  *     std::cout << e.mc_sError.c_str();
  *     exit( 0 );
  *   }
  *   @endcode
  *   @see StringException
  *************************************************** */
template <class tType>
tType *PtrCheck(tType *p)
{
    if (!p)
        throw(utils::StringException("Invalid Pointer"));
    else
        return p;
}
}

#endif //#ifndef __CHECKS_H_
