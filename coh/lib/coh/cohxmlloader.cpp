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
#include "common/xmlutils.h"

#include "coh.h"
#include "cohxmlloader.h"

#include <QBuffer>
#include <QCoreApplication>

#include <memory>

using namespace cmn;

namespace coh
{

class CohXmlHandler
{
    Q_DECLARE_TR_FUNCTIONS(CohXmlHandler)
public:
    CohXmlHandler();

    typedef CohSequence *Result;
    Result result();

    void parse(QXmlStreamReader *xml);

    void readCiSequence();
    void readSequence(CohSequence *parent);
    void readStimulus(CohSequence *parent);
    void readMeta(CohSequence *parent);
    void readParameter(QMap<QString, QString> *parameters);

private:
    QXmlStreamReader *xml;

    QScopedPointer<CohSequence> data;
};

// CohXmlHandler ===============================================================

CohXmlHandler::CohXmlHandler() : xml(Q_NULLPTR)
{
}

CohSequence *CohXmlHandler::result()
{
    return data.take();
}

void CohXmlHandler::parse(QXmlStreamReader *xml)
{
    data.reset();
    this->xml = xml;
    if (xml->readNextStartElement()) {
        if (xml->name() == QL1S("cisequence") &&
            xml->attributes().value(QL1S("version")) == QL1S("1.0"))
            readCiSequence();
        else
            xml->raiseError(
                tr("The file is not an cisequence version 1.0 file."));
    }
}

void CohXmlHandler::readCiSequence()
{
    while (xml->readNextStartElement()) {
        if (xml->name() == QL1S("sequence")) {
            readSequence(Q_NULLPTR);
        } else {
            xml->skipCurrentElement();
        }
    }
}

void CohXmlHandler::readSequence(CohSequence *parent)
{
    QString repeats;
    XML_CHECK_OPTIONAL_ATTRIBUTE(repeats, "1");

    CohSequence *sequence = new CohSequence(repeats.toUInt());
    if (!parent)
        data.reset(sequence);
    else
        parent->append(sequence);

    while (xml->readNextStartElement()) {
        if (xml->name() == QL1S("sequence")) {
            readSequence(sequence);
        } else if (xml->name() == QL1S("stimulus")) {
            readStimulus(sequence);
        } else if (xml->name() == QL1S("metadata")) {
            readMeta(sequence);
        } else {
            xml->skipCurrentElement();
        }
    }
}

void CohXmlHandler::readMeta(CohSequence *parent)
{
    QString key, value, type;
    while (xml->readNextStartElement()) {
        if (xml->name() == QL1S("key")) {
            XML_CHECK_ELEMENT_NONEMPTY(key);
        } else if (xml->name() == QL1S("value")) {
            XML_CHECK_ATTRIBUTE(type);
            XML_CHECK_ELEMENT(value);
        } else {
            xml->skipCurrentElement();
        }
    }

    XML_CHECK_NOERROR;
    if (key.isEmpty() || value.isEmpty() || type.isEmpty()) {
        xml->raiseError(tr("Incomplete meta element."));
        return;
    }
    parent->append(new CohMetaData(key, type, value));
}

void CohXmlHandler::readStimulus(CohSequence *parent)
{
    QString type;
    XML_CHECK_ATTRIBUTE(type);

    QMap<QString, QString> parameters;
    while (xml->readNextStartElement()) {
        if (xml->name() == QL1S("parameter")) {
            readParameter(&parameters);
        } else {
            xml->skipCurrentElement();
        }
    }

    XML_CHECK_NOERROR;
    QString trigger = parameters.value(QSL("trigger"));
    QString period = parameters.value(QSL("period"));
    if (type == QL1S("null")) {
        if (trigger.isEmpty() || period.isEmpty()) {
            xml->raiseError(tr("Incomplete null stimulus."));
            return;
        }
        parent->append(
            new CohNullStimulus(period.toDouble(), QVariant(trigger).toBool()));
    } else if (type == QL1S("biphasic")) {
        QString active = parameters.value(QSL("active"));
        QString reference = parameters.value(QSL("reference"));
        QString currentLevel = parameters.value(QSL("currentlevel"));
        QString phaseWidth = parameters.value(QSL("phasewidth"));
        QString phaseGap = parameters.value(QSL("phasegap"));
        if (trigger.isEmpty() || period.isEmpty() || active.isEmpty() ||
            reference.isEmpty() || currentLevel.isEmpty() ||
            phaseWidth.isEmpty() || phaseGap.isEmpty()) {
            xml->raiseError(tr("Incomplete biphasic stimulus."));
            return;
        }
        parent->append(new CohBiphasicStimulus(
            Coh::Electrode(active.toInt()), Coh::Electrode(reference.toInt()),
            currentLevel.toInt(), phaseWidth.toDouble(), phaseGap.toDouble(),
            period.toDouble(), QVariant(trigger).toBool()));
    } else if (type == QL1S("codacs")) {
        QString amplitude = parameters.value(QSL("amplitude"));
        if (trigger.isEmpty() || period.isEmpty() || amplitude.isEmpty()) {
            xml->raiseError(tr("Incomplete codacs stimulus."));
            return;
        }
        parent->append(new CohCodacsStimulus(
            amplitude.toInt(), period.toDouble(), QVariant(trigger).toBool()));
    }
}

void CohXmlHandler::readParameter(QMap<QString, QString> *parameters)
{
    QString name, parameter;
    XML_CHECK_ATTRIBUTE(name);
    XML_CHECK_ELEMENT_NONEMPTY(parameter);
    if (parameters->contains(name)) {
        xml->raiseError(
            tr("Parameter '%1' provided multiple times.").arg(name));
        return;
    }
    parameters->insert(name, parameter);
}

// Various functions ===========================================================

CohSequence *loadCohSequenceXml(const QByteArray &contents)
{
    return loadXml(contents, CohXmlHandler());
}

} // namespace coh
