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
 
#ifndef FIRTAPSTOOL_H
#define FIRTAPSTOOL_H

#include "utils/math.h"
//#include "file/binaryfile.h"
#include "containers/matrix.h"
using namespace streamapp;

static double sinc( const StreamType x )
{
  StreamType a;

  if( x != 0 )
    a = ( sin( math::gc_dPi * x ) ) / ( math::gc_dPi * x );
  else
    a = 1.0;

  return a;
}

  /**
    * Generate filtertaps for fir filters
    * based on resampling factor
    ************************************* */
class CFirTapsTool
{
public:
  CFirTapsTool( const unsigned ac_nDownFactor , const unsigned ac_nFilterLength ) :
    mc_aTaps( ac_nFilterLength, true ),
    mc_nDFac( ac_nDownFactor )
  {
  }

  ~CFirTapsTool()
  {
  }

  const ArrayAccess<StreamType>& mf_dfDesign_Aaf(  )
  {
    const unsigned nFirLen = mc_aTaps.mf_nGetBufferSize();
    const StreamType FirLen = (StreamType) nFirLen;
    ArrayStorage<StreamType>  nd( nFirLen, true );
    for( unsigned i = 0 ; i < nFirLen ; ++i )
    {
      mc_aTaps.mp_Set( i , i );
      nd.mp_Set( i , i - floor( FirLen / 2.0 ) );
    }

    const StreamType thc = ( 1.0 / mc_nDFac ) - ( 3.3 / FirLen );

    if( thc < 0.0 )
    {
      assert( 0 );
    }
    else
    {
      for( unsigned i = 0 ; i < nFirLen ; ++i )
      {
        mc_aTaps.mp_Set( i , thc * sinc( thc * nd( i ) ) * ( 0.54 - 0.46 * cos( 2 * math::gc_dPi * i / FirLen ) )  );
      }
    }

    /*streamapp::BinaryOutputFile file;
    file.mp_bOpen( "d:/taps", 1 );
    StreamBuf buf( 1, nFirLen );
    for( unsigned i = 0 ; i < nFirLen ; ++i )
      buf.mp_Set( 0, i, mc_aTaps( i ) );
    file.mp_lWrite( buf );
    file.mp_bClose();*/

    return mc_aTaps;
  }

  MatrixStorage<StreamType> mf_dfDesign_Aaf( const unsigned ac_nChannels )
  {
    MatrixStorage<StreamType> ret( ac_nChannels, mc_aTaps.mf_nGetBufferSize() );
    const ArrayAccess<StreamType>& p = mf_dfDesign_Aaf();
    for( unsigned j = 0 ; j < mc_aTaps.mf_nGetBufferSize() ; ++j )
    {
      for( unsigned i = 0 ; i < ac_nChannels ; ++i )
      {
        ret.mp_Set( i, j, p( j ) );
      }
    }

    /*streamapp::BinaryOutputFile file;
    file.mp_bOpen( "d:/taps", 1 );
    file.mp_lWrite( ret );
    file.mp_bClose();*/

    return ret;
  }

private:
  ArrayStorage<StreamType>  mc_aTaps;
  const unsigned            mc_nDFac;

  CFirTapsTool( const CFirTapsTool& );
  CFirTapsTool& operator = ( const CFirTapsTool& );
};

#endif //#ifndef FIRTAPSTOOL_H

