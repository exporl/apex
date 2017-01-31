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

#ifndef APEXDATATEST_H
#define APEXDATATEST_H

#include <QtTest/QtTest>
#include "../testmacros.h"
#include "xml/xercesinclude.h"

namespace apex
{
namespace data
{
class DatablockData;
class ConnectionData;
class CorrectorData;
}
}

/** Test main apex library (libapex) **/
class ApexMainTest : public QObject
{
        Q_OBJECT

    private Q_SLOTS:

        void initTestCase();
        void testQic();
        void testAseq();
        void testL34DatablockAseq();
        void testAseqParser();
        void testL34DatablockAseq_data();
        void testL34DatablockAseqMapping();
        void testL34DatablockInvalidFile();
        void testL34Datablock_invalid();
        void testL34DatablockQic();
        void testL34InvalidCL();
        void cleanupTestCase();
		
    private:


};

#endif
