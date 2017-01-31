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
 
/**
  * @file socketfunc.h
  * contains some helpers for working with sockets
  */

#ifndef __SOCKETFUNC_H__
#define __SOCKETFUNC_H__

#include "socket.h"
#include "containers/rawmemory.h"
using namespace streamapp;

namespace network
{

    /**
      * CommSocket
      *   extends ISocket with a somewhat higher level protocol.
      *   Works by first sending or receiving the data size
      *   before the actual data.
      *   Can also send or receive a 10 byte ID code first to
      *   enforce remote sites to know about each other.
      *   @note don't send something > 32767 bytes
      *   @TODO find out what's the optimum size to send
      ********************************************************** */
  class CommSocket
  {
  public:
      /**
        * Constructor.
        * @param ac_pSocket, should be connected or nothing will happen.
        * @param ac_bDisconnectOnKill if true, disconnects the socket upon destruction or error
        */
    CommSocket( ISocket* const ac_pSocket, const bool ac_bDisconnectOnKill = true );

      /**
        * Destructor.
        * Might close cocket.
        */
    ~CommSocket();

      /**
        * Set to use a "magic number".
        * If set, the number is sent before the size,
        * this expects the remote to do the same off course.
        * The number cannot be zero.
        * @param ac_nMagicNumber the code t use
        */
    INLINE void mp_UseMagicNumber( const unsigned ac_nMagicNumber = 0xff666fff )
    {
      s_assert( ac_nMagicNumber );
      mv_nMagicNumber = ac_nMagicNumber;
    }

      /**
        * Disable use of magic number.
        * This is the default.
        */
    INLINE void mp_NoMagicNumber()
    {
      mv_nMagicNumber = 0;
    }

      /**
        * Get the magic number in use.
        * Returns 0 if unused.
        * @return the number
        */
    INLINE const unsigned& mf_nGetMagicNumber() const
    {
      return mv_nMagicNumber;
    }

      /**
        * Return status of the read and write functions.
        */
    enum mt_eStatus
    {
      mc_eOk,         //!< no problems
      mc_eSockNoC,    //!< socket is not connected
      mc_eSockErrR,   //!< socket read error
      mc_eSockErrW,   //!< socket write error
      mc_eMagicErr    //!< magic number mismatch
    };

      /**
        * Try to read form the socket to fill the memory block.
        * Expects that first 10 bytes are received containing the size
        * of the data to receive.
        * If set to use a magic number, this will be read first and must
        * match the number set for this.
        * Then tries to read all data into the given memory.
        * Memory is resized to match the received size.
        * @param a_Destination the memory to put received data in
        * @return one of mt_eStatus
        * @see gf_bWriteToSocket
        */
    mt_eStatus mf_eReadFromSocket( RawMemory& a_Destination );

      /**
        * Try to write the memory block to the remote site.
        * Sends magic number first if required.
        * Then sends 10 bytes containing the memory's size,
        * and then sends the entire block.
        * @param ac_Source the memory to send
        * @return one of mt_eStatus
        * @see gf_bReadFromSocket()
        */
    mt_eStatus mf_eWriteToSocket( const RawMemoryAccess& ac_Source );


      /**
        * Try to write a part of the memory block to the other side.
        * @see mf_eWriteToSocket
        * @param ac_Source  the memory to send
        * @param ac_nBytesToSend number of bytes to send, must be < size of ac_Source
        * @return  one of mt_eStatus
        */
    mt_eStatus mf_eWriteToSocket( const RawMemoryAccess& ac_Source, const unsigned ac_nBytesToSend );

      /**
        * Get a reference to the socket used.
        * @return const reference
        */
    INLINE const ISocket& mf_GetSocket()
    {
      return *m_pSocket;
    }

      /**
        * Replace the socket used.
        * The old one remains untouched.
        * @param ac_pSocket the new socket.
        */
    INLINE void mp_SetSocket( ISocket* const ac_pSocket )
    {
      m_pSocket = ac_pSocket;
    }

  private:
    mt_eStatus mp_eReturnWithError( mt_eStatus a_Err );

    ISocket*  m_pSocket;
    bool      mc_bDisconnect;
    unsigned  mv_nMagicNumber;
    char      m_pRecvBuffer[ 32 ];

    CommSocket( const CommSocket& );
    CommSocket& operator = ( const CommSocket& );
  };

}

#endif //#ifndef __SOCKETFUNC_H__
