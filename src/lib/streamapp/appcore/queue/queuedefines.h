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
 
#ifndef __QUEUEDEFINES_H__
#define __QUEUEDEFINES_H__

namespace appcore
{

  #define QUEUE_USES_STD

    /**
      * Queue Status
      */
  enum gt_eQueueStatus
  {
    gc_eFill,     //!< items queued
    gc_eFull,     //!< queue is full
    gc_eEmpty,    //!< queue is empty
    gc_eReleased  //!< queue was released
  };

    /**
      * Queue Return Status
      */
  enum gt_eQueueReturnStatus
  {
    gc_eOk,       //!< successfully dequeued
    gc_eError,    //!< not dequeued, the queue is released
    gc_eTimeout   //!< timeout
  };

}

#endif //#ifndef __QUEUEDEFINES_H__
