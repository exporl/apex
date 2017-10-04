#include "callbackbuffer.h"

#include <QDebug>

#define NSAMPLES 1024
#define BOUND 1.0
#define LEFT_STEP 0.1f
#define RIGHT_STEP 0.03f

Callback::Callback(const QString &fileName) : file(fileName)
{
    hasFile = !fileName.isEmpty() && file.open(QFile::ReadOnly);
    samples = new float[NSAMPLES * 2];
}

Callback::~Callback()
{
    delete[] samples;
}

void Callback::setBuffer(QIODevice *dev)
{
    device = dev;
}

void Callback::callback()
{
    if (hasFile)
        device->write(file.read(4096));
    else {
        static float leftSample = 0.0;
        static float rightSample = 0.0;
        static bool leftUp = true;
        static bool rightUp = true;

        for (int i = 0; i < NSAMPLES; i++) {
            samples[2 * i] = leftSample;
            samples[2 * i + 1] = rightSample;

            if (leftUp)
                leftSample += LEFT_STEP;
            else
                leftSample -= LEFT_STEP;

            if (rightUp)
                rightSample += RIGHT_STEP;
            else
                rightSample -= RIGHT_STEP;

            if (leftSample > BOUND)
                leftUp = false;
            else if (leftSample < -BOUND)
                leftUp = true;

            if (rightSample > BOUND)
                rightUp = false;
            else if (rightSample < -BOUND)
                rightUp = true;
        }

        device->write((char *)samples, 2 * NSAMPLES * sizeof(float));
    }
}

CallbackBuffer::CallbackBuffer(Callback *cb, qint64 bufSize)
    : callback(cb),
      bufferSize(bufSize),
      bytesInBuffer(0),
      buffer(bufferSize > 0 ? new char[bufferSize] : 0)
{
}

CallbackBuffer::~CallbackBuffer()
{
    delete[] buffer;
}

qint64 CallbackBuffer::readData(char *data, qint64 maxLen)
{
    //     static QFile testFile("test.wav");
    //     static bool firstTime = true;
    //
    //     if (firstTime)
    //     {
    //         testFile.open(QFile::WriteOnly | QFile::Truncate);
    //         firstTime = false;
    //     }

    if (bufferSize == 0)
        return 0;

    if (bytesInBuffer == 0) {
        callback->callback();
        // return 0;
    }

    qint64 bytesToRead = qMin(maxLen, bytesInBuffer);
    // qCDebug(APEX_RS) << "reading" << bytesToRead << "from" << *this;
    memcpy(data, buffer, bytesToRead);
    // testFile.write(data, bytesToRead);
    bytesInBuffer -= bytesToRead;
    memmove(buffer, buffer + bytesToRead, bytesInBuffer);

    return bytesToRead;
}

qint64 CallbackBuffer::writeData(const char *data, qint64 len)
{
    if (bufferSize == 0)
        return 0;

    qint64 bytesToWrite = qMin(len, bufferSize - bytesInBuffer);
    // qCDebug(APEX_RS) << "writing" << bytesToWrite << "to" << *this;
    memcpy(buffer + bytesInBuffer, data, bytesToWrite);
    bytesInBuffer += bytesToWrite;
    return bytesToWrite;
}

QDebug &operator<<(QDebug &dbg, const CallbackBuffer &buffer)
{
    dbg.nospace() << "CallbackBuffer(bufferSize=" << buffer.bufferSize
                  << ", bytesInBuffer=" << buffer.bytesInBuffer << ")";

    return dbg.space();
}
