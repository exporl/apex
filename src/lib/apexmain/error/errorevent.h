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
 
#ifndef __APEX_ERROREVENT_H__
#define __APEX_ERROREVENT_H__

#include "status/statusitem.h"
#include <QEvent>

namespace apex
{

    /**
      * ErrorEvent
      *   a QEvent containing an error.
      *   ApexControl knows how to handle this;
      *   not sure if that i the right place,
      *   the error system is kinda messed up
      ***************************************** */
  class ErrorEvent : public QEvent
  {
  public:
    ErrorEvent( const StatusItem& ac_tError ) :
        QEvent( sc_tEventNumber ),
      m_Error( ac_tError )
    {
    }

    StatusItem m_Error;

    static const QEvent::Type sc_tEventNumber = (QEvent::Type) 9999;
  };

}

#endif //#ifndef __APEX_ERROREVENT_H__
