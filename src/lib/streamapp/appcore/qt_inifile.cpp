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
 
#include "appcore/qt_inifile.h"
#include "appcore/qt_utils.h"
#include "utils/stringutils.h"
#include "defines.h"

#include <assert.h>
#ifdef _MSC_VER
#include <tchar.h>
//#pragma warning ( disable : 4714 )  //marked as __forceinline not inlined
#pragma warning ( disable : 4127 )  //conditional expression is constant on sc_bPrintKeys
#else
#define _T( a ) a
typedef char TCHAR;
#endif
#include <stdio.h>
#include <fstream>
#include <qtextstream.h>

using namespace appcore;

namespace
{
  const bool      sc_bPrintKeys = false;  //print keys when loading/comitting file
  const QString   sc_sTrueString( "true" );
  const QString   sc_sFalseString( "false" );
}

INLINE QString f_ToString( const bool  ac_bValue )
{
  return f_sBooleanToString( ac_bValue );
}

INLINE QString f_ToString( const double ac_fValue )
{
  return QString::number( ac_fValue );
}

INLINE QString f_ToString( const int ac_iValue )
{
  return QString::number( ac_iValue );
}

INLINE bool  f_BoolFromString( const QString & ac_sString )
{
  return stringTo<bool>( sq( ac_sString ) );
}

INLINE double  f_FloatFromString( const QString & ac_sString )
{
  return stringTo<double>( sq( ac_sString ) );
}

INLINE int   f_IntFromString( const QString & ac_sString )
{
  return stringTo<int>( sq( ac_sString ) );
}

CIniFile::CIniFile() :
  mv_eStatus(mc_eOK),
  mv_bConvertEol( true )
{
}

CIniFile::~CIniFile()
{

}

  //I'm happy with the default generated copy ctor
/*CIniFile::CIniFile( const CIniFile & ac_IniFile ) :
  mv_eStatus( ac_IniFile.mv_eStatus ),
  mv_Keys( ac_IniFile.mv_Keys ),
  mv_sFilename( ac_IniFile.mv_sFilename )
{
}*/

bool CIniFile::mp_AddKey( const QString & ac_sKey )
{
    // first look up the key
  const CKeyVector::iterator    It = mf_LookupKey( ac_sKey );

    // check if it exists, if not, add it
  if( It == mv_Keys.end() )
  {
    CKeySection Key;
    Key.mv_sKeyName = ac_sKey;
    mv_Keys.push_back( Key );
    return false;
  }
  return true;
}

void CIniFile::mp_RemoveKey( const QString & ac_sKey )
{
  mp_SetStatus( mc_eOK );
    // first look up the key
  const CKeyVector::iterator    It = mf_LookupKey( ac_sKey );

    // check if it exists, if so, remove it
  if( It != mv_Keys.end() )
    mv_Keys.erase( It );
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );
}

namespace
{
  CIniFile::CValueVector::iterator sf_LookupValue( CIniFile::CKeySection & av_rKey, const QString & ac_sValueName )
  {
    const CIniFile::CValueVector::iterator  End = av_rKey.mv_Values.end();

    CIniFile::CValueVector::iterator    Result = End;
    CIniFile::CValueVector::iterator    It = av_rKey.mv_Values.begin();

    for( ; It != End ; ++It )
    {
      CIniFile::CValueSection & Value = *It;
      if( Value.mv_sName == ac_sValueName )
      {
        Result = It;
        break;
      }
    }

    return Result;
  }

  CIniFile::CValueVector::const_iterator sf_LookupValue( const CIniFile::CKeySection & av_rKey, const QString & ac_sValueName )
  {
    const CIniFile::CValueVector::const_iterator  End = av_rKey.mv_Values.end();

    CIniFile::CValueVector::const_iterator    Result = End;
    CIniFile::CValueVector::const_iterator    It = av_rKey.mv_Values.begin();

    for( ; It != End ; ++It )
    {
      const CIniFile::CValueSection & Value = *It;
      if( Value.mv_sName == ac_sValueName )
      {
        Result = It;
        break;
      }
    }

    return Result;
  }

