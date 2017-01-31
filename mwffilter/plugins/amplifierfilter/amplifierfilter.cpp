/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include <QStringList>

#include <cmath>

class AmplifierFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.amplifierfilter")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blocksize, unsigned fs) const;
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2 (amplifierfilter, AmplifierFilterCreator)
#endif

class AmplifierFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    AmplifierFilter (unsigned channels, unsigned blockSize);
    ~AmplifierFilter();

    virtual void resetParameters();
    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);
    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    unsigned channels;
    unsigned blockSize;

    bool disabled;
    // gains in dB
    double baseGain;
    double gain;
};


// AmplifierFilter =============================================================

AmplifierFilter::AmplifierFilter (unsigned channels, unsigned blockSize) :
    channels (channels),
    blockSize (blockSize)
{
    resetParameters();
}

AmplifierFilter::~AmplifierFilter()
{
}

void AmplifierFilter::resetParameters()
{
    disabled = false;
    baseGain = 0;
    gain = 0;
}

bool AmplifierFilter::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);
    return true;
}

bool AmplifierFilter::isValidParameter (const QString &type, int channel) const
{
    if (type == QLatin1String("gain") && channel == -1)
        return true;
    if (type == QLatin1String("basegain") && channel == -1)
        return true;
    if (type == QLatin1String("disabled") && channel == -1)
        return true;

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool AmplifierFilter::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type == QLatin1String("gain") && channel == -1) {
        bool ok;
        gain = value.toDouble (&ok);
        if (!ok) {
            setErrorMessage (QString::fromLatin1("Unable to convert gain value %1")
                    .arg (value));
            return false;
        }

        return true;
    }
    if (type == QLatin1String("basegain") && channel == -1) {
        bool ok;
        baseGain = value.toDouble (&ok);
        if (!ok) {
            setErrorMessage (QString::fromLatin1("Unable to convert gain value %1")
                    .arg (value));
            return false;
        }

        return true;
    }
    if (type == QLatin1String("disabled") && channel == -1) {
        disabled = value == QLatin1String("true") || value == QLatin1String("1") ||
                   value == QLatin1String("yes")  || value == QLatin1String("disabled");
        return true;
    }

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

void AmplifierFilter::process (double * const *data)
{
    if (disabled)
        return;

    const double gainMult = std::pow (10.0, (gain + baseGain) / 20);
    for (unsigned channel = 0; channel < channels; ++channel)
        for (unsigned i = 0; i < blockSize; ++i)
            data[channel][i] *= gainMult;
}

// AmplifierFilterCreator ======================================================

QStringList AmplifierFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("amplifier");
}

PluginFilterInterface *AmplifierFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    Q_UNUSED (sampleRate);

    if (name == QLatin1String("amplifier"))
        return new AmplifierFilter (channels, size);

    return NULL;
}

#include "amplifierfilter.moc"
