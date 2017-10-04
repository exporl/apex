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

#include "syllib/vad.h"

#include <QMap>
#include <QStringList>
#include <QUrl>

#include <cmath>

using namespace syl;

class VadFilterCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.vadfilter")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blockSize,
                                                unsigned fs) const;
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(vadfilter, VadFilterCreator)
#endif

class VadFilter : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    VadFilter(unsigned channels, unsigned blockSize);
    ~VadFilter();

    virtual void resetParameters();

    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);

    virtual void process(double *const *data);

private:
    const unsigned channels;
    const unsigned blockSize;

    QList<VoiceActivityDetector> filters;
    unsigned length;
    bool disabled;
};

// VadFilter ===================================================================

VadFilter::VadFilter(unsigned channels, unsigned blockSize)
    : channels(channels), blockSize(blockSize)
{
    resetParameters();
}

VadFilter::~VadFilter()
{
}

void VadFilter::resetParameters()
{
    length = blockSize;
    disabled = false;
}

bool VadFilter::isValidParameter(const QString &type, int channel) const
{
    if (type == QLatin1String("length") && channel == -1)
        return true;
    if (type == QLatin1String("disabled") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool VadFilter::setParameter(const QString &type, int channel,
                             const QString &value)
{
    if (type == QLatin1String("length") && channel == -1) {
        length = value.toUInt();
        return true;
    }
    if (type == QLatin1String("disabled") && channel == -1) {
        disabled =
            value == QLatin1String("true") || value == QLatin1String("1") ||
            value == QLatin1String("yes") || value == QLatin1String("disabled");
        return true;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool VadFilter::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        VoiceActivityDetector filter(length, true);
        filter.setWeirdRMSLimiting(true);
        filters.clear();
        while (unsigned(filters.size()) < channels)
            filters.append(filter);
    } catch (std::exception &e) {
        setErrorMessage(QString::fromLatin1("Unable to instantiate VAD: %1")
                            .arg(QString::fromLocal8Bit(e.what())));
        return false;
    }

    return true;
}

void VadFilter::process(double *const *data)
{
    if (disabled) {
        for (unsigned channel = 0; channel < channels; ++channel)
            for (unsigned i = 0; i < blockSize; ++i)
                data[channel][i] = 1;
        return;
    }

    for (unsigned channel = 0; channel < channels; ++channel)
        filters[channel].process(data[channel], blockSize);
}

// VadFilterCreator ============================================================

QStringList VadFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("vad");
}

PluginFilterInterface *VadFilterCreator::createFilter(const QString &name,
                                                      unsigned channels,
                                                      unsigned size,
                                                      unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    try {
        if (name == QLatin1String("vad"))
            return new VadFilter(channels, size);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "vadfilter.moc"
