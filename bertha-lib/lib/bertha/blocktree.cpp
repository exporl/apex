/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#include "common/exception.h"

#include "blocktree.h"
#include "experimentdata.h"

#include <QStringList>

#include <QtGlobal>

using namespace cmn;

namespace bertha
{

class BlockTreePrivate
{
public:
    void calculateBlockTree(const QList<ConnectionData> &connections);
    void calculateBlockList();

    void cacheBlock(const BlockData &block);

    QMap<QString, BlockData> blockCache;

    QMap<QString, QSet<QString>> targetBlockIds;
    QMap<QString, QSet<QString>> sourceBlocks;
    // including the device
    QSet<QString> leafSourceIds;
    // target-block, <target-port, pair<source-block, source-port>>
    QMap<QString, SourceConnectionsMap> sources;
    // source-block, <source-port, vector<pair<target-block, target-port>>>>
    QMap<QString, TargetConnectionsMap> targets;

    QString deviceId;
    QStringList orderedBlockIds;
};

// BlockTreePrivate ============================================================

void BlockTreePrivate::calculateBlockTree(
    const QList<ConnectionData> &connections)
{
    // Add all connections
    Q_FOREACH (const ConnectionData &connection, connections) {
        const QString sourceBlock = connection.sourceBlock();
        const QString targetBlock = connection.targetBlock();
        targetBlockIds[sourceBlock].insert(targetBlock);
        sourceBlocks[targetBlock].insert(sourceBlock);

        const QString sourcePort = connection.sourcePort();
        const QString targetPort = connection.targetPort();
        QMap<QString, QPair<QString, QString>> &sourceConnections =
            sources[targetBlock];
        if (sourceConnections.contains(targetPort))
            throw Exception(BlockTree::tr("Unable to connect two "
                                          "different ports to one input port"));
        sourceConnections[targetPort] = qMakePair(sourceBlock, sourcePort);

        QMap<QString, QVector<QPair<QString, QString>>> &targetConnections =
            targets[sourceBlock];
        targetConnections[sourcePort].append(
            qMakePair(targetBlock, targetPort));
    }
}

void BlockTreePrivate::calculateBlockList()
{
    orderedBlockIds.clear();
    leafSourceIds.clear();

    QSet<QString> seenSourceBlockIds;
    QSet<QString> waitingSourceBlockIds;

    // Find all leaf source blocks
    waitingSourceBlockIds += sourceBlocks.value(deviceId);
    while (!waitingSourceBlockIds.isEmpty()) {
        QSet<QString> newWaitingSourceBlocks;
        Q_FOREACH (const QString &blockId, waitingSourceBlockIds) {
            if (seenSourceBlockIds.contains(blockId))
                continue;
            seenSourceBlockIds += blockId;
            const QSet<QString> newSourceBlocks = sourceBlocks.value(blockId);
            // devices or blocks with no inputs
            if (blockId == deviceId || newSourceBlocks.isEmpty())
                leafSourceIds += blockId;
            else
                newWaitingSourceBlocks += newSourceBlocks;
        }
        waitingSourceBlockIds = newWaitingSourceBlocks;
    }

    // Check for cycles and order: iterates over all blocks, a block is marked
    // as completed when all input blocks are marked as completed
    QSet<QString> seenProcessingBlockIds;
    QSet<QString> waitingProcessingBlockIds;

    seenProcessingBlockIds += leafSourceIds;
    Q_FOREACH (const QString &blockId, leafSourceIds)
        waitingProcessingBlockIds += targetBlockIds.value(blockId);
    Q_FOREACH (const QString &blockId, leafSourceIds)
        if (blockId != deviceId)
            orderedBlockIds += blockId;

    while (!waitingProcessingBlockIds.isEmpty()) {
        QSet<QString> newWaitingProcessingBlocks;
        bool found = false;
        Q_FOREACH (const QString &blockId, waitingProcessingBlockIds) {
            // block is not used
            if (!seenSourceBlockIds.contains(blockId))
                continue;
            // block was already seen
            if (seenProcessingBlockIds.contains(blockId))
                continue;
            // not all input ports available at the moment
            if (!(sourceBlocks.value(blockId) - seenProcessingBlockIds)
                     .isEmpty()) {
                newWaitingProcessingBlocks += blockId;
                continue;
            }
            // prevent loops by ignore devices
            if (blockId == deviceId)
                continue;

            newWaitingProcessingBlocks += targetBlockIds.value(blockId);
            seenProcessingBlockIds += blockId;
            orderedBlockIds += blockId;
            found = true;
        }
        if (!found && !newWaitingProcessingBlocks.isEmpty())
            throw Exception(BlockTree::tr("Unable to process block cycles"));
        waitingProcessingBlockIds = newWaitingProcessingBlocks;
    }
}

void BlockTreePrivate::cacheBlock(const BlockData &block)
{
    const QString blockId = block.id();
    if (blockId.isEmpty())
        throw Exception(
            BlockTree::tr("Unable to process blocks with empty id"));
    if (blockCache.contains(blockId))
        throw Exception(BlockTree::tr("Unable to process blocks with "
                                      "identical id %1")
                            .arg(blockId));
    blockCache[blockId] = block;
}

// BlockTree ===================================================================

BlockTree::BlockTree(const ExperimentData &data) : d(new BlockTreePrivate)
{
    d->cacheBlock(data.device());
    Q_FOREACH (const BlockData &block, data.blocks())
        d->cacheBlock(block);

    d->deviceId = data.device().id();
    d->calculateBlockTree(data.connections());
    d->calculateBlockList();
}

BlockTree::~BlockTree()
{
    delete d;
}

QStringList BlockTree::orderedBlockIds(const QStringList &leafNodes) const
{
    // a list for the ordered, a set for the intersect operation
    QStringList filteredOrderedBlockIds(leafNodes);
    filteredOrderedBlockIds.removeAll(d->deviceId);
    QSet<QString> usedBlocks = leafNodes.toSet();
    Q_FOREACH (const QString &blockId, d->orderedBlockIds) {
// see if any of this block's source blocks are used; if they are, we
// needs this block
// bool QSet::intersects needs Qt 5.6
#if QT_VERSION >= 0x050600
        bool used = usedBlocks.intersects(d->sourceBlocks.value(blockId));
#else
        bool used = !(usedBlocks & d->sourceBlocks.value(blockId)).isEmpty();
#endif
        if (used) {
            filteredOrderedBlockIds += blockId;
            usedBlocks += blockId;
        }
    }
    return filteredOrderedBlockIds;
}

QStringList BlockTree::leafNodes() const
{
    return d->leafSourceIds.toList();
}

SourceConnectionsMap BlockTree::sourceConnections(const QString &block) const
{
    return d->sources.value(block);
}

TargetConnectionsMap BlockTree::targetConnections(const QString &block) const
{
    return d->targets.value(block);
}

}; // namespace bertha
