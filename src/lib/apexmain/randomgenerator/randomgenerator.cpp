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

#include "apexdata/randomgenerator/randomgeneratorparameters.h"

#include "randomgenerator.h"

namespace apex {

RandomGenerator::RandomGenerator(data::RandomGeneratorParameters* p_param):
  m_param(p_param)
{
        Q_ASSERT(m_param);
}


RandomGenerator::~RandomGenerator()
{
        if (m_param) delete m_param;
}


QString& RandomGenerator::GetParameter() const {
        return m_param->m_sParam;
}

}
