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
 
#ifndef __NOISY_H__
#define __NOISY_H__

#include <iostream>
#include <string>

#include "appcore/managedsingleton.h"

namespace utils
{

  class Noisy;

    /**
      * NoisyReport
      *   a singleton that will report Noisy statistics as the program terminates.
      *   Contains the counters for the operations.
      *   @see Noisy
      **************************************************************************** */
  class NoisyReport : public appcore::ManagedSingleton<NoisyReport>
  {
    friend class Noisy;
    friend class appcore::ManagedSingleton<NoisyReport>;
  private:
      /**
        * Private Constructor.
        */
    NoisyReport() :
     create( 0 ),
     assign( 0 ),
     copycons( 0 ),
     destroy( 0 )
    {
    }

    unsigned long create;
    unsigned long assign;
    unsigned long copycons;
    unsigned long destroy;

  public:
      /**
        * Destructor.
        * Prints the statistics.
        */
    ~NoisyReport()
    {
      std::cout <<  std::endl
        <<  "-------------------\n"
        <<  "Noisy creations: "         << create
        <<  "\nCopy-Constructions: "    << copycons
        <<  "\nAssignments: "           << assign
        <<  "\nDestructions: "          << destroy
        <<  std::endl;
    }
  };

    /**
      * Noisy
      *   utility to inspect various object operations.
      *   Cfr "Thinking in C++" by Bruce Eckel.
      *   All operations are tracked, and when the
      *   program terminates a report is printed.
      *   Can be used to track memory leaks or to find
      *   bottlenecks, eg places where lots of copies
      *   are made.
      ************************************************* */
  class Noisy
  {
  public:
      /**
        * Default Constructor.
        */
    Noisy() :
        m_ID( NoisyReport::sf_pInstance()->create++ ),
      m_sID( "unknown" )
    {
      mf_PrintInit();
    }

      /**
        * Constructor with ID.
        * @param ac_sID the ID string
        */
    Noisy( const std::string& ac_sID ) :
      m_ID( NoisyReport::sf_pInstance()->create++ ),
      m_sID( ac_sID )
    {
      mf_PrintInit();
    }

      /**
        * Copy Constructor.
        */
    Noisy( const Noisy& rv ) :
      m_ID( rv.m_ID ),
      m_sID( rv.m_sID + "copy" )
    {
      std::cout << rv.m_sID.c_str() << "copy[" << m_ID << "]" << std::endl;
      NoisyReport::sf_pInstance()->copycons++;
    }

      /**
        * Assignment operator
        * @param rv the object to copy
        * @return reference to this
        */
    Noisy& operator = ( const Noisy& rv )
    {
      std::cout << "(" << m_ID << ")=[" << rv.m_ID << "]" << std::endl;
      m_ID = rv.m_ID;
      m_sID = rv.m_sID + "assign";
      NoisyReport::sf_pInstance()->assign++;
      return *this;
    }

      /**
        * Destructor.
        */
    ~Noisy()
    {
      std::cout << m_sID.c_str() << "~[" << m_ID << "]" << std::endl;
      NoisyReport::sf_pInstance()->destroy++;
    }

      /**
        * Compare operator for the id.
        * @param lv left-hand object
        * @param rv right-hand object
        * @return true if lv.m_ID < rv.m_ID
        */
    friend bool operator < ( const Noisy& lv, const Noisy& rv )
    {
      return lv.m_ID < rv.m_ID;
    }

      /**
        * Compare operator for the id.
        * @param lv left-hand object
        * @param rv right-hand object
        * @return true if lv.m_ID > rv.m_ID
        */
    friend bool operator > ( const Noisy& lv, const Noisy& rv )
    {
      return lv.m_ID > rv.m_ID;
    }

      /**
        * Compare operator for the id.
        * @param lv left-hand object
        * @param rv right-hand object
        * @return true if lv.m_ID == rv.m_ID
        */
    friend bool operator ==( const Noisy& lv, const Noisy& rv )
    {
      return lv.m_ID == rv.m_ID;
    }

      /**
        * std::ostream operator for the id.
        * @param os an std::ostream reference
        * @param n a Noisy reference
        * @return os containing the id.
        */
    friend std::ostream& operator << ( std::ostream& os, const Noisy& n )
    {
      return os << n.m_ID << std::endl;
    }

      /**
        * Get the ID string
        * @return string reference
        */
    const std::string& mf_sGetID() const
    {
      return m_sID;
    }

    friend class NoisyReport;

  protected:
    void mf_PrintInit()
    {
      std::cout << m_sID.c_str() << "[" << m_ID << "]" << std::endl;
    }

    long        m_ID;
    std::string m_sID;
  };

    /**
      * NoisyGen
      *   generate Noisy instances for testing.
      *   Usage example:
      *   @code
      *   for( unsigned i = 0 ; i < 5 ; ++i )
      *     NoisyGen();
      *   @endcode
      ***************************************** */
  struct NoisyGen
  {
    Noisy operator()()
    { return Noisy(); }
  };

}

#endif //#ifndef __NOISY_H__
