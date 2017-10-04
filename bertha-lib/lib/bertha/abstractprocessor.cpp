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

#include "abstractprocessor.h"
#include "stringutils.h"

#include <fftw3.h>

using namespace cmn;

namespace bertha
{

// AbstractProcessor ===========================================================

const char *AbstractProcessor::classInfoValue(const QMetaProperty &property,
                                              const char *what)
{
    int index = pluginMetaObject->indexOfClassInfo(property.name() +
                                                   QByteArray("-") + what);
    return index > -1 ? pluginMetaObject->classInfo(index).value() : NULL;
}

void AbstractProcessor::setupOutputBuffers(
    const QList<BlockOutputPort *> &ports)
{
    Q_FOREACH (BlockOutputPort *const port, ports) {
        unsigned portBlockSize = port->blockSize ? port->blockSize : blockSize;
        if (port->domain == BlockPort::FrequencyDomain)
            portBlockSize = (portBlockSize / 2 + 1) * 2;
        QSharedPointer<PortDataType> data(
            reinterpret_cast<PortDataType *>(
                fftwf_malloc(sizeof(PortDataType) * portBlockSize)),
            fftwf_free);
        memset(data.data(), 0, sizeof(PortDataType) * portBlockSize);
        port->outputData = data;
    }
}

QVariantMap AbstractProcessor::prepareParameters(const QVariantMap &parameters)
{
    RETURN_VAL_IF_FAIL(pluginMetaObject, QVariantMap());

    QMap<QString, QString> caseMap;

    for (unsigned i = pluginMetaObject->propertyOffset();
         i < unsigned(pluginMetaObject->propertyCount()); ++i) {
        QMetaProperty property = pluginMetaObject->property(i);
        const QString propertyName(QLatin1String(property.name()));
        parameterProperties.append(property);
        BlockParameter parameter;
        if (const char *value = classInfoValue(property, "description"))
            parameter.description = tr(value);
        if (const char *value = classInfoValue(property, "unit"))
            parameter.unit = value == QByteArray("linear")
                                 ? BlockParameter::LinearUnit
                                 : BlockParameter::LogarithmicUnit;
        if (const char *value = classInfoValue(property, "minimum"))
            parameter.minimum = QByteArray(value).toDouble();
        if (const char *value = classInfoValue(property, "maximum"))
            parameter.maximum = QByteArray(value).toDouble();
        parameterInfos.append(parameter);
        parameterNames.append(propertyName);
        caseMap.insert(propertyName.toLower(), propertyName);
    }

    QVariantMap result;
    QMapIterator<QString, QVariant> j(parameters);
    while (j.hasNext()) {
        j.next();
        const QString parameterName = j.key().toLower();
        if (!caseMap.contains(parameterName))
            throw Exception(AbstractProcessor::tr("Unable to find parameter %1")
                                .arg(parameterName));
        result.insert(caseMap.value(parameterName), j.value());
    }
    return result;
}

QVariantMap AbstractProcessor::readOnlyParameters(const QVariantMap &parameters)
{
    QVariantMap result;
    QMapIterator<QString, QVariant> j(parameters);
    while (j.hasNext()) {
        j.next();
        const unsigned index = parameterIndex(j.key());
        if (!parameterProperties[index].isWritable())
            result.insert(j.key(), j.value());
    }
    return result;
}

void AbstractProcessor::setupParameters(const QList<BlockParameter> &parameters,
                                        const QVariantMap &initialParameters)
{
    Q_FOREACH (const BlockParameter &parameter, parameters)
        parameterInfos[parameterIndex(parameter.name)] = parameter;

    QMapIterator<QString, QVariant> j(initialParameters);
    while (j.hasNext()) {
        j.next();
        const unsigned index = parameterIndex(j.key());
        if (parameterProperties[index].isWritable())
            setParameterValue(index, prepareParameterValue(index, j.value()));
    }
}

QVariant AbstractProcessor::prepareParameterValue(unsigned index,
                                                  const QVariant &value) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterNames.count()), QVariant());
    const int vectorId = qMetaTypeId<QVector<double>>();
    const int doubleVectorId = qMetaTypeId<QVector<QVector<double>>>();

    if (parameterProperties[index].isEnumType())
        return checkParameterConstraints(
            index, stringToEnumValue(value.toString(),
                                     parameterProperties[index].enumerator()));

    switch (parameterProperties[index].userType()) {
    case QVariant::Int:
        return checkParameterConstraints(index, value.toInt());
    case QVariant::Double:
        return checkParameterConstraints(index, value.toDouble());
    default: {
    } // do nothing, handled beneath
    }
    // TODO: move the rest into the switch
    if (parameterProperties[index].userType() == QMetaType::Float) {
        if (value.type() == QVariant::String)
            return checkParameterConstraints(
                index, QVariant(value.toString().toDouble()));
        else if (value.userType() == QMetaType::Float)
            return checkParameterConstraints(
                index, QVariant(double(value.value<float>())));
    } else if (parameterProperties[index].userType() == QVariant::StringList) {
        if (value.type() == QVariant::String)
            return checkParameterConstraints(
                index, QVariant(stringToStringList(value.toString())));
        else if (value.type() == QVariant::List) {
            return checkParameterConstraints(index,
                                             QVariant(value.toStringList()));
        }
    } else if (parameterProperties[index].userType() == QVariant::String) {
        if (value.type() == QVariant::StringList)
            return checkParameterConstraints(
                index, QVariant(stringListToString(value.toStringList())));
        else if (value.userType() == vectorId)
            return checkParameterConstraints(
                index,
                QVariant(vectorToString(value.value<QVector<double>>())));
        else if (value.userType() == doubleVectorId)
            return checkParameterConstraints(
                index, QVariant(doubleVectorToString(
                           value.value<QVector<QVector<double>>>())));
    } else if (parameterProperties[index].userType() == vectorId) {
        if (value.type() == QVariant::String) {
            QVector<double> resultValue = stringToVector(value.toString());
            return checkParameterConstraints(index,
                                             QVariant(vectorId, &resultValue));
        } else if (value.type() == QVariant::List) {
            QVector<double> resultValue;
            QList<QVariant> listValues = value.toList();
            for (unsigned i = 0; i < unsigned(listValues.size()); ++i)
                resultValue.append(listValues[i].toDouble());
            return checkParameterConstraints(index,
                                             QVariant(vectorId, &resultValue));
        }
    } else if (parameterProperties[index].userType() == doubleVectorId) {
        if (value.type() == QVariant::String) {
            QVector<QVector<double>> resultValue =
                stringToDoubleVector(value.toString());
            return checkParameterConstraints(
                index, QVariant(doubleVectorId, &resultValue));
        } else if (value.type() == QVariant::List) {
            QVector<QVector<double>> resultValue;
            QList<QVariant> listValues = value.toList();
            bool isSingleVector = true;

            for (unsigned i = 0;
                 (i < unsigned(listValues.size())) && isSingleVector; ++i)
                isSingleVector = listValues[i].canConvert<double>();

            if (isSingleVector) {
                QVector<double> singleVector;
                for (unsigned i = 0; i < unsigned(listValues.size()); ++i)
                    singleVector.append(listValues[i].toDouble());
                resultValue.append(singleVector);
            } else {
                QVector<double> vecItem;
                for (unsigned i = 0; i < unsigned(listValues.size()); ++i) {
                    if (listValues[i].type() == QVariant::List) {
                        for (unsigned j = 0;
                             j < unsigned(listValues[i].toList().size()); ++j)
                            vecItem.append(
                                listValues[i].toList()[j].toDouble());
                        resultValue.append(vecItem);
                        vecItem.clear();
                    } else {
                        vecItem.append(listValues[i].toDouble());
                        resultValue.append(vecItem);
                        vecItem.clear();
                    }
                }
            }
            return checkParameterConstraints(
                index, QVariant(doubleVectorId, &resultValue));
        }
    }

    return checkParameterConstraints(index, value);
}

