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

#ifndef _BERTHA_SRC_LIB_BERTHA_BLOCKTREE_H_
#define _BERTHA_SRC_LIB_BERTHA_BLOCKTREE_H_

#include "blockdata.h"
#include "global.h"

#include <QCoreApplication>
#include <QPair>

namespace bertha
{

class ExperimentData;

class BlockTreePrivate;

typedef QMap<QString, QPair<QString, QString>> SourceConnectionsMap;
typedef QMap<QString, QVector<QPair<QString, QString>>> TargetConnectionsMap;
typedef QMapIterator<QString, QPair<QString, QString>> SourceConnectionsIter;
typedef QMapIterator<QString, QVector<QPair<QString, QString>>>
    TargetConnectionsIter;

// not shared
class BERTHA_EXPORT BlockTree
{
    Q_DECLARE_TR_FUNCTIONS(BlockTree)
public:
    // may throw
    BlockTree(const ExperimentData &data);
    ~BlockTree();

    QStringList
    orderedBlockIds(const QStringList &leafNodes = QStringList()) const;
    QStringList leafNodes() const;

    SourceConnectionsMap sourceConnections(const QString &block) const;
    TargetConnectionsMap targetConnections(const QString &block) const;

private:
    BlockTreePrivate *const d;
};

} // namespace bertha

#endif
