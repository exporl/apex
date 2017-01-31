/******************************************************************************
* Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSDISPATCHER_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSDISPATCHER_H_

#include "statusreporter.h"

#include <QMap>

namespace apex
{

struct StatusDispatcherPrivate;

/**
 * @class StatusDispatcher
 * @author Job Noorman
 *
 * @brief This class sends StatusItem's to multiple StatusReporter's.
 *
 * You can add as much StatusReporter's through
 * addReporter(StatusReporter*, bool) and after a reporter has been added, all
 * added items through the api of StatusReporter will also be added to the
 * added reporters.
 *
 * @note When StatusReporter::addItem(const StatusItem&) or one of its
 *       convenience methods is called when there are no reporters added yet, a
 *       ConsoleStatusReporter will automatically be added. This reporter will
 *       be removed the first time a reporter is manually added.
 */
class APEXTOOLS_EXPORT StatusDispatcher : public StatusReporter
{
    public:

        StatusDispatcher();
        virtual ~StatusDispatcher();

        /**
         * Add a reporter to the list of all reporters that will receive items.
         * If @p takeOwnership is true, the StatusReporter will be deleted
         * when this dispatcher is deleted.
         */
        void addReporter(StatusReporter* reporter, bool takeOwnership = true);

        /**
         * Removes the given reporter. It is also deleted if we have ownership.
         */
        void removeReporter(StatusReporter* reporter);

        /**
         * Returns a list of (read-only) reporters currently in use.
         */
        QList<const StatusReporter*> reporters() const;

        /**
         * Calls StatusReporter::clear() on itself and all added reporters.
         */
        void clear();

    private:

        void report(const StatusItem& item);

        //prevent copying
        StatusDispatcher(const StatusDispatcher&);
        StatusDispatcher& operator=(const StatusDispatcher&);

        StatusDispatcherPrivate* d;
};

}//ns apex

#endif
