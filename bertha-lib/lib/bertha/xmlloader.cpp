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
#include "common/paths.h"

#include "xmlloader.h"
#include "xmlparser.h"

#include <QtTest/QTest>

#include <QDir>
#include <QDomNode>

#include <QtDebug>

using namespace cmn;

namespace bertha
{

class XmlLoaderPrivate
{
public:
    ExperimentData load(const QString &filePath, const QByteArray &contents);

private:
    void handleBlocks(const QDomElement &element);
    void handleConnections(const QDomElement &element);
    void handleDevice(const QDomElement &element);

    QString file;
    ExperimentData experiment;
    XmlParser *parser;
};

ExperimentData XmlLoaderPrivate::load(const QString &filePath,
                                      const QByteArray &contents)
{
    experiment = ExperimentData();

    const QString schemaFile = QLatin1String("schema/experiment.xsd");
    const QString schemaPath =
        Paths::searchFile(schemaFile, Paths::dataDirectories());
    if (schemaPath.isEmpty())
        throw Exception(
            XmlLoader::tr("Unable to find schema file %1").arg(schemaFile));

    XmlParser parser(schemaPath);
    this->parser = &parser;

    const QDomDocument document =
        !filePath.isEmpty() ? parser.parse(filePath) : parser.parse(contents);

    const QDomElement rootNode = document.documentElement();

    for (QDomNode currentNode = rootNode.firstChild(); !currentNode.isNull();
         currentNode = currentNode.nextSibling()) {
        const QDomElement currentElement = currentNode.toElement();
        if (currentElement.isNull())
            continue;

        QString nodeName = currentNode.nodeName();
        if (nodeName == QLatin1String("blocks"))
            handleBlocks(currentElement);
        else if (nodeName == QLatin1String("connections"))
            handleConnections(currentElement);
        else if (nodeName == QLatin1String("device"))
            handleDevice(currentElement);
    }

    if (!filePath.isEmpty()) {
        experiment.setDocumentDirectory(QFileInfo(filePath).absolutePath());
        // TODO: can we solve this differently?
        QDir::setCurrent(QFileInfo(filePath).absolutePath());
    }

    return experiment;
}

// XmlLoader ===================================================================

XmlLoader::XmlLoader() : d(new XmlLoaderPrivate)
{
}

XmlLoader::~XmlLoader()
{
    delete d;
}

ExperimentData XmlLoader::loadContents(const QByteArray &contents)
{
    return d->load(QString(), contents);
}

ExperimentData XmlLoader::loadFile(const QString &filePath)
{
    return d->load(filePath, QByteArray());
}

void XmlLoaderPrivate::handleBlocks(const QDomElement &element)
{
    for (QDomNode currentNode = element.firstChild(); !currentNode.isNull();
         currentNode = currentNode.nextSibling()) {

        const QDomElement currentElement = currentNode.toElement();
        if (currentElement.isNull())
            continue;

        BlockData block;

        const QString blockId =
            parser->attribute(currentElement, QLatin1String("id"));
        Q_ASSERT(!blockId.isEmpty());
        block.setId(blockId);

        for (QDomNode subNode = currentNode.firstChild(); !subNode.isNull();
             subNode = subNode.nextSibling()) {
            QString nodeName = subNode.nodeName();

            const QDomElement subElement = subNode.toElement();
            if (subElement.isNull())
                continue;

            if (nodeName == QLatin1String("plugin")) {
                const QString plugin = subElement.text();
                Q_ASSERT(!plugin.isEmpty());
                block.setPlugin(plugin);
            } else if (nodeName == QLatin1String("outputports")) {
                const QString outputPorts = subElement.text();
                Q_ASSERT(!outputPorts.isEmpty());
                block.setOutputPorts(outputPorts.toInt());
            } else if (nodeName == QLatin1String("inputports")) {
                const QString inputPorts = subElement.text();
                Q_ASSERT(!inputPorts.isEmpty());
                block.setInputPorts(inputPorts.toInt());
            } else if (nodeName == QLatin1String("parameter")) {
                const QString parameterName =
                    parser->attribute(subElement, QLatin1String(("name")));
                const QString parameterValue = subElement.text();
                Q_ASSERT(!parameterName.isEmpty());
                Q_ASSERT(!parameterValue.isEmpty());
                block.setParameter(parameterName, parameterValue);
            } else {
                block.setParameter(nodeName, subElement.text());
            }
        }
        experiment.addBlock(block);
    }
}

void XmlLoaderPrivate::handleConnections(const QDomElement &element)
{
    for (QDomNode currentNode = element.firstChild(); !currentNode.isNull();
         currentNode = currentNode.nextSibling()) {

        const QDomElement currentElement = currentNode.toElement();
        if (currentElement.isNull())
            continue;

        ConnectionData connection;

        for (QDomNode subNode = currentElement.firstChild(); !subNode.isNull();
             subNode = subNode.nextSibling()) {
            QString nodeName = subNode.nodeName();

            const QDomElement subElement = subNode.toElement();
            Q_ASSERT(!subNode.isNull());

            if (nodeName == QLatin1String("from")) {
                const QString fromId =
                    parser->attribute(subElement, QLatin1String("id"));
                const QString fromPort =
                    parser->attribute(subElement, QLatin1String("port"));
                Q_ASSERT(!fromId.isEmpty());
                Q_ASSERT(!fromPort.isEmpty());
                connection.setSourceBlock(fromId);
                connection.setSourcePort(fromPort);
            } else if (nodeName == QLatin1String("to")) {
                const QString toId =
                    parser->attribute(subElement, QLatin1String("id"));
                const QString toPort =
                    parser->attribute(subElement, QLatin1String("port"));
                Q_ASSERT(!toId.isEmpty());
                Q_ASSERT(!toPort.isEmpty());
                connection.setTargetBlock(toId);
                connection.setTargetPort(toPort);
            }
        }
        experiment.addConnection(connection);
    }
}

void XmlLoaderPrivate::handleDevice(const QDomElement &element)
{
    DeviceData device;

    const QString deviceId = parser->attribute(element, QLatin1String("id"));
    Q_ASSERT(!deviceId.isEmpty());
    device.setId(deviceId);

    for (QDomNode currentNode = element.firstChild(); !currentNode.isNull();
         currentNode = currentNode.nextSibling()) {
        QString nodeName = currentNode.nodeName();

        const QDomElement currentElement = currentNode.toElement();
        if (currentElement.isNull())
            continue;

        if (nodeName == QLatin1String("driver")) {
            const QString driver = currentElement.text();
            Q_ASSERT(!driver.isEmpty());
            device.setPlugin(driver);
        } else if (nodeName == QLatin1String("samplerate")) {
            const QString sampleRate = currentElement.text();
            Q_ASSERT(!sampleRate.isEmpty());
            device.setSampleRate(sampleRate.toUInt());
        } else if (nodeName == QLatin1String("blocksize")) {
            const QString blockSize = currentElement.text();
            Q_ASSERT(!blockSize.isEmpty());
            device.setBlockSize(blockSize.toUInt());
        } else if (nodeName == QLatin1String("captureports")) {
            const QString outputPorts = currentElement.text();
            Q_ASSERT(!outputPorts.isEmpty());
            device.setOutputPorts(outputPorts.toInt());
        } else if (nodeName == QLatin1String("playbackports")) {
            const QString inputPorts = currentElement.text();
            Q_ASSERT(!inputPorts.isEmpty());
            device.setInputPorts(inputPorts.toInt());
        } else if (nodeName == QLatin1String("parameter")) {
            const QString parameterName =
                parser->attribute(currentElement, QLatin1String(("name")));
            const QString parameterValue = currentElement.text();
            Q_ASSERT(!parameterName.isEmpty());
            Q_ASSERT(!parameterValue.isEmpty());
            device.setParameter(parameterName, parameterValue);
        } else {
            device.setParameter(nodeName, currentElement.text());
        }
    }

    experiment.setDevice(device);
}

}; // namespace bertha
