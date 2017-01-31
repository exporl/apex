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

#include "systemutils.h"
#include <fstream>
#include <iostream>

using namespace std;

namespace
{
  const int sc_nBufferSize = 4096;
}

bool systemutils::f_bCopyFile( const std::string& ac_sInputFile, const std::string& ac_sOutputFile )
{
    //try open files
  ifstream inFileStream( ac_sInputFile.data(), ios::in | ios::binary );
  if( !inFileStream )
    return false;

  ofstream outFileStream( ac_sOutputFile.data(), ios::out | ios::binary );
  if( !outFileStream )
    return false;

    //copy
  char buff[ sc_nBufferSize ];
  int readBytes = sc_nBufferSize;

  while( readBytes == sc_nBufferSize )
  {
    inFileStream.read( buff, sc_nBufferSize );
    readBytes = (int)inFileStream.gcount();
    outFileStream.write( buff, readBytes );
  }
  inFileStream.close();
  outFileStream.close();
  return true;
}

bool systemutils::f_bFileExists( const std::string& ac_sFile )
{
  ifstream inFileStream( ac_sFile.data(), ios::in | ios::binary );
  if( !inFileStream )
    return false;
  inFileStream.close();
  return true;
}

bool systemutils::f_bClearFile( const std::string& ac_sFile )
{
  ifstream inFileStream( ac_sFile.data(), ios::trunc | ios::binary );
  if( !inFileStream )
    return false;
  inFileStream.close();
  return true;
}
