/******************************************************************************
 * Copyright (C) 2013  Michael Hofmann <mh21@mh21.de>                         *
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

#include "coh.h"
#include "cohprotoloader.h"
#include "protocoh.pb.h"

#include <QCoreApplication>

using namespace cmn;

namespace coh
{

class CohProtoLoader
{
    Q_DECLARE_TR_FUNCTIONS(CohProtoLoader)
public:
    CohProtoLoader(const QByteArray &contents);

    CohSequence *parse();

private:
    void parseCommand(const ProtoSlaveCommand &command);

    QByteArray contents;
    QScopedPointer<CohSequence> data;
    CohSequence *parent;
};

// CohProtoLoader ==============================================================

CohProtoLoader::CohProtoLoader(const QByteArray &contents) : contents(contents)
{
}

CohSequence *CohProtoLoader::parse()
{
    data.reset();

    ProtoSlaveCommand command;
    if (!command.ParseFromArray(contents.constData(), contents.size()))
        throw Exception(tr("Unable to parse datagram"));
    parseCommand(command);
    return data.take();
}

void CohProtoLoader::parseCommand(const ProtoSlaveCommand &command)
{
    if (command.has_metadata()) {
        const ProtoSlaveCommand::MetaData &metadata = command.metadata();
        QVariant value;
        if (metadata.has_stringvalue())
            value = QVariant(QString::fromUtf8(metadata.stringvalue().data()));
        else if (metadata.has_doublevalue())
            value = QVariant(metadata.doublevalue());
        else if (metadata.has_intvalue())
            value = QVariant(metadata.intvalue());
        else if (metadata.has_boolvalue())
            value = QVariant(metadata.boolvalue());
        else
            throw Exception(tr("Unable to load value from meta data command"));
        parent->append(
            new CohMetaData(QString::fromUtf8(metadata.key().data()), value));
    } else if (command.has_null()) {
        const ProtoSlaveCommand::Stimulus &stimulus = command.stimulus();
        const ProtoSlaveCommand::Null &null = command.null();
        parent->append(new CohNullStimulus(null.period_cycles() / 5.0,
                                           stimulus.trigger()));
    } else if (command.has_biphasic()) {
        const ProtoSlaveCommand::Stimulus &stimulus = command.stimulus();
        const ProtoSlaveCommand::Biphasic &biphasic = command.biphasic();
        parent->append(new CohBiphasicStimulus(
            Coh::Electrode(biphasic.active_electrode()),
            Coh::Electrode(biphasic.reference_electrode()),
            biphasic.current_level(), biphasic.phase_width_cycles() / 5.0,
            biphasic.phase_gap_cycles() / 5.0, biphasic.period_cycles() / 5.0,
            stimulus.trigger()));
    } else if (command.has_codacs()) {
        const ProtoSlaveCommand::Stimulus &stimulus = command.stimulus();
        const ProtoSlaveCommand::Codacs &codacs = command.codacs();
        parent->append(new CohCodacsStimulus(codacs.amplitude(),
                                             codacs.period_cycles() / 5.0,
                                             stimulus.trigger()));
    } else if (command.has_sequence()) {
        const ProtoSlaveCommand::Sequence &ciSequence = command.sequence();
        CohSequence *sequence = new CohSequence(ciSequence.number_repeats());
        if (!data)
            data.reset(sequence);
        else
            parent->append(sequence);
        const auto &commands = ciSequence.command();
        for (auto i = commands.begin(); i != commands.end(); ++i) {
            parent = sequence;
            parseCommand(*i);
        }
    } else {
        throw Exception(tr("Unknown pulse type"));
    }
}

// Various functions ===========================================================

CohSequence *loadCohSequenceProto(const QByteArray &contents)
{
    return CohProtoLoader(contents).parse();
}

} // namespace coh
