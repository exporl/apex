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
  * [ file ] _archs/win32/automation/olecontroller.h
  * [ fmod ] String
  * <^>
  */

#ifndef __STR_OLECONTROLLER_H__
#define __STR_OLECONTROLLER_H__

#include "idispatch.h"
#include "varenuminfo.h"

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

      /**
        * OleController
        *   controls OLE automation objects.
        *   First create an object, then call methods on it.
        *   For convenience, two different ways of passing parameters are supported:
        *   - by passing in an array of VARTYPE and the size of the array.
        *   - by a format description wich uses these characters:
        *       & = mark the following format character as VT_BYREF
        *       b = VT_BOOL
        *       i = VT_I2
        *       I = VT_I4
        *       r = VT_R2
        *       R = VT_R4
        *       c = VT_CY
        *       a = OleController::VT_BSTRA
        *       s = OleController::VT_BSTRS
        *       3 = OleController::VT_BSTR3
        *       S = VT_BSTR
        *       e = VT_ERROR
        *       d = VT_DATE
        *       v = VT_VARIANT (use to pass data types not described)
        *       D = VT_DISPATCH
        *       U = VT_UNKNOWN
        *
        *   Also, template methods are supplied so the user doesn't have to care
        *   care about VARTYPE conventions.
        *   There are some unpleasacies when using strings to get a value:
        *   first, the memory should be allocated somewhere, but the caller shouldn't
        *   have to care about how this happens.
        *   second, there are UNICODE strings and ANSI strings.
        *   The implementation gets around this as follows:
        *   - for setting a property or using a string as const argument to a method,
        *     passing a const char* or const wchar_t* (equal to const OLECHAR*) is always allowed,
        *     and indicated by 'a', 'S' and VT_BSTRA, VT_BSTR respectively. It's also possible to pass
        *     a string class: 's', '3' and VT_BSTRS, VT_BSTR3 are used for std::string* and String*.
        *   - for getting a property , passing char* or wchar_t* always fails with err = E_INVALID arg.
        *     Instead, a string class using char* must be used and passed as a pointer; internally
        *     a wchar_t* and VT_BSTR are used and conversion is done automaticaly.
        *   - using a char* or wchar_t* as reference argument to a method is simply never allowed
        *
        *   example:
        *   @code
            win32::automation::OleController Aut;
            if( Aut.mp_bCreateObject( "CodeComposer.IApplication" ) )
            {
                //get property by reference
              bool b = false;
              Aut.mp_bGetProperty( "Visible", b );
                //can also use BOOL
              BOOL B = FALSE:
              Aut.mp_bGetProperty( "Visible", "b", B );
              Aut.mp_bGetProperty( "Visible", VT_BOOL, B );
                //pass const string
              const String sWS( "d:/test3.wks" );
              Aut.mp_bInvokeMethod( "OpenWorkspace", sWS );
              Aut.mp_bInvokeMethod( "OpenWorkspace", "a", sWS.mf_pData() );
              Aut.mp_bInvokeMethod( "OpenWorkspace", "3", &sWS ); //must be pointer here!
                //pass const wide char
              const OLECHAR* p = OLESTR( "d:/test3.wks" );
              Aut.mp_bInvokeMethod( "OpenWorkspace", p );
              Aut.mp_bInvokeMethod( "OpenWorkspace", "S", p );
                //only way to get a string
              String sRet;
              Aut.mp_bGetProperty( "AppDirectory", sRet );
                //pass multiple parameters by reference
              long min = 0;
              long maj = 0;
              Aut.mp_bInvokeMethod( "GetApiVersion", "&I&I", &min, &maj );
                //or like this
              const VARTYPE params[] = { VT_I4 | VT_BYREF, VT_I4 | VT_BYREF };
              Aut.mp_bInvokeMethod( "GetApiVersion", 2, params, &min, &maj );
            }
        *   @endcode
        **************************************************************************** */
    class OleController
    {
    public:
        /**
          * Default Constructor.
          */
      OleController() s_no_throw;

        /**
          * Constructor.
          * @param a_pObject initial IDispatch instance
          */
      OleController( IDispatch* a_pObject ) s_no_throw;

        /**
          * Destructor.
          */
      ~OleController() s_no_throw;


        /**
          * Tries to create an automation object for an ID,
          * and retrieves it's IDispatch interface.
          * @param ac_tID the CLSID
          * @return true for success
          */
      bool mp_bCreateObject( const CLSID& ac_tID ) s_no_throw;

        /**
          * Tries to create an automation object for an ID,
          * and retrieves it's IDispatch interface.
          * @param ac_psProgID the program ID string
          * @return true for success
          */
      bool mp_bCreateObject( const char* ac_psProgID ) s_no_throw;

        /**
          * Check if we're alive.
          * @return true if object created
          */
      s_finline bool mf_bCreated() const s_no_throw
      {
        return m_pDispatch != 0;
      }

        /**
          * Check if we're free of errors.
          * @return false if no errors occurred
          */
      s_finline bool mf_bError() const s_no_throw
      {
        return !FAILED( mf_hGetLastError() );
      }

        /**
          * Get the object.
          * @return the IDispatch interface
          */
      s_finline IDispatchInstance& mf_GetObject() s_no_throw
      {
        return m_pDispatch;
      }

        /**
          * Destroy current object.
          */
      void mp_DestroyObject() s_no_throw;

        /**
          * Get the error code for the last operation.
          * @see gf_GetErrorDescription()
          * @return last error code
          */
      s_finline const HRESULT& mf_hGetLastError() const s_no_throw
      {
        return m_hResult;
      }


        /**
          * Get the dispatch ID for a service.
          * @param ac_psName the service name
          * @retrun the DISPID or  if not found
          */
      DISPID mp_GetDispatchID( const char* ac_psName ) s_no_throw;


        /**
          * Get the value of a property.
          * If you get a static_assert failure,
          * use one of the methods below instead.
          * @param tType property type
          * @param ac_psName property name
          * @param a_tVal result of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bGetProperty( const char* ac_psName, tType& a_tVal ) s_no_throw
      {
        return mp_bGetProperty( mp_GetDispatchID( ac_psName ), a_tVal );
      }

        /**
          * Get the value of a property.
          * @param ac_psName property name
          * @param ac_PropertyType property type
          * @param a_tVal result of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bGetProperty( const char* ac_psName, const VARTYPE ac_PropertyType, tType& a_tVal ) s_no_throw
      {
        return this->mp_bGetProperty( ac_psName, ac_PropertyType, &a_tVal );
      }

        /**
          * Get the value of a property.
          * Added for convenience to match with setters.
          * @param ac_psName property name
          * @param ac_cPropertyType fromat name
          * @param a_tVal result of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bGetProperty( const char* ac_psName, const char* ac_cPropertyType, tType& a_tVal ) s_no_throw
      {
        return this->mp_bGetProperty( ac_psName, ac_cPropertyType, &a_tVal );
      }

        /**
          * Get the value of a property.
          * If you get a static_assert failure,
          * use one of the methods below instead.
          * @param tType property type
          * @param ac_ID property id
          * @param a_tVal result of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bGetProperty( const DISPID ac_ID, tType& a_tVal ) s_no_throw;

        /**
          * Get the value of a property.
          * @param ac_ID property id
          * @param ac_PropertyType property type
          * @param a_tVal result of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bGetProperty( const DISPID ac_ID, const VARTYPE ac_PropertyType, tType& a_tVal ) s_no_throw
      {
        return this->mp_bGetProperty( ac_ID, ac_PropertyType, &a_tVal );
      }

        /**
          * Get the value of a property.
          * Added for convenience to match with setters.
          * @param ac_ID property id
          * @param ac_cPropertyType fromat name
          * @param a_tVal result of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bGetProperty( const DISPID ac_ID, const char* ac_cPropertyType, tType& a_tVal ) s_no_throw
      {
        return this->mp_bGetProperty( ac_ID, ac_cPropertyType, &a_tVal );
      }


        /**
          * Set the value of a property,
          * or multiple properties.
          * Uses format description.
          * @param ac_psName property name
          * @param ac_psParamInfo parameter description
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetProperty( const char* ac_psName, const char* ac_psParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property,
          * or multiple properties.
          * @param ac_psName property name
          * @param ac_nParams number of parameters
          * @param ac_pParamInfo parameter type array
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetProperty( const char* ac_psName, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property.
          * @param ac_psName property name
          * @param ac_tNewVal new property value of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bSetProperty( const char* ac_psName, const tType& ac_tNewVal ) s_no_throw
      {
        return mp_bSetProperty( mp_GetDispatchID( ac_psName ), ac_tNewVal );
      }

        /**
          * Set the value of a property,
          * or multiple properties.
          * Uses format description.
          * @param ac_ID property id
          * @param ac_psParamInfo parameter description
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetProperty( const DISPID ac_ID, const char* ac_psParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property,
          * or multiple properties.
          * @param ac_ID property id
          * @param ac_nParams number of parameters
          * @param ac_pParamInfo parameter type array
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetProperty( const DISPID ac_ID, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property.
          * @param ac_ID property id
          * @param ac_tNewVal new property value of correct type
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bSetProperty( const DISPID ac_ID, const tType& ac_tNewVal ) s_no_throw;


        /**
          * Set the value of a property,
          * or multiple properties, by reference.
          * @param ac_psName property name
          * @param ac_psFormat parameter description
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetPropertyRef( const char* ac_psName, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property,
          * or multiple properties, by reference.
          * Uses format description.
          * @param ac_psName property name
          * @param ac_psParamInfo parameter description
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetPropertyRef( const char* ac_psName, const char* ac_psParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property,
          * or multiple properties, by reference.
          * @param ac_ID property id
          * @param ac_psFormat parameter description
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetPropertyRef( const DISPID ac_ID, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw;

        /**
          * Set the value of a property,
          * or multiple properties, by reference.
          * Uses format description.
          * @param ac_ID property id
          * @param ac_psParamInfo parameter description
          * @return true if succeeded, false if property not found
          * or property type mismatch.
          */
      bool mp_bSetPropertyRef( const DISPID ac_ID, const char* ac_psParamInfo, ... ) s_no_throw;


        /**
          * Invoke a method without parameters.
          * @param ac_psName method name
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      s_finline bool mp_bInvokeMethod( const char* ac_psName ) s_no_throw
      {
        return mp_bInvokeMethod( mp_GetDispatchID( ac_psName ) );
      }

        /**
          * Invoke a method.
          * Uses format description.
          * @param ac_psName method name
          * @param ac_psParamInfo parameter description
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      bool mp_bInvokeMethod( const char* ac_psName, const char* ac_psParamInfo, ... ) s_no_throw;

        /**
          * Invoke a method.
          * @param ac_psName method name
          * @param ac_nParams number of parameters
          * @param ac_pParamInfo parameter description
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      bool mp_bInvokeMethod( const char* ac_psName, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw;

        /**
          * Invoke a method with a single parameter.
          * @param ac_psName method name
          * @param a_tVal parameter, can be in or out
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bInvokeMethod( const char* ac_psName, tType& a_tVal ) s_no_throw
      {
        return mp_bInvokeMethod( mp_GetDispatchID( ac_psName ), a_tVal );
      }

        /**
          * Invoke a method without parameters.
          * @param ac_ID method id
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      bool mp_bInvokeMethod( const DISPID ac_ID ) s_no_throw;

        /**
          * Invoke a method.
          * Uses format description.
          * @param ac_psName method name
          * @param ac_psParamInfo parameter description
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      bool mp_bInvokeMethod( const DISPID ac_ID, const char* ac_psParamInfo, ... ) s_no_throw;

        /**
          * Invoke a method.
          * @param ac_ID method id
          * @param ac_nParams number of parameters
          * @param ac_pParamInfo parameter description
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      bool mp_bInvokeMethod( const DISPID ac_ID, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw;

        /**
          * Invoke a method with a single parameter.
          * @param ac_ID method id
          * @param a_tVal parameter, can be in or out
          * @return true if succeeded, false if method not found
          * or property type mismatch.
          */
      template< class tType >
      s_finline bool mp_bInvokeMethod( const DISPID ac_ID, tType& a_tVal ) s_no_throw;

      static const VARTYPE VT_BSTRA = 1000; //!specifies const char*
      static const VARTYPE VT_BSTR3 = 1001; //!specifies String
      static const VARTYPE VT_BSTRS = 1002; //!specifies std::string

    private:
      s_finline bool mp_bGetProperty( const char* ac_psName, const VARTYPE ac_PropertyType, void* a_pRet ) s_no_throw
      {
        return mp_bGetProperty( mp_GetDispatchID( ac_psName ), ac_PropertyType, a_pRet );
      }
      s_finline bool mp_bGetProperty( const char* ac_psName, const char* ac_cPropertyType, void* a_pRet ) s_no_throw
      {
        return mp_bGetProperty( mp_GetDispatchID( ac_psName ), ac_cPropertyType, a_pRet );
      }
      bool mp_bGetProperty( const DISPID ac_ID, const VARTYPE ac_PropertyType, void* a_pRet ) s_no_throw;
      bool mp_bGetProperty( const DISPID ac_ID, const char* ac_cPropertyType, void* a_pRet ) s_no_throw;
      bool mp_bInvoke( WORD a_Flags, DISPID a_nDispatchId, VARTYPE a_vtRet, void* a_pRet,
        const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, va_list a_ArgList ) s_no_throw;
      bool mp_bInvoke( WORD a_Flags, DISPID a_nDispatchId, VARTYPE a_vtRet, void* a_pRet,
        const char* ac_pParamInfo, va_list a_ArgList ) s_no_throw;
      bool mp_bInvoke( WORD a_Flags, DISPID a_nDispatchId, VARTYPE a_vtRet, void* a_pRet,
        unsigned& a_nParams, VARIANTARG* a_pArgList ) s_no_throw;
      void mf_CheckPropertyPut( const WORD ac_Flags, DISPPARAMS& a_Params, const unsigned ac_nArgs ) s_no_throw;
      VARIANTARG* mf_pBuildArgList( unsigned& a_nArgs, const VARTYPE* ac_pParamInfo, va_list ac_ArgList ) s_no_throw;
      VARIANTARG* mf_pBuildArgList( unsigned& a_nArgs, const char* ac_pParamInfo, va_list ac_ArgList ) s_no_throw;
      void mf_CleanUpArgList( unsigned& ac_nArgs, VARIANTARG* a_pArgList ) s_no_throw;

      HRESULT           m_hResult;
      IDispatchInstance m_pDispatch;
    };


    /*s_static_integral_const_spec( tVarEnumType, String, OleController::VT_BSTR3, VARTYPE )*/
  #ifdef S_USE_STL
      s_static_integral_const_spec( tVarEnumType, std::string, OleController::VT_BSTRS, VARTYPE )
  #endif

    template< class tType >
    s_finline bool OleController::mp_bGetProperty( const DISPID ac_ID, tType& a_tVal ) s_no_throw
    {
      s_static_assert( tVarEnumType< tType >::value != VT_EMPTY, tTypeNotSupported )
      return mp_bGetProperty( ac_ID, tVarEnumType< tType >::value, &a_tVal );
    }

    template<>
    s_finline bool OleController::mp_bGetProperty< VARIANT >( const DISPID ac_ID, VARIANT& a_tVal ) s_no_throw
    {
      return mp_bGetProperty( ac_ID, VT_VARIANT, &a_tVal );
    }

    template<>
    s_finline bool OleController::mp_bGetProperty< bool >( const DISPID ac_ID, bool& a_tVal ) s_no_throw
    {
      BOOL bRet;
      if( mp_bGetProperty( ac_ID, VT_BOOL, &bRet ) )
      {
        a_tVal = bRet == 0 ? false : true;
        return true;
      }
      return false;
    }


    template< class tType >
    s_finline bool OleController::mp_bSetProperty( const DISPID ac_ID, const tType& ac_tNewVal ) s_no_throw
    {
      s_static_assert( tVarEnumType< tType >::value != VT_EMPTY, tTypeNotSupported )
      return mp_bSetProperty( ac_ID, tVarEnumType< tType >::value, ac_tNewVal );
    }

    template<>
    s_finline bool OleController::mp_bSetProperty< bool >( const DISPID ac_ID, const bool& ac_tNewVal ) s_no_throw
    {
      BOOL bSet = ac_tNewVal ? -1 : 0;
      const VARTYPE t = VT_BOOL;
      return mp_bSetProperty( ac_ID, 1, &t, bSet );
    }

    /*template<>
    s_finline bool OleController::mp_bSetProperty< String >( const DISPID ac_ID, const String& a_tVal ) s_no_throw
    {
      const VARTYPE t = OleController::VT_BSTRA;
      return mp_bSetProperty( ac_ID, 1, &t, (void*) a_tVal.mf_pData() );
    }*/