QVariant
AbstractProcessor::checkParameterConstraints(unsigned index,
                                             const QVariant &value) const
{
    const int vectorId = qMetaTypeId<QVector<double>>();
    const int doubleVectorId = qMetaTypeId<QVector<QVector<double>>>();

    const double min = parameterInfos[index].minimum;
    const double max = parameterInfos[index].maximum;

    if (value.type() == QVariant::Int || value.type() == QVariant::Double ||
        value.userType() == QMetaType::Float) {
        if (value.toDouble() > max)
            return max;
        if (value.toDouble() < min)
            return min;
    } else if (value.userType() == vectorId) {
        bool corrected = false;
        QVector<double> checkValue = value.value<QVector<double>>();
        for (unsigned i = 0; i < unsigned(checkValue.size()); ++i) {
            if (checkValue[i] > max) {
                checkValue[i] = max;
                corrected = true;
            }
            if (checkValue[i] < min) {
                checkValue[i] = min;
                corrected = true;
            }
        }
        if (corrected)
            return QVariant(vectorId, &checkValue);
    } else if (value.userType() == doubleVectorId) {
        bool corrected = false;
        QVector<QVector<double>> checkValue =
            value.value<QVector<QVector<double>>>();
        for (unsigned i = 0; i < unsigned(checkValue.size()); ++i) {
            for (unsigned j = 0; j < unsigned(checkValue[i].size()); ++j) {
                if (checkValue[i][j] > max) {
                    checkValue[i][j] = max;
                    corrected = true;
                }
                if (checkValue[i][j] < min) {
                    checkValue[i][j] = min;
                    corrected = true;
                }
            }
        }
        if (corrected)
            return QVariant(doubleVectorId, &checkValue);
    }

    return value;
}

