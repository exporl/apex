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
    * @file qt_utils.h
    * This file contains some utilities that are handy when using Qt.
    */

#ifndef __QUTILS_H__
#define __QUTILS_H__

#include "defines.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4714 )  //marked as __forceinline not inlined for some functions with QString in Qt4
#endif

#include <qglobal.h>

#if QT_VERSION < 0x040000
#include <qurl.h>
#include <qdir.h>
#include <qfile.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#else
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QChar>
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QStringList>
#endif


  /**
    * Convert number to string.
    * just calls QString::number for the appropriate type.
    * @param a number (int/uns/float/double/long/...)
    * @return QString::number( a )
    */
template<class tType>
INLINE QString qn( tType a )
{
  return QString::number( a );
}

namespace
{
    /**
      * Convert a double to a string,
      * specifying the precision.
      * @param a the number
      * @param ac_nPrecision
      * @return the string
      */
  INLINE QString qdn( double a, const unsigned ac_nPrecision = 3 )
  {
    static const char fmt = 'g';
    static const QString cnv( "%1" );
    return cnv.arg( a, 0, fmt, ac_nPrecision );
  }

    /**
      * Convert string to boolean.
      * @param ac_sVal the string
      * @return true if sring equals "true", else false
      */
  INLINE bool f_bStringToBoolean( const QString& ac_sVal )
  {
    return ( ac_sVal == "true" ) ? true : false;
  }

    /**
      * Convert boolean to string.
      * @param ac_bVal the boolean
      * @return "true" for ac_bVal == true, and "false" for ac_bVal == false
      */
  INLINE QString f_sBooleanToString( const bool ac_bVal )
  {
    return ( ac_bVal == true ) ? "true" : "false";
  }

    /**
      * Convert QString to std::string.
      * @param ac_sVal the QString
      * @return std::string with same content
      */
  INLINE std::string sq( const QString& ac_sVal )
  {
#if QT_VERSION < 0x040000
    return std::string( ac_sVal.data() );
#else
    return std::string( ac_sVal.toAscii().data() );
#endif
  }

    /**
      * Convert std::string to QString.
      * @param ac_sVal the std::string
      * @return QString with same content
      */
  INLINE QString qs( const std::string& ac_sVal )
  {
    return QString( ac_sVal.data() );
  }

    /**
      * Get current time, formatted as hh-mm.
      * @return the time string
      */
  INLINE QString f_sTimeStringMinutes()
  {
    return QTime::currentTime().toString( "hh-mm" );
  }

  /**
    * Get current time, formatted as hh:mm::ss.
    * @return the time string
    */
  INLINE QString f_sTimeStringSeconds()
  {
    return QTime::currentTime().toString( "hh:mm:ss" );
  }

  /**
    * Assures directory ands with a "/".
    * @param a_sDir the name to check, appends "/" if it's not there
    */
  INLINE void f_MakeDirEnd( QString& a_sDir )
  {
    static const QString c_sDirEnd( "/" );
    if( !a_sDir.endsWith( c_sDirEnd ) )
      a_sDir.append( c_sDirEnd );
  }

#if QT_VERSION < 0x040000
    /**
      * Creates (like touch) a file if it does not exist already.
      * @param ac_sFile the filename
      * @param ac_nQtOpenMode the openmode
      */
  void f_CreateFileIfNotExist( const QString& ac_sFile, const int ac_nQtOpenMode )
  {
    if( !QFile::exists( ac_sFile ) )
    {
      QFile f( ac_sFile );
      f.open( ac_nQtOpenMode );
      f.close();
    }
  }
#else
  void f_CreateFileIfNotExist( const QString& ac_sFile, const QIODevice::OpenMode ac_nQtOpenMode )
  {
    if( !QFile::exists( ac_sFile ) )
    {
      QFile f( ac_sFile );
      f.open( ac_nQtOpenMode );
      f.close();
    }
  }
#endif

#if QT_VERSION < 0x040000
    /**
      * Replace a single character in a string.
      * @param a_sStringToSearch the source string
      * @param ac_sCharacterToReplace the character to search
      * @param ac_sReplaceWith the character to replace ac_sCharacterToReplace with
      */
  void f_ReplaceCharInString( QString& a_sStringToSearch, const QString& ac_sCharacterToReplace, const QString& ac_sReplaceWith )
  {
    const char* c_pSearch = ac_sCharacterToReplace.data();
    const unsigned nLen = a_sStringToSearch.length();
    for( unsigned i = 0 ; i < nLen ; ++i )
    {
      if( a_sStringToSearch.at( i ) == *c_pSearch )
      {
        a_sStringToSearch.remove( i, 1 );
        a_sStringToSearch.insert( i, ac_sReplaceWith );
        i += ac_sReplaceWith.length();
      }
    }
  }
#else
  void f_ReplaceCharInString( QString& a_sStringToSearch, const QString& ac_sCharacterToReplace, const QString& ac_sReplaceWith )
  {
    const char c_Search = ac_sCharacterToReplace.data()->toAscii();
    const char* c_pSearch = &c_Search;
    const unsigned nLen = a_sStringToSearch.length();
    for( unsigned i = 0 ; i < nLen ; ++i )
    {
      if( a_sStringToSearch.at( i ) == *c_pSearch )
      {
        a_sStringToSearch.remove( i, 1 );
        a_sStringToSearch.insert( i, ac_sReplaceWith );
        i += ac_sReplaceWith.length();
      }
    }
  }
#endif

