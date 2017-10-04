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

class SinglePulseGeneratorCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.singlepulsegenerator")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blocksize,
                                                unsigned fs) const;
};

class SinglePulseGenerator : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    SinglePulseGenerator(unsigned channels, unsigned blockSize);
    ~SinglePulseGenerator();

    virtual void resetParameters();
    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);
    virtual void process(double *const *data);

private:
    const unsigned channels;
    const unsigned blockSize;
    bool fired;

    unsigned pulseWidth;
    double polarity;
};

// SinglePulseGenerator
// ==============================================================

SinglePulseGenerator::SinglePulseGenerator(unsigned channels,
                                           unsigned blockSize)
    : channels(channels), blockSize(blockSize)
{
    resetParameters();
}

SinglePulseGenerator::~SinglePulseGenerator()
{
}

void SinglePulseGenerator::resetParameters()
{
    fired = false;
    pulseWidth = 1u;
    polarity = 1.0;
}

bool SinglePulseGenerator::isValidParameter(const QString &type,
                                            int channel) const
{
    if (type == QL1S("pulsewidth") && channel == -1)
        return true;
    if (type == QL1S("polarity") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool SinglePulseGenerator::setParameter(const QString &type, int channel,
                                        const QString &value)
{
    if (!isValidParameter(type, channel))
        return false;

    if (type == QL1S("pulsewidth")) {
        bool ok;
        unsigned newPulseWidth = value.toUInt(&ok);
        if (!ok) {
            setErrorMessage(
                QString::fromLatin1("Unable to convert pulsewidth value %1")
                    .arg(value));
            return false;
        }
        if (newPulseWidth > blockSize) {
            setErrorMessage(
                QString::fromLatin1(
                    "Unable to have pulsewidth %1 larger than blocksize %2")
                    .arg(value)
                    .arg(QString::number(blockSize)));
            return false;
        }
        pulseWidth = newPulseWidth;
        return true;
    }

    if (type == QL1S("polarity")) {
        if (value == QL1S("positive")) {
            polarity = 1.0;
            return true;
        }
        if (value == QL1S("negative")) {
            polarity = -1.0;
            return true;
        }
        setErrorMessage(
            QString::fromLatin1("Unable to convert polarity value %1")
                .arg(value));
        return false;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool SinglePulseGenerator::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    fired = false;
    return true;
}

void SinglePulseGenerator::process(double *const *data)
{
    for (unsigned channel = 0; channel < channels; ++channel)
        memset(data[channel], 0, blockSize * sizeof(double));

    if (fired)
        return;
    const double pulse = polarity;
    for (unsigned i = 0; i < pulseWidth; ++i) {
        for (unsigned channel = 0; channel < channels; ++channel) {
            data[channel][i] = pulse;
        }
    }
    fired = true;
}

// SinglePulseGeneratorCreator==================================================

QStringList SinglePulseGeneratorCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("singlepulsegenerator");
}

PluginFilterInterface *
SinglePulseGeneratorCreator::createFilter(const QString &name,
                                          unsigned channels, unsigned size,
                                          unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    if (name == QLatin1String("singlepulsegenerator"))
        return new SinglePulseGenerator(channels, size);

    return NULL;
}

#include "singlepulsegenerator.moc"
