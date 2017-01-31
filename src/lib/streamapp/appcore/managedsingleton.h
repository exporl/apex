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
 
#ifndef __MYMANAGEDSINGLETON_H_
#define __MYMANAGEDSINGLETON_H_

#include "singleton.h"
#include <vector>
#include <algorithm>

//#define SINGLETONDEBUGGING
#ifdef SINGLETONDEBUGGING
#include "utils/tracer.h"
#include "utils/stringutils.h"
#endif

namespace appcore
{

  class SingletonManager;
  template<class tUnique>
  class ManagedSingleton;

    /**
      * ManagedSingletonBase
      *   base for ManagedSingletons. SingletonManager needs
      *   this in order to be able to properly delete the ManagedSingleton.
      *   @see SingletonManager
      *   @see ManagedSingleton
      ********************************************************************* */
  class ManagedSingletonBase
  {
    friend class SingletonManager;
  protected:
      /**
        * Constructor.
        */
    ManagedSingletonBase()
  #ifdef SINGLETONDEBUGGING
      : mc_sIndex( sf_sCreateName() )
  #endif
    {}

      /**
        * Destructor.
        */
    virtual ~ManagedSingletonBase()
    {}

    virtual void mp_DeleteInstance() const = 0;

  #ifdef SINGLETONDEBUGGING
      /**
        * Get the name.
        * Implementations may override this to supply a custom name.
        * @return the name
        */
    virtual const std::string mf_sName() const
    { return mc_sIndex; }
  #endif

  private:
  #ifdef SINGLETONDEBUGGING
    static std::string sf_sCreateName()
    {
      static int nCount = -1;
      return toString( ++nCount );
    }
    const std::string mc_sIndex;
  #endif

    ManagedSingletonBase( const ManagedSingletonBase& );
    ManagedSingletonBase& operator = ( const ManagedSingletonBase& );
  };

    /**
      * SingletonManager
      *   registers pointers to ManagedSingletons in the order they are created.
      *   The SingletonManager instance will be deleted at application exit, as
      *   it is a Singleton itself, hereby deleting all registered singletons
      *   in reverse order of creation.
      *   It is allowed to unregister a singleton with or without deleting it,
      *   and it is even safe to delete the ManagedSingleton instance itself.
      *   @see ManagedSingleton
      *   @see ManagedSingletonBase
      ************************************************************************** */
  class SingletonManager : public Singleton<SingletonManager>
  {
    friend class Singleton<SingletonManager>;
  public:
      /**
        * Destructor.
        * Calls mp_UnregisterAll( true ).
        * @see mp_UnregisterAll()
        */
    virtual ~SingletonManager()
    {
      mp_UnregisterAll();
    }

      /**
        * Register a ManagedSingletonBase for deletion.
        * @param ac_pToReg pointer to the singleton, must be non-zero
        */
    void mp_Register( ManagedSingletonBase* const ac_pToReg )
    {
      assert( ac_pToReg );
  #ifdef SINGLETONDEBUGGING
      DBG( "SingletonManager: registered " + ac_pToReg->mf_sName() )
  #endif
      m_Items.push_back( ac_pToReg );
    }

      /**
        * Unregister everything.
        * Deletes all singletons in reverse order of creation,
        * or just removes them from the list if ac_bAlsoDelete is false.
        * @param ac_bAlsoDelete true to delete the singletons
        */
    void mp_UnregisterAll( const bool ac_bAlsoDelete = true )
    {
      while( !m_Items.empty() )
      {
        const ManagedSingletonBase* p = m_Items.back();
        //m_Items.pop_back();
        mp_Unregister( p, ac_bAlsoDelete );
      }
    }

      /**
        * Unregister a singleton.
        * It might be needed to delete in custom order,
        * or to remove one from the list.
        * @param ac_pToUnReg pointer to the singleton, must be non-zero
        * @param ac_bAlsoDelete true to delete the singleton
        */
    void mp_Unregister( const ManagedSingletonBase* ac_pToUnReg, const bool ac_bAlsoDelete = true )
    {
      static bool bInside = false;
      assert( ac_pToUnReg );
      if( !bInside )
      {
        typedef std::vector<const ManagedSingletonBase*>::reverse_iterator  reverse_iterator;
        reverse_iterator rpos = std::find( m_Items.rbegin(), m_Items.rend(), ac_pToUnReg );
        if( rpos != m_Items.rend() )
        {
          m_Items.erase( (++rpos).base() );
          bInside = true;
          mp_Remove( ac_pToUnReg, ac_bAlsoDelete );
          bInside = false;
        }
        else
        {
          assert( 0 && "cannot unregister item if not registered first!!" );
        }
      }
    }

