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
 
#ifndef __STDREDIRECTOR_H__
#define __STDREDIRECTOR_H__

#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include "appcore/singleton.h"
using namespace appcore;

namespace utils
{

    /**
      * StdRedirector
      *   redirects all std output to files.
      *   Call once in main(), and the object
      *   captures all output the given files.
      *   @code
      *   utils::StdRedirector::sf_pInstance()
      *   @endcode
      ****************************************************** */
  class StdRedirector : public Singleton<StdRedirector>
  {
    friend class Singleton<StdRedirector>;
  private:
      /**
        * Private constructor.
        */
    StdRedirector();

      /**
        * Private Destructor.
        * Restores original std::cout and std::cerr outputs.
        */
    ~StdRedirector();

    std::ofstream   m_Cout;
    std::ofstream   m_Cerr;
    std::streambuf* m_pCoutBuf;
    std::streambuf* m_pCerrBuf;

  private:
    // not implemented, get rid of a warning of MSVC.NET
    StdRedirector &operator=(const StdRedirector &other);
    StdRedirector(const StdRedirector &other);
  };

    /**
      * CallbackStream
      *   redirects all input to the supplied callback.
      *   Much more versatile then StdRedirector.
      *   Usage example to redirect std::cout to std::cerr:
      *
      *   @code
      *     //callback function
      *   void outcallback( const char *ptr, std::streamsize count )
      *   {
      *     if( *ptr != '\n' )  //ignore eof
      *       std::cerr << "cout said: " << ptr;
      *   }
      *
      *   CallbackStream<> redirect( std::cout, mycallback );
      *   @endcode
      **************************************************************** */
  template <class Elem = char, class Tr = std::char_traits<Elem> >
  class CallbackStream : public std::basic_streambuf<Elem, Tr>
  {
      /**
        * Callback Function.
        */
    typedef void (*pfncb) ( const Elem*, std::streamsize _Count );

  public:
      /**
        * Constructor.
        * @param a_Stream the stream to redirect
        * @param a_Cb the callback function
        */
    CallbackStream( std::ostream& a_Stream, pfncb a_Cb ) :
      m_Stream( a_Stream ),
      m_pCbFunc( a_Cb ),
      mv_bEol( true ),
      mv_bEolFound( false )
    {
        //redirect stream
      m_pBuf = m_Stream.rdbuf( this );
    };

      /**
        * Destructor.
        * Restores the original stream.
        */
    ~CallbackStream()
    {
        //restore stream
      m_Stream.rdbuf( m_pBuf );
    }

      /**
        * Set eol mode.
        * If set, the callback is only called after receiving eol,
        * before that all data is appended in a buffer.
        * @param ac_bSet true for append
        */
    void mp_SetEolMode( const bool ac_bSet = true )
    { mv_bEol = ac_bSet;}

      /**
        * The eol character.
        */
    static const Elem smc_Eol = '\n';

      /**
        * Override xsputn and make it forward data to the callback function.
        */
    std::streamsize xsputn( const Elem* _Ptr, std::streamsize _Count )
    {
      if( mv_bEol )
      {
        m_sAppend.append( _Ptr, _Ptr + _Count );

        std::string::size_type pos = 0;
        while( pos != std::string::npos )
        {
          pos = m_sAppend.find( smc_Eol );
          if( pos != std::string::npos )
          {
            std::string tmp( m_sAppend.begin(), m_sAppend.begin() + pos );
            m_pCbFunc( tmp.c_str(), (std::streamsize) tmp.length() );
            m_sAppend.clear();
          }
        }
      }
      else
      {
        m_pCbFunc( _Ptr, _Count );
      }
      return _Count;
    }

      /**
        * Override overflow and make it forward data to the callback function.
        */
    typename Tr::int_type overflow( typename Tr::int_type v )
    {
      Elem ch = Tr::to_char_type( v );
      if( mv_bEol )
      {
        if( ch == smc_Eol )
        {
          m_pCbFunc( m_sAppend.c_str(), (std::streamsize) m_sAppend.length() );
          m_sAppend.clear();
        }
        else
        {
          m_sAppend.append( &ch, 1 );
        }
      }
      else
      {
        m_pCbFunc( &ch, 1 );
      }
      return Tr::not_eof( v );
    }

 protected:
    std::basic_ostream<Elem, Tr>& m_Stream;
    std::streambuf*               m_pBuf;
    pfncb                         m_pCbFunc;
    bool                          mv_bEol;
    bool                          mv_bEolFound;
    std::string                   m_sAppend;
  };

}

#endif //#ifndef __STDREDIRECTOR_H__
