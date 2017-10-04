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

#include "wavparameters.h"

#include <QDebug>

namespace apex
{

namespace data
{

// class WavFilterParameters

WavFilterParameters::WavFilterParameters() : FilterData()
{
    setValueByType("gain", 0.0);
    setValueByType("basegain", 0.0);
    setValueByType("invertgain", false);
}

WavFilterParameters::~WavFilterParameters()
{
}

double WavFilterParameters::gain() const
{
    return valueByType("gain").toDouble();
}

double WavFilterParameters::baseGain() const
{
    return valueByType("basegain").toDouble();
}

bool WavFilterParameters::invertGain() const
{
    return valueByType("invertgain").toBool();
}

// class WavFaderParameters

WavFaderParameters::WavFaderParameters() : WavFilterParameters()
{
    removeParameter(parameterByType("gain"));
    removeParameter(parameterByType("basegain"));
    removeParameter(parameterByType("invertgain"));
    setValueByType("length", 0);
    setValueByType("type", "cosine");
    setValueByType("direction", "in");
}

WavFaderParameters::~WavFaderParameters()
{
}

WavFaderParameters::FadeType WavFaderParameters::type() const
{
    QString type(valueByType(QString("type")).toString());

    if (type == "linear")
        return LINEAR;
    if (type == "cosine")
        return COSINE;
    return INVALID;
}

double WavFaderParameters::length() const
{
    return valueByType(QString("length")).toDouble();
}

bool WavFaderParameters::isFadeIn() const
{
    return valueByType(QString("direction")).toString() == "in";
}

// class WavGeneratorParameters

WavGeneratorParameters::WavGeneratorParameters() : WavFilterParameters()
{
    setValueByType("continuous", "false");
}

WavGeneratorParameters::~WavGeneratorParameters()
{
}

bool WavGeneratorParameters::isContinuous() const
{
    qCDebug(APEX_RS) << QString("iscontinuous: ")
                     << valueByType(QString("continuous")).toString();
    return valueByType(QString("continuous")).toString() == "true";
}

bool WavGeneratorParameters::isGenerator() const
{
    return true;
}

// class SinglePulseGeneratorParameters

SinglePulseGeneratorParameters::SinglePulseGeneratorParameters()
    : WavGeneratorParameters()
{
    setValueByType("pulsewidth", 1);
    setValueByType("polarity", true);
}

SinglePulseGeneratorParameters::~SinglePulseGeneratorParameters()
{
}

unsigned int SinglePulseGeneratorParameters::pulseWidth() const
{
    return valueByType("pulsewidth").toUInt();
}

bool SinglePulseGeneratorParameters::polarity() const
{
    return valueByType("polarity").toString() == "true";
}

// class DataLoopGeneratorParameters

DataLoopGeneratorParameters::DataLoopGeneratorParameters()
    : WavGeneratorParameters()
{
    setValueByType("randomjump", "false");
    setValueByType("jump", 0);
}

DataLoopGeneratorParameters::~DataLoopGeneratorParameters()
{
}

bool DataLoopGeneratorParameters::startsWithRandomJump() const
{
    return valueByType(QString("randomjump")).toString() == "true";
}

double DataLoopGeneratorParameters::jump() const
{
    return valueByType("jump").toDouble();
}

// class SineGeneratorParameters

SineGeneratorParameters::SineGeneratorParameters() : WavGeneratorParameters()
{
    setValueByType("frequency", 1000);
    setValueByType("phase", 0);
}

SineGeneratorParameters::~SineGeneratorParameters()
{
}

double SineGeneratorParameters::frequency() const
{
    return valueByType("frequency").toDouble();
}

double SineGeneratorParameters::phase() const
{
    return valueByType("phase").toDouble();
}

} // ns data
} // ns apex
