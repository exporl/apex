/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

#include "bertha/blocktree.h"
#include "bertha/experimentdata.h"

#include "common/testutils.h"

#include "tests.h"

using namespace bertha;

void BerthaTest::testData()
{
    TEST_EXCEPTIONS_TRY

    { // No blocks
        ExperimentData data;
        data.setDevice(
            DeviceData(QLatin1String("device"), QLatin1String("dummy")));
        data.addConnection(
            ConnectionData(QLatin1String("device"), QLatin1String("o1"),
                           QLatin1String("device"), QLatin1String("i1")));
        try {
            QCOMPARE(BlockTree(data).orderedBlockIds().size(), 0);
        } catch (const std::exception &e) {
            QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")
                                 .arg(QString::fromLocal8Bit(e.what()))));
        }
    }

    { // One block
        ExperimentData data;
        data.setDevice(
            DeviceData(QLatin1String("device"), QLatin1String("dummy")));
        data.addBlock(
            DeviceData(QLatin1String("block1"), QLatin1String("dummy")));
        try {
            QCOMPARE(BlockTree(data).orderedBlockIds().size(), 0);
            data.addConnection(
                ConnectionData(QLatin1String("block1"), QLatin1String("o1"),
                               QLatin1String("device"), QLatin1String("i1")));
            BlockTree tree(data);
            QCOMPARE(tree.orderedBlockIds(tree.leafNodes()).size(), 1);
            data.addConnection(
                ConnectionData(QLatin1String("device"), QLatin1String("o1"),
                               QLatin1String("block1"), QLatin1String("i1")));
            BlockTree tree2(data);
            QCOMPARE(tree2.orderedBlockIds(tree.leafNodes()).size(), 1);
        } catch (const std::exception &e) {
            QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")
                                 .arg(QString::fromLocal8Bit(e.what()))));
        }
        try { // Block not found
            ExperimentData data2 = data;
            data2.addConnection(
                ConnectionData(QLatin1String("block"), QLatin1String("o1"),
                               QLatin1String("device"), QLatin1String("i2")));
            BlockTree tree3(data2);
            Q_FOREACH (const QString &blockId,
                       tree3.orderedBlockIds(tree3.leafNodes()))
                data2.block(blockId);
            QFAIL("Exception expected");
        } catch (const std::exception &) {
            // expected
        }
        try { // Duplicate input port connection
            ExperimentData data2 = data;
            data2.addConnection(
                ConnectionData(QLatin1String("block1"), QLatin1String("o1"),
                               QLatin1String("device"), QLatin1String("i1")));
            BlockTree(data2).orderedBlockIds();
            QFAIL("Exception expected");
        } catch (const std::exception &) {
            // expected
        }
        { // Cycle
            ExperimentData data2 = data;
            data2.addConnection(
                ConnectionData(QLatin1String("block1"), QLatin1String("o1"),
                               QLatin1String("block1"), QLatin1String("i2")));
            try {
                BlockTree(data2).orderedBlockIds();
                QFAIL("Exception expected");
            } catch (const std::exception &) {
                // expected
            }
        }
    }

    { // Two blocks
        ExperimentData data;
        ExperimentData data2;
        data.setDevice(
            DeviceData(QLatin1String("device"), QLatin1String("dummy")));
        data.addBlock(
            DeviceData(QLatin1String("block1"), QLatin1String("dummy")));
        data.addBlock(
            DeviceData(QLatin1String("block2"), QLatin1String("dummy")));
        try { // Block pruning
            QCOMPARE(BlockTree(data).orderedBlockIds().size(), 0);
            data.addConnection(
                ConnectionData(QLatin1String("block2"), QLatin1String("o1"),
                               QLatin1String("device"), QLatin1String("i1")));
            BlockTree tree(data);
            QCOMPARE(tree.orderedBlockIds(tree.leafNodes()).size(), 1);
            data.addConnection(
                ConnectionData(QLatin1String("device"), QLatin1String("o1"),
                               QLatin1String("block1"), QLatin1String("i1")));
            BlockTree tree2(data);
            QCOMPARE(tree2.orderedBlockIds(tree2.leafNodes()).size(), 1);
            data.addConnection(
                ConnectionData(QLatin1String("device"), QLatin1String("o1"),
                               QLatin1String("block2"), QLatin1String("i1")));
            BlockTree tree3(data);
            QCOMPARE(tree3.orderedBlockIds(tree3.leafNodes()).size(), 1);
            data2 = data;
            // Loop that is not in the used part of the tree
            data.addConnection(
                ConnectionData(QLatin1String("block1"), QLatin1String("o1"),
                               QLatin1String("block1"), QLatin1String("i2")));
            BlockTree tree4(data);
            QCOMPARE(tree4.orderedBlockIds(tree4.leafNodes()).size(), 1);
        } catch (const std::exception &e) {
            QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")
                                 .arg(QString::fromLocal8Bit(e.what()))));
        }
        try { // Loop included
            data.addConnection(
                ConnectionData(QLatin1String("block1"), QLatin1String("o1"),
                               QLatin1String("device"), QLatin1String("i2")));
            QStringList blocks = BlockTree(data).orderedBlockIds();
            QFAIL("Exception expected");
        } catch (const std::exception &) {
            // expected
        }
        try {
            data = data2;
            data.addConnection(
                ConnectionData(QLatin1String("block1"), QLatin1String("o1"),
                               QLatin1String("device"), QLatin1String("i2")));
            BlockTree tree5(data);
            QCOMPARE(tree5.orderedBlockIds(tree5.leafNodes()).size(), 2);
        } catch (const std::exception &e) {
            QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")
                                 .arg(QString::fromLocal8Bit(e.what()))));
        }
    }

    TEST_EXCEPTIONS_CATCH
}
