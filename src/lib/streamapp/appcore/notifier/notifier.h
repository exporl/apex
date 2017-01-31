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
 
#ifndef __NOTIFIER_H__
#define __NOTIFIER_H__

#include <queue>

namespace appcore
{
  
    /**
      * Notifier
      *   not finished.
      ***************** */
  template<class tType>
  class Notifier
  {
  public:
    Notifier( const unsigned ac_nMaxNumClients );
    ~Notifier();

    enum mt_eStatus
    {
      mc_eIdle,
      mc_eSent,
      mc_eCompleted
    };

    Notifier* mf_Acquire();
    void mf_Release( Notifier* a_Notifier );
    
    void mp_SendNotification( const tType* ac_pNotification );
    bool mp_bWaitOnNotification( const int ac_ntimeOut, const tType* ac_pNotification );

    typedef std::queue<tType> mt_Queue;
    
  private:
    const unsigned mc_nMaxNumClients;
    unsigned m_nNumClients;

    mt_Queue m_Queue;

    Notifier( const Notifier& );
    Notifier& operator = ( const Notifier& );
  };

} 

#endif //#ifndef __NOTIFIER_H__
