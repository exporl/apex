/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "common/xmlutils.h"

#include "cohxmldumper.h"

#include <QBuffer>

using namespace cmn;

namespace coh
{

class CohXmlDumper : public CohCommandVisitor
{
public:
    CohXmlDumper(const CohSequence *sequence);

    void write(QXmlStreamWriter *xml);

private:
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

    void writeParameter(const QString &name, const QString &value);

    CohSequence *const sequence;
    QXmlStreamWriter *xml;
};

// CohXmlDumper ================================================================

CohXmlDumper::CohXmlDumper(const CohSequence *sequence)
    : sequence(const_cast<CohSequence *>(sequence)), xml(Q_NULLPTR)
{
}

void CohXmlDumper::writeParameter(const QString &name, const QString &value)
{
    xml->writeStartElement(QSL("parameter"));
    xml->writeAttribute(QSL("name"), name);
    xml->writeCharacters(value);
    xml->writeEndElement();
}

void CohXmlDumper::visit(CohMetaData *command)
{
    xml->writeStartElement(QSL("metadata"));
    xml->writeTextElement(QSL("key"), command->key());
    xml->writeStartElement(QSL("value"));
    xml->writeAttribute(QSL("type"), command->valueType());
    xml->writeCharacters(command->value().toString());
    xml->writeEndElement();
    xml->writeEndElement();
}

void CohXmlDumper::visit(CohNullStimulus *command)
{
    xml->writeStartElement(QSL("stimulus"));
    xml->writeAttribute(QSL("type"), QSL("null"));
    writeParameter(QSL("trigger"),
                   (command->hasTrigger() ? QSL("true") : QSL("false")));
    writeParameter(QSL("period"), QString::number(command->period(), 'f', 1));
    xml->writeEndElement();
}

void CohXmlDumper::visit(CohBiphasicStimulus *command)
{
    xml->writeStartElement(QSL("stimulus"));
    xml->writeAttribute(QSL("type"), QSL("biphasic"));
    writeParameter(QSL("trigger"),
                   command->hasTrigger() ? QSL("true") : QSL("false"));
    writeParameter(QSL("active"), QString::number(command->activeElectrode()));
    writeParameter(QSL("reference"),
                   QString::number(command->referenceElectrode()));
    writeParameter(QSL("currentlevel"),
                   QString::number(command->currentLevel()));
    writeParameter(QSL("phasewidth"),
                   QString::number(command->phaseWidth(), 'f', 1));
    writeParameter(QSL("phasegap"),
                   QString::number(command->phaseGap(), 'f', 1));
    writeParameter(QSL("period"), QString::number(command->period(), 'f', 1));
    xml->writeEndElement();
}

void CohXmlDumper::visit(CohCodacsStimulus *command)
{
    xml->writeStartElement(QSL("stimulus"));
    xml->writeAttribute(QSL("type"), QSL("codacs"));
    writeParameter(QSL("trigger"),
                   command->hasTrigger() ? QSL("true") : QSL("false"));
    writeParameter(QSL("amplitude"), QString::number(command->amplitude()));
    writeParameter(QSL("period"), QString::number(command->period(), 'f', 1));
    xml->writeEndElement();
}

void CohXmlDumper::visit(CohSequence *command)
{
    xml->writeStartElement(QSL("sequence"));
    if (command->numberRepeats() != 1)
        xml->writeAttribute(QSL("repeats"),
                            QString::number(command->numberRepeats()));

    QListIterator<CohCommand *> i(*command);
    while (i.hasNext())
        i.next()->accept(this);

    xml->writeEndElement();
}

void CohXmlDumper::write(QXmlStreamWriter *xml)
{
    this->xml = xml;
    xml->writeStartDocument();
    xml->writeDTD(QSL("<!DOCTYPE cisequence>"));
    xml->writeStartElement(QSL("cisequence"));
    xml->writeAttribute(QSL("version"), QSL("1.0"));

    sequence->accept(this);

    xml->writeEndElement();
    xml->writeEndDocument();
}

// Various functions ===========================================================

QByteArray dumpCohSequenceXml(const CohSequence *data)
{
    return dumpXml(CohXmlDumper(data));
}

} // namespace coh
