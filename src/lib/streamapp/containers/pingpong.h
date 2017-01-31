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
 
#ifndef DUBBELBUFFER_H
#define DUBBELBUFFER_H

#include "containers/matrix.h"

namespace streamapp
{

    /**
      * PingPong
      *   pingpong buffers of pointers to tType.
      *   Use to alternatley switch between 2 memory locations.
      *   See google for a complete pingpong buffer explanation.
      ********************************************************** */
  template<class tType>
  class PingPong
  {
  public:
    typedef ArrayStorage<tType*> tElementPtrArray;

      /**
        * Constructor.
        * The memory itself from the arguments will never be touched.
        * @param ac_pPing the first pointer
        * @param ac_pPong the second pointer
        */
    PingPong( tType* ac_pPing, tType* ac_pPong ) :
      mv_nIndex( 0 ),
      mv_Array( 2 )
    {
      mv_Array.mp_Set( 0, ac_pPing );
      mv_Array.mp_Set( 1, ac_pPong );
    }

      /**
        * Destructor.
        */
    virtual ~PingPong()
    {
    }

      /**
        * Get current buffer 0.
        * @return the pointer
        */
    tType* mf_GetTxBuffer() const
    {
      return mv_Array( mv_nIndex );
    }

      /**
        * Get current buffer 1.
        * @return the pointer
        */
    tType* mf_GetRxBuffer() const
    {
      int Rx = mv_nIndex ^ 1;
      return mv_Array( Rx );
    }

      /**
        * Switch the buffer pointers.
        */
    void mp_BufferSwitch()
    {
      mv_nIndex ^= 1;
    }

  private:
    volatile int      mv_nIndex;
    tElementPtrArray  mv_Array;

    PingPong( const PingPong& );
    PingPong& operator = ( const PingPong& );
  };

    /**
      * PingPongStreamBuf
      *   a PingPong implementation using MatrixStorage.
      *   @see PingPong
      *   @see MatrixStorage
      ************************************************** */
  template< class tType >
  class PingPongStreamBuf : public PingPong < MatrixAccess<tType> >
  {
  public:
      /**
        * Constructor.
        * Allocates the two MatrixStorage objects needed.
        * @param ac_nChannels the number of channels for the MatrixStorage
        * @param ac_nBufferSize the number of samples for the MatrixStorage
        */
    PingPongStreamBuf( const unsigned ac_nChannels, const unsigned ac_nBufferSize ) :
      PingPong < MatrixAccess<tType> >( new MatrixStorage<tType>( ac_nChannels, ac_nBufferSize ), new MatrixStorage<tType>( ac_nChannels, ac_nBufferSize ) )
    {
    }

      /**
        * Destructor.
        */
    ~PingPongStreamBuf()
    {
      delete PingPong< MatrixAccess<tType> >::mf_GetRxBuffer();
      delete PingPong< MatrixAccess<tType> >::mf_GetTxBuffer();
    }

  private:
    PingPongStreamBuf( const PingPongStreamBuf& );
    PingPongStreamBuf& operator = ( const PingPongStreamBuf& );
  };

}

#endif //DUBBELBUFFER_H
