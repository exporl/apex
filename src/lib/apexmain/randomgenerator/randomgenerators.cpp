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

#include "randomgenerators.h"
#include "randomgenerator.h"
#include "runner/experimentrundelegate.h"
#include "parameters/parametermanager.h"
#include "../stimulus/stimulus.h"
#include "apexcontrol.h"

//from libdata
#include "stimulus/stimulusparameters.h"

namespace apex
{

RandomGenerators::RandomGenerators(ExperimentRunDelegate& p_rd) :
        ApexModule(p_rd)
{
}

RandomGenerators::~RandomGenerators()
{
    qDeleteAll(m_generators);
}


void RandomGenerators::AddGenerator(QString id, RandomGenerator* p_generator)
{
    m_generators[id] = p_generator;
}

void RandomGenerators::doDeterministicGeneration()
{
    Q_FOREACH (RandomGenerator* generator, m_generators)
        generator->doDeterministicGeneration();
}

void RandomGenerators::ApplyGenerators()
{
    QMap<QString, RandomGenerator*>::const_iterator it;
    for (it = m_generators.begin(); it != m_generators.end(); ++it)
    {
        QString value = it.value()->GetNextValue();
        m_rd.GetParameterManager()->setParameter(it.value()->GetParameter(),
                                                 value, true);

        xmlresults.append(GetXMLString(it.key(),
                                       it.value()->GetParameter(),
                                       it.value()->GetLastValue()));
    }
}

QString RandomGenerators::GetResultXML() const
{
    QString result = "<randomgenerators>\n";

/*    for ( std::map<QString,RandomGenerator*>::const_iterator it =
            m_generators.begin(); it!=m_generators.end(); ++it) {
        QString value = (*it).second->GetLastValue();
        result += GetXMLString( (*it).first,
                                  (*it).second->GetParameter(),
                                    value );
    }*/

    result += xmlresults.join("\n");
    xmlresults.clear();

    result += "\n</randomgenerators>";

    return result;
}

 QString RandomGenerators::GetXMLString(const QString& generator,
                                        const QString& parameter,
                                        const QString& value) const
{
     return "<randomgenerator id=\"" + generator + "\" parameter=\"" +
             parameter + "\">" + value +
             "</randomgenerator>";
}

} // namespace apex