  void f_PrintKeys( const CIniFile::CKeyVector& ac_Keys )
  {
    CIniFile::CKeyVector::const_iterator    KeyIt = ac_Keys.begin();
    CIniFile::CKeyVector::const_iterator    KeyEnd = ac_Keys.end();

    for( ; KeyIt != KeyEnd ; ++KeyIt )
    {
      const CIniFile::CKeySection & Key = *KeyIt;

      std::cout << sq( Key.mv_sKeyName ).c_str() << std::endl;

      CIniFile::CValueVector::const_iterator  ValueIt =   Key.mv_Values.begin();
      CIniFile::CValueVector::const_iterator  ValueEnd =  Key.mv_Values.end();

      for( ; ValueIt != ValueEnd ; ++ValueIt )
      {
        std::cout << sq( (*ValueIt).mv_sName ).c_str() << " : " << sq( (*ValueIt).mv_sValue ).c_str() << std::endl;
      }
    }
  }

  const unsigned sc_nBufferSize = 4096;

  const QString sc_sEol = "\n";
  const QString sc_sEolConv = "%%";
}

void CIniFile::mp_SetValueString(const QString & ac_sKey, const QString & ac_sValueName, const QString & ac_sValue, const bool  ac_bNoCreate )
{
  mp_SetStatus( mc_eOK );

  QString sVal( ac_sValue );
  if( sVal.isEmpty() )
    sVal = "-1.IND";
  else
  {
    if( mv_bConvertEol )
      sVal.replace( sc_sEol, sc_sEolConv );
  }

    //first look up the key
  CKeyVector::iterator It = mf_LookupKey( ac_sKey );

  if( It != mv_Keys.end() )
  {
    CKeySection & Key = *It;
      //now look up the value
    const CValueVector::iterator It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
      (*It).mv_sValue = sVal;
    else if( !ac_bNoCreate )
    {
      CValueSection Value;
      Value.mv_sName = ac_sValueName;
      Value.mv_sValue = sVal;
      Key.mv_Values.push_back( Value );
    }
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );
}

void CIniFile::mp_SetValueBool(  const QString & ac_sKey, const QString & ac_sValueName, const bool      ac_bValue, const bool  ac_bNoCreate )
{
  mp_SetStatus( mc_eOK );

    // first look up the key
  CKeyVector::iterator    It = mf_LookupKey( ac_sKey );

  if( It != mv_Keys.end() )
  {
    CKeySection & Key = *It;
      // now look up the value
    const CValueVector::iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
    {
      const QString BoolString = f_ToString( ac_bValue );
      (*It).mv_sValue = BoolString;
    }
    else if( !ac_bNoCreate )
    {
      CValueSection   Value;
      Value.mv_sName = ac_sValueName;
      Value.mv_sValue = f_ToString( ac_bValue );;
      Key.mv_Values.push_back( Value );
    }
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );
}

void CIniFile::mp_SetValueFloat( const QString & ac_sKey, const QString & ac_sValueName, const double     ac_fValue, const bool  ac_bNoCreate )
{
  mp_SetStatus( mc_eOK );

    // first look up the key
  CKeyVector::iterator    It = mf_LookupKey( ac_sKey );

  if( It != mv_Keys.end() )
  {
    CKeySection & Key = *It;
      // now look up the value
    const CValueVector::iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
    {
      const QString FloatString = f_ToString( ac_fValue );
      (*It).mv_sValue = FloatString;
    }
    else if( !ac_bNoCreate )
    {
      CValueSection   Value;
      Value.mv_sName = ac_sValueName;
      Value.mv_sValue = f_ToString( ac_fValue );;
      Key.mv_Values.push_back( Value );
    }
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );
}

void CIniFile::mp_SetValueInt( const QString & ac_sKey, const QString & ac_sValueName, const int    ac_iValue, const bool  ac_bNoCreate )
{
  mp_SetStatus( mc_eOK );

    // first look up the key
  CKeyVector::iterator    It = mf_LookupKey( ac_sKey );

  if( It != mv_Keys.end() )
  {
    CKeySection & Key = *It;
      // now look up the value
    const CValueVector::iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
    {
      const QString IntString = f_ToString( ac_iValue );
      (*It).mv_sValue = IntString;
    }
    else if( !ac_bNoCreate )
    {
      CValueSection   Value;
      Value.mv_sName = ac_sValueName;
      Value.mv_sValue = f_ToString( ac_iValue );;
      Key.mv_Values.push_back( Value );
    }
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );
}

void CIniFile::mp_RemoveValue( const QString & ac_sKey, const QString & ac_sValueName )
{
  mp_SetStatus( mc_eOK );
    // first look up the key
  CKeyVector::iterator    It = mf_LookupKey( ac_sKey );

  if( It != mv_Keys.end() )
  {
    CKeySection & Key = *It;
      // now look up the value
    const CValueVector::iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
      Key.mv_Values.erase( It );
    else
      mp_SetStatus( mc_eErrorValueNoExist );
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );
}