#ifdef S_USE_STL
    template<>
    s_finline bool OleController::mp_bSetProperty< std::string >( const DISPID ac_ID, const std::string& a_tVal ) s_no_throw
    {
      const VARTYPE t = OleController::VT_BSTRS;
      return mp_bSetProperty( ac_ID, 1, &t, (void*) a_tVal.c_str() );
      return false;
    }
#endif


    template< class tType >
    s_finline bool OleController::mp_bInvokeMethod( const DISPID ac_ID, tType& a_tVal ) s_no_throw
    {
      s_static_assert( tVarEnumType< tType >::value != VT_EMPTY, tTypeNotSupported )
      return mp_bInvokeMethod( ac_ID, 1, &tVarEnumType< tType >::value, a_tVal );
    }

    template<>
    s_finline bool OleController::mp_bInvokeMethod< bool >( const DISPID ac_ID, bool& a_tVal ) s_no_throw
    {
      BOOL bSet = a_tVal ? -1 : 0;
      const VARTYPE t = VT_BOOL;
      if( mp_bInvokeMethod( ac_ID, 1, &t, bSet ) )
      {
        a_tVal = bSet == 0 ? false : true;
        return true;
      }
      return false;
    }

    /*template<>
    s_finline bool OleController::mp_bInvokeMethod< const String >( const DISPID ac_ID, const String& a_tVal ) s_no_throw
    {
      const VARTYPE t = OleController::VT_BSTRA;
      return mp_bInvokeMethod( ac_ID, 1, &t, a_tVal.mf_pData() );
    }*/

#ifdef S_USE_STL
    template<>
    s_finline bool OleController::mp_bInvokeMethod< std::string >( const DISPID ac_ID, const std::string& a_tVal ) s_no_throw
    {
      const VARTYPE t = OleController::VT_BSTRA;
      return mp_bInvokeMethod( ac_ID, 1, &t, a_tVal.c_str() );
    }
#endif

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_OLECONTROLLER_H__
