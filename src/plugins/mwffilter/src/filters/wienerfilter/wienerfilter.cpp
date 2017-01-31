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

#include "syllib/firfilter.h"

#include "pluginfilterinterface.h"

#include <QMap>
#include <QStringList>
#include <QUrl>

using namespace syl;

class WienerFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blockSize, unsigned fs) const;
};

Q_EXPORT_PLUGIN2 (wienerfilter, WienerFilterCreator)

class WienerFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    WienerFilter (unsigned channels, unsigned blockSize);
    ~WienerFilter();

    virtual void resetParameters();

    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);

    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    const unsigned channels;
    const unsigned blockSize;

    QString filePath;
    QList<FirFilter> filters;
    unsigned limit;
    bool disabled;
    QMap<QString, QString> parameters;
};


// WienerFilter ================================================================

WienerFilter::WienerFilter (unsigned channels, unsigned blockSize) :
    channels (channels),
    blockSize (blockSize),
    disabled (false)
{
    resetParameters();
}

WienerFilter::~WienerFilter()
{
}

void WienerFilter::resetParameters()
{
    filePath.clear();
    limit = 0;
    disabled = false;
    parameters.clear();
}

bool WienerFilter::isValidParameter (const QString &type, int channel) const
{
    if (type == "uri" && channel == -1)
        return true;
    if (type == "limit" && channel == -1)
        return true;
    if (type == "disabled" && channel == -1)
        return true;

    // All other parameters go to the parameters array
    if (channel == -1)
        return true;

    setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool WienerFilter::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type == "uri" && channel == -1) {
        filePath = QUrl (value).path();
        return true;
    }
    if (type == "limit" && channel == -1) {
        limit = value.toUInt();
        return true;
    }
    if (type == "disabled" && channel == -1) {
        disabled = value == "true" || value == "1" ||
                   value == "yes"  || value == "disabled";
        return true;
    }

    if (channel == -1) {
        parameters.insert (type, value);
        return true;
    }

    setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool WienerFilter::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);
    try {
        QString filePath (this->filePath);
        QMapIterator<QString, QString> i (parameters);
        while (i.hasNext()) {
            i.next();
            filePath.replace ("${" + i.key() + "}", i.value());
        }
        filters.clear();
        Q_FOREACH (const QVector<double> &taps,
                FirFilter::load (filePath, limit)) {
            FirFilter filter (taps, true);
            filters.append (filter);
            if (blockSize % filter.length() != 0) {
                setErrorMessage (QString ("Blocksize %1 is not multiple of FIR"
                    " filter length %2").arg (blockSize).arg (filter.length()));
                return false;
            }
        }
    } catch (std::exception &e) {
        setErrorMessage (QString ("Unable to instantiate FIR filter: %1")
                .arg (e.what()));
        return false;
    }

    if (unsigned (filters.size()) != channels) {
        setErrorMessage (QString ("Number of channels differ: plugin has %1 "
            "channels, FIR filter %2").arg (channels).arg (filters.size()));
        return false;
    }

    return true;
}

void WienerFilter::process (double * const *data)
{
    if (disabled)
        return;

    for (unsigned channel = 0; channel < channels; ++channel)
        filters[channel].process (data[channel], blockSize);

    for (unsigned channel = 1; channel < channels; ++channel)
        for (unsigned i = 0; i < blockSize; ++i)
            data[0][i] += data[channel][i];

    for (unsigned channel = 1; channel < channels; ++channel)
        memset (data[channel], 0, blockSize * sizeof (double));
}

// WienerFilterCreator =========================================================

QStringList WienerFilterCreator::availablePlugins() const
{
    return QStringList() << "wiener";
}

PluginFilterInterface *WienerFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    Q_UNUSED (sampleRate);

    try {
        if (name == "wiener")
            return new WienerFilter (channels, size);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "wienerfilter.moc"
