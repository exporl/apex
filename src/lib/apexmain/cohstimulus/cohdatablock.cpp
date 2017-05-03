/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/datablock/datablockdata.h"

#include "apexdata/map/apexmap.h"

#include "coh/aseqloader.h"
#include "coh/cohcloner.h"
#include "coh/cohiterator.h"
#include "coh/cohlevelchecker.h"
#include "coh/cohmapper.h"
#include "coh/cohxmlloader.h"

#include "common/exception.h"
#include "common/global.h"

#include "cohdatablock.h"
#include "cohdevice.h"

#include <QFile>
#include <QString>

using namespace coh;

namespace apex
{

namespace stimulus
{

class CohDataBlockPrivate
{
public:
    QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned> comfortLevels
            (data::ApexMap *pMap) const;
    CohSequence *map(data::ApexMap *pMap, double volume) const;

    bool codacsStimulus;
    bool needsMapping;
    bool needsChecking;
    QScopedPointer<coh::CohSequence> raw;
};

// CohDataBlockPrivate =========================================================

QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned> CohDataBlockPrivate::comfortLevels
        (data::ApexMap *pMap) const
{
    data::ChannelMap defaults(pMap->defaultMap());
    QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned> result;
    QMapIterator<int, data::ChannelMap> i(*pMap);
    while (i.hasNext()) {
        i.next();
        result[qMakePair(Coh::Electrode(i.value().stimulationElectrode()),
                Coh::Electrode(defaults.referenceElectrode()))] = i.value().comfortLevel();
    }
    return result;
}

CohSequence *CohDataBlockPrivate::map(data::ApexMap *pMap, double volume) const
{
    data::ChannelMap defaults(pMap->defaultMap());
    CohSequenceMapper mapper(raw.data());

    mapper.setDefaultReferenceElectrode(Coh::Electrode(defaults.referenceElectrode()));
    mapper.setDefaultPhaseWidth(defaults.phaseWidth());
    mapper.setDefaultPhaseGap(defaults.phaseGap());
    mapper.setDefaultPeriod(defaults.period());

    QMapIterator<int, data::ChannelMap> i(*pMap);
    while (i.hasNext()) {
        i.next();
        mapper.setChannel(i.key(), Coh::Electrode(i.value().stimulationElectrode()),
                i.value().thresholdLevel(), i.value().comfortLevel(), volume / 100.0);
    }

    return mapper.map();
}

// CohDataBlock ================================================================

CohDataBlock::CohDataBlock(const data::DatablockData& data, const QString& fileName,
                           const ExperimentRunDelegate* experiment) :
    DataBlock(data, fileName, experiment),
    dataPtr(new CohDataBlockPrivate)
{
    E_D(CohDataBlock);

    this->data.setNbChannels(1);

    if (data.file().isEmpty()) {
        d->raw.reset(loadCohSequenceXml(data.directData().toLatin1()));
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
            throw ApexStringException(tr("Unable to open file %1").arg(fileName));
        QByteArray contents = file.readAll();
        if (fileName.toLower().endsWith(QL1S(".xml"))) {
            d->raw.reset(loadCohSequenceXml(contents));
        } else if (fileName.toLower().endsWith(QL1S(".aseq"))) {
            d->raw.reset(loadCohSequenceAseq(contents, true));
        } else if (fileName.toLower().endsWith(QL1S(".qic"))) {
            throw ApexStringException(tr("qic files are not supported any more, use aseq instead (%1).").arg(fileName));
        } else {
            throw ApexStringException(tr("Unknown L34 file type."));
        }
    }

    CohIterator iterator(d->raw.data(), false);
    d->codacsStimulus = iterator.hasNext() && dynamic_cast<CohCodacsStimulus*>(iterator.next());
    d->needsMapping = !d->codacsStimulus && CohSequenceMapper(d->raw.data()).needsMapping();
    d->needsChecking = !d->codacsStimulus;
}

CohDataBlock::~CohDataBlock()
{
    delete dataPtr;
}

CohSequence* CohDataBlock::mappedData(data::ApexMap* pMap, float volume) const
{
    E_D(const CohDataBlock);

    QScopedPointer<CohSequence> result;

    result.reset(d->needsMapping ?
            d->map(pMap, volume) : cloneCohSequence(d->raw.data()));

    if (d->needsMapping && CohSequenceMapper(result.data()).needsMapping())
        throw ApexStringException(tr("Unable to completely map stimulus"));

    if (d->needsChecking && !coh::checkCohSequenceLevel(result.data(), d->comfortLevels(pMap)))
        throw ApexStringException(tr("Comfort level exceeded."));

    return result.take();
}

}
}
