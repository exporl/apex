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

#include "outputtest.h"

#ifdef WRITEWAVOUTPUT

#include "error/apexerror.h"
#include "services/errorhandler.h"
#include "datablock/datablocksdata.h"
#include <compare.h>
#include "wavdeviceio.h"

using apex::tError;
using apex::data::DatablocksData;
using streamapp::Compare;
using namespace apex::stimulus;

WavOutputTest::WavOutputTest( const DatablocksData& ac_DBlocks, const QString& ac_sFileToCompareWith ) throw()
{
  mp_Init( ac_DBlocks, ac_sFileToCompareWith );
}

WavOutputTest::~WavOutputTest() throw()
{
}

void WavOutputTest::mp_Init( const DatablocksData& a_DBlocks, const QString& ac_sFileToCompareWith ) throw()
{
  apex::tError info( tError::Message, ">>>>>> WavoutputTest", "getting test data" );
  ErrorHandler::Get().Add( info );

  DatablocksData::const_iterator itB = a_DBlocks.begin();
  DatablocksData::const_iterator itE = a_DBlocks.end();
  DatablocksData::const_iterator it;

  if( ac_sFileToCompareWith.isEmpty() )
  {
    for( it = itB ; it != itE ; ++it )
      //m_Files.push_back( it.value()->uri().path().toAscii().data() );
        FilePrefixConvertor::addPrefix(a_DBlocks.prefix(), it.value()->uri());
  }
  else
  {
    m_Files.push_back( ac_sFileToCompareWith.toAscii().data() );
  }
}

void WavOutputTest::mf_Run() const throw()
{
  apex::tError info( tError::Message, ">>>>>> WavoutputTest", "running test" );
  ErrorHandler::Get().Add( info );

  const unsigned nFiles = m_Files.size();
  streamapp::ArrayStorage< std::string > filez( nFiles );
  for( unsigned i = 0 ; i < nFiles ; ++i )
    filez.mp_Set( i, m_Files[ i ] );

  Compare::mt_eResult eRes = Compare::sf_eCompareWave( filez, WavDeviceIO::sc_sOutputName );

  if( eRes == Compare::mc_eIdentical )
  {
    info.sMessage = "TEST OK";
  }
  else if( eRes == Compare::mc_eNotIdentical )
  {
    info.sMessage = "TEST NOT OK";
    info.eType = tError::Error;
  }
  else if( eRes == Compare::mc_eFailedOpen )
  {
    info.sMessage = "TEST NOT STARTED: failed to open a file";
    info.eType = tError::Error;
  }

  ErrorHandler::Get().Add( info );
}

#endif //#ifdef WRITEWAVOUTPUT
