/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "dummysoundcard.h"

#include "apextools/global.h"

#include <QDebug>
#include <QTimer>
#include <QVector>

namespace streamapp
{

class DummyStreamProcessor : public QObject
{
    Q_OBJECT
public:
    DummyStreamProcessor(Callback *callback) : callback(callback)
    {
        timer = new QTimer(this);
        timer->setInterval(10);
        connect(timer, SIGNAL(timeout()), this, SLOT(processStream()));
    }

public Q_SLOTS:
    void startProcessing()
    {
        timer->start();
    }

    void processStream()
    {
        if (callback != nullptr)
            callback->mf_Callback();
    }

    void stopProcessing()
    {
        timer->stop();
        Q_EMIT finished();
    }

Q_SIGNALS:
    void finished();

private:
    Callback *callback;
    QTimer *timer;
};

class DummyReader : public AudioFormatReader
{
public:
    DummyReader(const DummySoundcard *h) : host(h)
    {
    }

    mt_eBitMode mf_eBitMode() const
    {
        return MSBfloat32;
    }
    unsigned long mf_lSampleRate() const
    {
        return host->mf_lGetSampleRate();
    }
    unsigned mf_nChannels() const
    {
        return host->mf_nGetIChan();
    }
    unsigned long Read(void **, const unsigned n)
    {
        return n;
    }

private:
    const DummySoundcard *const host;
};

class DummyWriter : public AudioFormatWriter
{
public:
    DummyWriter(const DummySoundcard *h) : host(h)
    {
    }

    mt_eBitMode mf_eBitMode() const
    {
        return MSBfloat32;
    }
    unsigned long mf_lSampleRate() const
    {
        return host->mf_lGetSampleRate();
    }
    unsigned mf_nChannels() const
    {
        return host->mf_nGetOChan();
    }
    unsigned long Write(const void **, const unsigned n)
    {
        return n;
    }

private:
    const DummySoundcard *const host;
};

DummySoundcard::DummySoundcard(const QString &driverName) : state(Closed)
{
    Q_UNUSED(driverName);
    qCDebug(APEX_THREADS, "Constructing DummySoundcard thread");
    connect(&processingThread, &QObject::destroyed,
            []() { qCDebug(APEX_THREADS, "Destroyed DummySoundcard thread"); });
}

DummySoundcard::~DummySoundcard()
{
    processingThread.quit();
    processingThread.wait();
}

bool DummySoundcard::mp_bOpenDriver(const unsigned nInChannels,
                                    const unsigned nOutChannels,
                                    const unsigned long sampleRate,
                                    const unsigned bufferSize)
{
    if (!mf_bIsOpen()) {
        this->nInChannels = nInChannels;
        this->nOutChannels = nOutChannels;
        this->sampleRate = sampleRate;
        this->bufferSize = bufferSize;
        state = Opened;
        return true;
    }

    return false;
}

bool DummySoundcard::mp_bCloseDriver()
{
    state = Closed;
    return true;
}

bool DummySoundcard::mf_bIsOpen() const
{
    return state == Opened;
}

bool DummySoundcard::mp_bStart(Callback &callback)
{
    if (mf_bIsOpen()) {
        state = Running;
        DummyStreamProcessor *streamProcessor =
            new DummyStreamProcessor(&callback);
        streamProcessor->moveToThread(&processingThread);
        connect(this, SIGNAL(startProcessing()), streamProcessor,
                SLOT(startProcessing()));
        connect(this, SIGNAL(stopProcessing()), streamProcessor,
                SLOT(stopProcessing()));
        connect(streamProcessor, SIGNAL(finished()), streamProcessor,
                SLOT(deleteLater()));
        processingThread.start();

        Q_EMIT startProcessing();
        return true;
    }

    return false;
}

bool DummySoundcard::mp_bStop()
{
    if (mf_bIsRunning()) {
        state = Opened;
        Q_EMIT stopProcessing();
        return true;
    }

    return false;
}

bool DummySoundcard::mf_bIsRunning() const
{
    return state == Running;
}

tSoundCardInfo DummySoundcard::mf_GetInfo() const
{
    tSoundCardInfo ret;

    for (unsigned i = 64; i <= 16384; i *= 2)
        ret.m_BufferSizes.push_back(i);

    ret.m_SampleRates.push_back(44100);
    ret.m_nDefaultBufferSize = 4096;
    ret.m_nMaxOutputChannels = 2;
    ret.m_nMaxInputChannels = 2;

    return ret;
}

unsigned long DummySoundcard::mf_lGetEstimatedLatency() const
{
    qFatal("Estimated latency not implemented in DummySoundcard");
    return 0;
}

unsigned long DummySoundcard::mf_lGetSampleRate() const
{
    if (mf_bIsOpen() || mf_bIsRunning())
        return sampleRate;

    return 0;
}

unsigned DummySoundcard::mf_nGetBufferSize() const
{
    if (mf_bIsOpen() || mf_bIsRunning())
        return bufferSize;

    return 0;
}

unsigned DummySoundcard::mf_nGetOChan() const
{
    if (mf_bIsOpen() || mf_bIsRunning())
        return nOutChannels;

    return 0;
}

unsigned DummySoundcard::mf_nGetIChan() const
{
    if (mf_bIsOpen() || mf_bIsRunning())
        return nInChannels;

    return 0;
}

const std::string &DummySoundcard::mf_sGetLastError() const
{
    return lastError;
}

void DummySoundcard::mp_ClearIOBuffers()
{
}

AudioFormatReader *DummySoundcard::mf_pCreateReader() const
{
    return new DummyReader(this);
}

AudioFormatWriter *DummySoundcard::mf_pCreateWriter() const
{
    return new DummyWriter(this);
}

} // ns streamapp

#include "dummysoundcard.moc"
