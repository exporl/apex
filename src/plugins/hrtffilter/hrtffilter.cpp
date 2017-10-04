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

#include "syllib/firfilter.h"

#include <QLoggingCategory>
#include <QMap>
#include <QStringList>
#include <QUrl>

Q_DECLARE_LOGGING_CATEGORY(APEX_HRTFFILTER)
Q_LOGGING_CATEGORY(APEX_HRTFFILTER, "apex.hrtffilter")

using namespace syl;

class HrtfFilterCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.hrtffilter")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blockSize,
                                                unsigned fs) const;
};

class HrtfFilter : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    HrtfFilter(unsigned channels, unsigned blockSize);
    ~HrtfFilter();

    virtual void resetParameters();

    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);

    virtual void process(double *const *data);

private:
    const unsigned channels;
    const unsigned blockSize;

    QString filePath;
    QString newFilePath;
    QList<FirFilter> filters;
    unsigned limit;
    bool disabled;
    bool buffered; // use filters in "asynchronous" mode?
    QMap<QString, QString> parameters;
};

// HrtfFilter ==================================================================

HrtfFilter::HrtfFilter(unsigned channels, unsigned blockSize)
    : channels(channels), blockSize(blockSize), disabled(false), buffered(false)
{
    resetParameters();
}

HrtfFilter::~HrtfFilter()
{
}

void HrtfFilter::resetParameters()
{
    filePath.clear();
    limit = 0;
    disabled = false;
    buffered = false;
    parameters.clear();
}

bool HrtfFilter::isValidParameter(const QString &type, int channel) const
{
    if (type == QLatin1String("file") && channel == -1)
        return true;
    if (type == QLatin1String("limit") && channel == -1)
        return true;
    if (type == QLatin1String("disabled") && channel == -1)
        return true;
    if (type == QLatin1String("buffered") && channel == -1)
        return true;

    // All other parameters go to the parameters array
    if (channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool HrtfFilter::setParameter(const QString &type, int channel,
                              const QString &value)
{
    if (type == QLatin1String("file") && channel == -1) {
        newFilePath = QUrl(value).path();
        return true;
    }
    if (type == QLatin1String("limit") && channel == -1) {
        limit = value.toUInt();
        return true;
    }
    if (type == QLatin1String("disabled") && channel == -1) {
        disabled =
            value == QLatin1String("true") || value == QLatin1String("1") ||
            value == QLatin1String("yes") || value == QLatin1String("disabled");
        return true;
    }
    if (type == QLatin1String("buffered") && channel == -1) {
        buffered =
            value == QLatin1String("true") || value == QLatin1String("1") ||
            value == QLatin1String("yes") || value == QLatin1String("buffered");
        return true;
    }

    if (channel == -1) {
        parameters.insert(type, value);
        return true;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool HrtfFilter::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        if (filePath != newFilePath) {
            filePath = newFilePath;

            QString filePath(this->filePath);
            QMapIterator<QString, QString> i(parameters);
            while (i.hasNext()) {
                i.next();
                filePath.replace(QLatin1String("${") + i.key() +
                                     QLatin1String("}"),
                                 i.value());
            }
            qCDebug(APEX_HRTFFILTER, "Loading FIR filters from file %s",
                    qPrintable(filePath));
            filters.clear();
            Q_FOREACH (const QVector<double> &taps,
                       FirFilter::load(filePath, limit)) {
                FirFilter filter(taps, !buffered);
                filters.append(filter);
                if (!buffered & (blockSize % filter.length() != 0)) {
                    setErrorMessage(QString::fromLatin1(
                                        "Blocksize %1 is not multiple of FIR"
                                        " filter length %2")
                                        .arg(blockSize)
                                        .arg(filter.length()));
                    return false;
                }
            }
        }

    } catch (std::exception &e) {
        setErrorMessage(
            QString::fromLatin1("Unable to instantiate FIR filters: %1")
                .arg(QString::fromLocal8Bit(e.what())));
        return false;
    }

    if (unsigned(filters.size()) != channels) {
        setErrorMessage(
            QString::fromLatin1("Number of channels differ: plugin has %1 "
                                "channels, FIR filter %2")
                .arg(channels)
                .arg(filters.size()));
        return false;
    }

    return true;
}

void HrtfFilter::process(double *const *data)
{
    if (disabled)
        return;

    for (unsigned channel = 1; channel < channels; ++channel)
        memcpy(data[channel], data[0], blockSize * sizeof(double));
    for (unsigned channel = 0; channel < channels; ++channel)
        filters[channel].process(data[channel], blockSize);
}

// HrtfFilterCreator ===========================================================

QStringList HrtfFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("firfilter");
}

PluginFilterInterface *
HrtfFilterCreator::createFilter(const QString &name, unsigned channels,
                                unsigned size, unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    try {
        if (name == QLatin1String("firfilter"))
            return new HrtfFilter(channels, size);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "hrtffilter.moc"
