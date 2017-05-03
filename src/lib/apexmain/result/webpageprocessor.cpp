/******************************************************************************
 * Copyright (C) 2016  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "apextools/global.h"

#include "webpageprocessor.h"

#include <QDebug>

static const char* ERROR_SOURCE = "WebPageProcessor";

using namespace apex;

WebPageProcessor::WebPageProcessor(QObject *parent) :
    QWebPage(parent)
{
}

void WebPageProcessor::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID)
{
    //Add the message to the debug log:
    qCDebug(APEX_RS) << ERROR_SOURCE << message << lineNumber << sourceID;
    //Add a warning to the message window:
    qCWarning(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(ERROR_SOURCE, lineNumber + message)));
}

WebPageProcessor::~WebPageProcessor()
{
    //Should there be something in the destructor?
}
