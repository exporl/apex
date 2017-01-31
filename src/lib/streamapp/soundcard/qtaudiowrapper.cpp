/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "qtaudiowrapper.h"

#include <QAudioOutput>
#include <QVector>
#include <QDebug>

#define SAMPLE_SIZE 4
#define qtAudioDebug() qDebug() << "QtAudioWrapper:"

namespace streamapp
{

//class CallbackBuffer *******************************************************

class CallbackBuffer : public QIODevice
{
        friend QDebug& operator<<(QDebug&, const CallbackBuffer&);

    public:

        CallbackBuffer(Callback* cb, qint64 bufSize);
        ~CallbackBuffer();

    protected:

        qint64 readData(char* data, qint64 maxLen);
        qint64 writeData(const char* data, qint64 len);

    private:

        Callback* callback;
        qint64 bufferSize;
        qint64 bytesInBuffer;
        char* buffer;
};

CallbackBuffer::CallbackBuffer(Callback* cb, qint64 bufSize) :
                            callback(cb),
                            bufferSize(bufSize),
                            bytesInBuffer(0),
                            buffer(bufferSize > 0 ? new char[bufferSize] : 0)
{
}

CallbackBuffer::~CallbackBuffer()
{
    delete[] buffer;
}

qint64 CallbackBuffer::readData(char* data, qint64 maxLen)
{
    //qtAudioDebug() << "reading" << maxLen << "bytes from" << *this;

    if (bufferSize == 0)
        return 0;

    if (bytesInBuffer == 0)
    {
        //qtAudioDebug() << "    calling back";
        callback->mf_Callback();
        //return 0;
    }

    qint64 bytesToRead = qMin(maxLen, bytesInBuffer);
    //qtAudioDebug() << "    started to read" << bytesToRead << "bytes";
    memcpy(data, buffer, bytesToRead);
    bytesInBuffer -= bytesToRead;
    memmove(buffer, buffer + bytesToRead, bytesInBuffer);
    qtAudioDebug() << "read" << bytesToRead << "bytes";
    return bytesToRead;
}

qint64 CallbackBuffer::writeData(const char* data, qint64 len)
{
    //qtAudioDebug() << "writing" << len << "bytes to" << *this;

    if (bufferSize == 0)
        return 0;

    qint64 bytesToWrite = qMin(len, bufferSize - bytesInBuffer);
    memcpy(buffer + bytesInBuffer, data, bytesToWrite);
    bytesInBuffer += bytesToWrite;
    return bytesToWrite;
}

QDebug& operator<<(QDebug& dbg, const CallbackBuffer& buffer)
{
    dbg.nospace() << "CallbackBuffer(bufferSize=" << buffer.bufferSize
                  << ", bytesInBuffer=" << buffer.bytesInBuffer << ")";

    return dbg.space();
}

//class QtAudioWriter ********************************************************

class QtAudioWriter : public AudioFormatWriter
{
    public:

        QtAudioWriter(const QtAudioWrapper* h);

        mt_eBitMode mf_eBitMode() const;
        unsigned long mf_lSampleRate() const;
        unsigned mf_nChannels() const;

        unsigned long Write(const void** buf, const unsigned nSamples);

    private:

