/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#ifndef _EXPORL_COMMON_TESTS_COMMONGUI_COMMONGUITEST_H_
#define _EXPORL_COMMON_TESTS_COMMONGUI_COMMONGUITEST_H_

#include <QObject>

class DummyQObject : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    QString dummySlot(const QString &myString)
    {
        Q_EMIT dummySignal(myString);
        return myString;
    }
Q_SIGNALS:
    void dummySignal(QString);
};

class CommonGuiTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void webViewTest();
    void webViewWebSocketsTest();
};

#endif
