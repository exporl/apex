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
 
#include "stdredirector.h"

using namespace utils;

namespace
{
  std::string sc_Out( "stdcout.txt" );
  std::string sc_Err( "stdcerr.txt" );
}

StdRedirector::StdRedirector() :
  m_Cout( sc_Out.c_str() ),
  m_Cerr( sc_Err.c_str() ),
  m_pCoutBuf( std::cout.rdbuf() ),
  m_pCerrBuf( std::cerr.rdbuf() )
{
  std::streambuf* outbuf = m_Cout.rdbuf();
  std::streambuf* errbuf = m_Cerr.rdbuf();
  std::cout.rdbuf( outbuf );
  std::cerr.rdbuf( errbuf );
}

StdRedirector::~StdRedirector()
{
  std::cout.rdbuf( m_pCoutBuf );
  std::cerr.rdbuf( m_pCerrBuf );
}
