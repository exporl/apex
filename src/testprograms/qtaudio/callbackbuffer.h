#ifndef CALLBACKDEVICE_H
#define CALLBACKDEVICE_H

#include <QIODevice>
#include <QFile>

class Callback
{
    public:

        Callback(const QString& fileName = QString());
        ~Callback();

        void setBuffer(QIODevice* dev);
        void callback();

    private:

        QIODevice* device;
        QFile file;
        bool hasFile;
        float* samples;
};

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

//QDebug& operator<<(QDebug& dbg, const CallbackBuffer& buffer);

#endif
