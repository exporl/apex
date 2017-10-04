/******************************************************************************
 * Copyright (C) 2016 Benjamin Dieudonné <benjamin.dieudonne@kuleuven.be>     *
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
#include "apextools/signalprocessing/iirfilter.h"

#include <QMap>
#include <QStringList>

#include <cmath>
#include <sndfile.h>

Q_DECLARE_LOGGING_CATEGORY(APEX_VOCODERFILTER)
Q_LOGGING_CATEGORY(APEX_VOCODERFILTER, "apex.vocoderfilter")

class VocoderFilterCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.vocoderfilter")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blockSize,
                                                unsigned fs) const;
};

class VocoderFilter : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    VocoderFilter(unsigned blockSize, unsigned sampleRate);
    ~VocoderFilter();

    virtual void resetParameters();

    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);

    virtual void process(double *const *data);

private:
    int vocoderChannels; // number of channels in vocoder filterbank
    const unsigned blockSize;
    const unsigned sampleRate;

    bool disabled;

    QVector<double *> vocoderChannelBuffers;

    QString filterBankFilename;
    QList<IirFilter *> filterBank;

    QString lowpassFilterFilename;
    QList<IirFilter *> lowpassBank;

    QString carriersFilename;
    QVector<double> carriers; // sample at <frame,vocoderChannel>:
                              // carriers[frame*nrVocoderChannels +
                              // vocoderChannel]

    QVector<unsigned int> carrierPositions; // monitor position in time to avoid
                                            // onset clicks for each sound block
                                            // (carriers are loopable)
    unsigned int carrierLength;

    bool newParameters;
};

// VocoderFilter ======================================================

VocoderFilter::VocoderFilter(unsigned blockSize, unsigned sampleRate)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      disabled(false),
      newParameters(true)
{
    resetParameters();
}

VocoderFilter::~VocoderFilter()
{
    qDeleteAll(vocoderChannelBuffers);
    qDeleteAll(lowpassBank);
    qDeleteAll(filterBank);
}

void VocoderFilter::resetParameters()
{
    filterBankFilename.clear();
    lowpassFilterFilename.clear();
    carriersFilename.clear();
    disabled = false;
    newParameters = true;
}

bool VocoderFilter::isValidParameter(const QString &type, int channel) const
{
    if (type == QLatin1String("disabled") && channel == -1)
        return true;
    if (type == QLatin1String("filterbank") && channel == -1)
        return true;
    if (type == QLatin1String("lowpass") && channel == -1)
        return true;
    if (type == QLatin1String("carriers") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool VocoderFilter::setParameter(const QString &type, int channel,
                                 const QString &value)
{
    if (type == QLatin1String("disabled") && channel == -1) {
        disabled =
            value == QLatin1String("true") || value == QLatin1String("1") ||
            value == QLatin1String("yes") || value == QLatin1String("disabled");
        return true;
    }
    if (type == QLatin1String("filterbank") && channel == -1) {
        filterBankFilename = value;
        newParameters = true;
        return true;
    }
    if (type == QLatin1String("lowpass") && channel == -1) {
        lowpassFilterFilename = value;
        newParameters = true;
        return true;
    }
    if (type == QLatin1String("carriers") && channel == -1) {
        carriersFilename = value;
        newParameters = true;
        return true;
    }

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool VocoderFilter::prepare(unsigned numberOfFrames)
{
    qCDebug(APEX_RS, "VocoderFilter::prepare ");
    Q_UNUSED(numberOfFrames);

    if (newParameters) {
        newParameters = false;

        // Read filter coefficients for filter bank from file and store in
        // memory
        qCDebug(APEX_VOCODERFILTER, "Loading IIR filters from file %s",
                qPrintable(filterBankFilename));
        qDeleteAll(filterBank);
        filterBank.clear();
        filterBank = IirFilter::load(filterBankFilename);

        // Initialize vocoderChannelBuffers
        vocoderChannels = filterBank.size();
        for (int vocoderChannel = 0;
             vocoderChannel < vocoderChannelBuffers.size(); ++vocoderChannel) {
            delete[] vocoderChannelBuffers[vocoderChannel];
        }
        vocoderChannelBuffers.clear();
        for (int vocoderChannel = 0; vocoderChannel < vocoderChannels;
             ++vocoderChannel) {
            vocoderChannelBuffers.append(new double[blockSize]);
        }

        // Read low-pass filter coefficients from file and store in memory
        qCDebug(APEX_VOCODERFILTER, "Loading lowpass IIR filter from file %s",
                qPrintable(lowpassFilterFilename));
        qDeleteAll(lowpassBank);
        lowpassBank.clear();
        for (int vocoderChannel = 0; vocoderChannel < vocoderChannels;
             ++vocoderChannel) {
            // beter zou zijn om niet telkens dezelfde file te moeten inlezen
            // (met copy constructor of zo)...
            IirFilter *lowpassFilter =
                IirFilter::load(lowpassFilterFilename)[0];
            lowpassBank.append(lowpassFilter);
        }
        // Read carrier samples from file and store in memory
        qCDebug(APEX_VOCODERFILTER, "Loading carrier samples from file %s",
                qPrintable(carriersFilename));
        QByteArray filePath = QFile::encodeName(carriersFilename);

        SF_INFO readSfinfo;
        readSfinfo.format = 0;
        SNDFILE *carriersFile = sf_open(filePath, SFM_READ, &readSfinfo);
        if (!carriersFile) {
            setErrorMessage(
                QString("Unable to open input file %1: %2")
                    .arg(QFile::decodeName(filePath),
                         QString::fromLocal8Bit(sf_strerror(NULL))));
            return false;
        }
        if (unsigned(readSfinfo.samplerate) != sampleRate) {
            setErrorMessage(
                QString(
                    "Source file %1 has wrong sample rate (%2 instead of %3)")
                    .arg(QFile::decodeName(filePath))
                    .arg(readSfinfo.samplerate)
                    .arg(sampleRate));
            return false;
        }
        if (readSfinfo.channels != vocoderChannels) {
            setErrorMessage(QString("Source file %1 has wrong number of "
                                    "channels (%2 instead of %3)")
                                .arg(QFile::decodeName(filePath))
                                .arg(readSfinfo.channels)
                                .arg(vocoderChannels));
            return false;
        }
        carriers.resize(readSfinfo.channels * readSfinfo.frames);
        sf_readf_double(carriersFile, carriers.data(), readSfinfo.frames);

        carrierLength = readSfinfo.frames;
        carrierPositions.resize(vocoderChannels);

        sf_close(carriersFile);
    }
    return true;
}

void VocoderFilter::process(double *const *data)
{
    qCDebug(APEX_RS, "VocoderFilter::process ");
    if (disabled)
        return;

    for (int vocoderChannel = 0; vocoderChannel < vocoderChannels;
         ++vocoderChannel) {
        // Filter bank
        memcpy(vocoderChannelBuffers[vocoderChannel], *data,
               blockSize * sizeof(double));
        filterBank[vocoderChannel]->process(
            vocoderChannelBuffers[vocoderChannel], blockSize);

        // Half-wave rectification
        for (unsigned int sample = 0; sample < blockSize; ++sample) {
            if (vocoderChannelBuffers[vocoderChannel][sample] < 0)
                vocoderChannelBuffers[vocoderChannel][sample] =
                    -vocoderChannelBuffers[vocoderChannel][sample];
        }

        // Low-pass filter
        lowpassBank[vocoderChannel]->process(
            vocoderChannelBuffers[vocoderChannel], blockSize);

        // Loop carriers and modulate with envelope
        for (unsigned int sample = 0; sample < blockSize; ++sample) {
            unsigned int carrierSample =
                (carrierPositions[vocoderChannel]) * vocoderChannels +
                vocoderChannel;
            vocoderChannelBuffers[vocoderChannel][sample] *=
                carriers[carrierSample];
            carrierPositions[vocoderChannel] =
                (carrierPositions[vocoderChannel] + 1) % carrierLength;
        }

        // Sum together and change data
    }
    memset(*data, 0, blockSize * sizeof(double));
    for (unsigned int sample = 0; sample < blockSize; ++sample) {
        for (int vocoderChannel = 0; vocoderChannel < vocoderChannels;
             ++vocoderChannel) {
            (*data)[sample] += vocoderChannelBuffers[vocoderChannel][sample];
        }
    }
}

// VocoderFilterCreator ===============================================

QStringList VocoderFilterCreator::availablePlugins() const
{
    return QStringList() << "vocoder";
}

PluginFilterInterface *
VocoderFilterCreator::createFilter(const QString &name, unsigned channels,
                                   unsigned size, unsigned sampleRate) const
{
    Q_UNUSED(channels);

    try {
        if (name == "vocoder")
            return new VocoderFilter(size, sampleRate);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "vocoderfilter.moc"
