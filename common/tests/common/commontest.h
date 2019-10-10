/******************************************************************************
 * Copyright (C) 2007,2012  Michael Hofmann <mh21@piware.de>                  *
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

#ifndef _EXPORL_COMMON_TESTS_COMMON_COMMONTEST_H_
#define _EXPORL_COMMON_TESTS_COMMON_COMMONTEST_H_

#include <QObject>
#include <QString>

class CommonTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void random();
    void sndfilewrappertest();
    void utilTest();
    void webSocketTest();
};

#endif
