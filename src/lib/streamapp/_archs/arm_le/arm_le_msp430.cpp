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
 
#include "arm_le_msp430.h"
#include "defines.h"
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

#include <i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#define I2C_DEVICE "/dev/i2c/0"
#define I2C_DEV_ADDR 0x23

using namespace controller;

MSP430Control::MSP430Control() :
  mv_bInit( false ),
  m_nOldVal( 0 ),
  m_nfdI2C( -1 )
{
  /*if( mf_bInitController() )
  {
    DBG( "MSP430 initted" );
  }
  else
  {
    DBG( "MSP430 init failed!!" );
  }*/
}

MSP430Control::~MSP430Control()
{
  //mf_bDeInitController();
}

bool MSP430Control::mf_bInitController()
{
  if( mv_bInit )
    return true;

  m_nfdI2C = open( I2C_DEVICE, O_RDWR );

  if( m_nfdI2C < 0 )
  {
    DBGPF( "Error while opening i2c device %s", I2C_DEVICE );
    return false;
  }

  if( ioctl( m_nfdI2C, I2C_SLAVE, I2C_DEV_ADDR ) == -1 )
  {
    DBGPF( "Failed to set I2C_SLAVE to %#x", I2C_DEV_ADDR );
    return false;
  }

  if( mf_bGetIrVal( &m_nOldVal ) == false )
  {
    return false;
  }

  DBG( "MSP430 initted" );

  return mv_bInit = true;
}

bool MSP430Control::mf_bDeInitController()
{
  if( !mv_bInit )
    return false;

  if( close( m_nfdI2C ) == -1 )
  {
    DBGPF( "Failed to close i2c device %s", I2C_DEVICE );
    return false;
  }

  m_nfdI2C = -1;

  DBG( "MSP430 de-initted" );

  return true;
}

bool MSP430Control::mf_bGetIrVal( int *val )
{
  static __u8 buf[ 4 ];

  buf[ 0 ] = 2;
  buf[ 1 ] = 3;

  if( write( m_nfdI2C, buf, 2 ) != 2 )
  {
    DBGPF( "Failed to write get IR value command to device [%s]\n", strerror( errno ) );
    return false;
  }

  usleep( 100 );

  if( read( m_nfdI2C, buf, 4 ) != 4 )
  {
    DBG( "Failed to read RTC data\n" );
    return false;
  }

  //DBGPF( "Received ir buffer %d %d\n", buf[ 2 ], buf[ 3 ] );

#if 0
  if( buf[ 0 ] != 4 || buf[ 1 ] != 3 )
  {
    DBGPF( "Failed to read IR ACK from device [%d %d]\n", buf[ 0 ], buf[ 1 ] );
    return false;
  }
#endif

  *val = buf[ 2 ] | ( buf[ 3 ] << 8 );

  return true;
}

bool MSP430Control::mf_bPoll( mt_eControlCode& a_Code )
{
  static int key = 0;
  static int val = 0;

  if( !mv_bInit )
  {
    DBG( "MSP430 not initialized!!" );
    a_Code = mc_eNoButton;
    return false;
  }

  if( mf_bGetIrVal( &val ) == false )
  {
    DBG( "error receiving!!" );
    return false;
  }

  if( val != m_nOldVal )
  {
    key = val & ~( 1 << 11 ); //muha
    m_nOldVal = val;
  }
  else
  {
    key = 0;
  }

  //DBGPF( "got key %#x", key );

  usleep( 100 );

  switch( key )
  {
    case 0x300c :
      a_Code = mc_ePower; break;
    case 0x3020 :
      a_Code = mc_eDown; break;
    case 0x3021 :
      a_Code = mc_eUp; break;
    case 0x3011 :
      a_Code = mc_eLeft; break;
    case 0x3010 :
      a_Code = mc_eRight; break;
    case 0x300d :
      a_Code = mc_eSelect; break;
    default:
      a_Code = mc_eNoButton; break;
  };

  return true;
}
