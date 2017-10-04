/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHout ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "common/exception.h"
#include "common/xmlutils.h"

#include "cohnicxmldumper.h"

#include <QBuffer>
#include <QFile>
#include <QTextStream>

using namespace cmn;

namespace coh
{

class CohNicXmlDumper : public CohCommandVisitor
{
public:
    CohNicXmlDumper(const CohSequence *sequence);

    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

    void write(QIODevice *device);

private:
    const int spaces;
    CohSequence *const sequence;
    int level;
    QTextStream out;
};

// CohNicXmlDumper =============================================================

CohNicXmlDumper::CohNicXmlDumper(const CohSequence *sequence)
    : spaces(3), sequence(const_cast<CohSequence *>(sequence))
{
}

void CohNicXmlDumper::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void CohNicXmlDumper::visit(CohNullStimulus *command)
{
    out << xmlIndent(level++, spaces)
        << (command->hasTrigger() ? "<trigger>\n" : "<stimulus>\n");
    out << xmlIndent(level, spaces) << "<parameters><ae>0</ae><re>0</re><t>"
        << QString::number(command->period(), 'f', 1) << "</t></parameters>\n";
    out << xmlIndent(--level, spaces)
        << (command->hasTrigger() ? "</trigger>\n" : "</stimulus>\n");
}

void CohNicXmlDumper::visit(CohBiphasicStimulus *command)
{
    out << xmlIndent(level++, spaces)
        << (command->hasTrigger() ? "<trigger>\n" : "<stimulus>\n");
    out << xmlIndent(level, spaces) << "<parameters><ae>"
        << QString::number(command->activeElectrode()) << "</ae><re>"
        << QString::number(command->referenceElectrode()) << "</re><cl>"
        << QString::number(command->currentLevel()) << "</cl><pw>"
        << QString::number(command->phaseWidth(), 'f', 1) << "</pw><pg>"
        << QString::number(command->phaseGap(), 'f', 1) << "</pg><t>"
        << QString::number(command->period(), 'f', 1) << "</t></parameters>\n";
    out << xmlIndent(--level, spaces)
        << (command->hasTrigger() ? "</trigger>\n" : "</stimulus>\n");
}

void CohNicXmlDumper::visit(CohCodacsStimulus *command)
{
    out << xmlIndent(level++, spaces)
        << (command->hasTrigger() ? "<trigger>\n" : "<stimulus>\n");
    out << xmlIndent(level, spaces) << "<parameters>"
        << "<ae>" << QString::number(command->activeElectrode()) << "</ae><re>"
        << QString::number(CohCodacsStimulus::referenceElectrode())
        << "</re><cl>" << QString::number(command->currentLevel())
        << "</cl><pw>"
        << QString::number(CohCodacsStimulus::phaseWidth(), 'f', 1)
        << "</pw><pg>" << QString::number(CohCodacsStimulus::phaseGap(), 'f', 1)
        << "</pg><t>" << QString::number(command->period(), 'f', 1) << "</t>"
        << "</parameters>\n";
    out << xmlIndent(--level, spaces)
        << (command->hasTrigger() ? "</trigger>\n" : "</stimulus>\n");
}

void CohNicXmlDumper::visit(CohSequence *command)
{
    out << xmlIndent(level++, spaces) << "<nic:sequence";
    if (level == 1)
        out << " xmlns:nic='http://www.cochlear.com/nic'";
    out << ">\n";

    if (command->numberRepeats() != 1 && level == 1) {
        // No repeat at the root level
        visit(command);
    } else {
        // The streaming interface can only repeat single stimuli
        for (unsigned i = 0; i < command->numberRepeats(); ++i) {
            QListIterator<CohCommand *> j(*command);
            while (j.hasNext())
                j.next()->accept(this);
        }
    }

    out << xmlIndent(--level, spaces) << "</nic:sequence>\n";
}

void CohNicXmlDumper::write(QIODevice *device)
{
    out.setDevice(device);
    out.setCodec("UTF-8");

    level = 0;
    sequence->accept(this);

    out.flush();
}

// Various functions ===========================================================

QByteArray dumpCohSequenceNicXml(const CohSequence *data)
{
    QBuffer buffer;
    // No QFile::Text so we always create linux line endings
    buffer.open(QIODevice::WriteOnly);
    CohNicXmlDumper(data).write(&buffer);
    return buffer.data();
}

} // namespace coh
