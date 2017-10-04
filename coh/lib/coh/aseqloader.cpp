/******************************************************************************
 * Copyright (C) 2015  Michael Hofmann <mh21@mh21.de>                         *
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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
#include "common/listutils.h"
#include "common/rifffile.h"

#include "aseqloader.h"

#include <QBuffer>
#include <QStack>

#include <QtEndian>

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#error Byte order not implemented
#endif

using namespace cmn;

namespace coh
{

template <typename T>
class DataContainer : public QVector<T>
{
public:
    T val(int i) const;
    void read(QIODevice *file, unsigned nextLength, unsigned realFrames);
};

template <>
class DataContainer<bool>
{
public:
    DataContainer();
    bool isEmpty() const;
    bool val(int i) const;
    void read(QIODevice *file, unsigned nextLength, unsigned realFrames);

private:
    int size;
    QVector<quint8> contents;
};

class AseqReader : public RiffFile
{
public:
    AseqReader(QIODevice *device, bool acceptIncomplete);

    CohSequence *result();
    void read();

private:
    template <class T>
    void readPart(T *buffer, unsigned length, const QString &exception);
    void readRiffChunkHeader();
    template <class T>
    unsigned readChunkRemainder(quint32 id, DataContainer<T> *target);
    unsigned readListChunkRemainder(StringVariantList *target);
    unsigned readInfoChunkRemainder();
    quint32 readOneChunk();
    void updateFrames(unsigned newFrames);
    static bool checkId(quint8 *data, quint32 id);

    QScopedPointer<CohSequence> data;
    QStack<CohSequence *> parents;
    unsigned frames;
    bool acceptIncomplete;

    QMap<quint32, unsigned> infoSizes;

    DataContainer<bool> triggers;
    DataContainer<qint16> amplitudes;
    DataContainer<qint8> actives;
    DataContainer<qint8> references;
    DataContainer<quint8> levels;
    DataContainer<float> widths;
    DataContainer<float> gaps;
    DataContainer<float> intervals;
    DataContainer<qint8> channels;
    DataContainer<float> magnitudes;
    StringVariantList metas;

    const static quint32 RIFF_ID;
    const static quint32 ASEQ_ID;
    const static quint32 INFO_ID;
    const static quint32 TRIG_ID;
    const static quint32 AMPL_ID;
    const static quint32 AELE_ID;
    const static quint32 RELE_ID;
    const static quint32 CURL_ID;
    const static quint32 PHWI_ID;
    const static quint32 PHGA_ID;
    const static quint32 PERI_ID;
    const static quint32 CHAN_ID;
    const static quint32 MAGN_ID;
    const static quint32 LIST_ID;
    const static quint32 META_ID;
    const static quint32 MKEY_ID;
    const static quint32 MSTR_ID;
    const static quint32 MDBL_ID;
    const static quint32 MINT_ID;
    const static quint32 MBOL_ID;
};

const quint32 AseqReader::RIFF_ID = RiffFile::makeId("RIFF");
const quint32 AseqReader::ASEQ_ID = RiffFile::makeId("ASEQ");
const quint32 AseqReader::INFO_ID = RiffFile::makeId("INFO");
const quint32 AseqReader::TRIG_ID = RiffFile::makeId("TRIG");
const quint32 AseqReader::AMPL_ID = RiffFile::makeId("AMPL");
const quint32 AseqReader::AELE_ID = RiffFile::makeId("AELE");
const quint32 AseqReader::RELE_ID = RiffFile::makeId("RELE");
const quint32 AseqReader::CURL_ID = RiffFile::makeId("CURL");
const quint32 AseqReader::PHWI_ID = RiffFile::makeId("PHWI");
const quint32 AseqReader::PHGA_ID = RiffFile::makeId("PHGA");
const quint32 AseqReader::PERI_ID = RiffFile::makeId("PERI");
const quint32 AseqReader::CHAN_ID = RiffFile::makeId("CHAN");
const quint32 AseqReader::MAGN_ID = RiffFile::makeId("MAGN");
const quint32 AseqReader::LIST_ID = RiffFile::makeId("LIST");
const quint32 AseqReader::META_ID = RiffFile::makeId("META");
const quint32 AseqReader::MKEY_ID = RiffFile::makeId("MKEY");
const quint32 AseqReader::MSTR_ID = RiffFile::makeId("MSTR");
const quint32 AseqReader::MDBL_ID = RiffFile::makeId("MDBL");
const quint32 AseqReader::MINT_ID = RiffFile::makeId("MINT");
const quint32 AseqReader::MBOL_ID = RiffFile::makeId("MBOL");

// DataContainer ===============================================================

template <class T>
T DataContainer<T>::val(int i) const
{
    switch (DataContainer<T>::size()) {
    case 0:
        throw Exception(AseqReader::tr("No data available"));
    case 1:
        return DataContainer<T>::at(0);
    default:
        return DataContainer<T>::at(i);
    }
}

template <class T>
void DataContainer<T>::read(QIODevice *file, unsigned nextLength,
                            unsigned realFrames)
{
    if (nextLength % sizeof(T))
        throw Exception(AseqReader::tr("Invalid chunk length"));
    unsigned frames = nextLength / sizeof(T);
    if (frames < realFrames)
        throw Exception(AseqReader::tr("Not enough data in chunk"));
    DataContainer<T>::resize(frames);
    if (file->read(reinterpret_cast<char *>(DataContainer<T>::data()),
                   nextLength) != nextLength)
        throw Exception(AseqReader::tr("Incomplete chunk"));
    DataContainer<T>::resize(realFrames);
}

DataContainer<bool>::DataContainer() : size(0)
{
}

bool DataContainer<bool>::isEmpty() const
{
    return size == 0;
}

bool DataContainer<bool>::val(int i) const
{
    switch (size) {
    case 0:
        throw Exception(AseqReader::tr("No data available"));
    case 1:
        return (contents.at(0) & 1) > 0;
    default:
        return (contents.at(i / 8) & (1 << (i % 8))) > 0;
    }
}

void DataContainer<bool>::read(QIODevice *file, unsigned nextLength,
                               unsigned realFrames)
{
    this->size = realFrames;
    if (nextLength * 8 < realFrames)
        throw Exception(AseqReader::tr("Not enough data in chunk"));
    contents.resize(nextLength);
    if (file->read(reinterpret_cast<char *>(contents.data()), nextLength) !=
        nextLength)
        throw Exception(AseqReader::tr("Incomplete chunk"));
}

// AseqReader ==================================================================

AseqReader::AseqReader(QIODevice *device, bool acceptIncomplete)
    : RiffFile(device, false), frames(0), acceptIncomplete(acceptIncomplete)
{
}

CohSequence *AseqReader::result()
{
    return data.take();
}

bool AseqReader::checkId(quint8 *data, quint32 id)
{
    return qFromLittleEndian<quint32>(data) == id;
}

template <class T>
void AseqReader::readPart(T *buffer, unsigned length, const QString &exception)
{
    if (file->read(reinterpret_cast<char *>(buffer), length) != length)
        throw Exception(exception);
}

template <class T>
unsigned AseqReader::readChunkRemainder(quint32 id, DataContainer<T> *target)
{
    if (!infoSizes.contains(id))
        throw Exception(tr("Length of field not found in info chunk"));
    unsigned frames = infoSizes[id];
    target->read(file, nextLength, frames);
    skipChunk();
    return frames;
}

unsigned AseqReader::readListChunkRemainder(StringVariantList *target)
{
    unsigned toGo = nextLength;
    if (toGo < 4)
        throw Exception(tr("List chunk too short"));
    uchar listChunkHeader[4];
    readPart(listChunkHeader, 4, tr("Unable to read list header"));
    if (!checkId(listChunkHeader, META_ID)) {
        skipChunk();
        return 0;
    }
    toGo -= 4;
    QString lastKey;
    QByteArray scratch;
    while (toGo > 8) {
        uchar itemHeader[8];
        readPart(itemHeader, 8, tr("Unable to read item header"));
        toGo -= 8;
        unsigned id = qFromLittleEndian<quint32>(itemHeader);
        unsigned itemLength = qFromLittleEndian<quint32>(itemHeader + 4);
        if (toGo < itemLength)
            throw Exception(tr("List item too short"));
        unsigned skipLength = (itemLength + 1) / 2 * 2;
        if (toGo < skipLength)
            throw Exception(tr("List item too short"));
        toGo -= skipLength;
        scratch.resize(skipLength);
        readPart(scratch.data(), skipLength, tr("Unable to read list item"));
        scratch.resize(itemLength);
        if (id == MKEY_ID) {
            lastKey = QString::fromUtf8(scratch);
        } else if (id == MSTR_ID || id == MDBL_ID || id == MINT_ID ||
                   id == MBOL_ID) {
            if (id == MSTR_ID) {
                target->append(
                    qMakePair(lastKey, QVariant(QString::fromUtf8(scratch))));
            } else if (id == MDBL_ID) {
                union {
                    double d;
                    quint64 u;
                };
                for (unsigned i = 0; i < itemLength; i += 8) {
                    u = qFromLittleEndian<quint64>(
                        (uchar *)(scratch.data() + i));
                    target->append(qMakePair(lastKey, QVariant(d)));
                }
            } else if (id == MINT_ID) {
                for (unsigned i = 0; i < itemLength; i += 4) {
                    target->append(
                        qMakePair(lastKey, QVariant(qFromLittleEndian<qint32>((
                                               uchar *)(scratch.data() + i)))));
                }
            } else if (id == MBOL_ID) {
                if (itemLength != 1)
                    throw Exception(tr("Boolean item too long"));
                for (unsigned i = 0; i < itemLength; i += 1) {
                    target->append(
                        qMakePair(lastKey, QVariant(bool(scratch.data() + i))));
                }
            }
        }
    }
    if (toGo != 0)
        throw Exception(tr("Extra data in list chunk"));
    skipChunk();
    return 0;
}

unsigned AseqReader::readInfoChunkRemainder()
{
    unsigned toGo = nextLength;
    if (toGo < 4)
        throw Exception(tr("Info chunk too short"));
    uchar versionBuffer[4];
    readPart(versionBuffer, 4, tr("Unable to read version"));
    toGo -= 4;
    union {
        float version;
        quint32 u;
    };
    u = qFromLittleEndian<quint32>(versionBuffer);
    if (version == 1) {
        // fake all triggers to 0
        QBuffer buffer;
        buffer.setData(QByteArray(1, 0));
        buffer.open(QIODevice::ReadOnly);
        infoSizes.insert(TRIG_ID, 1);
        triggers.read(&buffer, 1, 1);
        version += 1;
    }
    if (version != 2)
        throw Exception(tr("Unsupported version"));

    while (toGo >= 8) {
        uchar infoField[8];
        readPart(infoField, 8, tr("Unable to read info field"));
        infoSizes.insert(qFromLittleEndian<quint32>(infoField),
                         qFromLittleEndian<quint32>(infoField + 4));
        toGo -= 8;
    }
    if (toGo != 0)
        throw Exception(tr("Extra data in info chunk"));
    skipChunk();
    return 0;
}

void AseqReader::readRiffChunkHeader()
{
    uchar riffChunkHeader[12];
    readPart(riffChunkHeader, 12, tr("Unable to read riff header"));
    if (!checkId(riffChunkHeader, RIFF_ID))
        throw Exception(tr("no riff file"));
    nextLength = qFromLittleEndian<quint32>(riffChunkHeader + 4);
    // We read the next 4 bytes already
    nextStart = file->pos() - 4;
    if (!checkId(riffChunkHeader + 8, ASEQ_ID))
        throw Exception(tr("no aseq file"));
}

quint32 AseqReader::readOneChunk()
{
    quint32 id = readChunkHeader();
    if (id == TRIG_ID)
        return readChunkRemainder(id, &triggers);
    if (id == AMPL_ID)
        return readChunkRemainder(id, &amplitudes);
    if (id == AELE_ID)
        return readChunkRemainder(id, &actives);
    if (id == RELE_ID)
        return readChunkRemainder(id, &references);
    if (id == CURL_ID)
        return readChunkRemainder(id, &levels);
    if (id == PHWI_ID)
        return readChunkRemainder(id, &widths);
    if (id == PHGA_ID)
        return readChunkRemainder(id, &gaps);
    if (id == PERI_ID)
        return readChunkRemainder(id, &intervals);
    if (id == CHAN_ID)
        return readChunkRemainder(id, &channels);
    if (id == MAGN_ID)
        return readChunkRemainder(id, &magnitudes);
    if (id == LIST_ID)
        return readListChunkRemainder(&metas);
    if (id == INFO_ID)
        return readInfoChunkRemainder();
    skipChunk();
    return 0;
}

void AseqReader::updateFrames(unsigned newFrames)
{
    if (newFrames <= 1)
        return;
    if (frames > 1 && frames != newFrames)
        throw Exception(tr("chunks of different lengths"));
    frames = newFrames;
}

void AseqReader::read()
{
    readRiffChunkHeader();
    while (!file->atEnd())
        updateFrames(readOneChunk());

    data.reset(new CohSequence(1));

    Q_FOREACH (const auto &meta, metas)
        data->append(new CohMetaData(meta.first, meta.second));

    for (unsigned i = 0; i < frames; ++i) {
        CohCommand *command = NULL;
        if (!amplitudes.isEmpty()) {
            QScopedPointer<CohCodacsStimulus> stimulus(
                CohCodacsStimulus::incompleteStimulus());
            if (!triggers.isEmpty())
                stimulus->setTrigger(triggers.val(i));
            if (!intervals.isEmpty())
                stimulus->setPeriod(intervals.val(i));
            if (!amplitudes.isEmpty())
                stimulus->setAmplitude(amplitudes.val(i));
            data->append(command = stimulus.take());
        } else if ((!actives.isEmpty() && actives.val(i) == 0) ||
                   (!channels.isEmpty() && channels.val(i) == 0)) {
            QScopedPointer<CohNullStimulus> stimulus(
                CohNullStimulus::incompleteStimulus());
            if (!triggers.isEmpty())
                stimulus->setTrigger(triggers.val(i));
            if (!intervals.isEmpty())
                stimulus->setPeriod(intervals.val(i));
            data->append(command = stimulus.take());
        } else {
            QScopedPointer<CohBiphasicStimulus> stimulus(
                CohBiphasicStimulus::incompleteStimulus());
            if (!triggers.isEmpty())
                stimulus->setTrigger(triggers.val(i));
            if (!actives.isEmpty())
                stimulus->setActiveElectrode(Coh::Electrode(actives.val(i)));
            if (!references.isEmpty())
                stimulus->setReferenceElectrode(
                    Coh::Electrode(references.val(i)));
            if (!levels.isEmpty())
                stimulus->setCurrentLevel(levels.val(i));
            if (!widths.isEmpty())
                stimulus->setPhaseWidth(widths.val(i));
            if (!gaps.isEmpty())
                stimulus->setPhaseGap(gaps.val(i));
            if (!intervals.isEmpty())
                stimulus->setPeriod(intervals.val(i));
            if (!channels.isEmpty())
                stimulus->setChannelIndex(channels.val(i));
            if (!magnitudes.isEmpty())
                stimulus->setChannelMagnitude(magnitudes.val(i));
            data->append(command = stimulus.take());
        }
        if (!acceptIncomplete && !command->isValid())
            throw Exception(tr("Incomplete stimulus not allowed"));
    }
}

// Various functions ===========================================================

CohSequence *loadCohSequenceAseq(const QByteArray &contents,
                                 bool acceptIncomplete)
{
    QBuffer buffer(const_cast<QByteArray *>(&contents));
    buffer.open(QIODevice::ReadOnly);

    AseqReader reader(&buffer, acceptIncomplete);
    reader.read();

    return reader.result();
}

} // namespace coh
