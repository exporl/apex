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

#include "common/exception.h"

#include "blockinterface.h"
#include "blockprocessor.h"
#include "blocktree.h"
#include "deviceprocessor.h"
#include "experimentdata.h"
#include "experimentprocessor.h"
#include "ringbuffer.h"

#include <QList>
#include <QSemaphore>
#include <QStringList>
#include <QVector>

using namespace cmn;

namespace bertha
{

typedef QPair<BlockOutputPort *, BlockInputPort *> BlockPortPair;
typedef QPair<QString, QVariant> Parameter;
typedef QMap<QString, QSharedPointer<AbstractProcessor>> ProcessorMap;

class QueuedParameter
{
public:
    QueuedParameter() : index(0)
    {
    }

    QueuedParameter(const QWeakPointer<AbstractProcessor> &processor,
                    unsigned index, const QVariant &value)
        : processor(processor), index(index), value(value)
    {
    }

    QWeakPointer<AbstractProcessor> processor;
    unsigned index;
    QVariant value;
};

class ExperimentProcessorPrivate : public QObject
{
    Q_OBJECT
public:
    void checkPortCompatibility(const AbstractProcessor *const source,
                                const BlockOutputPort *const outputPort,
                                const AbstractProcessor *const target,
                                const BlockInputPort *const inputPort) const;

    void initDeviceProcessor();
    void initBlockProcessors(const QStringList &leafNodes,
                             bool recreate = false);
    void connectAndSwapBlockProcessors();
    void deinitBlockProcessors();

    AbstractProcessor *processor(const QString &blockId,
                                 const ProcessorMap &other = ProcessorMap());

    void setState(ExperimentProcessor::State newState);

    QVector<QueuedParameter>
    prepareParameters(const QMap<QString, QList<Parameter>> &parameters);

    ExperimentProcessor *p;

    ExperimentData experiment;
    QScopedPointer<BlockTree> blockTree;
    ProcessorMap processors;
    ProcessorMap newProcessors;
    DeviceProcessor *deviceProcessor;
    QVector<BlockProcessor *> currentBlockProcessors;
    QVector<BlockProcessor *> updatedBlockProcessors;
    QVector<BlockProcessor *> unusedBlockProcessors;

    QVector<BlockPortPair> newConnections;
    QVector<BlockInputPort *> unusedConnections;

    RingBuffer<QueuedParameter> queuedParameters;
    QMap<QString, QList<Parameter>> newParameters;

    ExperimentProcessor::State state;

