/******************************************************************************
 * Copyright (C) 2016  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef _APEX_SRC_LIB_APEXMAIN_WAVSTIMULUS_BERTHABUFFER_H_
#define _APEX_SRC_LIB_APEXMAIN_WAVSTIMULUS_BERTHABUFFER_H_

#include "streamapp/streamappdefines.h"

#include "apextools/global.h"

namespace bertha
{
class ExperimentData;
}
namespace apex
{
namespace stimulus
{
class BerthaBufferPrivate;

class APEX_EXPORT BerthaBuffer : public QObject
{
    Q_OBJECT
public:
    BerthaBuffer(const bertha::ExperimentData &data);
    ~BerthaBuffer();

    void loadNewExperiment(const bertha::ExperimentData &data);

    void start();
    /* Run leafnodes marked permanent through addPermanentLeafNodes.
     * These are continuous filters.
     */
    void runPermanentLeafNodesOnly();
    void stop();
    void release();

    void setGain(const unsigned channel, const double gain);
    void queueParameter(const QString &id, const QString &parameter,
                        const QVariant &value);
    void setParameter(const QString &id, const QString &parameter,
                      const QVariant &value);

    void addPermanentLeafNode(const QString &node);
    void setActiveDataBlocks(const QStringList &nodes);

    bool endOfInputReached() const;

    const streamapp::Stream &processStream();

Q_SIGNALS:
    void requestData(QVector<float *> *buffer);

private:
    BerthaBufferPrivate *const d;
};
}
}
#endif // _APEX_SRC_LIB_APEXMAIN_WAVSTIMULUS_BERTHABUFFER_H_
