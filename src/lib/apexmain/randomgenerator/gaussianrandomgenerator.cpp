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

#include "gaussianrandomgenerator.h"

namespace apex {

GaussianRandomGenerator::GaussianRandomGenerator(data::RandomGeneratorParameters* p_param)
 : RandomGenerator(p_param)
{
}


GaussianRandomGenerator::~GaussianRandomGenerator()
{
}


QString GaussianRandomGenerator::GetNextValue() {

        Q_ASSERT(0 && "not implemented");
        /*if (m_param->m_nValueType==RandomGeneratorParameters::VALUE_INT) {
                int result = (int) ApexTools::RandomRange((int)m_param->m_dMin, (int)m_param->m_dMax);
                return QString::number(result);
        } else if (m_param->m_nValueType==RandomGeneratorParameters::VALUE_DOUBLE) {
                double result = (double) ApexTools::RandomRange(m_param->m_dMin, m_param->m_dMax);
                return QString::number(result);
        } else {
                qCDebug(APEX_RS, "Error: not implemented");
        }*/

  return QString();

}

void GaussianRandomGenerator::doDeterministicGeneration()
{

}

}
