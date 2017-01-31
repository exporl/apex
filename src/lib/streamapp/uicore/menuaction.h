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
 
#ifndef __MENUACTION_H__
#define __MENUACTION_H__

#include "appcore/events/events.h"
using namespace appcore;

namespace uicore
{

    /**
      * Action enum.
      * Deprecated.
      */
  enum gt_eAction
  {
    gc_eNoAction,
    gc_eMenuAction,
    gc_eCommandAction
  };

    /**
      * gt_MenuAction
      *   struct with action data.
      *   Deprecated, will be replaced by Message stuff.
      ************************************************** */
  struct gt_MenuAction
  {
    unsigned  m_nTargetID;
    int       m_nCommand;
    unsigned  m_nDataLen;
    void*     m_pData;
  };

  typedef IEventPoster<gt_eAction> MenuActionPoster;
  typedef IEventDispatcher<gt_eAction> MenuActionDispatcher;
  typedef IEventCommunicator<gt_eAction> MenuActionCommunicator;


    /**
      * ActionComponent
      *   a Component that can post MenuActions
      ***************************************** */
  class ActionComponent : public MenuActionPoster
  {
  protected:
    ActionComponent( MenuActionDispatcher* a_pTarget = 0 ) :
           m_ActionType( gc_eNoAction )
           {
             MenuActionPoster::mp_InstallEventDispatcher( a_pTarget );
           }

  public:
      /**
        * Get a reference to the Action.
        * Used to set the actual commands etc by the application.
        * @return a mutable reference
        */
    INLINE gt_MenuAction& mp_GetAction()
    {
      return m_Action;
    }

      /**
        * Set the Action type.
        * Defaults to no action, hence nothing happens when
        * mp_DoAction is called.
        * @param ac_eActionType
        */
    INLINE void mp_SetActionType( const gt_eAction ac_eActionType )
    {
      m_ActionType = ac_eActionType;
    }

      /**
        * Post the action to the target, if any.
        */
    virtual void mp_DoAction()
    {
      if( m_ActionType != gc_eNoAction )
        MenuActionPoster::mp_DispatchEventToListener( m_ActionType, &m_Action );
    }

  protected:
    gt_MenuAction   m_Action;
    gt_eAction      m_ActionType;
  };

}

#endif //#ifndef __MENUACTION_H__