        const QtAudioWrapper* const host;
};

QtAudioWriter::QtAudioWriter(const QtAudioWrapper* h) : host(h)
{
    Q_ASSERT(host != 0);
}

AudioFormat::mt_eBitMode QtAudioWriter::mf_eBitMode() const
{
    return MSBfloat32;
}

unsigned long QtAudioWriter::mf_lSampleRate() const
{
    return host->mf_lGetSampleRate();
}

unsigned QtAudioWriter::mf_nChannels() const
{
    return host->mf_nGetOChan();
}

unsigned long QtAudioWriter::Write(const void** buf, const unsigned nSamples)
{
    //qtAudioDebug() << "writing" << nSamples << "samples";
    QIODevice* writeBuffer = host->buffer();

    if (writeBuffer == 0)
        return nSamples;

    int nChannels = host->mf_nGetOChan();
    if (nChannels == 1)
    {
        return writeBuffer->write((char*)buf,
                                  nSamples * SAMPLE_SIZE) / SAMPLE_SIZE;
    }
    else if (nChannels == 2)
    {
        unsigned i;
        float** inBuf = (float**)buf;
        float* outBuf = new float[2 * nSamples];

        for (i = 0; i < nSamples; i++)
        {
            outBuf[2 * i] = inBuf[0][i];
            outBuf[2 * i + 1] = inBuf[1][i];
        }

        qint64 written = writeBuffer->write((char*)outBuf,
                                nSamples * 2 * SAMPLE_SIZE) / (2 * SAMPLE_SIZE);

        delete[] outBuf;
        //qtAudioDebug() << "written" << written << "samples";
        return written;
    }
    else
        qFatal("QtAudioWrapper: unsupported number of channels: %i", nChannels);

    return 0;
}

//class QtAudioReader ********************************************************

class QtAudioReader : public AudioFormatReader
{
    public:

        QtAudioReader(const QtAudioWrapper* h);

        mt_eBitMode mf_eBitMode() const;
        unsigned long mf_lSampleRate() const;
        unsigned mf_nChannels() const;

        unsigned long Read(void** buf, const unsigned nSamples);

    private:

        const QtAudioWrapper* const host;
};

QtAudioReader::QtAudioReader(const QtAudioWrapper* h) : host(h)
{
    Q_ASSERT(host != 0);
}

AudioFormat::mt_eBitMode QtAudioReader::mf_eBitMode() const
{
    return MSBfloat32;
}

unsigned long QtAudioReader::mf_lSampleRate() const
{
    return host->mf_lGetSampleRate();
}

unsigned QtAudioReader::mf_nChannels() const
{
    return host->mf_nGetIChan();
}

unsigned long QtAudioReader::Read(void** buf, const unsigned nSamples)
{
    Q_UNUSED(buf);
    Q_UNUSED(nSamples);

    memset(buf, 0, nSamples * SAMPLE_SIZE);
    return 0;
}

//class QtAudioWrapperPrivate ************************************************

class QtAudioWrapperPrivate : public QObject
{
        Q_OBJECT

    public:

        QtAudioWrapperPrivate();

        QAudioFormat format;
        QAudioOutput* output;
        CallbackBuffer* buffer;
        std::string lastError;

    private Q_SLOTS:

