#include "apextools/global.h"

#include "callbackbuffer.h"

#include <QAudioOutput>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QThread>
#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(APEX_QTAUDIO)
Q_LOGGING_CATEGORY(APEX_QTAUDIO, "apex.qtaudio")

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
            qCDebug(APEX_QTAUDIO) << "supported stuff:";
            qCDebug(APEX_QTAUDIO) << "codecs:" << info.supportedCodecs();
            qCDebug(APEX_QTAUDIO) << "byte orders:" << info.supportedByteOrders();
            qCDebug(APEX_QTAUDIO) << "channels:" << info.supportedChannelCounts();
            qCDebug(APEX_QTAUDIO) << "sample rates:" << info.supportedSampleRates();
            qCDebug(APEX_QTAUDIO) << "sample sizes:" << info.supportedSampleSizes();
            qCDebug(APEX_QTAUDIO) << "sample types:" << info.supportedSampleTypes();

            QAudioFormat format = info.preferredFormat();
            //format.setFrequency(8000);
            format.setChannelCount(2);
            format.setSampleSize(32);
            //format.setCodec("audio/pcm");
            //format.setByteOrder(QAudioFormat::BigEndian);
            format.setSampleType(QAudioFormat::Float);

            qCDebug(APEX_QTAUDIO) << "\nformat used:";
            qCDebug(APEX_QTAUDIO) << "codec:" << format.codec();
            qCDebug(APEX_QTAUDIO) << "byte order:" << format.byteOrder();
            qCDebug(APEX_QTAUDIO) << "channels:" << format.channelCount();
            qCDebug(APEX_QTAUDIO) << "sample rate:" << format.sampleRate();
            qCDebug(APEX_QTAUDIO) << "sample size:" << format.sampleSize();
            qCDebug(APEX_QTAUDIO) << "sample type:" << format.sampleType();

            return format;
        }

    public slots:

        void printError()
        {
            if (output.error() != QAudio::NoError)
                qCDebug(APEX_QTAUDIO) << "Error:" << output.error();
        }

        void startOutput()
        {
            qCDebug(APEX_QTAUDIO) << "starting output";
            output.start(&buffer);
        }

        void stopOutput()
        {
            qCDebug(APEX_QTAUDIO) << "stopping output";
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
