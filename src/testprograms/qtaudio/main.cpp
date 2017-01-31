#include <QAudioOutput>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QThread>

#include "callbackbuffer.h"

QDebug& operator<<(QDebug& dbg, QAudioFormat::Endian endian)
{
    return dbg << (endian == QAudioFormat::BigEndian ? "BigEndian" : "LittleEndian");
}

QDebug& operator<<(QDebug& dbg, QAudioFormat::SampleType type)
{
    QString string;

    switch (type)
    {
        case QAudioFormat::SignedInt: string = "SignedInt"; break;
        case QAudioFormat::UnSignedInt: string = "UnSignedInt"; break;
        case QAudioFormat::Float: string = "Float"; break;
        default: string = "Unknown";
    }

    return dbg << qPrintable(string);
}

QDebug& operator<<(QDebug& dbg, QAudio::Error error)
{
    QString string;

    switch (error)
    {
        case QAudio::OpenError: string = "OpenError"; break;
        case QAudio::IOError: string = "IOError"; break;
        case QAudio::UnderrunError: string = "UnderrunError"; break;
        case QAudio::FatalError: string = "FatalError"; break;
        default: string = "NoError";
    }

    return dbg << qPrintable(string);
}

class AudioRunner : public QThread
{
        Q_OBJECT

    public:

        AudioRunner(const QString& file = QString()) : callback(file),
                                                       output(createFormat()),
                                                       buffer(&callback, 4096)
        {
            buffer.open(CallbackBuffer::ReadWrite);
            callback.setBuffer(&buffer);
            connect(&output, SIGNAL(stateChanged(QAudio::State)),
                    this, SLOT(printError()));
        }

        void run()
        {
            //output.start(&buffer);
            QTimer::singleShot(0, this, SLOT(startOutput()));
            //QTimer::singleShot(400, this, SLOT(quit()));
            exec();
        }

    private:

        QAudioFormat createFormat() const
        {
            QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
            qDebug() << "supported stuff:";
            qDebug() << "codecs:" << info.supportedCodecs();
            qDebug() << "byte orders:" << info.supportedByteOrders();
            qDebug() << "channels:" << info.supportedChannels();
            qDebug() << "sample rates:" << info.supportedFrequencies();
            qDebug() << "sample sizes:" << info.supportedSampleSizes();
            qDebug() << "sample types:" << info.supportedSampleTypes();

            QAudioFormat format = info.preferredFormat();
            //format.setFrequency(8000);
            format.setChannels(2);
            format.setSampleSize(32);
            //format.setCodec("audio/pcm");
            //format.setByteOrder(QAudioFormat::BigEndian);
            format.setSampleType(QAudioFormat::Float);

            qDebug() << "\nformat used:";
            qDebug() << "codec:" << format.codec();
            qDebug() << "byte order:" << format.byteOrder();
            qDebug() << "channels:" << format.channels();
            qDebug() << "sample rate:" << format.frequency();
            qDebug() << "sample size:" << format.sampleSize();
            qDebug() << "sample type:" << format.sampleType();

            return format;
        }

    public slots:

        void printError()
        {
            if (output.error() != QAudio::NoError)
                qDebug() << "Error:" << output.error();
        }

        void startOutput()
        {
            qDebug() << "starting output";
            output.start(&buffer);
        }

        void stopOutput()
        {
            qDebug() << "stopping output";
            //quit();
            //terminate();
            output.stop();
        }

    private:

        Callback callback;
        QAudioOutput output;
        CallbackBuffer buffer;
};

class ThreadRunner : public QThread
{
        Q_OBJECT

    public:

        ThreadRunner(const QString& file = QString()) : fileName(file) {}

        void run()
        {
            runner = new AudioRunner(fileName);
            runner->start();
            exec();
        }

    private slots:

        void stop()
        {
            runner->terminate();
        }

    private:

        QString fileName;
        AudioRunner* runner;
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QString file;
    if (argc > 1)
        file = argv[1];

    ThreadRunner runner(file);
    runner.start();
    QTimer::singleShot(1000, &runner, SLOT(quit()));

    return app.exec();
}

#include "main.moc"