QString CIniFile::mf_sGetValueString( const QString & ac_sKey, const QString & ac_sValueName ) const
{
  mp_SetStatus( mc_eOK );
    //first look up the key
  CKeyVector::const_iterator    It = mf_LookupKey( ac_sKey );

  QString Result;

  if( It != mv_Keys.end() )
  {
    const CKeySection & Key = *It;
      //now look up the value
    const CValueVector::const_iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
      Result = (*It).mv_sValue;
    else
      mp_SetStatus( mc_eErrorValueNoExist );
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  //assert( mv_eStatus == mc_eOK );
  if( mv_eStatus != mc_eOK )
    return QString( "" );
  else
  {
    if( mv_bConvertEol )
      Result.replace( sc_sEolConv, sc_sEol );
  }

  return Result;
}

bool CIniFile::mf_bGetValueBool(   const QString & ac_sKey, const QString & ac_sValueName ) const
{
  mp_SetStatus( mc_eOK );
    // first look up the key
  CKeyVector::const_iterator    It = mf_LookupKey( ac_sKey );

  bool  bResult = false;

  if( It != mv_Keys.end() )
  {
    const CKeySection & Key = *It;
      // now look up the value
    const CValueVector::const_iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
      bResult = f_BoolFromString( (*It).mv_sValue );
    else
      mp_SetStatus( mc_eErrorValueNoExist );
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );

  return bResult;
}

double CIniFile::mf_fGetValueFloat(  const QString & ac_sKey, const QString & ac_sValueName ) const
{
  mp_SetStatus( mc_eOK );
   // first look up the key
  CKeyVector::const_iterator    It = mf_LookupKey( ac_sKey );

  double   fResult = 0.f;

  if( It != mv_Keys.end() )
  {
    const CKeySection & Key = *It;
      // now look up the value
    const CValueVector::const_iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
      fResult = f_FloatFromString( (*It).mv_sValue );
    else
      mp_SetStatus( mc_eErrorValueNoExist );
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );

  return fResult;
}

int CIniFile::mf_iGetValueInt(  const QString & ac_sKey, const QString & ac_sValueName ) const
{
  mp_SetStatus( mc_eOK );
   // first look up the key
  CKeyVector::const_iterator    It = mf_LookupKey( ac_sKey );

  int iResult = 0;

  if( It != mv_Keys.end() )
  {
    const CKeySection & Key = *It;
      // now look up the value
    const CValueVector::const_iterator  It = sf_LookupValue( Key, ac_sValueName );

    if( It != Key.mv_Values.end() )
      iResult = f_IntFromString( (*It).mv_sValue );
    else
      mp_SetStatus( mc_eErrorValueNoExist );
  }
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );

  return iResult;
}

void CIniFile::mp_CommitToFile() const
{
  mp_SetStatus( mc_eOK );
  #ifdef _MSC_VER
  FILE    * pFile = _tfopen( sq( mv_sFilename ).data(), _T("w") );
  #else
#if QT_VERSION < 0x040000
  FILE    * pFile = fopen( mv_sFilename.data(), "w" );
#else
  FILE    * pFile = fopen( mv_sFilename.toAscii().data(), "w" );
#endif
  #endif

  CKeyVector::const_iterator    KeyIt = mv_Keys.begin();
  CKeyVector::const_iterator    KeyEnd = mv_Keys.end();

  if( pFile != 0 )
  {
    for( ; KeyIt != KeyEnd ; ++KeyIt )
    {
      const CKeySection & Key = *KeyIt;

#if QT_VERSION < 0x040000
      fprintf( pFile, _T("[%s]\n"), Key.mv_sKeyName.data() );
#else
      fprintf( pFile, _T("[%s]\n"), Key.mv_sKeyName.toAscii().data() );
#endif

      CValueVector::const_iterator  ValueIt =   Key.mv_Values.begin();
      CValueVector::const_iterator  ValueEnd =  Key.mv_Values.end();

      for( ; ValueIt != ValueEnd ; ++ValueIt )
      {
        const CValueSection & Value = *ValueIt;

       // _ftprintf( pFile, _T("%s = %s\n"), Value.mv_sName.data(), Value.mv_sValue.data() );
#if QT_VERSION < 0x040000
       fprintf( pFile, _T("%s = %s\n"), Value.mv_sName.data(), Value.mv_sValue.data() );
#else
       fprintf( pFile, _T("%s = %s\n"), Value.mv_sName.toAscii().data(), Value.mv_sValue.toAscii().data() );
#endif
      }

      //_ftprintf( pFile, _T("\n") );
      fprintf( pFile, _T("\n") );
    }

    fclose( pFile );
  }
  else
    mp_SetStatus( mc_eFileOpenError );

  /* assert( mv_eStatus == mc_eOK );*/
}

