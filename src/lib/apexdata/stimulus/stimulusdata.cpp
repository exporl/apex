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

#include "apextools/apextools.h"

#include "stimulusdata.h"

apex::data::StimulusData::StimulusData()
{
}

apex::data::StimulusData::StimulusData(const StimulusData& o)
{
    m_description=o.m_description;
    m_fixParams=o.m_fixParams;
    m_varParams=o.m_varParams;
    m_datablocksContainer=o.m_datablocksContainer;
    m_id=o.m_id;
}

const apex::data::StimulusParameters&
        apex::data::StimulusData::GetFixedParameters() const
{
    return m_fixParams;
}

const QString  apex::data::StimulusData::GetID() const
{
    return m_id;
}

const QString apex::data::StimulusData::description() const
{
    return m_description;
}

QString apex::data::StimulusDatablocksContainer::typeName() const
{
    switch (type)
    {
        case DATABLOCKS:
            return "datablocks";
        case DATABLOCK:
            return "datablock";
        case SEQUENTIAL:
            return "sequential";
        case SIMULTANEOUS:
            return "simultaneous";
    }

    Q_ASSERT(false);
    return QString();
}

bool apex::data::StimulusDatablocksContainer::
        operator==(const StimulusDatablocksContainer& other) const
{
    return  ApexTools::haveSameContents(*this, other) &&
            id == other.id &&
            type == other.type;
}

bool apex::data::StimulusData::operator==(const StimulusData & other) const
{
    return  m_description == other.m_description &&
            m_fixParams == other.m_fixParams &&
            m_varParams == other.m_varParams &&
            m_datablocksContainer == other.m_datablocksContainer &&
            m_id == other.m_id;
}

void apex::data::StimulusData::setFixedParameters(
        const StimulusParameters& parameters)
{
    m_fixParams = parameters;
}

void apex::data::StimulusData::setDatablocksContainer(
        const StimulusDatablocksContainer& cont)
{
    m_datablocksContainer = cont;
}

void apex::data::StimulusData::setId(const QString& id)
{
    m_id = id;
}




QStringList apex::data::StimulusDatablocksContainer::toStringList() const
{
    if (type==DATABLOCK)
        return QStringList() << id;

    QStringList result;
    result.append( typeName() + ": " );
    for (const_iterator it=begin(); it!=end(); ++it) {
        QStringList a( it->toStringList() );
        for (QStringList::const_iterator subit=a.begin(); subit!=a.end();
             ++subit) {
            result.append("*" + *subit);
        }
    }

    return result;
}

QString apex::data::StimulusDatablocksContainer::toString() const
{
    return toStringList().join("\n");
}


