  private:
      /**
        * Private Constructor.
        */
    SingletonManager()
    {}

      /**
        * Remove a ManagedSingletonBase.
        * Does nothing when ac_bAlsoDelete is false, except fro printing the name
        * when SINGLETONDEBUGGING is set.
        * @param ac_pToUnReg pointer to the singleton
        * @param ac_bAlsoDelete true to delete the singleton
        */
    void mp_Remove( const ManagedSingletonBase* ac_pToUnReg, const bool ac_bAlsoDelete ) const
    {
      if( ac_bAlsoDelete )
      {
  #ifdef SINGLETONDEBUGGING
        DBG( "SingletonManager: unregistered " + ac_pToUnReg->mf_sName() )
  #endif
        try
        {
          ac_pToUnReg->mp_DeleteInstance();
        }
        catch(...)
        {
          assert( 0 );
        }
      }
  #ifdef SINGLETONDEBUGGING
      else
        DBG( "SingletonManager: unregistered " + ac_pToUnReg->mf_sName() + " without deleting" );
  #endif
    }

    std::vector<const ManagedSingletonBase*> m_Items;
  };

    /**
      * ManagedSingleton
      *   template implementation for ManagedSingletonBase.
      *   Behaves like a Singleton, except that it registers itself with
      *   SingletonManager, in order to be deleted from there.
      *   If it gets deleted earlier, by sf_DeleteInstance() or by it's
      *   destructor, it unregisters itself so there are no surprises
      *   when SingletonManager gets deleted.
      *   @see Singleton
      *   @see SingletonManager
      *   @see ManagedSingletonBase
      ******************************************************************* */
  template<class tUnique>
  class ManagedSingleton : public ManagedSingletonBase
  {
  public:
      /**
        * Get the instance.
        * Registers the instance with SingletonManager.
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
            tUnique* pNew = new tUnique;
            bInside = false;
            sm_pInstance = pNew;
            SingletonManager::sf_pInstance()->mp_Register( sm_pInstance );
          }
          else
          {
            assert( 0 && "two ore more threads tried to create the ManagedSingleton's insatnce at the same time!" );
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
        * The singleton will unregister itself here.
        * If it's not been created, does nothing.
        */
    static void sf_DeleteInstance()
    {
      if( sm_pInstance != 0 )
      {
        tUnique* const pOld = sm_pInstance;
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
        * Protected constructor.
        */
    ManagedSingleton() : ManagedSingletonBase()
    {}

      /**
        * Protected Destructor.
        */
    virtual ~ManagedSingleton()
    {
      if( sm_pInstance )  //ok this is tricky: if the instance throws an exception while being cretaed, and the cpp lib
                          //supports stack unwinding, this destructor gets called when sm_pInstance is still 0
      {
        if( SingletonManager::sf_pInstanceNoCreate() != 0 ) //if it is zero, it means it is currently being deleted, so we are already unregistered
          SingletonManager::sf_pInstance()->mp_Unregister( sm_pInstance, false );
        mf_ClearInstance();
#ifdef SINGLETONDEBUGGING
        DBG( "ManagedSingleton: deleted " + mf_sName() + " via destructor" )
#endif
      }
    }

    void mp_DeleteInstance() const
    {
      if( sm_pInstance != 0 )
      {
        tUnique* const pOld = sm_pInstance;
        sm_pInstance = 0;
        delete pOld;
      }
    }

  private:
    static tUnique* sm_pInstance;
    static const CriticalSection  sm_Lock;
  };

  template<class tUnique>
  tUnique* ManagedSingleton<tUnique>::sm_pInstance = 0;

  template<class tUnique>
  const CriticalSection ManagedSingleton<tUnique>::sm_Lock;

  #define s_make_safe_singleton( classname, singletonname ) \
  class singletonname : public appcore::ManagedSingleton< singletonname >, public classname \
  { friend class appcore::ManagedSingleton< singletonname >; \
    private : singletonname() throw() {} \
              singletonname( const singletonname& ); \
              singletonname& operator = ( const singletonname& ); \
    public : ~singletonname() throw() {} };

}

#endif //#ifndef __MYMANAGEDSINGLETON_H_
