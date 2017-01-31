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
 
#ifndef __WAV2BIN_H__
#define __WAV2BIN_H__

#include <string>

namespace streamapp
{

    /**
      * Convert a wavefile to a binary file.
      * Generates one binary file per channel.
      * @warning creates a buffer to keep the whole file
      * at once, so the amount of memory used can get
      * quite large, if not too large
      * @param ac_sWaveFile the input file's path
      * @param ac_sBinaryFilePrefix the prefix for the binary outputfile(s).
      */
  void f_Wav2Bin( const std::string& ac_sWaveFile, const std::string& ac_sBinaryFilePrefix );

}

#endif //#ifndef __WAV2BIN_H__
