#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"

#include "preferencerating.h"
#include "ui_preferencerating.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>

using namespace bertha;
using namespace cmn;

class PRPrivate
{
public:
    PRPrivate();
    QScopedPointer<ExperimentProcessor> processor;

    Ui::PreferenceRating ui;
    ExperimentData createExperiment(const QString &file1, const QString &file2);
};

PRPrivate::PRPrivate()
{
}

ExperimentData PRPrivate::createExperiment(const QString &file1,
                                           const QString &file2)
{
    const QString file1Path =
        Paths::searchFile(file1, Paths::dataDirectories());
    const QString file2Path =
        Paths::searchFile(file2, Paths::dataDirectories());

    ExperimentData expdata;

    DeviceData dev;
    dev.setId(QLatin1String("soundcard"));
#ifdef Q_WS_WIN
    dev.setPlugin(QLatin1String("Asio"));
#else
    dev.setPlugin(QLatin1String("Jack"));
#endif
    dev.setBlockSize(2048);
    dev.setSampleRate(44100);
    dev.setOutputPorts(0);
    dev.setInputPorts(1);
    expdata.setDevice(dev);

    BlockData reader1;
    reader1.setId(QLatin1String("file1reader"));
    reader1.setPlugin(QLatin1String("FileSource"));
    reader1.setParameter(QLatin1String("fileName"), file1Path);
    reader1.setParameter(QLatin1String("loop"), true);
    reader1.setParameter(QLatin1String("zeroPadLoop"), false);
    expdata.addBlock(reader1);

    BlockData reader2;
    reader2.setId(QLatin1String("file2reader"));
    reader2.setPlugin(QLatin1String("FileSource"));
    reader2.setParameter(QLatin1String("fileName"), file2Path);
    reader2.setParameter(QLatin1String("loop"), true);
    reader2.setParameter(QLatin1String("zeroPadLoop"), false);
    expdata.addBlock(reader2);

    /*BlockData sum(QLatin1String("sum"),QLatin1String("Summation"));
    sum.setInputPorts(2);
    sum.setOutputPorts(1);
    expdata.addBlock(sum);*/
    BlockData mixer(QLatin1String("mixer"), QLatin1String("Mixer"));
    mixer.setInputPorts(2);
    mixer.setOutputPorts(1);
    mixer.setParameter(QLatin1String("logarithmicGain"), false);
    mixer.setParameter(QLatin1String("matrix"), QLatin1String("[[1 0]]"));
    expdata.addBlock(mixer);

    expdata.addConnection(
        ConnectionData(QLatin1String("file1reader"), QLatin1String("output-1"),
                       QLatin1String("mixer"), QLatin1String("input-1")));

    expdata.addConnection(
        ConnectionData(QLatin1String("file2reader"), QLatin1String("output-1"),
                       QLatin1String("mixer"), QLatin1String("input-2")));

    expdata.addConnection(ConnectionData(
        QLatin1String("mixer"), QLatin1String("output-1"),
        QLatin1String("soundcard"), QLatin1String("playback-1")));

    return expdata;
}

PreferenceRating::PreferenceRating(QWidget *parent)
    : QMainWindow(parent), d(new PRPrivate)
{
    d->ui.setupUi(this);
    connect(d->ui.startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(d->ui.stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(d->ui.file1Button, SIGNAL(clicked()), this, SLOT(file1()));
    connect(d->ui.file2Button, SIGNAL(clicked()), this, SLOT(file2()));
}

PreferenceRating::~PreferenceRating()
{
    delete d;
}

void PreferenceRating::start()
{
    try {
        ExperimentData expdata =
            d->createExperiment(QLatin1String("testdata/sin50Hz.wav"),
                                QLatin1String("testdata/sin1000Hz.wav"));
        d->processor.reset(new ExperimentProcessor(expdata));
        d->processor->prepare();
        d->processor->start();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, QLatin1String("Error"),
                              QString::fromLocal8Bit(e.what()));
    }
}

void PreferenceRating::stop()
{
    d->processor->stop();
}

void PreferenceRating::file1()
{
    d->processor->setParameterValue(QLatin1String("mixer"),
                                    QLatin1String("matrix"),
                                    QLatin1String("[[1 0]]"));
}

void PreferenceRating::file2()
{
    d->processor->setParameterValue(QLatin1String("mixer"),
                                    QLatin1String("matrix"),
                                    QLatin1String("[[0 1]]"));
}
