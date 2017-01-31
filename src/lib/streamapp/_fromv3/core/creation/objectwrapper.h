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
 
#ifndef __STR_OBJECTWRAPPER_H__
#define __STR_OBJECTWRAPPER_H__

#include "core/_main.h"

S_NAMESPACE_BEGIN

namespace creation
{

    /**
      * ObjectWrapper
      *   wrapper for handles requiring a specific
      *   action on them when destructed. A handle is
      *   normally a simple type like void*, and since
      *   that's POD type, it doe not have a dsetructor
      *   or assignment operator. Still, the handle might
      *   require closing in some way.So this class tranforms
      *   the handle into an object, and executes a closing
      *   action on it when destructed. The type of handle,
      *   it's 'invlaid' value and it's closing action are
      *   specified in thandleInfo.
      ******************************************************* */
  template< class thandleInfo >
  class ObjectWrapper
  {
  public:
    typedef typename thandleInfo::type    type;
    typedef ObjectWrapper< thandleInfo >  thistype;

      /**
        * Constructor.
        */
    s_finline ObjectWrapper() s_no_throw :
      m_Handle( thandleInfo::smc_tInvalid )
    {
    }

      /**
        * Constructor.
        * @param a_hHandle the handle to initialize with
        */
    s_finline ObjectWrapper( type a_hHandle ) s_no_throw :
      m_Handle( a_hHandle )
    {
    }

      /**
        * Copy Constructor.
        * @param a_hHandle the handle to initialize with
        */
    s_finline ObjectWrapper( const thistype& a_hHandle ) s_no_throw
    {
      this->operator = ( a_hHandle );
    }

      /**
        * Destructor.
        * Closes the handle.
        */
    s_finline virtual ~ObjectWrapper() s_no_throw
    {
      mp_Close();
    }


      /**
        * operator ()
        * @return the type member reference
        */
    s_finline type& operator () () s_no_throw
    {
      return m_Handle;
    }

      /**
        * operator type&
        * @return the type member
        */
    s_finline operator type& () s_no_throw
    {
      return m_Handle;
    }

      /**
        * operator ->
        * Only works if type is a pointer..
        * @return the type member
        */
    s_finline type& operator -> () s_no_throw
    {
      return m_Handle;
    }

      /**
        * operator bool
        * @return true if our handle is valid
        */
    s_finline operator bool () const s_no_throw
    {
      return m_Handle != thandleInfo::smc_tInvalid;
    }


      /**
        * Assignment Operator.
        * @param a_hHandle type to assign
        * @return reference to this
        */
    s_finline thistype& operator = ( type a_hHandle ) s_no_throw
    {
      if( a_hHandle != m_Handle )
      {
        mp_Close();
        m_Handle = a_hHandle;
      }
      return *this;
    }

      /**
        * Assignment Operator.
        * @param a_hHandle ObjectWrapper to copy
        * @return reference to this
        */
    s_finline thistype& operator = ( const thistype& a_hHandle ) s_no_throw
    {
      if( &a_hHandle != this )
      {
        mp_Close();
        m_Handle = a_hHandle.m_Handle;
      }
      return *this;
    }


      /**
        * Get the handle.
        * @return the type member
        */
    s_finline type& mf_hGetHandle() s_no_throw
    {
      return m_Handle;
    }

      /**
        * Close the handle now.
        */
    s_finline void mp_Close() s_no_throw
    {
      if( m_Handle != thandleInfo::smc_tInvalid )
      {
        thandleInfo::smf_Close( m_Handle );
        m_Handle = thandleInfo::smc_tInvalid;
      }
    }

  private:
    type m_Handle;
  };

}

S_NAMESPACE_END

#endif //#ifndef __STR_OBJECTWRAPPER_H__
