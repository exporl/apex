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

#include "apextools/random.h"

#include "apextools/signalprocessing/iirfilter.h"
#include "apextools/signalprocessing/peakfilter.h"

#include <QMap>
#include <QStringList>
#include <QUrl>

#include <cmath>

class ScrambleSpectrumFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.scramblespectrumfilter")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blockSize, unsigned fs) const;
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2 (scramblespectrumfilter, ScrambleSpectrumFilterCreator)
#endif


class ScrambleSpectrumFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    ScrambleSpectrumFilter (unsigned channels, unsigned blockSize, double fs);
    ~ScrambleSpectrumFilter();

    virtual void resetParameters();

    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);

    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    const unsigned channels;
    const unsigned blockSize;
    double fs;              // sample rate

    bool disabled;

    double minfreq;         // hertz
    double maxfreq;         // hertz
    double maxgain;
    double bandwidth;       // octaves

    QList<IirFilter*> filters;
};


// ScrambleSpectrumFilter ======================================================

ScrambleSpectrumFilter::ScrambleSpectrumFilter (
        unsigned channels, unsigned blockSize, double fs) :
    channels (channels),
    blockSize (blockSize),
    fs(fs),
    disabled (false)
{
    resetParameters();
}

ScrambleSpectrumFilter::~ScrambleSpectrumFilter()
{
    qDeleteAll(filters);
    filters.clear();
}



void ScrambleSpectrumFilter::resetParameters()
{
    disabled = false;
    minfreq=50;
    maxfreq=floor(fs/2);
    maxgain=10;
    bandwidth=(double)(1)/3;
}

bool ScrambleSpectrumFilter::isValidParameter (const QString &type, int channel) const
{
    if (type == "range" && channel == -1)
        return true;
    if (type == "minfreq" && channel == -1)
        return true;
     if (type == "maxfreq" && channel == -1)
        return true;
   if (type == "bandwidth" && channel == -1)
        return true;

    setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool ScrambleSpectrumFilter::setParameter (const QString &type, int channel,
        const QString &value)
{
    bool ok(true);
    if (type == "range" && channel == -1) {
        maxgain=value.toDouble(&ok);
        if (!ok)
            return false;
        return true;
    }

    if (type == "minfreq" && channel == -1) {
        minfreq=value.toDouble(&ok);
        if (!ok)
            return false;
        return true;
    }

    if (type == "maxfreq" && channel == -1) {
        maxfreq=value.toDouble(&ok);
        if (!ok)
            return false;
        return true;
    }

   if (type == "bandwidth" && channel == -1) {
       double oldvalue=bandwidth;
        bandwidth=value.toDouble(&ok);
        qCDebug(APEX_RS, "set bandwidth to %f", bandwidth);
        if (!ok)
            return false;
        if (bandwidth<=0) {
             setErrorMessage("Bandwidth must be >0");
             bandwidth=oldvalue;
             return false;
        }

        return true;
    }

    if (type == "disabled" && channel == -1) {
        disabled = value == "true" || value == "1" ||
                   value == "yes"  || value == "disabled";
        return true;
    }

    /*setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));*/
    return false;
}

bool ScrambleSpectrumFilter::prepare (unsigned numberOfFrames)
{
    qCDebug(APEX_RS, "ScrambleSpectrumFilter::prepare ");
    Q_UNUSED(numberOfFrames);
    Q_ASSERT(bandwidth>0);
    // Create peak filters per critical band
    qDeleteAll(filters);
    filters.clear();


    double fact=pow(2,bandwidth);
    apex::Random random;

    double oldfreq=minfreq;
    double newfreq=oldfreq*fact;

    QVector<double> B;
    QVector<double> A;
    B.resize(3);
    A.resize(3);
    int count=0;
    while (newfreq<maxfreq) {
       double gain=random.nextDouble(-maxgain,maxgain);

        // Calculate peak filter
        apex::PeakFilter::peakFilter(B,A,gain,
                (oldfreq+newfreq)/2, newfreq-oldfreq,fs);

        IirFilter* filter = new IirFilter;
        filter->setCoefficients(B,A);
        filters.push_back( filter  );

        oldfreq=newfreq;
        newfreq=newfreq*fact;
        ++count;

//        qCDebug(APEX_RS, "newfreq=%f", newfreq);
    }
    qCDebug(APEX_RS, "Created %d filters", count);

    return true;
}



void ScrambleSpectrumFilter::process (double * const *data)
{
    if (disabled)
        return;

    Q_FOREACH(IirFilter* filter, filters) {

        for (unsigned channel = 0; channel < channels; ++channel)
        filter->process (data[channel], blockSize);

    }
}

// ScrambleSpectrumFilterCreator ===============================================

QStringList ScrambleSpectrumFilterCreator::availablePlugins() const
{
    return QStringList() << "scramblespectrum";
}

PluginFilterInterface *ScrambleSpectrumFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    Q_UNUSED (sampleRate);

    try {
        if (name == "scramblespectrum")
            return new ScrambleSpectrumFilter (channels, size, sampleRate);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "scramblespectrumfilter.moc"