    QAtomicInt currentBuffer;
    QSemaphore swapBlockProcessorsSemaphore, pauseMainThreadSemaphore;

private Q_SLOTS:
    void callback();
};

// ExperimentProcessorPrivate ==================================================

// may throw
void ExperimentProcessorPrivate::checkPortCompatibility(
    const AbstractProcessor *const source,
    const BlockOutputPort *const outputPort,
    const AbstractProcessor *const target,
    const BlockInputPort *const inputPort) const
{
    const unsigned inputPortBlockSize = inputPort->blockSize == 0
                                            ? source->getBlockSize()
                                            : inputPort->blockSize;
    const unsigned outputPortBlockSize = outputPort->blockSize == 0
                                             ? target->getBlockSize()
                                             : outputPort->blockSize;
    const unsigned inputPortSampleRate = inputPort->sampleRate == 0
                                             ? source->getSampleRate()
                                             : inputPort->sampleRate;
    const unsigned outputPortSampleRate = outputPort->sampleRate == 0
                                              ? target->getSampleRate()
                                              : outputPort->sampleRate;
    const BlockPort::Domain inputPortDomain = inputPort->domain;
    const BlockPort::Domain outputPortDomain = outputPort->domain;
    if (inputPortBlockSize != outputPortBlockSize)
        throw Exception(tr("Unable to link block ports with different "
                           "block size: %1 - %2: %3, %4 - %5: %6")
                            .arg(source->id(), outputPort->name)
                            .arg(inputPortBlockSize)
                            .arg(target->id(), inputPort->name)
                            .arg(outputPortBlockSize));
    if (inputPortSampleRate != outputPortSampleRate)
        throw Exception(tr("Unable to link block ports with different "
                           "sample rate: %1 - %2: %3, %4 - %5: %6")
                            .arg(source->id(), outputPort->name)
                            .arg(inputPortSampleRate)
                            .arg(target->id(), inputPort->name)
                            .arg(outputPortSampleRate));
    if (inputPortDomain != outputPortDomain)
        throw Exception(tr("Unable to link block ports with different "
                           "domains: %1 - %2: %3, %4 - %5: %6")
                            .arg(source->id(), outputPort->name,
                                 inputPortDomain == BlockPort::TimeDomain
                                     ? QLatin1String("time domain")
                                     : QLatin1String("frequency domain"))
                            .arg(target->id(), inputPort->name,
                                 outputPortDomain == BlockPort::TimeDomain
                                     ? QLatin1String("time domain")
                                     : QLatin1String("frequency domain")));
}

// may throw
void ExperimentProcessorPrivate::initDeviceProcessor()
{
    processors.clear();
    // Setup the device processor, may throw
    const DeviceData deviceData = experiment.device();
    const QString deviceId = deviceData.id();
    deviceProcessor = new DeviceProcessor(deviceData);
    deviceProcessor->connectCallback(this, SLOT(callback()));
    processors.insert(deviceId,
                      QSharedPointer<AbstractProcessor>(deviceProcessor));
    currentBuffer = 0;
}

// may throw
void ExperimentProcessorPrivate::initBlockProcessors(
    const QStringList &leafNodes, bool recreate)
{
    const QStringList updatedOrderedBlockIds =
        blockTree->orderedBlockIds(leafNodes);

    // Setup the new/updated block processors, may throw
    updatedBlockProcessors.clear();
    // merge the existing blocks into the new blocks, the unused blocks we will
    // remove later
    QStringList newBlockProcessorIds;
    QList<BlockProcessor *> newBlockProcessors;
    newProcessors = processors;
    // no Q_FOREACH to make clear that order is important here
    for (unsigned i = 0; i < unsigned(updatedOrderedBlockIds.size()); ++i) {
        const QString blockId = updatedOrderedBlockIds[i];
        if (newProcessors.contains(blockId) && !recreate) {
            updatedBlockProcessors.append(
                static_cast<BlockProcessor *>(processor(blockId)));
        } else {
            BlockProcessor *newProcessor = new BlockProcessor(
                experiment.block(blockId), experiment.device().blockSize(),
                experiment.device().sampleRate());
            newProcessors.insert(
                blockId, QSharedPointer<AbstractProcessor>(newProcessor));
            newBlockProcessors.append(newProcessor);
            newBlockProcessorIds.append(blockId);
            updatedBlockProcessors.append(newProcessor);

            Q_FOREACH (const Parameter &par, newParameters.value(blockId)) {
                const unsigned parameter =
                    newProcessor->parameterIndex(par.first);
                const QVariant preparedValue =
                    newProcessor->prepareParameterValue(parameter, par.second);
                newProcessor->setParameterValue(parameter, preparedValue);
            }
            newParameters.remove(blockId);
        }
    }

    // Determine now unused block processors
    // if we're recreating, the unused one will just be the old processor map
    QSet<QString> unusedBlockProcessorIds =
        newProcessors.keys().toSet() - updatedOrderedBlockIds.toSet();
    unusedBlockProcessorIds.remove(experiment.device().id());
    unusedBlockProcessors.clear();
    Q_FOREACH (const QString &blockId, unusedBlockProcessorIds) {
        unusedBlockProcessors.append(
            static_cast<BlockProcessor *>(processor(blockId)));
    }

    // Connect the inputs of the new blocks to existing outputs directly, might
    // throw
    Q_FOREACH (const QString &blockId, newBlockProcessorIds) {
        SourceConnectionsIter it(blockTree->sourceConnections(blockId));
        const AbstractProcessor *target = processor(blockId, newProcessors);
        while (it.hasNext()) {
            it.next();
            const QString targetPort = it.key();
            BlockInputPort *inputPort = target->findInputPort(targetPort);
            AbstractProcessor *source;
            try {
                source = processor(it.value().first, newProcessors);
            } catch (...) {
                // ignore if the source block cannot be found
                continue;
            }
            const QString sourcePort = it.value().second;
            BlockOutputPort *outputPort = source->findOutputPort(sourcePort);
            checkPortCompatibility(source, outputPort, target, inputPort);
            inputPort->inputData = outputPort->outputData;
        }
    }

    // Prepare the connection of outputs of the new blocks to preexisting
    // blocks, might throw
    // on recreate this is needed for new blocks to device
    newConnections.clear();
    Q_FOREACH (const QString &blockId, newProcessors.keys()) {
        const AbstractProcessor *source = processor(blockId, newProcessors);
        TargetConnectionsIter it(blockTree->targetConnections(blockId));
        while (it.hasNext()) {
            it.next();
            const QString sourcePort = it.key();
            BlockOutputPort *outputPort = source->findOutputPort(sourcePort);
            Q_FOREACH (const auto &targetConnection, it.value()) {
                if (recreate &&
                    targetConnection.first != experiment.device().id())
                    continue;
                AbstractProcessor *target;
                try {
                    // only look for preexisting processors
                    target = processor(targetConnection.first);
                } catch (...) {
                    // ignore if the target block cannot be found
                    continue;
                }
                const QString targetPort = targetConnection.second;
                BlockInputPort *inputPort = target->findInputPort(targetPort);
                checkPortCompatibility(source, outputPort, target, inputPort);
                newConnections.append(qMakePair(outputPort, inputPort));
            }
        }
    }

    // Prepare disconnection of anything that was reading from the unused blocks
    // on recreate this is is only relevant when it involves the device
    unusedConnections.clear();
    Q_FOREACH (const QString &blockId, unusedBlockProcessorIds) {
        Q_FOREACH (const auto &targetConnections,
                   blockTree->targetConnections(blockId)) {
            Q_FOREACH (const auto &targetConnection, targetConnections) {
                // ignore targets that are also unused
                if (unusedBlockProcessorIds.contains(targetConnection.first))
                    continue;
                const AbstractProcessor *target =
                    processor(targetConnection.first);
                const QString targetPort = targetConnection.second;
                BlockInputPort *inputPort = target->findInputPort(targetPort);
                unusedConnections.append(inputPort);
            }
        }
    }

    // if experiment is running we need to prepare the blocks now, might throw
    if (state == ExperimentProcessor::ExperimentRunning ||
        state == ExperimentProcessor::ExperimentPrepared) {
        unsigned i = 0;
        try {
            for (i = 0; i < unsigned(newBlockProcessors.size()); ++i)
                newBlockProcessors[i]->prepare(0);
        } catch (...) {
            for (unsigned j = 0; j < i; ++j)
                newBlockProcessors[j]->release();
            throw;
        }
    }

    Q_FOREACH (const QString &blockId, unusedBlockProcessorIds)
        newProcessors.remove(blockId);
}

void ExperimentProcessorPrivate::connectAndSwapBlockProcessors()
{
    // disconnect
    Q_FOREACH (BlockInputPort *blockPort, unusedConnections)
        blockPort->inputData.clear();
    // connect
    Q_FOREACH (const BlockPortPair &blockPortPair, newConnections)
        blockPortPair.second->inputData = blockPortPair.first->outputData;
    // use updated processor list
    currentBlockProcessors.swap(updatedBlockProcessors);
    processors.swap(newProcessors);

    // set any parameters we might have
    Q_FOREACH (const QString &blockId, newParameters.keys()) {
        AbstractProcessor *blockProcessor = processor(blockId);
        Q_FOREACH (const Parameter &par, newParameters[blockId]) {
            const unsigned parameter =
                blockProcessor->parameterIndex(par.first);
            const QVariant preparedValue =
                blockProcessor->prepareParameterValue(parameter, par.second);
            blockProcessor->setParameterValue(parameter, preparedValue);
        }
    }
}

void ExperimentProcessorPrivate::deinitBlockProcessors()
{
    Q_FOREACH (BlockProcessor *blockProcessor, unusedBlockProcessors)
        blockProcessor->release();

    unusedBlockProcessors.clear();
    newProcessors.clear();
    newParameters.clear();
}

AbstractProcessor *
ExperimentProcessorPrivate::processor(const QString &blockId,
                                      const ProcessorMap &other)
{
    AbstractProcessor *result = other.value(blockId).data();
    if (!result)
        result = processors.value(blockId).data();
    if (!result)
        throw Exception(tr("Unable to find block %1").arg(blockId));
    return result;
}

void ExperimentProcessorPrivate::callback()
{
    // This can happen if the soundcard is slow to stop the callback
    if (state != ExperimentProcessor::ExperimentRunning)
        return;

    bool continueProcessing = true;

    if (!deviceProcessor->processOutput())
        continueProcessing = false;

    // no Q_FOREACH to make clear that order is important here
    for (unsigned i = 0; i < unsigned(currentBlockProcessors.size()); ++i)
        currentBlockProcessors[i]->process();

    if (!deviceProcessor->processInput())
        continueProcessing = false;

    while (!queuedParameters.isEmpty()) {
        QueuedParameter parameter(queuedParameters.pop());
        QSharedPointer<AbstractProcessor> processor(parameter.processor);
        if (processor)
            processor->setParameterValue(parameter.index, parameter.value);
    }

    if (swapBlockProcessorsSemaphore.tryAcquire()) {
        connectAndSwapBlockProcessors();
        pauseMainThreadSemaphore.release();
    }

    if (!continueProcessing)
        p->stop();

    currentBuffer.fetchAndAddRelaxed(1);
}

void ExperimentProcessorPrivate::setState(ExperimentProcessor::State newState)
{
    state = newState;
    // this may be called in another thread
    QMetaObject::invokeMethod(
        p, "stateChanged", Qt::AutoConnection,
        Q_ARG(bertha::ExperimentProcessor::State, newState));
}

QVector<QueuedParameter> ExperimentProcessorPrivate::prepareParameters(
    const QMap<QString, QList<Parameter>> &parameters)
{
    QVector<QueuedParameter> queuedParameters;
    Q_FOREACH (const QString &blockId, parameters.keys()) {
        AbstractProcessor *blockProcessor = processor(blockId);
        Q_FOREACH (const Parameter &par, parameters[blockId]) {
            const unsigned parameter =
                blockProcessor->parameterIndex(par.first);
            const QVariant preparedValue =
                blockProcessor->prepareParameterValue(parameter, par.second);
            queuedParameters.push_back(
                QueuedParameter(processors.value(blockId).toWeakRef(),
                                parameter, preparedValue));
        }
    }
    return queuedParameters;
}

// ExperimentProcessor =========================================================

// may throw
ExperimentProcessor::ExperimentProcessor(const ExperimentData &experiment,
                                         bool lazy)
    : d(new ExperimentProcessorPrivate)
{
    qRegisterMetaType<State>();

    d->p = this;
    d->experiment = experiment;
    d->state = ExperimentLoaded;

    d->blockTree.reset(new BlockTree(experiment));
    d->initDeviceProcessor();
    if (!lazy)
        setActiveLeafNodes(d->blockTree->leafNodes());
}

ExperimentProcessor::~ExperimentProcessor()
{
    release();
    delete d;
}

void ExperimentProcessor::setActiveLeafNodes(
    const QStringList &leafNodes,
    const QMap<QString, QList<QPair<QString, QVariant>>> &parameters,
    bool recreate)
{
    // prepare
    d->newParameters = parameters;
    d->initBlockProcessors(leafNodes, recreate);

    // remove any parameters from blocks that are to be disabled
    Q_FOREACH (const QString &blockId, d->newParameters.keys().toSet() -
                                           d->newProcessors.keys().toSet())
        d->newParameters.remove(blockId);

    if (d->state == ExperimentProcessor::ExperimentRunning) {
        // blocks are prepared, signal the callback thread it can disconnect the
        // old blocks, connect the new ones, and swap the new blocks with
        // current blocks
        d->swapBlockProcessorsSemaphore.release();
        // wait until the callback thread is done with the swap before cleanup
        if (!d->pauseMainThreadSemaphore.tryAcquire(1, 1000)) {
            // callback thread is stuck inside the swap
            if (!d->swapBlockProcessorsSemaphore.tryAcquire())
                qFatal("Unable to recover from reinitialization, stuck in "
                       "callback inside "
                       "the swap block");
            else
                qFatal("Unable to recover from reinitialization, callback "
                       "never reached the "
                       "swap block");
        }
    } else {
        d->connectAndSwapBlockProcessors();
    }

    // cleanup
    d->deinitBlockProcessors();
}

QStringList ExperimentProcessor::leafNodes() const
{
    return d->blockTree->leafNodes();
}

void ExperimentProcessor::prepare()
{
    if (d->state != ExperimentLoaded)
        return;

    // TODO: duration, needs help from the sound drivers
    d->deviceProcessor->prepare();
    unsigned i = 0;
    try {
        for (i = 0; i < unsigned(d->currentBlockProcessors.size()); ++i)
            d->currentBlockProcessors[i]->prepare(0);
    } catch (...) {
        for (unsigned j = 0; j < i; ++j)
            d->currentBlockProcessors[j]->release();
        d->deviceProcessor->release();
        throw;
    }

    d->setState(ExperimentPrepared);
}

void ExperimentProcessor::start()
{
    if (d->state == ExperimentLoaded)
        prepare();

    if (d->state != ExperimentPrepared)
        return;

    d->deviceProcessor->start();

    d->setState(ExperimentRunning);
}

void ExperimentProcessor::stop()
{
    if (d->state != ExperimentRunning)
        return;

    d->deviceProcessor->stop();

    d->setState(ExperimentPrepared);
}

void ExperimentProcessor::release()
{
    if (d->state == ExperimentRunning)
        stop();

    if (d->state != ExperimentPrepared)
        return;

    Q_FOREACH (BlockProcessor *const processor, d->currentBlockProcessors)
        processor->release();
    d->deviceProcessor->release();

    d->setState(ExperimentLoaded);
}

void ExperimentProcessor::setParameterValue(const QString &blockId,
                                            const QString &parameterId,
                                            const QVariant &value)
{
    QMap<QString, QList<QPair<QString, QVariant>>> parameters;
    parameters[blockId].append(qMakePair(parameterId, value));
    setParameterList(parameters);
}

void ExperimentProcessor::setParameterList(
    const QMap<QString, QMap<QString, QVariant>> &parameters)
{
    QMap<QString, QList<Parameter>> parameterList;
    Q_FOREACH (const QString &blockId, parameters.keys()) {
        Q_FOREACH (const QString &par, parameters[blockId].keys()) {
            parameterList[blockId].append(
                qMakePair(par, parameters[blockId][par]));
        }
    }
    setParameterList(parameterList);
}

void ExperimentProcessor::setParameterList(
    const QMap<QString, QList<QPair<QString, QVariant>>> &parameters)
{
    if (!d->queuedParameters.pushList(d->prepareParameters(parameters)))
        throw Exception(tr("Unable to queue parameters"));

    if (d->state != ExperimentProcessor::ExperimentRunning) {
        while (!d->queuedParameters.isEmpty()) {
            QueuedParameter parameter(d->queuedParameters.pop());
            QSharedPointer<AbstractProcessor> processor(parameter.processor);
            if (processor)
                processor->setParameterValue(parameter.index, parameter.value);
        }
    }
}

QVariant ExperimentProcessor::parameterValue(const QString &blockId,
                                             const QString &parameterId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    const unsigned parameter = processor->parameterIndex(parameterId);

    if (d->state == ExperimentRunning) {
        // TODO: this should happen in the processing thread
        return processor->parameterValue(parameter);
    } else {
        return processor->parameterValue(parameter);
    }
}

QVariant
ExperimentProcessor::sanitizedParameterValue(const QString &blockId,
                                             const QString &parameterId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    const unsigned parameter = processor->parameterIndex(parameterId);

    if (d->state == ExperimentRunning) {
        // TODO: this should happen in the processing thread
        return processor->sanitizedParameterValue(parameter);
    } else {
        return processor->sanitizedParameterValue(parameter);
    }
}

QString
ExperimentProcessor::parameterStringValue(const QString &blockId,
                                          const QString &parameterId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    const unsigned parameter = processor->parameterIndex(parameterId);

    if (d->state == ExperimentRunning) {
        // TODO: this should happen in the processing thread
        return processor->parameterStringValue(parameter);
    } else {
        return processor->parameterStringValue(parameter);
    }
}

BlockParameter
ExperimentProcessor::parameterInfo(const QString &blockId,
                                   const QString &parameterId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    const unsigned parameter = processor->parameterIndex(parameterId);

    if (d->state == ExperimentRunning) {
        // TODO: this should happen in the processing thread
        return processor->parameterInfo(parameter);
    } else {
        return processor->parameterInfo(parameter);
    }
}

QString ExperimentProcessor::parameterType(const QString &blockId,
                                           const QString &parameterId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    const unsigned parameter = processor->parameterIndex(parameterId);

    if (d->state == ExperimentRunning) {
        // TODO: this should happen in the processing thread
        return processor->parameterType(parameter);
    } else {
        return processor->parameterType(parameter);
    }
}

QStringList
ExperimentProcessor::enumeratorTypes(const QString &blockId,
                                     const QString &parameterId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    const unsigned parameter = processor->parameterIndex(parameterId);

    if (d->state == ExperimentRunning) {
        // TODO: this should happen in the processing thread
        return processor->enumeratorTypes(parameter);
    } else {
        return processor->enumeratorTypes(parameter);
    }
}

QStringList ExperimentProcessor::blockNames() const
{
    return d->processors.keys();
}

QStringList ExperimentProcessor::parameterNames(const QString &blockId) const
{
    AbstractProcessor *processor = d->processor(blockId);

    return processor->parameters();
}

ExperimentProcessor::State ExperimentProcessor::state() const
{
    return d->state;
}

int ExperimentProcessor::currentBuffer() const
{
    return d->currentBuffer.load();
}

QStringList ExperimentProcessor::dumpActiveConnections() const
{
    QStringList result;
    QMapIterator<QString, QSharedPointer<AbstractProcessor>> processorsIt(
        d->processors);
    // data, blockname, portname
    QMap<PortDataType *, QPair<QString, QString>> portsByInputData;
    QMap<PortDataType *, QPair<QString, QString>> portsByOutputData;
    while (processorsIt.hasNext()) {
        processorsIt.next();
        const QString &blockId = processorsIt.key();
        QStringList inputPorts =
            d->blockTree->sourceConnections(blockId).keys();
        Q_FOREACH (const QString &inputPort, inputPorts) {
            BlockInputPort *blockPort =
                processorsIt.value()->findInputPort(inputPort);
            if (!blockPort->inputData.isNull())
                portsByInputData.insert(blockPort->inputData.data(),
                                        qMakePair(blockId, inputPort));
        }
        QStringList outputPorts =
            d->blockTree->targetConnections(blockId).keys();
        Q_FOREACH (const QString &outputPort, outputPorts) {
            BlockOutputPort *blockPort =
                processorsIt.value()->findOutputPort(outputPort);
            if (!blockPort->outputData.isNull())
                portsByOutputData.insert(blockPort->outputData.data(),
                                         qMakePair(blockId, outputPort));
        }
    }

    QMapIterator<PortDataType *, QPair<QString, QString>> inputIt(
        portsByInputData);
    while (inputIt.hasNext()) {
        inputIt.next();
        if (portsByOutputData.contains(inputIt.key())) {
            QPair<QString, QString> output =
                portsByOutputData.value(inputIt.key());
            result << output.first + QL1S(":") + output.second + QL1S(" ") +
                          inputIt.value().first + QL1S(":") +
                          inputIt.value().second;
        }
    }
    return result;
}

}; // namespace bertha

#include "experimentprocessor.moc"
