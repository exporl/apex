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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSITEM_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSITEM_H_

#include "../global.h"

#include <QList>
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
 * rep << StatusItem(StatusItem::Error)
 *              .setSource("Apex")
 *              .setMessage("Some error...");
 * @endcode
 */
class APEXTOOLS_EXPORT StatusItem
{
    public:

        typedef QList<StatusItem> List;

        enum Level
        {
            Unused  = 0x0,   //use when there's nothing to report
            Message = 0x1,
            Warning = 0x2,
            Error   = 0x4
        };
        Q_DECLARE_FLAGS(Levels, Level);

        StatusItem(Level level = Unused, const QString& source  = "",
                                         const QString& message = "");
        StatusItem(const StatusItem& other);
        ~StatusItem();

        //getters

        Level level() const;
        const QString& source() const;
        const QString& message() const;
        const QDateTime& dateTime() const;

        //setters

        StatusItem& setLevel(Level level);
        StatusItem& setSource(const QString& source);
        StatusItem& setMessage(const QString& message);

        StatusItem& operator=(const StatusItem& other);
        bool operator==(const StatusItem& other) const;

    private:

        StatusItemPrivate* d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(apex::StatusItem::Levels);

}//ns apex

#endif
