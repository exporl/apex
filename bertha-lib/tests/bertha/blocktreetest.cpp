/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com> *
 * Copyright (C) 2017 Division of Experimental Otorhinolaryngology K.U.Leuven *
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

#include "bertha/blockdata.h"
#include "bertha/blocktree.h"
#include "bertha/connectiondata.h"
#include "bertha/devicedata.h"
#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"

#include "common/testutils.h"

#include "tests.h"

using namespace bertha;

namespace
{
QString addBlockToExperimentData(ExperimentData &data, const QString &source,
                                 const QString &target,
                                 const QString &targetPort)
{
    BlockData block(source, QL1S("dummy"));
    data.addBlock(block);
    if (!target.isEmpty())
        data.addConnection(
            ConnectionData(source, QL1S("output-1"), target, targetPort));
    return source;
}
}

void BerthaTest::testBlockTree()
{
    TEST_EXCEPTIONS_TRY

    ExperimentData data;
    DeviceData devData(QL1S("device"), QL1S("dummy"));
    devData.setInputPorts(4);
    data.setDevice(devData);

    // regular tree
    addBlockToExperimentData(data, QL1S("sin"), QL1S("gain"), QL1S("input-1"));
    addBlockToExperimentData(data, QL1S("sin2"), QL1S("gain2"),
                             QL1S("input-1"));
    addBlockToExperimentData(data, QL1S("gain"), QL1S("device"),
                             QL1S("playback-1"));
    addBlockToExperimentData(data, QL1S("gain2"), QL1S("device"),
                             QL1S("playback-2"));
    BlockTree tree(data);
    QStringList ids = tree.orderedBlockIds(tree.leafNodes());
    QCOMPARE(ids.size(), 4);
    QVERIFY(ids.indexOf(QL1S("sin")) < ids.indexOf(QL1S("gain")));
    QVERIFY(ids.indexOf(QL1S("sin2")) < ids.indexOf(QL1S("gain2")));

    // dead end gets thrown away
    addBlockToExperimentData(data, QL1S("sum"), QL1S(""), QL1S(""));
    addBlockToExperimentData(data, QL1S("sin3"), QL1S("sum"), QL1S("input-1"));
    addBlockToExperimentData(data, QL1S("sin4"), QL1S("sum"), QL1S("input-2"));
    BlockTree tree2(data);
    ids = tree2.orderedBlockIds(tree2.leafNodes());
    QCOMPARE(ids.size(), 4);
    QVERIFY(ids.indexOf(QL1S("sin")) < ids.indexOf(QL1S("gain")));
    QVERIFY(ids.indexOf(QL1S("sin2")) < ids.indexOf(QL1S("gain2")));

    // connect the summation block
    data.addConnection(ConnectionData(QL1S("sum"), QL1S("output-1"),
                                      QL1S("device"), QL1S("playback-3")));

    BlockTree tree3(data);
    ids = tree3.orderedBlockIds(tree3.leafNodes());
    QCOMPARE(ids.size(), 7);
    QVERIFY(ids.indexOf(QL1S("sin")) < ids.indexOf(QL1S("gain")));
    QVERIFY(ids.indexOf(QL1S("sin2")) < ids.indexOf(QL1S("gain2")));
    QVERIFY(ids.indexOf(QL1S("sin3")) < ids.indexOf(QL1S("sum")));
    QVERIFY(ids.indexOf(QL1S("sin4")) < ids.indexOf(QL1S("sum")));

    // add connection to block that doesn't exist
    addBlockToExperimentData(data, QL1S("sin5"), QL1S("nonExisting"),
                             QL1S("input-1"));
    BlockTree tree4(data);
    ids = tree4.orderedBlockIds(tree4.leafNodes());
    QVERIFY(!ids.contains(QL1S("sin5")));

    // add connection from block that doesn't exist
    data.addConnection(ConnectionData(QL1S("sum2"), QL1S("output-1"),
                                      QL1S("device"), QL1S("playback-4")));
    try {
        BlockTree tree5(data);
        Q_FOREACH (const QString &blockId,
                   tree5.orderedBlockIds(tree5.leafNodes()))
            data.block(blockId);
        QFAIL("Exception expected");
    } catch (const std::exception &) {
        data.removeConnectionAt(data.connections().size() - 1);
        // expected
    }

    TEST_EXCEPTIONS_CATCH
}

void BerthaTest::testBlockTreeLeafNodeSelection()
{
    TEST_EXCEPTIONS_TRY

    ExperimentData data;
    DeviceData devData(QL1S("device"), QL1S("dummy"));
    devData.setInputPorts(2);
    data.setDevice(devData);

    QSet<QString> usedBlocks;
    QSet<QString> unusedBlocks;
    usedBlocks += addBlockToExperimentData(data, QL1S("sin"), QL1S("gain"),
                                           QL1S("input-1"));
    unusedBlocks += addBlockToExperimentData(data, QL1S("sin2"), QL1S("gain2"),
                                             QL1S("input-1"));
    usedBlocks += addBlockToExperimentData(data, QL1S("gain"), QL1S("sum"),
                                           QL1S("input-1"));
    unusedBlocks += addBlockToExperimentData(data, QL1S("gain2"), QL1S("sum"),
                                             QL1S("input-2"));
    usedBlocks += addBlockToExperimentData(data, QL1S("sum"), QL1S("device"),
                                           QL1S("playback-1"));

    BlockTree tree(data);

    // only ask for sin
    QStringList leafNodes;
    leafNodes << QL1S("sin");
    QStringList orderedBlockIds = tree.orderedBlockIds(leafNodes);
    QVERIFY(orderedBlockIds.toSet().contains(usedBlocks));
    QVERIFY(!orderedBlockIds.toSet().contains(unusedBlocks));
    QVERIFY(orderedBlockIds.indexOf(QL1S("sin")) <
            orderedBlockIds.indexOf(QL1S("gain")));

    // ask for both sin and sin2
    leafNodes << QL1S("sin2");
    orderedBlockIds = tree.orderedBlockIds(leafNodes);
    QVERIFY(orderedBlockIds.toSet().contains(usedBlocks + unusedBlocks));

    // drop the summation block
    addBlockToExperimentData(data, QL1S("sin3"), QL1S("device"),
                             QL1S("playback-2"));
    BlockTree tree2(data);
    leafNodes.clear();
    leafNodes << QL1S("sin3");
    orderedBlockIds = tree2.orderedBlockIds(leafNodes);
    QCOMPARE(orderedBlockIds.size(), 1);
    QVERIFY(orderedBlockIds.contains(QL1S("sin3")));
    QVERIFY(!orderedBlockIds.contains(QL1S("sum")));

    // drop all blocks
    BlockTree tree3(data);
    orderedBlockIds = tree3.orderedBlockIds(QStringList());
    QCOMPARE(orderedBlockIds.size(), 0);

    TEST_EXCEPTIONS_CATCH
}
