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

#ifndef EXCEPTIONSS_H
#define EXCEPTIONSS_H

#include <exception>
#include <string>

/**
  * namespace with all kinds of utilities and tools
  ************************************************* */
namespace utils
{

/**
  * StringException
  *   exception with an std::string.
  *   Used in a lot of places in streamapp.
  *   @code
  *   throw( StringException( "problem!" );
  *   @endcode
  *   @see PtrCheck for an example
  ***************************************** */
struct StringException : std::exception {
    explicit StringException(const std::string &ac_sError)
        : mc_sError(ac_sError)
    {
    }

    ~StringException() throw()
    {
    }

    std::string mc_sError;
};
}

#endif //#ifndef EXCEPTIONSS_H
