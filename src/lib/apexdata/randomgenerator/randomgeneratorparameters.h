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

#ifndef _EXPORL_SRC_LIB_APEXDATA_RANDOMGENERATOR_RANDOMGENERATORPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXDATA_RANDOMGENERATOR_RANDOMGENERATORPARAMETERS_H_

#include "../parameters/apexparameters.h"

#include "apextools/global.h"

#include "apextools/xml/xmltools.h"

#include <QString>

namespace apex
{

class RandomGenerator;
class UniformRandomGenerator;
class GaussianRandomGenerator;
class RandomGeneratorFactory;
class ExperimentParser;

namespace data
{

class APEXDATA_EXPORT RandomGeneratorParameters : public data::ApexParameters
{
public:
    RandomGeneratorParameters();
    ~RandomGeneratorParameters();

    virtual bool Parse(const QDomElement &p_paramElement) Q_DECL_OVERRIDE;

    enum { TYPE_UNIFORM, TYPE_GAUSSIAN };
    enum { VALUE_INT, VALUE_DOUBLE };

    bool operator==(const RandomGeneratorParameters &other) const;

protected:
    virtual bool SetParameter(const QString &, const QString &, const QString &,
                              const QDomElement &) Q_DECL_OVERRIDE;

    friend class apex::RandomGenerator;
    friend class apex::UniformRandomGenerator;
    friend class apex::GaussianRandomGenerator;
    friend class apex::RandomGeneratorFactory;
    friend class apex::ExperimentParser;

protected:
    double m_dMin;
    double m_dMax;
    QString m_sParam; // parameter to be set by this generator
    int m_nType;
    int m_nValueType;
};
}
}

#endif
