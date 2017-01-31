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
  * [ file ] _archs/win32/automation/idispatch.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_IDISPATCH_H__
#define __STR_IDISPATCH_H__

#include "win32_headers.h"
#include "core/creation/objectwrapper.h"

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

      /**
        * tIDipsatchInfo
        *   the tHandleInfo implementation
        *   for IDispatch or IUnknown.
        *   Calls release() n smf_Close().
        ********************************** */
    template< class tType >
    s_dll struct tIDipsatchInfo
    {
      typedef tType type;
      static const type smc_tInvalid;
      static void smf_Close( type& a_pInst ) s_no_throw
      {
        try
        {
          a_pInst->Release();
          a_pInst = 0;
        }
        catch( ... )
        {
        }
      }
    };

    template< class tType >
    const typename tIDipsatchInfo< tType >::type tIDipsatchInfo< tType >::smc_tInvalid = 0;


      /**
        * tGetIUnknownWrapper
        *   helper for getting an ObjectWrapper for any
        *   IUnknown derived type (IDispatch, IConnectionPoint,...)
        *********************************************************** */
    template< class tType >
    struct tGetIUnknownWrapper
    {
      typedef creation::ObjectWrapper< tIDipsatchInfo< tType > > type;
    };


      /**
        * IUnknownInstance
        *   wrapper for IUnknown, so we don't
        *   have to care about releasing.
        ************************************** */
    typedef tGetIUnknownWrapper< IUnknown* >::type IUnknownInstance;


      /**
        * IDispatchInstance
        *   wrapper for IDispatch, so we don't
        *   have to care about releasing.
        ************************************** */
    class IDispatchInstance : public tGetIUnknownWrapper< IDispatch* >::type
    {
      typedef tGetIUnknownWrapper< IDispatch* >::type parent;
    public:
        /**
          * Default Constructor.
          */
      IDispatchInstance() s_no_throw;

        /**
          * Constructor.
          * @param a_pObj initial IDispatch
          */
      IDispatchInstance( IDispatch* a_pObj ) s_no_throw;

        /**
          * Constructor.
          * Calls mp_bCreateObject().
          * @param ac_psAppName control name
          */
      IDispatchInstance( const char* ac_psAppName ) s_no_throw;

        /**
          * Constructor.
          * Calls mp_bCreateObject().
          * @param ac_ID control ID
          */
      IDispatchInstance( const CLSID ac_ID ) s_no_throw;

        /**
          * Destructor.
          */
      ~IDispatchInstance() s_no_throw;

        /**
          * Tries to create an automation object for an ID,
          * and retrieves it's IDispatch interface.
          * @param ac_tID the CLSID
          * @return NOERROR for success
          */
      HRESULT mp_bCreateObject( const CLSID& ac_tID ) s_no_throw;

        /**
          * Tries to create an automation object for an ID,
          * and retrieves it's IDispatch interface.
          * @param ac_psProgID the program ID string
          * @return NOERROR for success
          */
      HRESULT mp_bCreateObject( const char* ac_psProgID ) s_no_throw;
    };

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_IDISPATCH_H__
