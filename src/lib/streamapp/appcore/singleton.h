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

#ifndef __MYSINGLETON_H_
#define __MYSINGLETON_H_

#include "../defines.h"

#include "threads/locks.h"

#include <QtGlobal>

namespace appcore
{

  template<class tUnique>
  class SingletonDestroyer;

    /**
      * Singleton
      *   a singleton is a class of which only one instance can exist.
      *   If the instance is not deleted during the run of the program,
      *   it will be deleted automatically when the program exits.
      *   This singleton is threadsafe and protected against double deletion.
      *   In other words, it's safe to call delete sf_pInsance() if
      *   the derived class' destructor would be public, although it is
      *   preferred to use sf_DeleteInstance().
      *
      *   Usage:
      *   @code
      *   class foo : public Singleton<foo>
      *   {
      *     friend class Singleton<foo>;
      *   private:
      *     ~foo();
      *     foo();
      *   public:
      *     void MethA();
      *   }
      *
      *   foo::sf_pInstance()->MethA();
      *   @endcode
      *
      *********************************************************************** */
  template<class tUnique>
  class Singleton
  {
  public:
      /**
        * Get the instance.
        * Creates the instance if this isn't done already.
        * @return pointer to the instance
        */
    static tUnique* sf_pInstance()
    {
      if( sm_pInstance == 0 )
      {
        const Lock L( sm_Lock );
        if( sm_pInstance == 0 )
        {
          static bool bInside = false;
          if( !bInside )
          {
            bInside = true;
            tUnique* pNew = new tUnique();
            bInside = false;
            sm_pInstance = pNew;
            sm_Destroyer.SetDoomed( sm_pInstance );
          }
          else
          {
            Q_ASSERT( 0 && "two ore more threads tried to create the Singleton's insatnce at the same time!" );
          }
        }
      }
      return sm_pInstance;
    }

      /**
        * Get the instance pointer.
        * Doesn't create an instance if it's not there, just returns the pointer.
        * This can be used to check whether the instance is alive.
        * @return instance pointer
        */
    static INLINE tUnique* sf_pInstanceNoCreate()
    {
      return sm_pInstance;
    }

      /**
        * Safely delete the instance.
        * If it's not been created, does nothing.
        */
    static void sf_DeleteInstance()
    {
      if( sm_pInstance != 0 )
      {
        tUnique* const pOld = sm_pInstance;
        sm_pInstance = 0;
        delete pOld;
      }
    }

  protected:
      /**
        * Safely clear the instance pointer.
        */
    void mf_ClearInstance()
    {
      if( sm_pInstance == this )
        sm_pInstance = 0;
    }

      /**
        * Protected Constructor.
        */
    Singleton()
    {}

      /**
        * Protected Destructor.
        */
    virtual ~Singleton()
    {
      mf_ClearInstance(); //if only one can be born, there will be nothing left when that one dies
    }

    static tUnique*               sm_pInstance;
    static const CriticalSection  sm_Lock;
    friend class SingletonDestroyer<tUnique>;
    static SingletonDestroyer<tUnique> sm_Destroyer;
  };

  template<class tUnique>
  const CriticalSection Singleton<tUnique>::sm_Lock;

  template<class tUnique>
  tUnique* Singleton<tUnique>::sm_pInstance = 0;

  template<class tUnique>
  SingletonDestroyer<tUnique> Singleton<tUnique>::sm_Destroyer;   //delete the singleton when application exits


    /**
      * SingletonDestroyer
      *   Deletes Singleton at application exit.
      *   The sf_DeleteInstance() method of the Singleton that's set 'doomed'
      *   will be called when Destroyer's destructor is called.
      *   @see Singleton
      *********************************************************************** */
  template <class tUnique>
  class SingletonDestroyer
  {
  public:
      /**
        * Constructor.
        */
    SingletonDestroyer() :
      m_pDoomed( 0 )
    {}

      /**
        * Destructor.
        * Calls sf_DeleteInstance() on the doomed object.
        */
    ~SingletonDestroyer()
    {
      if( m_pDoomed )
        m_pDoomed->sf_DeleteInstance();
    }

      /**
        * Set the doomed Singleton.
        * @param a_pDoomed the Singleton to be deleted
        */
    void SetDoomed( Singleton<tUnique>* a_pDoomed )
    {
      if( m_pDoomed )
        Q_ASSERT( 0 && "this destroyer object already has a resource assigned to it" );
      m_pDoomed = a_pDoomed;
    }

  private:
    SingletonDestroyer( const SingletonDestroyer<tUnique>& );
    void operator= ( const SingletonDestroyer<tUnique>& );

    Singleton<tUnique>* m_pDoomed;
  };

}

#endif //#ifndef __MYSINGLETON_H_
