/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "apexmain/filter/pluginfilterinterface.h"

#include "apextools/global.h"

#include <cmath>

#include <QMap>

class SineGeneratorCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.sinegenerator")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blocksize,
                                                unsigned fs) const;
};

class SineGenerator : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    SineGenerator(unsigned channels, unsigned blockSize, unsigned sampleRate);
    ~SineGenerator();

    virtual void resetParameters();
    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);
    virtual void process(double *const *data);

private:
    bool parseDouble(double &result, const QString &value, const QString &type);

    const unsigned channels;
    const unsigned blockSize;
    const unsigned sampleRate;

    QString id;
    double frequency;
    double baseGain;
    double gain;
    double phase;
};

static QMap<QString, double> sineTimes;

// SineGenerator
// ==============================================================

SineGenerator::SineGenerator(unsigned channels, unsigned blockSize,
                             unsigned sampleRate)
    : channels(channels), blockSize(blockSize), sampleRate(sampleRate)
{
    resetParameters();
}

SineGenerator::~SineGenerator()
{
}

void SineGenerator::resetParameters()
{
    frequency = 1000.0;
    phase = 0.0;
    baseGain = 0.0;
    gain = 0.0;
}

bool SineGenerator::isValidParameter(const QString &type, int channel) const
{
    if (type == QL1S("frequency") && channel == -1)
        return true;
    if (type == QL1S("phase") && channel == -1)
        return true;
    if (type == QL1S("basegain") && channel == -1)
        return true;
    if (type == QL1S("gain") && channel == -1)
        return true;
    if (type == QL1S("id") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool SineGenerator::setParameter(const QString &type, int channel,
                                 const QString &value)
{
    if (!isValidParameter(type, channel))
        return false;

    if (type == QL1S("frequency")) {
        bool ok = parseDouble(frequency, value, type);
        if (ok)
            frequency = frequency * M_PI * 2.0;
        return ok;
    }

    if (type == QL1S("phase"))
        return parseDouble(phase, value, type);

    if (type == QL1S("basegain"))
        return parseDouble(baseGain, value, type);

    if (type == QL1S("gain"))
        return parseDouble(gain, value, type);

    if (type == QL1S("id")) {
        id = value;
        return true;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool SineGenerator::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    if (!sineTimes.contains(id))
        sineTimes[id] = 0.0;
    return true;
}

void SineGenerator::process(double *const *data)
{
    const double gainMult = std::pow(10.0, (gain + baseGain) / 20);
    for (unsigned i = 0; i < blockSize; ++i) {
        sineTimes[id]++;
        const double t = sineTimes[id] / sampleRate;
        const double d = gainMult * sin(frequency * t + phase);
        for (unsigned channel = 0; channel < channels; ++channel) {
            data[channel][i] = d;
        }
    }
}

bool SineGenerator::parseDouble(double &result, const QString &value,
                                const QString &type)
{
    bool ok;
    double parsedValue = value.toDouble(&ok);
    if (ok)
        result = parsedValue;
    else
        setErrorMessage(
            QString::fromLatin1("Unable to parse value %1 for parameter %2")
                .arg(value)
                .arg(type));
    return ok;
}

// SineGeneratorCreator==================================================

QStringList SineGeneratorCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("sinegenerator");
}

PluginFilterInterface *
SineGeneratorCreator::createFilter(const QString &name, unsigned channels,
                                   unsigned size, unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    if (name == QLatin1String("sinegenerator"))
        return new SineGenerator(channels, size, sampleRate);

    return NULL;
}

#include "sinegenerator.moc"
