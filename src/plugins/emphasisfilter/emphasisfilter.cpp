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

#include "syllib/arfilter.h"

#include <QMap>
#include <QStringList>
#include <QUrl>

using namespace syl;

class EmphasisFilterCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.emphasisfilter")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blockSize,
                                                unsigned fs) const;
};

class EmphasisFilter : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    EmphasisFilter(unsigned channels, unsigned blockSize);
    ~EmphasisFilter();

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
    QList<ArFilter> filters;
    unsigned limit;
    bool disabled;
    QMap<QString, QString> parameters;
};

// EmphasisFilter ==============================================================

EmphasisFilter::EmphasisFilter(unsigned channels, unsigned blockSize)
    : channels(channels), blockSize(blockSize), disabled(false)
{
    resetParameters();
}

EmphasisFilter::~EmphasisFilter()
{
}

void EmphasisFilter::resetParameters()
{
    filePath.clear();
    limit = 0;
    disabled = false;
    parameters.clear();
}

bool EmphasisFilter::isValidParameter(const QString &type, int channel) const
{
    if (type == QLatin1String("file") && channel == -1)
        return true;
    if (type == QLatin1String("limit") && channel == -1)
        return true;
    if (type == QLatin1String("disabled") && channel == -1)
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

bool EmphasisFilter::setParameter(const QString &type, int channel,
                                  const QString &value)
{
    if (type == QLatin1String("file") && channel == -1) {
        filePath = QUrl(value).path();
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

bool EmphasisFilter::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        QString filePath(this->filePath);
        QMapIterator<QString, QString> i(parameters);
        while (i.hasNext()) {
            i.next();
            filePath.replace(QLatin1String("${") + i.key() + QLatin1String("}"),
                             i.value());
        }
        const ArFilter filter(ArFilter::load(filePath, limit));
        filters.clear();
        while (unsigned(filters.size()) < channels)
            filters.append(filter);
    } catch (std::exception &e) {
        setErrorMessage(
            QString::fromLatin1("Unable to instantiate FIR filters: %1")
                .arg(QString::fromLocal8Bit(e.what())));
        return false;
    }

    return true;
}

void EmphasisFilter::process(double *const *data)
{
    if (disabled)
        return;

    for (unsigned channel = 0; channel < channels; ++channel)
        filters[channel].process(data[channel], blockSize);
}

// EmphasisFilterCreator =======================================================

QStringList EmphasisFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("emphasis");
}

// empty name creates default plugin, caller must free returned class,
// returns NULL if impossible to create
PluginFilterInterface *
EmphasisFilterCreator::createFilter(const QString &name, unsigned channels,
                                    unsigned size, unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    try {
        if (name == QLatin1String("emphasis"))
            return new EmphasisFilter(channels, size);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "emphasisfilter.moc"
