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
 
#ifndef APEXUNIFORMRANDOMGENERATOR_H
#define APEXUNIFORMRANDOMGENERATOR_H

#include "randomgenerator.h"

//from apextools
#include "random.h"

namespace apex {

/**
@author Tom Francart,,,
*/
class UniformRandomGenerator : public RandomGenerator
{
    public:

        UniformRandomGenerator(data::RandomGeneratorParameters* p_param);
        ~UniformRandomGenerator();

        QString GetNextValue();
        void doDeterministicGeneration();

    private:

        Random generator;

};

}

#endif
