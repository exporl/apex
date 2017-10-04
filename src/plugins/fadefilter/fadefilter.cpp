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

#include "apextools/global.h"

#include <QStringList>

#include <cmath>

class FadeFilterCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.fadefilter")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blocksize,
                                                unsigned fs) const;
};

class FadeFilter : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    FadeFilter(unsigned channels, unsigned blockSize, unsigned sampleRate);
    ~FadeFilter();

    virtual void resetParameters();
    virtual bool isValidParameter(const QString &name, int channel) const;
    virtual bool setParameter(const QString &name, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);
    virtual void process(double *const *data);

private:
    const unsigned channels;
    const unsigned blockSize;
    const unsigned sampleRate;

    unsigned longestRowLength;
    bool disabled;
    unsigned position;
    unsigned length;
    unsigned fadeInDuration;
    unsigned fadeOutDuration;
    QString type;
    QString direction;
};

#include "fadefilter.moc"

// FadeFilterCreator ===========================================================

QStringList FadeFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("fader");
}

PluginFilterInterface *
FadeFilterCreator::createFilter(const QString &name, unsigned channels,
                                unsigned size, unsigned sampleRate) const
{
    try {
        if (name == QLatin1String("fader"))
            return new FadeFilter(channels, size, sampleRate);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

// FadeFilter ==================================================================

FadeFilter::FadeFilter(unsigned channels, unsigned blockSize,
                       unsigned sampleRate)
    : channels(channels), blockSize(blockSize), sampleRate(sampleRate)
{
    resetParameters();
}

FadeFilter::~FadeFilter()
{
}

void FadeFilter::resetParameters()
{
    disabled = false;
    length = 0;
    type = QLatin1String("cosine");
    direction = QLatin1String("in");
}

bool FadeFilter::isValidParameter(const QString &name, int channel) const
{
    if (name == QLatin1String("disabled") && channel == -1)
        return true;
    if (name == QLatin1String("length") && channel == -1)
        return true;
    if (name == QLatin1String("type") && channel == -1)
        return true;
    if (name == QLatin1String("direction") && channel == -1)
        return true;
    if (name == QLatin1String("longestrowlength") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(name));
    return false;
}

bool FadeFilter::setParameter(const QString &name, int channel,
                              const QString &value)
{
    if (!isValidParameter(name, channel))
        return false;

    // length is passed in ms, we need it in samples
    if (name == QLatin1String("length")) {
        bool ok;
        double lengthInMs = value.toDouble(&ok);
        if (!ok) {
            setErrorMessage(
                QString::fromLatin1("Unable to convert length duration %1")
                    .arg(value));
            return false;
        }
        length = (unsigned)((lengthInMs / 1000.0) * sampleRate);

        return true;
    }

    if (name == QLatin1String("longestrowlength")) {
        bool ok;
        longestRowLength = value.toUInt(&ok);
        if (!ok) {
            setErrorMessage(
                QString::fromLatin1("Unable to set longestrow %1").arg(value));
            return false;
        }
    }

    if (name == QLatin1String("type")) {
        if (value != QLatin1String("cosine") &&
            value != QLatin1String("linear")) {
            setErrorMessage(
                QString::fromLatin1("Invalid value %1 for type").arg(value));
            return false;
        }
        type = value;
        return true;
    }

    if (name == QLatin1String("direction")) {
        if (value != QLatin1String("in") && value != QLatin1String("out")) {
            setErrorMessage(
                QString::fromLatin1("Invalid value %1 for direction")
                    .arg(value));
            return false;
        }
        direction = value;
        return true;
    }

    if (name == QLatin1String("disabled")) {
        disabled =
            value == QLatin1String("true") || value == QLatin1String("1") ||
            value == QLatin1String("yes") || value == QLatin1String("disabled");
        return true;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(name));
    return false;
}

bool FadeFilter::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    position = 0;
    if (direction == QLatin1String("in")) {
        fadeInDuration = length;
        fadeOutDuration = 0;
    } else {
        fadeInDuration = 0;
        fadeOutDuration = length;
    }
    return true;
}

void FadeFilter::process(double *const *data)
{
    if (disabled) {
        position += blockSize;
        return;
    }

    if (fadeInDuration != 0 && position < fadeInDuration) {
        const unsigned max = qMin(blockSize, fadeInDuration - position);
        for (unsigned channel = 0; channel < channels; ++channel) {
            for (unsigned i = 0; i < max; ++i) {
                if (type == QLatin1String("linear"))
                    data[channel][i] *= double(position + i) / fadeInDuration;
                else
                    data[channel][i] *=
                        (0.5 * (1.0 - cos(M_PI * (double)(position + i) /
                                          fadeInDuration)));
            }
        }
    }

    if (fadeOutDuration != 0 &&
        longestRowLength - fadeOutDuration < position + blockSize) {
        for (unsigned channel = 0; channel < channels; ++channel) {
            for (unsigned i = 0; i < blockSize; ++i) {
                if (position + i >= longestRowLength - fadeOutDuration) {
                    if (type == QLatin1String("linear"))
                        data[channel][i] *=
                            (double)(longestRowLength - position - i) /
                            (double)fadeOutDuration;
                    else
                        data[channel][i] *=
                            0.5 * (1.0 - cos(M_PI * (double)(longestRowLength -
                                                             position - i) /
                                             fadeOutDuration));
                }
            }
        }
    }
    position += blockSize;
}
