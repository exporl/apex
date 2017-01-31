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
 
#ifndef __WIN32_GETMACADRESS_H__
#define __WIN32_GETMACADRESS_H__

#include "typedefs.h"

  /**
    * namespace with win32 specific networking stuff
    ************************************************ */
namespace win32net
{

     /**
      * Fetches the MAC address(es).
      * @return vector containing all MACs, or empty if none found
      */
  streamapp::tStringVector gf_saGetMACaddress();

    /**
      * Prints the MAC address stored in an 8 byte array.
      * Normally only 6 bytes are used, see AdressLen in PIP_ADAPTER_INFO.
      * @param a_cMACData the adress
      * @return result
      */
  std::string gf_sPrintMACaddress( const unsigned char a_cMACData[ 8 ] );

}

#endif //#ifndef __WIN32_GETMACADRESS_H__
