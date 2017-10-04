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

#include "common/random.h"

#include <QScopedPointer>

class NoiseGeneratorCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.noisegenerator")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blocksize,
                                                unsigned fs) const;
};

class NoiseGenerator : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    NoiseGenerator(unsigned channels, unsigned blockSize);
    ~NoiseGenerator();

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

    bool deterministic;
    double baseGain;
    double gain;
};

static QScopedPointer<cmn::Random> randomGenerator;

// NoiseGenerator
// ==============================================================

NoiseGenerator::NoiseGenerator(unsigned channels, unsigned blockSize)
    : channels(channels), blockSize(blockSize)
{
}

NoiseGenerator::~NoiseGenerator()
{
}

void NoiseGenerator::resetParameters()
{
    deterministic = false;
    baseGain = 0;
    gain = 0;
}

bool NoiseGenerator::isValidParameter(const QString &type, int channel) const
{
    if (type == QL1S("deterministic") && channel == -1)
        return true;
    if (type == QL1S("basegain") && channel == -1)
        return true;
    if (type == QL1S("gain") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool NoiseGenerator::setParameter(const QString &type, int channel,
                                  const QString &value)
{
    if (!isValidParameter(type, channel))
        return false;

    if (type == QLatin1String("basegain"))
        return parseDouble(baseGain, value, type);

    if (type == QLatin1String("gain"))
        return parseDouble(gain, value, type);

    if (type == QL1S("deterministic")) {
        if (value == QL1S("true")) {
            deterministic = true;
            return true;
        }
        if (value == QL1S("false")) {
            deterministic = false;
            return true;
        }
        setErrorMessage(
            QString::fromLatin1("Unable to convert deterministic value %1")
                .arg(value));
        return false;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool NoiseGenerator::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);

    if (!randomGenerator && deterministic)
        randomGenerator.reset(new cmn::Random(0));
    else if (!randomGenerator)
        randomGenerator.reset(new cmn::Random());

    return true;
}

void NoiseGenerator::process(double *const *data)
{
    const double gainMult = std::pow(10.0, (gain + baseGain) / 20);
    for (unsigned channel = 0; channel < channels; ++channel)
        for (unsigned i = 0; i < blockSize; ++i)
            data[channel][i] = randomGenerator->nextGaussian() * gainMult;
}

bool NoiseGenerator::parseDouble(double &result, const QString &value,
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

// NoiseGeneratorCreator==================================================

QStringList NoiseGeneratorCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("noisegenerator");
}

PluginFilterInterface *
NoiseGeneratorCreator::createFilter(const QString &name, unsigned channels,
                                    unsigned size, unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    if (name == QLatin1String("noisegenerator"))
        return new NoiseGenerator(channels, size);

    return NULL;
}

#include "noisegenerator.moc"
