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

#include "apexdata/procedure/proceduredata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"

#include "correctorparser.h"
#include "proceduredataparser.h"

#include <QStringList>

namespace apex
{

namespace parser
{

void ProcedureDataParser::Parse(const QDomElement &p_base, data::ProcedureData* p_data)
{
    for (QDomElement currentNode = p_base.firstChildElement(); !currentNode.isNull();
            currentNode = currentNode.nextSiblingElement()) {
        QString tag   = currentNode.tagName();
        QString id    = currentNode.attribute(XMLKeys::gc_sID);
        QString value = currentNode.text();

        bool result = SetParameter(tag, id, value, currentNode, p_data);

        if (!result)
            qCDebug(APEX_RS, "ApexParametersParser: could not parse tag %s", qPrintable(tag));
    }
}

bool ProcedureDataParser::SetParameter(const QString &p_name, const QString &id,
        const QString &p_value, const QDomElement &d, data::ProcedureData* data)
{
    Q_UNUSED(id);
    Q_ASSERT(data);

    if ( p_name == "presentations") {
        data->setPresentations(p_value.toInt());
    } else if ( p_name == "skip") {
        data->setSkip(p_value.toInt());
    } else if (p_name == "intervals") {
        QString count = d.attribute(QSL("count"));
        data->setNumberOfChoices(count.toInt());

        QString select = d.attribute(QSL("select"));
        if (!select.isEmpty()) {
            QStringList slist = select.split(",");
            for (QStringList::const_iterator constIterator = slist.constBegin();
                    constIterator != slist.constEnd(); ++constIterator) {
                int pos;
                bool ok;
                pos=(*constIterator).toInt(&ok);
                if (!ok) throw ApexStringException("Part of choices/select not integer");
//                if (pos>param->m_choices.nChoices)
//                    throw(ApexStringException("Element of choices/select can not be > choices itself"));
                data->addStimulusInterval(pos-1);
            }
        }

        for (QDomElement alternative = d.firstChildElement(); !alternative.isNull();
                alternative = alternative.nextSiblingElement()) {
            Q_ASSERT(alternative.tagName() == "interval");

            QString number = alternative.attribute(QSL("number"));
            QString value = alternative.attribute(QSL("element"));

            Q_ASSERT(!number.isEmpty());
            Q_ASSERT(!value.isEmpty());

            bool ok;
            int interval = number.toInt(&ok) - 1;

            Q_ASSERT(ok);
            Q_ASSERT(interval >= 0);

            data->setInterval(interval, value);
        }

        if (!data->choices().isValid()) {
             throw ApexStringException(tr("procedure/choices is invalid. "
                "Check whether each interval has an associated screen element"));
        }
    } else if(p_name == "corrector") {
        if(data->choices().hasMultipleIntervals()) {
            throw ApexStringException(tr("Corrector specified with >= 2 alternatives. This is probably unwanted."));
        }
        data->setCorrectorData(CorrectorParser().Parse(d));
    } else if ( p_name == "defaultstandard") {
        data->setDefaultStandard(p_value);
    } else if ( p_name == "rev_for_mean") {
        // NOP
    } else if ( p_name == "order") {
        if ( p_value=="random")
            data->setOrder(data::ProcedureData::RandomOrder);
        else if ( p_value=="sequential")
            data->setOrder(data::ProcedureData::SequentialOrder);
        else if ( p_value=="onebyone")      // only for multiprocedure
            data->setOrder(data::ProcedureData::OneByOneOrder);
        else
            throw ApexStringException("ProcedureDataParser::SetParameter: unknown order");

    } else if (p_name == "input_during_stimulus") {
        data->setInputDuringStimulus(QVariant(p_value).toBool());
    } else if (p_name == "pause_between_stimuli") {
        data->setPauseBetweenStimuli(p_value.toInt());
    } else if (p_name == "time_before_first_trial") {
        data->setTimeBeforeFirstStimulus(p_value.toDouble());
    } else if (p_name == "uniquestandard") {
        data->setUniqueStandard(QVariant(p_value).toBool());
    } else {
        return false;
    }
    return true;
}

}
}
