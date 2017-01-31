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

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4267 ) //size_t to unsigned int
#endif

  /**
    * Convert from string to anything else.
    * @param s the string
    * @return the requested type
    */
template<typename T> T stringTo( const std::string& s )
{
  std::istringstream iss( s );
  T x;
  iss >> x;
  return x;
}

  /**
    * Convert anything to string.
    * @param x the requested type
    * @return the string
    */
template<typename T> std::string toString( const T& x )
{
  std::ostringstream oss;
  oss << x;
  return oss.str();
}

  /**
    * Print vector contents to std::cout.
    * Uses toString().
    * @param ac_Vector the vector to print
    */
template<typename T> void PrintVector( const std::vector<T>& ac_Vector )
{
  const std::vector<std::string>::size_type n = ac_Vector.size();
  for( std::vector<std::string>::size_type i = 0 ; i < n ; ++i )
    std::cout << toString( ac_Vector.at( i ) ).c_str() << std::endl ;
  std::cout << std::endl;
}

#endif //#ifndef STRINGUTILS_H
