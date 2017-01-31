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

#ifndef DELETERR_H
#define DELETERR_H

namespace appcore
{

    /**
      * Deleter
      *   generic deleter like in Effective STL
      *   uses a functor because it can be inlined.
      *   Usage example with for_each:
      *   \code
      *   for_each( begin, end, Deleter() );
      *   \endcode
      ************************************************ */
  struct Deleter
  {
      /**
        * Delete.
        * @param p pointer to the memory to delete
        */
    template < class tType >
    void operator() ( tType * p )
    {
      delete p;
    }
  };

    /**
      * ZeroizingDeleter
      *   much safer than Deleter if used consequently,
      *   since delete( 0 ) doesn't crash your application.
      *
      *   \code
      *   delete pointer;
      *   pointer = 0;
      *   \endcode
      *
      *   can be replaced with
      *
      *   \code
      *   ZeroizingDeleter()( pointer );
      *   \endcode
      *
      *   @see Deleter
      ***************************************************** */
  struct ZeroizingDeleter
  {
      /**
        * Delete and set to zero.
        * Reference is necessary for setting it to zero.
        * @param p pointer to the memory to delete
        */
    template < class tType >
    void operator() ( tType*& p )
    {
      delete p;
      p = 0;
    }
  };

}

#endif //#ifndef DELETERR_H
