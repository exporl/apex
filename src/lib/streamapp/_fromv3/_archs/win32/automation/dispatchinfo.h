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
 
#ifndef __STR_DISPATCHINFO_H__
#define __STR_DISPATCHINFO_H__

#include "win32_headers.h"
#ifndef S_STLCOMPATIBLE
  #include "core/containers/arrays.h"
#else
  #include <vector>
#endif

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

    class DispatchInfoBuilder;

      /**
        * tDispatchInfo
        *   contains the description for calling
        *   a method from an IDispatch: ID, flags
        *   and argument and return info.
        ***************************************** */
    struct tDispatchInfo
    {
        /**
          * Constructor.
          */
      tDispatchInfo() s_no_throw;

        /**
          * Destructor.
          */
      ~tDispatchInfo() s_no_throw;

        /**
          * Copy Constructor.
          */
      tDispatchInfo( const tDispatchInfo& ) s_no_throw;

        /**
          * Assignment operator.
          */
      tDispatchInfo& operator = ( const tDispatchInfo& ) s_no_throw;

      DISPID    m_DispID;       //!dispatch id
      BSTR      m_sName;        //!dispatch name
      WORD      m_wFlag;        //invoke flag
      short     m_nVfOffset;    //virtual function offset
      CALLCONV  m_CallConv;     //!calling convention
      VARTYPE   m_OutputType;   //!return type
      int       m_nParams;      //!number of arguments
      WORD*     m_pParamTypes;  //!argument types

      friend class DispatchInfoBuilder;
    };


      /**
        * DispatchInfoBuilder
        *   class responsible for acquiring
        *   info from an IDispatch instance.
        ************************************ */
    class DispatchInfoBuilder
    {
    public:
  #ifndef S_STLCOMPATIBLE
      typedef containers::Arrays< CLSID >::tDynamicArray         mt_CLSIDs;
      typedef containers::Arrays< tDispatchInfo >::tDynamicArray mt_Dispatchinfos;
  #else
      typedef std::vector< CLSID >          mt_CLSIDs;
      typedef std::vector< tDispatchInfo >  mt_Dispatchinfos;
  #endif

        /**
          * Build info array for the given IDispatch.
          * @param a_pDispatch IDispatch instance
          * @param a_Array destination array
          * @return S_OK if succeeded
          */
      static HRESULT smf_hBuildInfo( IDispatch* a_pDispatch, mt_Dispatchinfos& a_Array ) s_no_throw;

        /**
          * Get up to a_nMaxPoints connection point Ids.
          * Queries a_pDispatch for it's IConnectionPointContainer,
          * then enumerates all connection points and stores the IDs
          * in a_pArray.
          * @param a_pDispatch IDispatch instance
          * @param a_pArray destination array
          * @param a_nMaxPoints size of a_pArray, contains num IDs found after call
          * @return S_OK for success
          */
      static HRESULT smf_hGetConnectionPointIDs( IDispatch* a_pDispatch, mt_CLSIDs& a_Array ) s_no_throw;

    private:
      DispatchInfoBuilder() s_no_throw;
      ~DispatchInfoBuilder() s_no_throw;
    };

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_DISPATCHINFO_H__