void CIniFile::mp_LoadFromFile()
{
  mp_Clear();

  mp_SetStatus( mc_eOK );

  #ifdef _MSC_VER
  FILE    * pFile = _tfopen( sq( mv_sFilename ).data(), _T("r") );
  #else
#if QT_VERSION < 0x040000
  FILE    * pFile = fopen( mv_sFilename.data(), "r" );
#else
  FILE    * pFile = fopen( mv_sFilename.toAscii().data(), "r" );
#endif
  #endif

  if( pFile != 0 )
  {
    static  TCHAR   Buffer[sc_nBufferSize];

      // read a key
    //_fgetts( Buffer, sc_nBufferSize-1, pFile );
    fgets( Buffer, sc_nBufferSize-1, pFile );

    while( feof( pFile ) == 0 )
    {
        // the name looks like [<name>], we have to chop the '[',']'
      //const size_t  c_nLen = _tcslen( Buffer );
      const size_t c_nLen = strlen( Buffer );

      Buffer[c_nLen-2] = _T('\0');    // chop the endline and the ']'

      const QString KeyName = (Buffer+1);   // chop the '['

      CKeySection     NewKey;
      NewKey.mv_sKeyName = KeyName;

      //TCHAR  *  pLine = _fgetts( Buffer, sc_nBufferSize-1, pFile );
      TCHAR  *  pLine = fgets( Buffer, sc_nBufferSize-1, pFile );

        // now read values until we get to the end of line or an empty line
      //while( (pLine != 0) && (_tcslen(pLine) != 0 ) && (pLine[0] != TCHAR(10) ) )
      while( (pLine != 0) && (strlen(pLine) != 0 ) && (pLine[0] != TCHAR(10) ) )
      {
          // the value looks like <name> = <value>
          // so we look for the first whitespace starting from the front

          // chop the endline
        //Buffer[_tcslen(pLine)-1] = _T('\0');    // chop the endline
        Buffer[ strlen(pLine)-1 ] = _T('\0');    // chop the endline
        QString ValueLine = Buffer;

#if QT_VERSION < 0x040000
        const int   c_iPos = ValueLine.find( _T(" = ") );
#else
        const int   c_iPos = ValueLine.indexOf( _T(" = ") );
#endif

        assert( c_iPos != -1 );

        const QString ValueName   = ValueLine.left( c_iPos );
        const QString ValueString = ValueLine.right( ValueLine.length() - (c_iPos+3) );

        CValueSection NewValue;
        NewValue.mv_sName   =  ValueName;
        NewValue.mv_sValue  =  ValueString;

        NewKey.mv_Values.push_back( NewValue );
        ++mv_nTotalValueCount;

        //pLine = _fgetts( Buffer, sc_nBufferSize-1, pFile );
        pLine = fgets( Buffer, sc_nBufferSize-1, pFile );
      }

      mv_Keys.push_back( NewKey );



      //_fgetts( Buffer, sc_nBufferSize-1, pFile );
      fgets( Buffer, sc_nBufferSize-1, pFile );
    }

    fclose( pFile );
  }
  else
  {
    mp_SetStatus( mc_eFileOpenError );
  }

  //assert( mv_eStatus == mc_eOK );

  if( sc_bPrintKeys )
    f_PrintKeys( mv_Keys );
}

CIniFile::mt_eStatus CIniFile::mf_eGetStatus() const
{
  return mv_eStatus;
}

CIniFile::CKeyVector::iterator CIniFile::mf_LookupKey( const QString & ac_sKey )
{
  const CKeyVector::iterator    End     = mv_Keys.end();
  CKeyVector::iterator          Result  = End;
  CKeyVector::iterator          It      = mv_Keys.begin();

  for( ; It != End ; ++It )
  {
    CKeySection & c_rKey = *It;
    if( c_rKey.mv_sKeyName == ac_sKey )
    {
      Result = It;
      break;
    }

  }

  return Result;
}

