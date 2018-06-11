/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
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

#ifndef _APEX_SRC_TESTS_BERTHAPLUGINS_BERTHAPLUGINSTEST_H_
#define _APEX_SRC_TESTS_BERTHAPLUGINS_BERTHAPLUGINSTEST_H_

#include "../testmacros.h"

#include <QObject>
#include <QtTest>

class BerthaPluginsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void apexAdapterTest();
    void apexCompatibleDataBlockTest();
};

#endif
