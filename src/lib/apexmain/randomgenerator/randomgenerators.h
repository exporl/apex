/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RANDOMGENERATOR_RANDOMGENERATORS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RANDOMGENERATOR_RANDOMGENERATORS_H_

#include "apexmodule.h"

#include <QMap>
#include <QStringList>

namespace apex
{

class RandomGenerator;

namespace stimulus
{
class Stimulus;
}

/**
@author Tom Francart,,,
*/
class RandomGenerators : public ApexModule
{
public:
    RandomGenerators(ExperimentRunDelegate &p_rd);
    ~RandomGenerators();

    void AddGenerator(QString id, RandomGenerator *p_generator);
    void doDeterministicGeneration();

    /**
     * Send parameters from randomgenerators to parametermanager
     */
    void ApplyGenerators();

    virtual QString GetResultXML() const;

private:
    QString GetXMLString(const QString &generator, const QString &parameter,
                         const QString &value) const;

    QMap<QString, RandomGenerator *> m_generators;
    mutable QStringList xmlresults;
};
}

#endif
