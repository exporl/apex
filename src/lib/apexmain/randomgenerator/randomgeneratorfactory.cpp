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

#include "apexdata/randomgenerator/randomgeneratorparameters.h"

#include "apextools/global.h"

#include "gaussianrandomgenerator.h"
#include "randomgenerator.h"
#include "randomgeneratorfactory.h"
#include "randomgenerators.h"
#include "uniformrandomgenerator.h"

namespace apex
{

RandomGenerators *RandomGeneratorFactory::GetRandomGenerators(
    ExperimentRunDelegate &p_rd,
    QMap<QString, data::RandomGeneratorParameters *> p_param)
{
    RandomGenerators *rgs = new RandomGenerators(p_rd);

    for (QMap<QString, data::RandomGeneratorParameters *>::const_iterator it =
             p_param.begin();
         it != p_param.end(); ++it) {
        RandomGenerator *rg = NULL;
        qCDebug(APEX_RS, "Creating random generator with id=%s",
                qPrintable(it.key()));

        switch (it.value()->m_nType) {
        case data::RandomGeneratorParameters::TYPE_UNIFORM:
            rg = new UniformRandomGenerator(it.value());
            break;
        case data::RandomGeneratorParameters::TYPE_GAUSSIAN:
            rg = new GaussianRandomGenerator(it.value());
            break;
        default:
            qCCritical(APEX_RS, "Invalid random generator type");
            continue;
        }

        rgs->AddGenerator(it.key(), rg);
    }
    return rgs;
}
}
