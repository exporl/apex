/******************************************************************************
* Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
*                                                                            *
* This file is part of APEX 4.                                               *
*                                                                            *
* APEX 4 is free software: you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by       *
* the Free Software Foundation, either version 2 of the License, or          *
* (at your option) any later version.                                        *
*                                                                            *
* APEX 4 is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          *
* along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
*****************************************************************************/

#include "statusitem.h"

#include <QDateTime>
#include <QString>

namespace apex
{

struct StatusItemPrivate {
    StatusItem::Level level;
    QString source;
    QString message;
    QDateTime dateTime;
};

StatusItem::StatusItem(StatusItem::Level level, const QString &source,
                       const QString &message)
    : d(new StatusItemPrivate())
{
    d->message = message;
    d->source = source;
    d->level = level;
    d->dateTime = QDateTime::currentDateTime();
}

StatusItem::StatusItem(const StatusItem &other)
    : d(new StatusItemPrivate(*other.d))
{
}

StatusItem::~StatusItem()
{
    delete d;
}

const QString &StatusItem::message() const
{
    return d->message;
}

StatusItem &StatusItem::setMessage(const QString &message)
{
    d->message = message;
    return *this;
}

const QString &StatusItem::source() const
{
    return d->source;
}

StatusItem &StatusItem::setSource(const QString &source)
{
    d->source = source;
    return *this;
}

StatusItem::Level StatusItem::level() const
{
    return d->level;
}

StatusItem &StatusItem::setLevel(StatusItem::Level type)
{
    d->level = type;
    return *this;
}

const QDateTime &StatusItem::dateTime() const
{
    return d->dateTime;
}

StatusItem &StatusItem::operator=(const StatusItem &other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}

bool StatusItem::operator==(const StatusItem &other) const
{
    return d->level == other.d->level && d->dateTime == other.d->dateTime &&
           d->message == other.d->message && d->source == other.d->source;
}
}
