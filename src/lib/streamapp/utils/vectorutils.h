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
 
#ifndef __VECTORUTILS_H__
#define __VECTORUTILS_H__

#include <vector>

namespace utils
{

  template< class tType >
  bool f_bHasElement( const std::vector<tType>& ac_VectorToSearch, const tType& ac_ItemToFind )
  {
    const typename std::vector<tType>::size_type c_nItems = ac_VectorToSearch.size();
    for( typename std::vector<tType>::size_type i = 0 ; i < c_nItems ; ++ i )
      if( ac_VectorToSearch[ i ] == ac_ItemToFind )
        return true;
    return false;
  }

    //!-1 = not found
  template< class tType >
  int f_nFindElement( const std::vector<tType>& ac_VectorToSearch, const tType& ac_ItemToFind )
  {
    const typename std::vector<tType>::size_type c_nItems = ac_VectorToSearch.size();
    for( typename std::vector<tType>::size_type i = 0 ; i < c_nItems ; ++ i )
      if( ac_VectorToSearch[ i ] == ac_ItemToFind )
        return (int) i;
    return -1;
  }

  template< class tType >
  void f_AddFrom( const std::vector<tType>& ac_VectorToAddFrom, std::vector<tType>& ac_VectorToAddTo )
  {
    const typename std::vector<tType>::size_type c_nItems = ac_VectorToAddFrom.size();
    for( typename std::vector<tType>::size_type i = 0 ; i < c_nItems ; ++ i )
      ac_VectorToAddTo.push_back( ac_VectorToAddFrom.at( i ) ); 
  }

}

#endif //#ifndef __VECTORUTILS_H__