  /**
    * Replace an entire string in a string.
    * FIXME only does first occurence
    * @param a_sStringToSearch the source string
    * @param ac_sStringToReplace the string to search
    * @param ac_sReplaceWith the string to replace ac_sStringToReplace with
    */
  void f_ReplaceStringInString( QString& a_sStringToSearch, const QString& ac_sStringToReplace, const QString& ac_sReplaceWith )
  {
#if QT_VERSION < 0x040000
    const int nIndex = a_sStringToSearch.find( ac_sStringToReplace, 0 );
#else
    const int nIndex = a_sStringToSearch.indexOf( ac_sStringToReplace, 0 );
#endif
    if( nIndex != -1 )
    {
      const unsigned nLengthToClear = ac_sStringToReplace.length();
      a_sStringToSearch.remove( (unsigned) nIndex, nLengthToClear );
      a_sStringToSearch.insert( (unsigned) nIndex, ac_sReplaceWith );
    }
  }

    /**
      * Split a multiline string in it's lines.
      * @param ac_sMultiLineText the string to split
      * @return a QStringList containing the lines
      */
  QStringList f_SplitStringInLines( const QString& ac_sMultiLineText )
  {
    static char eol = '\n';
    QStringList ret;
#if QT_VERSION < 0x040000
    int index = ac_sMultiLineText.find( eol, 0 );
#else
    int index = ac_sMultiLineText.indexOf( eol, 0 );
#endif
    if( index == -1 )
    {
      ret.push_back( ac_sMultiLineText );
      return ret;
    }
    else
    {
      ret.push_back( ac_sMultiLineText.left( (unsigned) index ) );
      unsigned len = ac_sMultiLineText.length();
      QString rest( ac_sMultiLineText.right( len - ( (unsigned) index + 1 ) ) );
#if QT_VERSION < 0x040000
      index = rest.find( eol, 0 );
#else
      index = rest.indexOf( eol, 0 );
#endif
      while( index != -1 )
      {
        ret.push_back( rest.left( (unsigned) index ) );
        len = rest.length();
        rest = rest.right( len - ( (unsigned) index + 1 ) );
#if QT_VERSION < 0x040000
        index = rest.find( eol, 0 );
#else
        index = rest.indexOf( eol, 0 );
#endif
      }
      ret.push_back( rest );
      return ret;
    }
  }

    /**
      * Split a multiline string in it's lines.
      * @param ac_pMultiLineText the string to split
      * @param ac_nLength the number of characters in ac_pMultiLineText
      * @return a QStringList containing the lines
      */
  QStringList f_SplitStringInLines( const char* ac_pMultiLineText, const unsigned ac_nLength )
  {
    static char eol = 0;
    QStringList ret;

    unsigned nIndex = 0;
    for( unsigned i = 0 ; i < ac_nLength ; ++i )
    {
      if( ac_pMultiLineText[ i ] == eol )
      {
        ret.push_back( QString( ac_pMultiLineText + nIndex ) );
        nIndex = ( i + 1 );
      }
    }

    return ret;
  }

    /**
      * Get the directory where a file is in.
      * eg returns c:/test/ if file is c:/test/test.foo
      * @param ac_sFileName the file (absolute path)
      * @return the directory
      */
  QString f_sDirectoryFromFile( const QString& ac_sFileName )
  {
    QUrl url( ac_sFileName );
    QString p = url.path();
#if QT_VERSION < 0x040000
    const QString f = url.fileName();
    p.setLength( p.length() - f.length() - 1 );
#else
    QFileInfo info( ac_sFileName );
    const QString f = info.fileName();
    p.resize( p.length() - f.length() - 1 );
#endif
    return p;
  }

    /**
      * Check directory existance.
      * @param ac_sDir the directory name
      * @return true if the directory exists
      */
  INLINE bool f_bCeckDirExists( const QString& ac_sDir )
  {
    return QDir( ac_sDir ).exists();
  }

  /**
    * Check file existance.
    * @param ac_sFile the filename
    * @return true if the file exists
    */
  INLINE bool f_bCeckFileExists( const QString& ac_sFile )
  {
    return QFile( ac_sFile ).exists();
  }

}

#endif //__QUTILS_H__
