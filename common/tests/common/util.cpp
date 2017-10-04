/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "commontest.h"

#include <QDir>
#include <QTest>
#include <QTextStream>

using namespace cmn;

void CommonTest::utilTest()
{
    TEST_EXCEPTIONS_TRY

    const QString dirName = createTempDirectory();
    QVERIFY(QDir(dirName).exists());

    {
        FileDeleter deleter(dirName);
    }
    QVERIFY(!QDir(dirName).exists());

    QString name;
    QString file;
    {
        TemporaryDirectory dir;
        QVERIFY(dir.dir().absolutePath().startsWith(QDir::tempPath()));
        name = dir.dir().absolutePath();
        file = dir.dir().filePath("test.txt");
        QCOMPARE(dir.file("test.txt"), file);
        {
            QFile testFile(file);
            QVERIFY(testFile.open(QIODevice::WriteOnly));
            QTextStream testContent(&testFile);
            testContent << "Blablabla " << name;
        }
        QVERIFY(QFile(file).exists());
        QCOMPARE(dir.addFile("test.txt"), file);

        try {
            dir.addFile("/test.txt");
            QFAIL("No std::exception for invalid file name");
        } catch (const std::exception &) {
        }

        try {
            dir.addFile("../test.txt");
            QFAIL("No std::exception for invalid file name");
        } catch (const std::exception &) {
        }
    }

    QVERIFY(!QFile(file).exists());
    QVERIFY(!QDir(name).exists());

    TEST_EXCEPTIONS_CATCH
}
