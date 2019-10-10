/******************************************************************************
 * Copyright (C) 2008,2015  Michael Hofmann <mh21@mh21.de>                    *
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

#include "common/global.h"

#include "cohexpander.h"
#include "cohmatlabdumper.h"

#include <QTextStream>

namespace coh
{

class CohMatlabDumper : public CohCommandVisitor
{
public:
    CohMatlabDumper(const CohSequence *sequence);

    QString dump();

    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    const CohSequence *sequence;
    QScopedPointer<CohSequence> expanded;
    QString result;
    QTextStream stream;
    unsigned index;
    unsigned count;
    QMap<bool, QList<unsigned>> triggers;
    QMap<Coh::Electrode, QList<unsigned>> activeElectrodes;
    QMap<Coh::Electrode, QList<unsigned>> referenceElectrodes;
    QMap<double, QList<unsigned>> periods;
    QMap<int, QList<unsigned>> amplitudes;
    QMap<double, QList<unsigned>> phaseWidths;
    QMap<double, QList<unsigned>> phaseGaps;
    QMap<int, QList<unsigned>> currentLevels;
    QMap<QString, QVariantList> metas;
};

// CohMatlabDumper =============================================================

CohMatlabDumper::CohMatlabDumper(const CohSequence *sequence)
    : sequence(sequence), stream(&result)
{
}

template <typename K>
void streamStimuli(QTextStream &stream, const QByteArray &name,
                   const QMap<K, QList<unsigned>> &map)
{
    Q_FOREACH (const auto &key, map.keys()) {
        if (!key)
            continue;

        const QList<unsigned> indexes = map.value(key);
        QStringList singleIndexes;
        unsigned first = 0;
        unsigned previous = 0;
        Q_FOREACH (unsigned index, indexes) {
            unsigned matlabIndex = index + 1;
            if (!previous || matlabIndex == previous + 1) {
                if (!first)
                    first = matlabIndex;
            } else {
                if (first != previous)
                    stream << "sequence." << name << "("
                           << QByteArray::number(first) << ':'
                           << QByteArray::number(previous) << ") = " << key
                           << ";\n";
                else
                    singleIndexes += QString::number(previous);
                first = matlabIndex;
            }
            previous = matlabIndex;
        }

        if (first != previous)
            stream << "sequence." << name << "(" << QByteArray::number(first)
                   << ':' << QByteArray::number(previous) << ") = " << key
                   << ";\n";
        else
            singleIndexes += QString::number(previous);

        if (singleIndexes.size() == 1)
            stream << "sequence." << name << "(" << singleIndexes.at(0)
                   << ") = " << key << ";\n";
        else if (singleIndexes.size() > 1)
            stream << "sequence." << name << "(["
                   << singleIndexes.join(QL1S(" ")) << "]) = " << key << ";\n";
    }
}

void streamMeta(QTextStream &stream, const QMap<QString, QVariantList> &map)
{
    if (map.isEmpty())
        return;

    stream << "\nsequence.metas = struct;\n";
    Q_FOREACH (const auto &key, map.keys()) {
        stream << "sequence.metas." << key << " = ";
        const QVariantList values = map.value(key);
        const bool isString = values.at(0).type() == QVariant::String;
        stream << (isString ? "{ " : "[ ");
        Q_FOREACH (const auto &value, values) {
            stream << (isString ? "'" : "") << value.toString()
                   << (isString ? "', " : ", ");
        }
        stream << (isString ? "}" : "]") << ";\n";
    }
}

QString CohMatlabDumper::dump()
{
    stream << "sequence = struct;\n";

    expanded.reset(expandCohSequence(sequence, Coh::AllStimuli));
    count = expanded->count();
    expanded->accept(this);
    stream << "sequence.triggers = zeros(" << count << ", 1);\n";
    stream << "sequence.electrodes = zeros(" << count << ", 1);\n";
    stream << "sequence.references = zeros(" << count << ", 1);\n";
    stream << "sequence.periods = zeros(" << count << ", 1);\n";
    stream << "sequence.amplitudes = zeros(" << count << ", 1);\n";
    stream << "sequence.current_levels = zeros(" << count << ", 1);\n";
    stream << "sequence.phase_widths = zeros(" << count << ", 1);\n";
    stream << "sequence.phase_gaps = zeros(" << count << ", 1);\n";
    stream << "\n";

    streamStimuli(stream, "triggers", triggers);
    streamStimuli(stream, "electrodes", activeElectrodes);
    streamStimuli(stream, "references", referenceElectrodes);
    streamStimuli(stream, "periods", periods);
    streamStimuli(stream, "amplitudes", amplitudes);
    streamStimuli(stream, "current_levels", currentLevels);
    streamStimuli(stream, "phase_widths", phaseWidths);
    streamStimuli(stream, "phase_gaps", phaseGaps);

    // no Coh::Sequence, so repeated meta data is only included once
    expanded.reset(expandCohSequence(sequence, Coh::MetaData));
    count = expanded->count();
    expanded->accept(this);
    streamMeta(stream, metas);

    stream.flush();

    return result;
}

void CohMatlabDumper::visit(CohMetaData *command)
{
    metas[command->key()].append(command->value());
}

void CohMatlabDumper::visit(CohRfFreeStimulus *command)
{
    periods[command->period()].append(index);
    currentLevels[-1].append(index);
}

void CohMatlabDumper::visit(CohNullStimulus *command)
{
    triggers[command->hasTrigger()].append(index);
    periods[command->period()].append(index);
}

void CohMatlabDumper::visit(CohBiphasicStimulus *command)
{
    triggers[command->hasTrigger()].append(index);
    periods[command->period()].append(index);
    activeElectrodes[command->activeElectrode()].append(index);
    referenceElectrodes[command->referenceElectrode()].append(index);
    phaseWidths[command->phaseWidth()].append(index);
    phaseGaps[command->phaseGap()].append(index);
    currentLevels[command->currentLevel()].append(index);
}

void CohMatlabDumper::visit(CohCodacsStimulus *command)
{
    triggers[command->hasTrigger()].append(index);
    periods[command->period()].append(index);
    amplitudes[command->amplitude()].append(index);
}

void CohMatlabDumper::visit(CohSequence *command)
{
    for (index = 0; index < count; index += 1)
        (*command)[index]->accept(this);
}

// Various functions ===========================================================

QString dumpCohSequenceMatlab(const CohSequence *sequence)
{
    return CohMatlabDumper(sequence).dump();
}

} // namespace coh