        void onStateChange();
};

QtAudioWrapperPrivate::QtAudioWrapperPrivate() :
            format(QAudioDeviceInfo::defaultInputDevice().preferredFormat()),
            output(0), buffer(0), lastError("No error")
{
    format.setSampleType(QAudioFormat::Float);
    format.setSampleSize(32);
}

void QtAudioWrapperPrivate::onStateChange()
{
    QString state;
    switch (output->state())
    {
        case QAudio::StoppedState:
            state = "Stopped";
            break;
        case QAudio::SuspendedState:
            state = "Suspended";
            break;
        case QAudio::IdleState:
            state = "Idle";
            break;
        case QAudio::ActiveState:
            state = "Active";
            break;

        default:
            state = "Unknown";
    }

    qtAudioDebug() << "Entering state:" << state;

    if (output->error() != QAudio::NoError)
    {
        QString error;
        switch (output->error())
        {
            case QAudio::OpenError:
                error = "OpenError";
                break;
            case QAudio::IOError:
                error = "IOError";
                break;
            case QAudio::UnderrunError:
                error = "UnderrunError";
                break;
            case QAudio::FatalError:
                error = "FatalError";
                break;

            default:
                error = "NoError";
        }

        qtAudioDebug() << "Audio error:" << error;
    }
}

//class QtAudioWrapper ********************************************************

QtAudioWrapper::QtAudioWrapper(const std::string& driverName) :
                                            d(new QtAudioWrapperPrivate())
{
    Q_UNUSED(driverName);
}

QtAudioWrapper::~QtAudioWrapper()
{
    mp_bCloseDriver();
    delete d;
}

bool QtAudioWrapper::mp_bOpenDriver(const unsigned nInChannels,
                                    const unsigned nOutChannels,
                                    const unsigned long sampleRate,
                                    const unsigned bufferSize)
{
    Q_UNUSED(nInChannels);
    //qtAudioDebug() << "opening driver";

    if (!mf_bIsOpen())
    {
        d->format.setChannels(nOutChannels);
        d->format.setFrequency(sampleRate);
        d->output = new QAudioOutput(d->format);
        d->output->setBufferSize(bufferSize * SAMPLE_SIZE * nOutChannels);
        QObject::connect(d->output, SIGNAL(stateChanged(QAudio::State)),
                         d, SLOT(onStateChange()));

        return true;
    }
    else
        return false;
}

bool QtAudioWrapper::mp_bCloseDriver()
{
    //qtAudioDebug() << "closing driver";
    if (mf_bIsOpen())
    {
        mp_bStop();

        delete d->output;
        d->output = 0;

        //qtAudioDebug() << "    closed";
        return true;
    }

    return false;
}

bool QtAudioWrapper::mf_bIsOpen() const
{
    return d->output != 0;
}

bool QtAudioWrapper::mp_bStart(Callback& callback)
{
    //qtAudioDebug() << "starting driver";
    if (mf_bIsOpen() && !mf_bIsRunning())
    {
        d->buffer = new CallbackBuffer(&callback, d->output->bufferSize());
        d->buffer->open(CallbackBuffer::ReadWrite);
        d->output->start(d->buffer);

        //qtAudioDebug() << "    started";
        return true;
    }

    return false;
}

bool QtAudioWrapper::mp_bStop()
{
    qtAudioDebug() << "stopping driver";
    if (mf_bIsRunning())
    {
        d->output->stop();
        delete d->buffer;
        d->buffer = 0;

        qtAudioDebug() << "    stopped";
        return true;
    }

    return false;
}

bool QtAudioWrapper::mf_bIsRunning() const
{
    if (mf_bIsOpen())
    {
        QAudio::State state = d->output->state();
        return state == QAudio::ActiveState || state == QAudio::IdleState;
    }

    return false;
}

tSoundCardInfo QtAudioWrapper::mf_GetInfo() const
{
    tSoundCardInfo ret;
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();

    Q_FOREACH (unsigned sampleRate, info.supportedFrequencies())
        ret.m_SampleRates.push_back(sampleRate);

    for (unsigned i = 64; i <= 16384; i *= 2)
        ret.m_BufferSizes.push_back(i);

    ret.m_nDefaultBufferSize = 4096;
    ret.m_nMaxOutputChannels = info.supportedChannels().size();
    ret.m_nMaxInputChannels = 0;

    return ret;
}

unsigned long QtAudioWrapper::mf_lGetEstimatedLatency() const
{
    qFatal("Estimated latency not implemented in QtAudioWrapper");
    return 0;
}

unsigned long QtAudioWrapper::mf_lGetSampleRate() const
{
    if (mf_bIsOpen())
        return d->format.frequency();

    return 0;
}

unsigned QtAudioWrapper::mf_nGetBufferSize() const
{
    if (mf_bIsOpen())
        return d->output->bufferSize();

    return 0;
}

unsigned QtAudioWrapper::mf_nGetOChan() const
{
    if (mf_bIsOpen())
        return d->format.channels();

    return 0;
}

unsigned QtAudioWrapper::mf_nGetIChan() const
{
    return 0;
}

const std::string& QtAudioWrapper::mf_sGetLastError() const
{
    return d->lastError;
}

void QtAudioWrapper::mp_ClearIOBuffers()
{

}

AudioFormatReader* QtAudioWrapper::mf_pCreateReader() const
{
    //qtAudioDebug() << "creating reader";
    return new QtAudioReader(this);
}

AudioFormatWriter* QtAudioWrapper::mf_pCreateWriter() const
{
    //qtAudioDebug() << "creating writer";
    return new QtAudioWriter(this);
}

QIODevice* QtAudioWrapper::buffer() const
{
    return d->buffer;
}

} //ns streamapp

#include "qtaudiowrapper.moc"
