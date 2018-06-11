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

#include "berthabuffer.h"

#include "bertha/connectiondata.h"
#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"

#include <QtMath>

namespace apex
{
namespace stimulus
{
class BerthaBufferPrivate : public QObject
{
    Q_OBJECT
public:
    BerthaBufferPrivate();
    ~BerthaBufferPrivate();

    void prepare();
    void start();
    void stop();
    void release();

public:
    enum State { ExperimentLoaded, ExperimentPrepared, ExperimentRunning };

    State berthaStatus;

    bertha::ExperimentData data;
    QScopedPointer<bertha::ExperimentProcessor> processor;
    QScopedPointer<streamapp::StreamBuf> streamBuf;

    QVector<QVector<float>> doubleVecBuf;
    QVector<float *> vecBuf;
    QStringList activeDataBlocks;
    QStringList filters;
    QSet<QString> permanentLeafNodes;

    unsigned channels;
    unsigned blockSize;

    QVector<double> gains;

    QMap<QString, QList<QPair<QString, QVariant>>> parameters;
    // QList<QVariantList> parameters;
};

BerthaBufferPrivate::BerthaBufferPrivate()
{
}

BerthaBufferPrivate::~BerthaBufferPrivate()
{
    release();
}

void BerthaBufferPrivate::prepare()
{
    switch (berthaStatus) {
    case ExperimentLoaded:
        processor->prepare();
        berthaStatus = ExperimentPrepared;
        break;
    case ExperimentPrepared:
        // do nothing if already prepared
        break;
    case ExperimentRunning:
        stop();
        break;
    }
}

void BerthaBufferPrivate::start()
{
    switch (berthaStatus) {
    case ExperimentLoaded:
        prepare();
    // fall through
    case ExperimentPrepared:
        processor->start();
        berthaStatus = ExperimentRunning;
        break;
    case ExperimentRunning:
        // do nothing if already running
        break;
    }
}

void BerthaBufferPrivate::stop()
{
    switch (berthaStatus) {
    case ExperimentLoaded:
        // do nothing if not running
        break;
    case ExperimentPrepared:
        // do nothing if not running
        break;
    case ExperimentRunning:
        processor->stop();
        berthaStatus = ExperimentPrepared;
        break;
    }
}

void BerthaBufferPrivate::release()
{
    switch (berthaStatus) {
    case ExperimentLoaded:
        // do nothing if not running
        break;
    case ExperimentRunning:
        stop();
    // fall through
    case ExperimentPrepared:
        processor->release();
        berthaStatus = ExperimentLoaded;
        break;
    }
}

// BerthaBuffer ==================================================

BerthaBuffer::BerthaBuffer(const bertha::ExperimentData &data)
    : d(new BerthaBufferPrivate)
{
    loadNewExperiment(data);
}

BerthaBuffer::~BerthaBuffer()
{
    delete d;
}

bool BerthaBuffer::endOfInputReached() const
{
    if (d->berthaStatus != BerthaBufferPrivate::ExperimentRunning)
        return true;

    Q_FOREACH (const QString &dbId, d->activeDataBlocks) {
        try {
            if (!d->processor->parameterValue(dbId, QSL("donePlaying"))
                     .toBool())
                return false;
        } catch (...) {
            return false;
        }
    }
    return true;
}

void BerthaBuffer::loadNewExperiment(const bertha::ExperimentData &data)
{
    qCDebug(APEX_RS, "number of blocks: %d", data.blocks().size());
    d->processor.reset(new bertha::ExperimentProcessor(data));

    d->data = data;
    d->channels = d->data.device().inputPorts();
    d->blockSize = d->data.device().blockSize();
    d->gains = QVector<double>(d->channels, 1.0);

    d->filters = d->processor->leafNodes();
    Q_FOREACH (const bertha::BlockData &blockData, data.blocks())
        if (blockData.plugin() == QL1S("ApexCompatibleDataBlock"))
            d->filters.removeAll(blockData.id());

    d->streamBuf.reset(new streamapp::StreamBuf(d->channels, d->blockSize));

    for (unsigned i = 0; i < d->channels; ++i) {
        d->doubleVecBuf.append(QVector<float>(d->blockSize));
        d->vecBuf.append(d->doubleVecBuf[i].data());
    }

    QObject *soundCard =
        d->processor
            ->parameterValue(data.device().id(), QLatin1String("pointer"))
            .value<QObject *>();
    QObject::connect(this, SIGNAL(requestData(QVector<float *> *)), soundCard,
                     SLOT(apexCallback(QVector<float *> *)),
                     Qt::DirectConnection);

    d->berthaStatus = d->ExperimentLoaded;
}

void BerthaBuffer::start()
{
    QSet<QString> newLeafNodes(
        QSet<QString>::fromList(d->activeDataBlocks + d->filters) +
        d->permanentLeafNodes);
    d->processor->setActiveLeafNodes(newLeafNodes.toList(), d->parameters,
                                     true);
    d->start();
    d->parameters.clear();
}

void BerthaBuffer::runPermanentLeafNodesOnly()
{
    d->processor->setActiveLeafNodes(d->permanentLeafNodes.toList());
}

void BerthaBuffer::stop()
{
    qCDebug(APEX_RS, "berthabuffer stop");
    d->stop();
}

void BerthaBuffer::release()
{
    qCDebug(APEX_RS, "berthabuffer release");
    d->release();
}

void BerthaBuffer::setGain(const unsigned channel, const double gain)
{
    d->gains[channel] = gain;
}

void BerthaBuffer::queueParameter(const QString &id, const QString &parameter,
                                  const QVariant &value)
{
    d->parameters[id].append(qMakePair<QString, QVariant>(parameter, value));
}

void BerthaBuffer::setParameter(const QString &id, const QString &parameter,
                                const QVariant &value)
{
    d->processor->setParameterValue(id, parameter, value);
}

void BerthaBuffer::addPermanentLeafNode(const QString &node)
{
    d->permanentLeafNodes << node;
}

void BerthaBuffer::setActiveDataBlocks(const QStringList &dataBlocks)
{
    d->activeDataBlocks = dataBlocks;
}

const streamapp::Stream &BerthaBuffer::processStream()
{
    if (d->berthaStatus == d->ExperimentRunning) {
        emit requestData(&d->vecBuf);
        for (unsigned i = 0; i < d->channels; ++i) {
            double gain =
                d->gains[i] == 1.0 ? 1.0 : qPow(10, d->gains[i] / 20.0);
            for (unsigned j = 0; j < d->blockSize; ++j)
                d->streamBuf->mp_Set(i, j, d->vecBuf[i][j] * gain);
        }
        return *(d->streamBuf);
    } else {
        // TODO: should generate error!
        qCCritical(APEX_RS,
                   "Unable to process stream if bertha is not running");
        return *(d->streamBuf);
    }
}
}
}

#include "berthabuffer.moc"
