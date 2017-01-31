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

#include "statusdispatcher.h"

#include "consolestatusreporter.h"

namespace apex
{

struct StatusDispatcherPrivate
{
    QMap<StatusReporter*, bool> reporters;
    bool addedReporter;
};

StatusDispatcher::StatusDispatcher() : d(new StatusDispatcherPrivate)
{
    d->addedReporter = false;
}

StatusDispatcher::~StatusDispatcher()
{
    Q_FOREACH (StatusReporter* reporter, d->reporters.keys())
    {
        if (d->reporters[reporter])
            delete reporter;
    }

    delete d;
}

void StatusDispatcher::addReporter(StatusReporter* reporter, bool takeOwnership)
{
    if (!d->addedReporter)
    {
        Q_ASSERT(d->reporters.size() <= 1);

        if (!d->reporters.isEmpty())
            removeReporter(d->reporters.keys().first());
    }

    d->reporters[reporter] = takeOwnership;
    d->addedReporter = true;
}

void StatusDispatcher::removeReporter(StatusReporter* reporter)
{
    if (d->reporters.contains(reporter))
    {
        if (d->reporters.take(reporter)) //if we have ownership
            delete reporter;
    }
}

QList<const StatusReporter*> StatusDispatcher::reporters() const
{
    QList<const StatusReporter*> ret;

    Q_FOREACH (StatusReporter* reporter, d->reporters.keys())
        ret << reporter;

    return ret;
}

void StatusDispatcher::report(const StatusItem& item)
{
    if (d->reporters.isEmpty())
        d->reporters[new ConsoleStatusReporter()] = true;

    Q_FOREACH (StatusReporter* reporter, d->reporters.keys())
        reporter->addItem(item);
}

void StatusDispatcher::clear()
{
    StatusReporter::clear();

    //TODO check if this really is what we want
    Q_FOREACH (StatusReporter* reporter, d->reporters.keys())
        reporter->clear();
}

}
