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

#ifndef __SYSTEMUTILS_H__
#define __SYSTEMUTILS_H__

#include <QString>

  /**
    * namespace with system utilities
    ********************************* */
namespace systemutils
{

    /**
      * Make a binary copy of a file (without using system calls).
      * @param ac_sInputFile the file to copy
      * @param ac_sOutputFile the copy of the file
      * @return true on succesfull copy
      */
  bool f_bCopyFile( const QString &ac_sInputFile, const QString &ac_sOutputFile );

    /**
      * Check if a file exists.
      * @return true if the file exists and can be opened for reading
      */
  bool f_bFileExists( const QString &ac_sFile );

    /**
      * Clear an entire file.
      */
  bool f_bClearFile( const QString &ac_sFile );

}

#endif //#ifndef __SYSTEMUTILS_H__
