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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSITEM_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSITEM_H_

#include "../global.h"

#include <QString>

class QDateTime;

namespace apex
{

struct StatusItemPrivate;

/**
 * @class  StatusItem
 * @author Job Noorman
 *
 * @brief Represents an item that can be reported by a StatusReporter.
 *
 * The date and time will be set to the current date on construction.
 * Supports the "named parameter" idiom:
 * @code
 * StatusReporter rep;
 * rep << StatusItem(StatusItem::Fatal)
 *              .setSource("Apex")
 *              .setMessage("Some error...");
 * @endcode
 */
class APEXTOOLS_EXPORT StatusItem
{
public:
    enum Level {
        Unused = 0x00, // use when there's nothing to report
        Debug = 0x01,
        Info = 0x02,
        Warning = 0x04,
        Critical = 0x08,
        Fatal = 0x10,
        AllButDebug = 0x1E,
        All = 0x1F
    };
    Q_DECLARE_FLAGS(Levels, Level);

    StatusItem(Level level = Unused, const QString &source = QString(),
               const QString &message = QString());
    StatusItem(const StatusItem &other);
    ~StatusItem();

    Level level() const;
    const QString &source() const;
    const QString &message() const;
    const QDateTime &dateTime() const;

    StatusItem &setLevel(Level level);
    StatusItem &setSource(const QString &source);
    StatusItem &setMessage(const QString &message);

    StatusItem &operator=(const StatusItem &other);
    bool operator==(const StatusItem &other) const;

private:
    StatusItemPrivate *d;
};

} // ns apex

Q_DECLARE_METATYPE(apex::StatusItem)
Q_DECLARE_OPERATORS_FOR_FLAGS(apex::StatusItem::Levels);

#endif