QVariant AbstractProcessor::sanitizedParameterValue(unsigned index) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterProperties.count()),
                       QVariant());
    const QVariant value = parameterValue(index);

    if (parameterProperties[index].isEnumType())
        return enumValueToKey(parameterProperties[index], value.toInt());

    const int userType = value.userType();
    switch (userType) {
    case QMetaType::Float:
        return double(value.value<float>());
    case QVariant::UInt:
        return int(value.toUInt());
    case QVariant::Bool:
        return value.toBool() ? QLatin1String("true") : QLatin1String("false");
    default: {
    } // do nothing
    }

    const int vectorId = qMetaTypeId<QVector<double>>();
    const int doubleVectorId = qMetaTypeId<QVector<QVector<double>>>();
    if (userType == vectorId) {
        QVariantList varList;
        Q_FOREACH (double number, value.value<QVector<double>>())
            varList.append(number);
        return varList;
    } else if (userType == doubleVectorId) {
        const QVector<QVector<double>> vector =
            value.value<QVector<QVector<double>>>();
        QList<QVariant> doubleVarList;
        Q_FOREACH (const QVector<double> &vector,
                   value.value<QVector<QVector<double>>>()) {
            QVariantList varList;
            Q_FOREACH (double value, vector)
                varList.append(value);
            doubleVarList.append(QVariant(varList));
        }
        return QVariant(doubleVarList);
    }

    return value;
}

QString AbstractProcessor::parameterStringValue(unsigned index) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterProperties.count()),
                       QString());
    const QVariant value = parameterValue(index);

    const int vectorId = qMetaTypeId<QVector<double>>();
    const int doubleVectorId = qMetaTypeId<QVector<QVector<double>>>();

    if (value.userType() == QMetaType::Float) {
        return QString::number(value.value<float>());
    } else if (value.userType() == vectorId) {
        return vectorToString(value.value<QVector<double>>());
    } else if (value.userType() == doubleVectorId) {
        return doubleVectorToString(value.value<QVector<QVector<double>>>());
    } else if (value.type() == QVariant::StringList) {
        return stringListToString(value.toStringList());
    } else if (parameterProperties[index].isEnumType()) {
        return enumValueToKey(parameterProperties[index], value.toInt());
    }

    return value.toString();
}

QVariant AbstractProcessor::parameterValue(unsigned index) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterProperties.count()),
                       QVariant());
    return parameterProperties[index].read(plugin.data());
}

