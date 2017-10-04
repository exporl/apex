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

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "apexparameters.h"

#include <QMap>
#include <QString>

using namespace apex::XMLKeys;
using namespace apex::data;

ApexParameters::const_iterator::const_iterator(
    const QMap<QString, QString>::const_iterator &it)
{
    *this = it;
}

ApexParameters::const_iterator &ApexParameters::const_iterator::
operator=(const QMap<QString, QString>::const_iterator &it)
{
    this->it = it;
    return *this;
}
ApexParameters::const_iterator &ApexParameters::const_iterator::
operator++() // prefix++
{
    it++;
    return *this;
}

ApexParameters::const_iterator ApexParameters::const_iterator::
operator++(int) // postfix++
{
    const_iterator copy(it);
    ++(*this);
    return copy;
}

bool ApexParameters::const_iterator::
operator==(const const_iterator &other) const
{
    return it == other.it;
}

bool ApexParameters::const_iterator::
operator!=(const const_iterator &other) const
{
    return !operator==(other);
}

const QString &ApexParameters::const_iterator::key()
{
    return it.key();
}

const QString &ApexParameters::const_iterator::value()
{
    return it.value();
}

QMap<QString, QString>::const_iterator ApexParameters::begin() const
{
    return parameters.begin();
}

QMap<QString, QString>::const_iterator ApexParameters::end() const
{
    return parameters.end();
}

ApexParameters::ApexParameters()
{
}

ApexParameters::~ApexParameters()
{
}

bool ApexParameters::Parse(const QDomElement &p_paramElement)
{
    m_paramElement = p_paramElement;

    for (QDomElement currentNode = m_paramElement.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        QString tag = currentNode.tagName();
        QString id = currentNode.attribute(gc_sID);
        QString value = currentNode.text();

        SetParameter(tag, id, value, currentNode);
    }

    return true;
}

QString ApexParameters::GetParameter(const QString &p_name) const
{
    return parameters.value(p_name);
}

bool ApexParameters::HasParameter(const QString &p_name) const
{
    return parameters.contains(p_name);
}

/**
 * Add errors/warnings to errorHandler
 * @param errorHandler
 * @return true if no fatal errors
 */
bool ApexParameters::CheckParameters()
{
    return true;
}

bool ApexParameters::operator==(const ApexParameters &other) const
{
    return parameters == other.parameters;
}
