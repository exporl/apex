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
#include "cohprotodumper.h"
#include "protocoh.pb.h"

#include <QCoreApplication>

using namespace cmn;

namespace coh
{

class CohProtoDumper : public CohCommandVisitor
{
    Q_DECLARE_TR_FUNCTIONS(CohProtoDumper)
public:
    CohProtoDumper(const CohSequence *sequence);

    QByteArray write();

    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohSequence *const sequence;
    ProtoSlaveCommand root;
    ProtoSlaveCommand::Sequence *current;
};

// CohProtoDumper ==============================================================

CohProtoDumper::CohProtoDumper(const CohSequence *sequence)
    : sequence(const_cast<CohSequence *>(sequence))
{
}

void CohProtoDumper::visit(CohMetaData *ciCommand)
{
    ProtoSlaveCommand *const command = current->add_command();

    ProtoSlaveCommand::MetaData *const metadata = command->mutable_metadata();
    metadata->set_key(ciCommand->key().toUtf8());
    QVariant value = ciCommand->value();
    switch (value.type()) {
    case QVariant::String:
        metadata->set_stringvalue(value.toString().toUtf8());
        break;
    case QVariant::Double:
        metadata->set_doublevalue(value.toDouble());
        break;
    case QVariant::Int:
        metadata->set_intvalue(value.toInt());
        break;
    case QVariant::Bool:
        metadata->set_boolvalue(value.toBool());
        break;
    default:
        throw Exception(tr("Unable to represent type %1 in protobuf format")
                            .arg(value.type()));
    }
}

void CohProtoDumper::visit(CohRfFreeStimulus *ciCommand)
{
    ProtoSlaveCommand *const command = current->add_command();

    ProtoSlaveCommand::RfFree *const rffree = command->mutable_rffree();
    rffree->set_period_cycles(roundedCohCycles(ciCommand->period()));
}

void CohProtoDumper::visit(CohNullStimulus *ciCommand)
{
    ProtoSlaveCommand *const command = current->add_command();

    ProtoSlaveCommand::Stimulus *const stimulus = command->mutable_stimulus();
    stimulus->set_trigger(ciCommand->hasTrigger());

    ProtoSlaveCommand::Null *const null = command->mutable_null();
    null->set_period_cycles(roundedCohCycles(ciCommand->period()));
}

void CohProtoDumper::visit(CohBiphasicStimulus *ciCommand)
{
    ProtoSlaveCommand *const command = current->add_command();

    ProtoSlaveCommand::Stimulus *const stimulus = command->mutable_stimulus();
    stimulus->set_trigger(ciCommand->hasTrigger());

    ProtoSlaveCommand::Biphasic *const biphasic = command->mutable_biphasic();
    biphasic->set_active_electrode(ciCommand->activeElectrode());
    biphasic->set_reference_electrode(ciCommand->referenceElectrode());
    biphasic->set_period_cycles(roundedCohCycles(ciCommand->period()));
    biphasic->set_current_level(ciCommand->currentLevel());
    biphasic->set_phase_width_cycles(roundedCohCycles(ciCommand->phaseWidth()));
    biphasic->set_phase_gap_cycles(roundedCohCycles(ciCommand->phaseGap()));
}

void CohProtoDumper::visit(CohCodacsStimulus *ciCommand)
{
    ProtoSlaveCommand *const command = current->add_command();

    ProtoSlaveCommand::Stimulus *const stimulus = command->mutable_stimulus();
    stimulus->set_trigger(ciCommand->hasTrigger());

    ProtoSlaveCommand::Codacs *const codacs = command->mutable_codacs();
    codacs->set_amplitude(ciCommand->amplitude());
    codacs->set_period_cycles(roundedCohCycles(ciCommand->period()));
}

void CohProtoDumper::visit(CohSequence *ciCommand)
{
    ProtoSlaveCommand *const command = current ? current->add_command() : &root;

    ProtoSlaveCommand::Sequence *const sequence = command->mutable_sequence();
    sequence->set_number_repeats(ciCommand->numberRepeats());

    QListIterator<CohCommand *> i(*ciCommand);
    while (i.hasNext()) {
        current = sequence;
        i.next()->accept(this);
    }
}

QByteArray CohProtoDumper::write()
{
    root.Clear();
    current = NULL;
    sequence->accept(this);

    QByteArray data(root.ByteSize(), 0);
    root.SerializeWithCachedSizesToArray(
        reinterpret_cast<unsigned char *>(data.data()));
    return data;
}

// Various functions ===========================================================

QByteArray dumpCohSequenceProto(const CohSequence *data)
{
    return CohProtoDumper(data).write();
}

} // namespace coh