AbstractProcessor::AbstractProcessor(
    const BlockData &data, unsigned blockSize, unsigned sampleRate,
    const QMap<QString, BlockDescription> &plugins)
    : blockId(data.id()), blockSize(blockSize), sampleRate(sampleRate)
{
    qRegisterMetaType<QVector<double>>();
    qRegisterMetaType<QVector<QVector<double>>>();

    if (!plugins.contains(data.plugin()))
        throw cmn::Exception(tr("Unable to instantiate %1: no suitable plugin "
                                "library. Available plugins: %2")
                                 .arg(data.plugin())
                                 .arg(QStringList(plugins.keys())
                                          .join(QString::fromLatin1(","))));

    const BlockDescription block(plugins.value(data.plugin()));
    pluginMetaObject = block.metaObject;
    const QVariantMap parameters = prepareParameters(data.parameters());
    const QVariantMap readOnlyParameters = this->readOnlyParameters(parameters);

    if (block.userData.isValid()) {
        plugin.reset(block.metaObject->newInstance(
            Q_ARG(unsigned, blockSize), Q_ARG(unsigned, sampleRate),
            Q_ARG(int, data.inputPorts()), Q_ARG(int, data.outputPorts()),
            Q_ARG(QVariantMap, readOnlyParameters),
            Q_ARG(QVariant, block.userData)));
        if (!plugin && readOnlyParameters.isEmpty())
            plugin.reset(block.metaObject->newInstance(
                Q_ARG(unsigned, blockSize), Q_ARG(unsigned, sampleRate),
                Q_ARG(int, data.inputPorts()), Q_ARG(int, data.outputPorts()),
                Q_ARG(QVariant, block.userData)));
    } else {
        plugin.reset(block.metaObject->newInstance(
            Q_ARG(unsigned, blockSize), Q_ARG(unsigned, sampleRate),
            Q_ARG(int, data.inputPorts()), Q_ARG(int, data.outputPorts()),
            Q_ARG(QVariantMap, readOnlyParameters)));
        if (!plugin && readOnlyParameters.isEmpty())
            plugin.reset(block.metaObject->newInstance(
                Q_ARG(unsigned, blockSize), Q_ARG(unsigned, sampleRate),
                Q_ARG(int, data.inputPorts()), Q_ARG(int, data.outputPorts())));
    }
    if (!plugin)
        throw cmn::Exception(tr("Unable to instantiate %1:"
                                " no suitable constructor")
                                 .arg(data.plugin()));

    basicBlock = dynamic_cast<BasicBlockInterface *>(plugin.data());

    Q_FOREACH (BlockInputPort *const port, basicBlock->inputPorts())
        inputPorts.insert(port->name, port);
    Q_FOREACH (BlockOutputPort *const port, basicBlock->outputPorts())
        outputPorts.insert(port->name, port);
    setupOutputBuffers(outputPorts.values());
    setupParameters(basicBlock->parameterInfos(), parameters);
}

BlockOutputPort *AbstractProcessor::findOutputPort(const QString &name) const
{
    return findPort(outputPorts, name);
}

BlockInputPort *AbstractProcessor::findInputPort(const QString &name) const
{
    return findPort(inputPorts, name);
}

AbstractProcessor::~AbstractProcessor()
{
}

QString AbstractProcessor::id() const
{
    return blockId;
}

unsigned AbstractProcessor::getBlockSize() const
{
    return blockSize;
}

unsigned AbstractProcessor::getSampleRate() const
{
    return sampleRate;
}

QStringList AbstractProcessor::parameters() const
{
    return parameterNames;
}

unsigned AbstractProcessor::parameterIndex(const QString &parameter) const
{
    const int index = parameterNames.indexOf(parameter);
    if (index < 0)
        throw Exception(tr("Unable to find parameter %1").arg(parameter));
    return index;
}

void AbstractProcessor::setParameterValue(unsigned index, const QVariant &value)
{
    RETURN_IF_FAIL(index < unsigned(parameterProperties.count()));

    parameterProperties[index].write(plugin.data(), value);
}

BlockParameter AbstractProcessor::parameterInfo(unsigned index) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterNames.count()),
                       BlockParameter());

    return parameterInfos[index];
}

QString AbstractProcessor::parameterType(unsigned index) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterNames.count()), QString());

    return QString::fromLatin1(parameterProperties[index].typeName());
}

QStringList AbstractProcessor::enumeratorTypes(unsigned index) const
{
    RETURN_VAL_IF_FAIL(index < unsigned(parameterNames.count()), QStringList());

    if (!parameterProperties[index].isEnumType())
        return QStringList();
    else {
        QStringList keys;
        for (int i = 0; i < parameterProperties[index].enumerator().keyCount();
             ++i)
            keys << QString::fromLatin1(
                parameterProperties[index].enumerator().key(i));
        return keys;
    }
}

} // namespace bertha
