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
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/containers/looplogic.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_LOOPLOGIC_H__
#define __STR_LOOPLOGIC_H__

#include "../../../defines.h"

#include "../mpl/if.h"

#include "../typetraits/checks.h"

S_NAMESPACE_BEGIN

namespace containers
{

    /**
      * tLoopLogicBase
      *   helper for LoopLogic.
      *   Figures out type used.
      ************************** */
  template< class tType, bool tUseReferences >
  struct tLoopLogicBase
  {
    s_static_assert_inclass( types::tIsReference< tType >::value == false, AlreadyAReference )

    typedef tType     tExtType;
    typedef tType&    mt_RefType;
    typedef typename mpl::tIf< tUseReferences, mt_RefType, tExtType >::type tIntType;
  };


    /**
      * LoopLogic
      *   provides logic for looping, ie given a startpoint,
      *   endpoint and looppoints, calculates the movements.
      *   @param tType a size type
      *   @param tUseReferences true if references should be used
      *   FIXME optimize logic (although on the other hand it's
      *   nice to see what's going on whilst stepping)
      ************************************************************ */
  template< class tType, bool tUseReferences >
  class LoopLogic : public tLoopLogicBase< tType, tUseReferences >
  {
  public:
    typedef tLoopLogicBase< tType, tUseReferences > mt_Parent;
    typedef typename mt_Parent::tExtType tExtType;
    typedef typename mt_Parent::tIntType tIntType;

      /**
        * Constructor.
        * @param ac_nLength the endpoint
        */
    LoopLogic( const tExtType ac_nLength ) :
      mv_nRead( 0 ),
      mv_nLoopStart( 0 ),
      mv_nLoopEnd( 0 ),
      mc_nLength( ac_nLength )
    {
    }

      /**
        * Constructor.
        * @param ac_nLength endpoint
        * @param a_nLoopStart loop start
        * @param a_nLoopEnd loop end
        * @param a_nRead read point
        */
    LoopLogic( tIntType ac_nLength, tIntType a_nLoopStart, tIntType a_nLoopEnd, tIntType a_nRead ) :
      mv_nRead( a_nRead ),
      mv_nLoopStart( a_nLoopStart ),
      mv_nLoopEnd( a_nLoopEnd ),
      mc_nLength( ac_nLength )
    {
    }

      /**
        * Destructor.
        */
    ~LoopLogic()
    {
    }

      /**
        * Get maximum elements that can be read.
        * Equals ac_nRequested, or less if the
        * end of the loop was encountered.
        * @param ac_nRequested max elements to read
        * @return number of elements to be read
        */
    tType mf_nGetPiece( const tType ac_nRequested ) const
    {
      tType nMoved = 0;
      if( this->mv_nLoopEnd > this->mv_nLoopStart )
        nMoved = this->mv_nLoopEnd - this->mv_nRead;
      else if( this->mv_nLoopEnd < this->mv_nLoopStart && this->mv_nRead >= this->mv_nLoopStart )
        nMoved = this->mc_nLength - this->mv_nRead;
      else if( this->mv_nLoopEnd < this->mv_nLoopStart && this->mv_nRead <= this->mv_nLoopEnd )
        nMoved = this->mv_nLoopEnd + 1 - this->mv_nRead;
      else
        Q_ASSERT( 0 );
      return s_min( ac_nRequested, nMoved );
    }

      /**
        * Moves the read pointer.
        * Assumes the result of mf_nGetPiece()
        * is passed in.
        * @param ac_nMoved number of elements just read
        * @return new read position
        */
    tType mf_nGetNewPos( const tType ac_nMoved )
    {
      this->mv_nRead += ac_nMoved;
      if( this->mv_nLoopEnd > this->mv_nLoopStart && this->mv_nRead == this->mv_nLoopEnd )
        return this->mv_nLoopStart;
      else if( this->mv_nLoopEnd < this->mv_nLoopStart && this->mv_nRead == this->mc_nLength )
        return 0;
      else if( this->mv_nLoopEnd < this->mv_nLoopStart && this->mv_nRead == this->mv_nLoopEnd + 1 )
        return this->mv_nLoopStart;
      else
        return this->mv_nRead;
    }

    tIntType mv_nRead;
    tIntType mv_nLoopStart;
    tIntType mv_nLoopEnd;
    tIntType mc_nLength;

    s_track_mem_leaks( LoopLogic )

  private:
    LoopLogic( const LoopLogic& );
    LoopLogic& operator = ( const LoopLogic& );
  };

}

S_NAMESPACE_END

#endif //#ifndef __STR_LOOPLOGIC_H__
