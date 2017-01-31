/******************************************************************************
 * Copyright (C) 2008  Tom Francart                                           *
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

#include "pluginfilterinterface.h"

#include <QStringList>

#ifdef Q_OS_WIN32
#include <windows.h>            // Sleep()
#else
#include <unistd.h>
#endif

void milliSleep (unsigned millis)
{
#ifdef Q_OS_WIN32
    Sleep (millis);
#else
    usleep (1000 * millis);
#endif
}

class DelayFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blocksize, unsigned fs) const;
};

Q_EXPORT_PLUGIN2 (delayfilter, DelayFilterCreator)

class DelayFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    DelayFilter (unsigned channels, unsigned blockSize);
    ~DelayFilter();

    virtual void resetParameters();
    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);
    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    unsigned channels;
    unsigned blockSize;

    int delay;          // delay in ms
};


// DelayFilter =============================================================

DelayFilter::DelayFilter (unsigned channels, unsigned blockSize) :
    channels (channels),
    blockSize (blockSize),
    delay(0)
{
    resetParameters();

    // Connect with delay server
}

DelayFilter::~DelayFilter()
{
}

void DelayFilter::resetParameters()
{
    delay=0;
}

bool DelayFilter::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);

    return true;
}

bool DelayFilter::isValidParameter (const QString &type, int channel) const
{
    if (type == "delay" && channel == -1)
        return true;

    return false;
}

bool DelayFilter::setParameter (const QString &type, int channel,
        const QString &value)
{

    if (type=="delay" && channel==-1) {
        delay=value.toInt();
        return true;
    }

    setErrorMessage (QString ("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

void DelayFilter::process (double * const *data)
{
    Q_UNUSED(data);
    milliSleep(delay);
}

// DelayFilterCreator ======================================================

QStringList DelayFilterCreator::availablePlugins() const
{
    return QStringList() << "delayfilter";
}

PluginFilterInterface *DelayFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    Q_UNUSED (sampleRate);

    if (name == "delayfilter")
        return new DelayFilter (channels, size);

    return NULL;
}

#include "delayfilter.moc"
