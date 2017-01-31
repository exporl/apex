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

#include "statusreporter.h"

#include <QDebug>

namespace apex
{

struct StatusReporterPrivate
{
    StatusItem::List    items;
    StatusItem::Levels  reportLevels;
    uint                nbMessages;
    uint                nbWarnings;
    uint                nbErrors;
};

StatusReporter::StatusReporter() : d(new StatusReporterPrivate())
{
    setMinimumReportLevel(StatusItem::Message);
    clearCounters();
}

StatusReporter::StatusReporter(const StatusReporter& other) :
                                        d(new StatusReporterPrivate(*other.d))
{
}

StatusReporter::~StatusReporter()
{
    delete d;
}

void StatusReporter::addError(const QString& source, const QString& message)
{
    addItem(StatusItem(StatusItem::Error, source, message));
}

void StatusReporter::addWarning(const QString& source, const QString& message)
{
    addItem(StatusItem(StatusItem::Warning, source, message));
}

void StatusReporter::addMessage(const QString& source, const QString& message)
{
    addItem(StatusItem(StatusItem::Message, source, message));
}

void StatusReporter::addItem(const StatusItem& item)
{
    d->items << item;
    //qCDebug(APEX_RS) << "adding item with source" << item.source() << "and level" << item.level();

    if (doReport(item))
    {
        switch (item.level())
        {
            case StatusItem::Error:
                d->nbErrors++;
                break;
            case StatusItem::Warning:
                d->nbWarnings++;
                break;
            case StatusItem::Message:
                d->nbMessages++;
                break;
            default:
                break;
        }

        report(item);
    }
}

void StatusReporter::addItems(const StatusItem::List& items)
{
    Q_FOREACH (StatusItem item, items)
        addItem(item);
}

void StatusReporter::setReportLevels(StatusItem::Levels levels)
{
    d->reportLevels = levels;
}

void StatusReporter::setMinimumReportLevel(StatusItem::Level level)
{
    //clear levels
    d->reportLevels ^= d->reportLevels;

    switch (level)
    {
        //yep, they all fall through
        case StatusItem::Unused:  d->reportLevels |= StatusItem::Unused;
        case StatusItem::Message: d->reportLevels |= StatusItem::Message;
        case StatusItem::Warning: d->reportLevels |= StatusItem::Warning;
        case StatusItem::Error:   d->reportLevels |= StatusItem::Error;
    }
}

StatusItem::List StatusReporter::items() const
{
    return d->items;
}

unsigned int StatusReporter::numberOfErrors() const
{
    return d->nbErrors;
}

unsigned int StatusReporter::numberOfMessages() const
{
    return d->nbMessages;
}

unsigned int StatusReporter::numberOfWarnings() const
{
    return d->nbWarnings;
}

StatusItem::Levels StatusReporter::reportLevels() const
{
    return d->reportLevels;
}

void StatusReporter::clear()
{
    d->items.clear();
    clearCounters();
}

void StatusReporter::clearCounters()
{
    d->nbErrors = d->nbMessages = d->nbWarnings = 0;
}

bool StatusReporter::doReport(const StatusItem& item)
{
    return d->reportLevels & item.level();
}

void StatusReporter::rereport()
{
    Q_FOREACH (StatusItem item, d->items)
    {
        if (doReport(item))
            report(item);
    }
}

StatusReporter& StatusReporter::operator=(const StatusReporter& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}

StatusReporter& StatusReporter::operator<<(const StatusItem& item)
{
    addItem(item);
    return *this;
}

StatusReporter& StatusReporter::operator<<(const StatusItem::List& items)
{
    addItems(items);
    return *this;
}

bool StatusReporter::operator==(const StatusReporter& other) const
{
    return  d->nbErrors == other.d->nbErrors &&
            d->nbMessages == other.d->nbMessages &&
            d->nbWarnings == other.d->nbWarnings &&
            d->items == other.d->items &&
            d->reportLevels == other.d->reportLevels;
}

void StatusReporter::report(const StatusItem& /*item*/)
{
}

}
