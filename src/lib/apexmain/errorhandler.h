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

#ifndef _APEX_SRC_LIB_APEXMAIN_ERRORHANDLER_H_
#define _APEX_SRC_LIB_APEXMAIN_ERRORHANDLER_H_

#include "apextools/status/statusitem.h"

#include <QObject>
#include <QQueue>

namespace apex
{

class APEX_EXPORT ErrorHandler :
    public QObject
{
    Q_OBJECT
public:
    ErrorHandler();
    virtual ~ErrorHandler();

    unsigned numberOfErrors() const;
    void clearCounters();

    static ErrorHandler *instance();

protected:
    virtual void connectNotify(const QMetaMethod &signal) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void addItem(const apex::StatusItem &item);

Q_SIGNALS:
    void itemAdded(const apex::StatusItem &item);

private:
    static void messageOutput(QtMsgType type,
            const QMessageLogContext &context,
            const QString &msg);

private:
    static ErrorHandler *globalInstance;
    static QtMessageHandler oldHandler;

    unsigned nbErrors;
    QQueue<StatusItem> queuedItems;
};

}
#endif

