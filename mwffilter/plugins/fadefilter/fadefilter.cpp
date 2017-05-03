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

class FadeFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.fadefilter")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blocksize, unsigned fs) const;
};

class FadeFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    FadeFilter (unsigned channels, unsigned blockSize, unsigned sampleRate);
    ~FadeFilter();

    virtual void resetParameters();
    virtual bool isValidParameter (const QString &name, int channel) const;
    virtual bool setParameter (const QString &name, int channel,
            const QString &value);

    virtual bool prepare (unsigned numberOfFrames);
    virtual void process (double * const *data);

private:
    unsigned channels;
    unsigned blockSize;
    unsigned sampleRate;

    bool disabled;
    unsigned fadeInDuration;
    unsigned fadeOutDuration;

    unsigned totalFrames;
    unsigned position;
};

#include "fadefilter.moc"

// FadeFilterCreator ===========================================================

QStringList FadeFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("fade");
}

PluginFilterInterface *FadeFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    try {
        if (name == QLatin1String("fade"))
            return new FadeFilter (channels, size, sampleRate);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

// FadeFilter ==================================================================

FadeFilter::FadeFilter (unsigned channels, unsigned blockSize,
        unsigned sampleRate) :
    channels (channels),
    blockSize (blockSize),
    sampleRate (sampleRate)
{
    resetParameters();
}

FadeFilter::~FadeFilter()
{
}

void FadeFilter::resetParameters()
{
    disabled = false;
    fadeInDuration = unsigned (0.5 * sampleRate);
    fadeOutDuration = 0;
}

bool FadeFilter::isValidParameter (const QString &name, int channel) const
{
    if (name == QLatin1String("fadein") && channel == -1)
        return true;
    if (name == QLatin1String("fadeout") && channel == -1)
        return true;
    if (name == QLatin1String("disabled") && channel == -1)
        return true;

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (name));
    return false;
}

bool FadeFilter::setParameter (const QString &name, int channel,
        const QString &value)
{
    if (name == QLatin1String("fadein") && channel == -1) {
        bool ok;
        fadeInDuration = unsigned (sampleRate * value.toDouble (&ok));
        if (!ok) {
            setErrorMessage (QString::fromLatin1("Unable to convert fadein duration %1")
                    .arg (value));
            return false;
        }

        return true;
    }
    if (name == QLatin1String("fadeout") && channel == -1) {
        bool ok;
        fadeOutDuration = unsigned (sampleRate * value.toDouble (&ok));
        if (!ok) {
            setErrorMessage (QString::fromLatin1("Unable to convert fadeout value %1")
                    .arg (value));
            return false;
        }

        return true;
    }
    if (name == QLatin1String("disabled") && channel == -1) {
        disabled = value == QLatin1String("true") || value == QLatin1String("1") ||
                   value == QLatin1String("yes")  || value == QLatin1String("disabled");
        return true;
    }

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (name));
    return false;
}

bool FadeFilter::prepare (unsigned numberOfFrames)
{
    position = 0;
    totalFrames = numberOfFrames;
    fadeInDuration = qMin (totalFrames, fadeInDuration);
    fadeOutDuration = qMin (totalFrames, fadeOutDuration);
    return true;
}

void FadeFilter::process (double * const *data)
{
    if (disabled) {
        position += blockSize;
        return;
    }

    if (position < fadeInDuration) {
        const unsigned max = qMin (fadeInDuration - position, blockSize);
        for (unsigned channel = 0; channel < channels; ++channel)
            for (unsigned i = 0; i < max; ++i)
                data[channel][i] *= double (position + i) / fadeInDuration;
    }

    if (position + blockSize > totalFrames - fadeOutDuration) {
        const unsigned min = blockSize - qMin (position + blockSize +
                fadeOutDuration - totalFrames, blockSize);
        const unsigned max = qMin (position + blockSize, totalFrames) - position;
        for (unsigned channel = 0; channel < channels; ++channel)
            for (unsigned i = min; i < max; ++i) {
                data[channel][i] *= double (totalFrames - position - i) /
                    fadeOutDuration;
            }
    }

    position += blockSize;
}
