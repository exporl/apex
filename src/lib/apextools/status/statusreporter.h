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

#ifndef APEX_STATUSREPORTER_H
#define APEX_STATUSREPORTER_H

#include "statusitem.h"
#include "global.h"

#include <vector>

namespace apex
{

struct StatusReporterPrivate;

/**
 * @class StatusReporter
 * @author Job Noorman
 *
 * @brief This class can be used to report StatusItem's.
 *
 * This class doesn't actually do any reporting, it just holds a list of added
 * items and provides convenient ways of adding them.
 * If you want a reporter that reports items in a specific way (i.e. print
 * to the console) inherit this class and override report(const StatusItem&).
 * This method will be called on every added item with a StatusItem::level()
 * which is in this reporters report levels.
 * This class holds counters (numberOfErrors(), numberOfWarnings() and
 * numberOfMessages()) that hold the number of items <b>reported</b> after
 * the last call to clear() or clearCounters().
 * The list returned by items() is a list of <b>all</b> items added, whether
 * reported or not, since the last call to clear().
 */
class APEXTOOLS_EXPORT StatusReporter
{
    public:

        StatusReporter();
        StatusReporter(const StatusReporter& other);
        virtual ~StatusReporter();

        void addItem(const StatusItem& item);
        void addItems(const StatusItem::List& items);
        void addError(const QString& source, const QString& message);
        void addWarning(const QString& source, const QString& message);
        void addMessage(const QString& source, const QString& message);

        /**
         * After a call to this method, only items with a StatusItem::level()
         * in the given @p levels will be reported.
         *
         * @note All added items will still be added to the list returned by
         *       items().
         *
         * @see setMinimumReportLevel(StatusItem::Level)
         */
        void setReportLevels(StatusItem::Levels levels);

        /**
         * After a call to this method, only items with a StatusItem::level()
         * greater than or equal to @p level will be reported.
         *
         * @note All added items will still be added to the list returned by
         *       items().
         *
         * @see setReportLevels(StatusItem::Levels)
         */
        void setMinimumReportLevel(StatusItem::Level level);

        StatusItem::List items() const;
        uint numberOfErrors() const;
        uint numberOfWarnings() const;
        uint numberOfMessages() const;
        StatusItem::Levels reportLevels() const;

        /**
         * Calls clearCounters() and clears the list returned by items().
         */
        virtual void clear();

        /**
         * Resets all counters to 0.
         */
        void clearCounters();

        /**
         * Calls report(const StatusItem&) on all items on which
         * doReport(const StatusItem&) returns true;
         */
        void rereport();

        StatusReporter& operator=(const StatusReporter& other);
        StatusReporter& operator<<(const StatusItem& item);
        StatusReporter& operator<<(const StatusItem::List& items);
        bool operator==(const StatusReporter& other) const;

    private:

        /**
         * Override this method to get an item reported to i.e. the console
         * or some widget.
         */
        virtual void report(const StatusItem& item);
        bool doReport(const StatusItem& item);

        StatusReporterPrivate* d;
};

}//ns apex

#endif
