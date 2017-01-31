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
 
#include "wav2bin.h"
#include "file/wavefile.h"
#include "file/binaryfile.h"
#include "streamappfactory.h"
#include "utils/stringexception.h"

using namespace streamapp;

void streamapp::f_Wav2Bin( const std::string& ac_sWaveFile, const std::string& ac_sBinaryFilePrefix )
{
  PositionableAudioFormatReader* p = StreamAppFactory::sf_pInstance()->mf_pReader( ac_sWaveFile );
  InputStream* pIn = StreamAppFactory::sf_pInstance()->mf_pReaderStream( p, p->mf_lTotalSamples() );

  BinaryOutputFile out;
  if( !out.mp_bOpen( ac_sBinaryFilePrefix, p->mf_nChannels() ) )
    throw( utils::StringException( "Couldn't open binary file " + ac_sBinaryFilePrefix ) );

  out.Write( pIn->Read() );
  out.mp_bClose();

  delete pIn;
}
