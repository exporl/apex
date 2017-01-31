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

#ifndef __MULTICALLBACK_H__
#define __MULTICALLBACK_H__

#include "callback.h"
#include "containers/dynarray.h"

namespace streamapp
{

    /**
      * MultiCallback
      *   keeps array of Callbacks that are called one after another.
      *************************************************************** */
  class MultiCallback : public Callback
  {
  public:
      /**
        * Constructor.
        * @param ac_bDeleteContent if true, deletes all callbacks added
        */
    MultiCallback( const bool ac_bDeleteContent = false );

      /**
        * Destructor.
        */
    ~MultiCallback();

      /**
        * Adds a Callback at the first free position.
        * @param a_pCallback the Callback to add, must be non-zero
        */
    void mp_AddItem( Callback* a_pCallback );

      /**
        * Remove the specified Callback.
        * Nothing is done when a_pCallback isn't in the list.
        * @param a_pCallback the one to remove
        */
    void mp_RemoveItem( Callback* a_pCallback );

      /**
        * Get the current number of Callbacks.
        * @return the number
        */
    unsigned mf_nGetNumItems() const;

      /**
        * Callback functions.
        * Calls all Callbacks added, one by one.
        */
    void mf_Callback();

  private:
    const bool mc_bDeleteContent;
    DynamicArray<Callback*> m_Callbacks;

    MultiCallback( const MultiCallback& );
    MultiCallback& operator = ( const MultiCallback& );
  };

}

#endif //#ifndef __MULTICALLBACK_H__
