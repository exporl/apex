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
 
#ifndef __QUEUEMANAGER_H__
#define __QUEUEMANAGER_H__

#include "defines.h"
#include "queuebase.h"
#include "utils/tracer.h"
#include "appcore/singleton.h"

#ifdef QUEUE_USES_STD
#include <map>
#include <string>
#endif

namespace appcore
{

    /**
      * QueueManager
      *   class for getting the Queue classes by name.
      ************************************************ */
  class QueueManager : public Singleton<QueueManager>
  {
    friend class Singleton<QueueManager>;
  protected:
      /**
        * Protected Constructor.
        * @param ac_nMaxSize the maximum queue size
        */
    QueueManager( const unsigned ac_nMaxSize = 10 ) :
      mv_nMaxQSize( ac_nMaxSize )
    {
    }

  public:
      /**
        * Destructor.
        * Deletes all created QueueBase objects.
        * Should only be called after releasing all.
        */
    ~QueueManager()
    {
      mt_ManagersCIt itE = m_Managers.end();
      for( mt_ManagersCIt it = m_Managers.begin() ; it != itE ; ++it )
        delete (*it).second;
      m_Managers.clear();
    }

      /**
        * Set the maximum queue size.
        * This only has effect for queues created after this call.
        * @param ac_nMaxSize the maximum size
        */
    INLINE void mp_SetMaxQueueSize( const unsigned ac_nMaxSize )
    {
      mv_nMaxQSize = ac_nMaxSize;
    }

      /**
        * Create an EnQueue for the queue with the specified name and type.
        * If the queue doesn't exist, it is created.
        * @param ac_sQueueName the queue name
        * @param tQueueElement the queue type
        * @return pointer to an EnQueue, or 0 for error
        */
    template<class tQueueElement>
    EnQueue<tQueueElement>* mf_pGetEnQueue( const std::string& ac_sQueueName )
    {
      QueueBase<tQueueElement>* pQueue = mf_pQueueManager<tQueueElement>( ac_sQueueName );
      if( pQueue )
        return pQueue->mf_pRegisterEnQueue();
      return 0;
    }

      /**
        * Create an DeQueue for the queue with the specified name and type.
        * If the queue doesn't exist, it is created.
        * @param ac_sQueueName the queue name
        * @param tQueueElement the queue type
        * @return pointer to an DeQueue, or 0 for error
        */
    template<class tQueueElement>
    DeQueue<tQueueElement>* mf_pGetDeQueue( const std::string& ac_sQueueName )
    {
      QueueBase<tQueueElement>* pQueue = mf_pQueueManager<tQueueElement>( ac_sQueueName );
      if( pQueue )
        return pQueue->mf_pRegisterDeQueue();
      return 0;
    }

      /**
        * Add the specified QueueBase as a named queue.
        * Used eg for adding a derived QueueBase.
        * @param ac_sQueueName the queue name
        * @param ac_pQueueBase the queue base
        * @param tQueueElement the queue type
        * @return true if added
        */
    template<class tQueueElement>
    bool mf_bAddQueueBase( const std::string& ac_sQueueName, QueueBase<tQueueElement>* const ac_pQueueBase )
    {
        //see if name is known
      mt_ManagersCIt itFound = m_Managers.find( ac_sQueueName );
      if( itFound != m_Managers.end() )
      {
        if( ac_pQueueBase == dynamic_cast<QueueBase<tQueueElement>*>( (*itFound).second ) )
          return true;
        return false;
      }
      m_Managers[ ac_sQueueName ] = ac_pQueueBase;
      return true;
    }

      /**
        * Release the specified queue.
        * @param ac_sQueueName the queue name
        * @return false if queue name not found
        */
    bool mf_bReleaseQueue( const std::string& ac_sQueueName )
    {
      mt_ManagersCIt itFound = m_Managers.find( ac_sQueueName );
      if( itFound != m_Managers.end() )
      {
        (*itFound).second->mp_Release();
        return true;
      }
      return false;
    }

      /**
        * Release all queues.
        */
    void mf_ReleaseAll()
    {
      mt_ManagersCIt itE = m_Managers.end();
      for( mt_ManagersCIt it = m_Managers.begin() ; it != itE ; ++it )
        (*it).second->mp_Release();
    }

  protected:
    template<class tQueueElement>
    QueueBase<tQueueElement>* mf_pQueueManager( const std::string& ac_sQueueName )
    {
        //first see if the name is known
      mt_ManagersCIt itFound = m_Managers.find( ac_sQueueName );
      if( itFound != m_Managers.end() )
      {
          //if so, it must be for a manager of the same type since we use a simple map
        QueueBase<tQueueElement>* pRet = 0;
        try
        {
          pRet = dynamic_cast<QueueBase<tQueueElement>*>( (*itFound).second );
        }
        catch(...)
        {
          s_assert( 0 ); //should never come here since we cast pointers and not references
        }
        s_assert( pRet );
        return pRet;
      }

        //name and queue don't exist so create new and add
      QueueBase<tQueueElement>* pRet = new QueueBase<tQueueElement>( mv_nMaxQSize );
      m_Managers[ ac_sQueueName ] = pRet;

      return pRet;
    }

    typedef std::map<const std::string, tQueueBase*>  mt_Managers;
    typedef mt_Managers::const_iterator               mt_ManagersCIt;

    mt_Managers   m_Managers;
    unsigned      mv_nMaxQSize;

  private:
    QueueManager( const QueueManager& );
    QueueManager& operator = ( const QueueManager& );
  };

}

#endif //#ifndef __QUEUEMANAGER_H__
