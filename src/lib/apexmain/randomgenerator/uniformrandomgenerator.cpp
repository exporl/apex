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

#include "apextools/apextools.h"

#include "uniformrandomgenerator.h"

#include <QtMath>

namespace apex
{

UniformRandomGenerator::UniformRandomGenerator(
    data::RandomGeneratorParameters *p_param)
    : RandomGenerator(p_param)
{
}

UniformRandomGenerator::~UniformRandomGenerator()
{
}

QString UniformRandomGenerator::GetNextValue()
{
    QString sResult;

    if (m_param->m_nValueType == data::RandomGeneratorParameters::VALUE_INT) {
        // int result = (int) ApexTools::RandomRange((int)m_param->m_dMin,
        // (int)m_param->m_dMax);
        // int result = generator.nextDouble(qCeil(m_param->m_dMin),
        // qFloor(m_param->m_dMax) + 1);
        int result = (int)generator.nextUInt(
                         qFloor(m_param->m_dMax - m_param->m_dMin) + 1) +
                     m_param->m_dMin;
        sResult = QString::number(result);
    } else if (m_param->m_nValueType ==
               data::RandomGeneratorParameters::VALUE_DOUBLE) {
        // double result = (double) ApexTools::RandomRange(m_param->m_dMin,
        // m_param->m_dMax);
        double result = generator.nextDouble(m_param->m_dMin, m_param->m_dMax);
        sResult = QString::number(result);
    } else {
        qFatal("Error: not implemented");
    }

    m_lastValue = sResult;
    return sResult;
}

void UniformRandomGenerator::doDeterministicGeneration()
{
    generator.setSeed(0);
}
}
