/*******************************************************e**********************
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

#include "syllib/adaptivewiener.h"

#include "pluginfilterinterface.h"

#include <QMap>
#include <QStringList>
#include <QUrl>

using namespace syl;

class AdaptiveWienerFilterCreator :
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

Q_EXPORT_PLUGIN2 (adaptivewienerfilter, AdaptiveWienerFilterCreator)

class AdaptiveWienerFilterPlugin:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    AdaptiveWienerFilterPlugin (unsigned channels, unsigned blockSize,
            unsigned sampleRate);
    ~AdaptiveWienerFilterPlugin();

    virtual void resetParameters();

    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);

    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    const unsigned channels;
    const unsigned blockSize;
    const unsigned sampleRate;

    std::auto_ptr<AdaptiveWienerFilter> filter;
    unsigned length;
    double muInv;
    bool disabled;
};


// AdaptiveWienerFilter ========================================================

AdaptiveWienerFilterPlugin::AdaptiveWienerFilterPlugin (unsigned channels,
        unsigned blockSize, unsigned sampleRate) :
    channels (channels),
    blockSize (blockSize),
    sampleRate (sampleRate)
{
    resetParameters();
}

AdaptiveWienerFilterPlugin::~AdaptiveWienerFilterPlugin()
{
}

void AdaptiveWienerFilterPlugin::resetParameters()
{
    length = blockSize;
    muInv = 0;
    disabled = false;
}

bool AdaptiveWienerFilterPlugin::isValidParameter (const QString &type,
        int channel) const
{
    if (type == "length" && channel == -1)
        return true;
    if (type == "muinv" && channel == -1)
        return true;
    if (type == "disabled" && channel == -1)
        return true;

    setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool AdaptiveWienerFilterPlugin::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type == "length" && channel == -1) {
        length = value.toUInt();
        return true;
    }
    if (type == "muinv" && channel == -1) {
        muInv = value.toDouble();
        return true;
    }
    if (type == "disabled" && channel == -1) {
        disabled = value == "true" || value == "1" ||
                   value == "yes"  || value == "disabled";
        return true;
    }

    setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool AdaptiveWienerFilterPlugin::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);
    try {
        if (channels < 3) {
            setErrorMessage ("Need at least 3 channels");
            return false;
        }
        if (channels % 2 == 0) {
            setErrorMessage ("Need an odd number of channels");
            return false;
        }
        filter.reset (new AdaptiveWienerFilter (sampleRate, (channels - 1) / 2,
                    length, muInv, true));
        if (blockSize % filter->length() != 0) {
            setErrorMessage (QString ("Blocksize %1 is not multiple of Wiener"
                " filter length %2").arg (blockSize).arg (filter->length()));
            return false;
        }
    } catch (std::exception &e) {
        setErrorMessage (QString ("Unable to instantiate Wiener filter: %1")
                .arg (e.what()));
        return false;
    }

    return true;
}

void AdaptiveWienerFilterPlugin::process (double * const *data)
{
    if (disabled)
        return;

    const unsigned signalChannels = (channels - 1) / 2;
    filter->process (data, data + signalChannels, data[2 * signalChannels],
            blockSize);

    for (unsigned channel = 1; channel < channels; ++channel)
        memset (data[channel], 0, blockSize * sizeof (double));
}

// AdaptiveWienerFilterCreator =================================================

QStringList AdaptiveWienerFilterCreator::availablePlugins() const
{
    return QStringList() << "adaptivewiener";
}

PluginFilterInterface *AdaptiveWienerFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    try {
        if (name == "adaptivewiener")
            return new AdaptiveWienerFilterPlugin (channels, size, sampleRate);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "adaptivewienerfilter.moc"
