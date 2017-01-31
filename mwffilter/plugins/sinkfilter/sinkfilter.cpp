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

#include <sndfile.h>

#include <QDateTime>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QUrl>
#include <QVector>
#include <QDebug>

class FileSinkFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.sinkfilter")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blockSize, unsigned fs) const;
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2 (sinkfilter, FileSinkFilterCreator)
#endif

class FileSinkFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    FileSinkFilter (unsigned channels, unsigned blockSize, unsigned sampleRate);
    ~FileSinkFilter();

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

    unsigned count;
    SNDFILE *file;
    QString filePath;
    QString format;
    QMap<QString, QString> parameters;
    QMap<QString, int> formats;
    QVector<double> scratch;
};


// FileSinkFilter ==============================================================

FileSinkFilter::FileSinkFilter (unsigned channels, unsigned blockSize,
        unsigned sampleRate) :
    channels (channels),
    blockSize (blockSize),
    sampleRate (sampleRate),
    count (0),
    file (NULL),
    scratch (channels * blockSize)
{
    formats.insert (QLatin1String("pcm8"), SF_FORMAT_PCM_S8);
    formats.insert (QLatin1String("pcm16"), SF_FORMAT_PCM_16);
    formats.insert (QLatin1String("pcm24"), SF_FORMAT_PCM_24);
    formats.insert (QLatin1String("pcm32"), SF_FORMAT_PCM_32);
    formats.insert (QLatin1String("float"), SF_FORMAT_FLOAT);
    formats.insert (QLatin1String("double"), SF_FORMAT_DOUBLE);

    resetParameters();
}

FileSinkFilter::~FileSinkFilter()
{
    if (file)
        sf_close (file);
}

void FileSinkFilter::resetParameters()
{
    filePath.clear();
    format = QLatin1String("float");
    parameters.clear();
}

bool FileSinkFilter::isValidParameter (const QString &type, int channel) const
{
    if (type == QLatin1String("uri") && channel == -1)
        return true;
    if (type == QLatin1String("format") && channel == -1)
        return true;

    // All other parameters go to the parameters array
    if (channel == -1)
        return true;

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool FileSinkFilter::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type == QLatin1String("uri") && channel == -1) {
        filePath = QUrl (value).path();
        return true;
    }
    if (type == QLatin1String("format") && channel == -1) {
        format = value;
        return true;
    }

    if (channel == -1) {
        parameters.insert (type, value);
        return true;
    }

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool FileSinkFilter::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);
    try {
        if (file)
            sf_close (file);
        file = NULL;

        QString filePath (this->filePath);
        QMapIterator<QString, QString> i (parameters);
        while (i.hasNext()) {
            i.next();
            filePath.replace (QLatin1String("${") + i.key() + QLatin1String("}"), i.value());
        }
        filePath.replace (QLatin1String("${count}"), QString::number (count));
        filePath.replace (QLatin1String("${timestamp}"), QString::number
                (QDateTime::currentDateTime().toTime_t()));

        SF_INFO fileInfo;
        fileInfo.format = SF_FORMAT_WAV;
        if (!formats.contains (format)) {
            setErrorMessage (QString::fromLatin1("Unknown file format %1").arg (format));
            return false;
        }
        fileInfo.format |= formats.value (format);
        fileInfo.channels = channels;
        fileInfo.samplerate = sampleRate;
        file = sf_open (QFile::encodeName (filePath), SFM_WRITE, &fileInfo);
        if (!file) {
            setErrorMessage (QString::fromLatin1("Unable to open wave file %1 "
                        "for writing").arg (filePath));
            return false;
        }
    } catch (std::exception &e) {
        setErrorMessage (QString::fromLatin1("Unable to instantiate FIR filters: %1")
                .arg (QString::fromLocal8Bit(e.what())));
        return false;
    }

    ++count;
    return true;
}

void FileSinkFilter::process (double * const *data)
{
    Q_ASSERT (file);

    for (unsigned channel = 0; channel < channels; ++channel)
        for (unsigned i = 0; i < blockSize; ++i)
            scratch[i * channels + channel] = data[channel][i];

    sf_writef_double (file, scratch.data(), blockSize);
}

// FileSinkFilterCreator =======================================================

QStringList FileSinkFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("filesink");
}

PluginFilterInterface *FileSinkFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    try {
        if (name == QLatin1String("filesink"))
            return new FileSinkFilter (channels, size, sampleRate);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "sinkfilter.moc"
