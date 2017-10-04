/******************************************************************************
* Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "errorhandler.h"

#include <QMetaMethod>

namespace apex
{

ErrorHandler *ErrorHandler::globalInstance = NULL;
QtMessageHandler ErrorHandler::oldHandler = NULL;

// ErrorHandler ================================================================

ErrorHandler::ErrorHandler() : nbErrors(0)
{
    globalInstance = this;
    qRegisterMetaType<apex::StatusItem>();
    oldHandler = qInstallMessageHandler(messageOutput);
}

ErrorHandler::~ErrorHandler()
{
    globalInstance = NULL;
    qInstallMessageHandler(oldHandler);
}

ErrorHandler *ErrorHandler::instance()
{
    return globalInstance;
}

void ErrorHandler::messageOutput(QtMsgType type,
                                 const QMessageLogContext &context,
                                 const QString &msg)
{
    QString source = QString::fromLatin1("%2 (%3: %1, %4)")
                         .arg(context.line)
                         .arg(context.category, context.file, context.function);
    StatusItem item(StatusItem::Unused, source, msg);
    switch (type) {
    case QtDebugMsg:
        item.setLevel(StatusItem::Debug);
        break;
    case QtInfoMsg:
        item.setLevel(StatusItem::Info);
        break;
    case QtWarningMsg:
        item.setLevel(StatusItem::Warning);
        break;
    case QtCriticalMsg:
        item.setLevel(StatusItem::Critical);
        break;
    case QtFatalMsg:
        item.setLevel(StatusItem::Fatal);
        break;
    }
    if (ErrorHandler *instance = ErrorHandler::instance())
        QMetaObject::invokeMethod(instance, "addItem", Qt::AutoConnection,
                                  QGenericReturnArgument(),
                                  Q_ARG(apex::StatusItem, item));
    oldHandler(type, context, msg);
}

void ErrorHandler::addItem(const StatusItem &item)
{
    if ((item.level() & (StatusItem::Fatal | StatusItem::Critical)) > 0)
        nbErrors++;

    static const QMetaMethod signal =
        QMetaMethod::fromSignal(&ErrorHandler::itemAdded);
    if (isSignalConnected(signal))
        Q_EMIT itemAdded(item);
    else
        queuedItems.enqueue(item);
}

unsigned ErrorHandler::numberOfErrors() const
{
    return nbErrors;
}

void ErrorHandler::clearCounters()
{
    nbErrors = 0;
}

void ErrorHandler::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&ErrorHandler::itemAdded)) {
        while (!queuedItems.isEmpty())
            Q_EMIT itemAdded(queuedItems.dequeue());
    }
}
}