CIniFile::CKeyVector::const_iterator CIniFile::mf_LookupKey( const QString & ac_sKey ) const
{
  const CKeyVector::const_iterator    End     = mv_Keys.end();
  CKeyVector::const_iterator          Result  = End;
  CKeyVector::const_iterator          It      = mv_Keys.begin();

  for( ; It != End ; ++It )
  {
    const CKeySection & c_rKey = *It;
    if( c_rKey.mv_sKeyName == ac_sKey )
    {
      Result = It;
      break;
    }

  }

  return Result;
}

void CIniFile::mp_SetFilename( const QString & ac_sFilename )
{
  mv_sFilename = ac_sFilename;
}

void CIniFile::mp_SetStatus( const mt_eStatus  ac_eStatus ) const
{
  mv_eStatus = ac_eStatus;
}

unsigned CIniFile::mf_nGetKeyCount() const
{
  return (unsigned) mv_Keys.size();
}

unsigned CIniFile::mf_nGetTotalValueCount() const
{
  return mv_nTotalValueCount;
}

unsigned CIniFile::mf_nGetValueCount( const QString & ac_sKey ) const
{
  mp_SetStatus( mc_eOK );

  CKeyVector::const_iterator Result = mf_LookupKey( ac_sKey );

  unsigned nResult = 0;

  if( Result != mv_Keys.end() )
    nResult = (unsigned) (*Result).mv_Values.size();
  else
    mp_SetStatus( mc_eErrorKeyNoExist );

  assert( mv_eStatus == mc_eOK );

  return nResult;
}

CIniFile CIniFile::mf_CalcDiff( const CIniFile & ac_IniFile, const bool  ac_bRemoveEmptyKeys ) const
{
  mp_SetStatus( mc_eOK );
    // copy this to new inifile
  CIniFile      Result( *this );

    // iterate over all the keys in ac_IniFile
  CKeyVector::const_iterator          KeyIt   = ac_IniFile.mv_Keys.begin();
  const CKeyVector::const_iterator    KeyEnd  = ac_IniFile.mv_Keys.end();

  for( ; KeyIt != KeyEnd ; ++KeyIt )
  {
    const CKeySection & c_Key = *KeyIt;

    const CKeyVector::iterator  ThisKeyIt = Result.mf_LookupKey( c_Key.mv_sKeyName );

    if( ThisKeyIt != Result.mv_Keys.end() )
    {
      CValueVector::const_iterator        ValueIt  = c_Key.mv_Values.begin();
      const CValueVector::const_iterator  ValueEnd = c_Key.mv_Values.end();

      for( ; ValueIt != ValueEnd ; ++ValueIt )
      {
        const CValueSection &   c_Value = *ValueIt;

        const CValueVector::iterator  ThisValueIt = sf_LookupValue( *ThisKeyIt, c_Value.mv_sName );

        if( ThisValueIt != (*ThisKeyIt).mv_Values.end() )
        {
            // we found a matching value, remove it from our key
          (*ThisKeyIt).mv_Values.erase( ThisValueIt );

            // check if the key is empty, if so, remove it
          if( (*ThisKeyIt).mv_Values.size() == 0 && ac_bRemoveEmptyKeys )
            Result.mv_Keys.erase( ThisKeyIt );
        }
      }
    }
  }

  assert( mv_eStatus == mc_eOK );

  return Result;
}

const QString& CIniFile::mf_sGetFilename() const
{
  return  mv_sFilename;
}

bool CIniFile::operator==( const CIniFile & ac_IniFile ) const
{
  CIniFile    Diff = mf_CalcDiff( ac_IniFile );

    // if they had the same amount of keys before, and their difference has none
    // they must be equal
  return (mf_nGetKeyCount() == ac_IniFile.mf_nGetKeyCount()) && (Diff.mf_nGetKeyCount() == 0);
}

void CIniFile::mp_Clear()
{
  mp_SetStatus( mc_eOK );
  mv_nTotalValueCount = 0;
  mv_Keys.clear();
}

QString CIniFile::mf_sGetKey( const unsigned i ) const
{
  mp_SetStatus( mc_eOK );

  QString   Result;

  if( i < mv_Keys.size() )
    Result = mv_Keys[i].mv_sKeyName;
  else
    mp_SetStatus( mc_eIndexOutOfRange );

  assert( mv_eStatus == mc_eOK );

  return Result;
}
