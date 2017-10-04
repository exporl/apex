#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"

#include "conditionsparser.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>

using namespace bertha;
using namespace cmn;

enum PtcState {
    Idle,
    Calibrating,
    TonePlaying,
    Loaded,
    Running,
    Stopped,
    Finished
};

class PtcResult
{
public:
    PtcResult(int s, float g);
    int sample;
    float gain;
};

PtcResult::PtcResult(int s, float g) : sample(s), gain(g)
{
}

class PtcPrivate
{
public:
    PtcPrivate();
    QScopedPointer<ExperimentProcessor> processor;

    unsigned numberOfSamples(const QString &filename);
    ExperimentData basicExperiment(const QString &toneFile,
                                   const QString &noiseFile,
                                   const QString &maskerFile);

    Ui::MainWindow ui;

    QTimer pollBerthaTimer;
    QTimer gainTimer;

    // constants
    static const int userKey;

    static const float gainStep; // change gain in steps of gainStep
    static const QString experimentFile;

    // user settable
    float gainRate; // dB/s
    float maxGain;
    bool maskerEnabled;

    // not user settable
    int totalFrames;
    bool increaseGain;
    float currentGain;
    int blockSize;
    PtcState state;

    QMap<QString, Condition> conditions;
    QList<PtcResult> results; // we store a list of gain reversals
};

const int PtcPrivate::userKey = Qt::Key_Space;
const float PtcPrivate::gainStep = 0.5;
const QString PtcPrivate::experimentFile =
    QLatin1String("experiments/ptc_fast_experiment.xml");

PtcPrivate::PtcPrivate()
    : gainRate(1),
      maxGain(0),
      maskerEnabled(true),
      totalFrames(-1),
      increaseGain(false),
      currentGain(-10),
      blockSize(-1),
      state(Idle)
{
}

unsigned PtcPrivate::numberOfSamples(const QString &filename)
{
    SF_INFO readSfinfo;
    readSfinfo.format = 0;
    SNDFILE *inputFile =
        sf_open(QFile::encodeName(filename), SFM_READ, &readSfinfo);
    if (!inputFile) {
        qCDebug(EXPORL_BERTHA,
                "Could not open file %s to determine number of samples",
                qPrintable(filename));
        sf_close(inputFile);
    }
    return readSfinfo.frames;
}

ExperimentData PtcPrivate::basicExperiment(const QString &toneFile,
                                           const QString &noiseFile,
                                           const QString &maskerFile)
{
    bool playTone = !toneFile.isEmpty();
    bool playNoise = !noiseFile.isEmpty();
    bool playMasker = !maskerFile.isEmpty();

    const QString xmlPath =
        Paths::searchFile(experimentFile, Paths::dataDirectories());
    const QString tonePath =
        Paths::searchFile(toneFile, Paths::dataDirectories());
    const QString noisePath =
        Paths::searchFile(noiseFile, Paths::dataDirectories());
    const QString maskerPath =
        Paths::searchFile(maskerFile, Paths::dataDirectories());

    QFile file(xmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qCDebug(EXPORL_BERTHA, ) << "cannot open file " << xmlPath;

    QByteArray data = file.readAll();
    ExperimentData expdata((XmlLoader().loadContents(data)));

#ifdef Q_WS_WIN
    {
        DeviceData d(expdata.device());
        d.setPlugin(QLatin1String("Asio"));
        expdata.setDevice(d);
    }
#endif

    if (playTone) {
        BlockData reader;
        reader.setId(QLatin1String("tonereader"));
        reader.setPlugin(QLatin1String("FileSource"));
        reader.setParameter(QLatin1String("fileName"), tonePath);
        reader.setParameter(QLatin1String("loop"), true);
        reader.setParameter(QLatin1String("zeroPadLoop"), false);
        expdata.addBlock(reader);

        BlockData amp(QLatin1String("toneamp"), QLatin1String("Gain"));
        amp.setParameter(QLatin1String("rampLength"), 0.01);
        expdata.addBlock(amp);

        expdata.addConnection(ConnectionData(
            QLatin1String("tonereader"), QLatin1String("output-1"),
            QLatin1String("toneamp"), QLatin1String("input-1")));

        if (!playNoise)
            expdata.addConnection(ConnectionData(
                QLatin1String("toneamp"), QLatin1String("output-1"),
                QLatin1String("soundcard"), QLatin1String("playback-1")));
    }

    if (playNoise) {
        BlockData reader;
        reader.setId(QLatin1String("noisereader"));
        reader.setPlugin(QLatin1String("FileSource"));
        reader.setParameter(QLatin1String("fileName"), noisePath);
        expdata.addBlock(reader);

        BlockData amp(QLatin1String("noiseamp"), QLatin1String("Gain"));
        amp.setParameter(QLatin1String("rampLength"), 0.01);
        expdata.addBlock(amp);

        expdata.addConnection(ConnectionData(
            QLatin1String("noisereader"), QLatin1String("output-1"),
            QLatin1String("noiseamp"), QLatin1String("input-1")));

        if (!playTone)
            expdata.addConnection(ConnectionData(
                QLatin1String("noiseamp"), QLatin1String("output-1"),
                QLatin1String("soundcard"), QLatin1String("playback-1")));
    }

    if (playTone && playNoise) {
        if (playMasker) {
            BlockData reader;
            reader.setId(QLatin1String("maskerreader"));
            reader.setPlugin(QLatin1String("FileSource"));
            reader.setParameter(QLatin1String("fileName"), maskerPath);
            reader.setParameter(QLatin1String("loop"), true);
            reader.setParameter(QLatin1String("zeroPadLoop"), false);
            expdata.addBlock(reader);

            BlockData amp(QLatin1String("maskeramp"), QLatin1String("Gain"));
            amp.setParameter(QLatin1String("rampLength"), 0.01);
            expdata.addBlock(amp);

            expdata.addConnection(ConnectionData(
                QLatin1String("maskerreader"), QLatin1String("output-1"),
                QLatin1String("maskeramp"), QLatin1String("input-1")));

            expdata.addConnection(ConnectionData(
                QLatin1String("maskeramp"), QLatin1String("output-1"),
                QLatin1String("sum"), QLatin1String("input-3")));
        }

        BlockData sum(QLatin1String("sum"), QLatin1String("Summation"));
        if (playMasker)
            sum.setInputPorts(3);
        else
            sum.setInputPorts(2);
        sum.setOutputPorts(1);
        expdata.addBlock(sum);

        expdata.addConnection(
            ConnectionData(QLatin1String("toneamp"), QLatin1String("output-1"),
                           QLatin1String("sum"), QLatin1String("input-2")));

        expdata.addConnection(
            ConnectionData(QLatin1String("noiseamp"), QLatin1String("output-1"),
                           QLatin1String("sum"), QLatin1String("input-1")));

        expdata.addConnection(ConnectionData(
            QLatin1String("sum"), QLatin1String("output-1"),
            QLatin1String("soundcard"), QLatin1String("playback-1")));
    }

    return expdata;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), d(new PtcPrivate)
{
    d->ui.setupUi(this);

    connect(d->ui.startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(d->ui.stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(d->ui.saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(d->ui.quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(d->ui.calibrationButton, SIGNAL(clicked()), this,
            SLOT(calibrate()));
    connect(d->ui.playToneButton, SIGNAL(clicked()), this, SLOT(playTone()));
    connect(d->ui.resetButton, SIGNAL(clicked()), this,
            SLOT(resetExperiment()));
    connect(d->ui.toneGainSpin, SIGNAL(valueChanged(double)), this,
            SLOT(changeToneGain(double)));
    connect(d->ui.maxNoiseGainSpin, SIGNAL(valueChanged(double)), this,
            SLOT(changeMaxGain(double)));
    connect(d->ui.maskerEnabledCheckBox, SIGNAL(stateChanged(int)), this,
            SLOT(enableMasker(int)));
    connect(d->ui.selectNoiseFileButton, SIGNAL(clicked()), this,
            SLOT(selectNoiseFile()));
    connect(d->ui.selectToneFileButton, SIGNAL(clicked()), this,
            SLOT(selectToneFile()));
    connect(d->ui.selectMaskerFileButton, SIGNAL(clicked()), this,
            SLOT(selectMaskerFile()));
    connect(&d->pollBerthaTimer, SIGNAL(timeout()), this,
            SLOT(updateProgress()));
    connect(&d->gainTimer, SIGNAL(timeout()), this, SLOT(updateGain()));
    connect(d->ui.conditionsCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(setCondition(QString)));
    connect(d->ui.hideNoiseCheck, SIGNAL(toggled(bool)), d->ui.noiseGainSpin,
            SLOT(setHidden(bool)));

    QTimer::singleShot(0, this, SLOT(loadConditions()));
}

MainWindow::~MainWindow()
{
    if (d->state != Idle)
        d->processor->release();
    delete d;
}

void MainWindow::loadConditions()
{
    // parse conditions
    ConditionsParser parser;
    QString file = Paths::searchFile(QLatin1String("config/ptcfast.xml"),
                                     Paths::dataDirectories());

    if (!QFile::exists(file)) {
        QMessageBox::warning(
            this, tr("File does not exist"),
            tr("The condition file %1 does not exist").arg(file));
        return;
    }

    try {
        d->conditions = parser.parse(file);
    } catch (const std::exception &e) {
        QString error = tr("There was an error loading the conditions:\n\n%1");
        QMessageBox::critical(this, tr("Error loading conditions"),
                              error.arg(QLatin1String(e.what())));
    }

    // load them in the combobox
    Q_FOREACH (QString condition, d->conditions.keys())
        d->ui.conditionsCombo->addItem(condition);
}

void MainWindow::setCondition(const QString &name)
{
    Q_ASSERT(d->state == Idle);
    Q_ASSERT(d->conditions.contains(name));

    Condition condition = d->conditions[name];
    d->ui.toneFileEdit->setText(condition.tone);
    d->ui.noiseFileEdit->setText(condition.noise);
    d->ui.maskerFileEdit->setText(condition.masker);
}

void MainWindow::loadExperiment()
{
    QString noiseFile(d->ui.noiseFileEdit->text());
    QString toneFile(d->ui.toneFileEdit->text());
    QString maskerFile;

    if (d->maskerEnabled)
        maskerFile = d->ui.maskerFileEdit->text();

    ExperimentData expdata =
        d->basicExperiment(toneFile, noiseFile, maskerFile);

    d->blockSize = expdata.device().blockSize();
    const QString noisePath =
        Paths::searchFile(noiseFile, Paths::dataDirectories());
    d->totalFrames = d->numberOfSamples(noisePath) / d->blockSize;
    d->ui.progressBar->setMaximum(d->totalFrames);

    try {
        d->processor.reset(new ExperimentProcessor(expdata));
        d->state = Loaded;

        d->processor->prepare();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, QLatin1String("Error"),
                              QString::fromLocal8Bit(e.what()));
        return;
    }
}

void MainWindow::start()
{
    if (d->state == Idle)
        loadExperiment();

    d->ui.noiseGainSpin->setEnabled(false);
    d->ui.toneGainSpin->setEnabled(false);
    d->ui.maxNoiseGainSpin->setEnabled(false);
    d->ui.maskerGainSpin->setEnabled(false);
    d->ui.maskerEnabledCheckBox->setEnabled(false);
    d->ui.startButton->setEnabled(false);
    d->ui.stopButton->setEnabled(true);
    d->ui.calibrationButton->setEnabled(false);
    d->ui.playToneButton->setEnabled(false);
    d->ui.gainRateSpin->setEnabled(false);

    d->gainRate = d->ui.gainRateSpin->value();

    d->currentGain = d->ui.noiseGainSpin->value();
    float toneGain = d->ui.toneGainSpin->value();
    float maskerGain = d->ui.maskerGainSpin->value();
    d->processor->setParameterValue(QLatin1String("noiseamp"),
                                    QLatin1String("gain"), d->currentGain);
    d->processor->setParameterValue(QLatin1String("toneamp"),
                                    QLatin1String("gain"), toneGain);
    if (d->maskerEnabled)
        d->processor->setParameterValue(QLatin1String("maskeramp"),
                                        QLatin1String("gain"), maskerGain);

    try {
        d->processor->start();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, QLatin1String("Error"),
                              QString::fromLocal8Bit(e.what()));
        return;
    }

    d->pollBerthaTimer.start(100);
    d->gainTimer.start(1000 * d->gainStep / d->gainRate);

    d->state = Running;
    grabKeyboard();
}

void MainWindow::stop()
{
    d->gainTimer.stop();
    d->pollBerthaTimer.stop();
    d->processor->stop();
    d->state = Stopped;
    releaseKeyboard();

    d->ui.startButton->setEnabled(true);
    d->ui.stopButton->setEnabled(false);
    d->ui.saveButton->setEnabled(true);
}

void MainWindow::updateProgress()
{
    int currentBuffer = d->processor->currentBuffer();
    d->ui.progressBar->setValue(currentBuffer);
    d->ui.blocksLabel->setText(QString::number(currentBuffer));

    if (currentBuffer >= d->totalFrames) {
        stop();
        d->state = Finished;
        save();
    }
}

void MainWindow::save()
{
    releaseKeyboard();
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save results"), QLatin1String("ptc_fast_results.csv"),
        tr("PTC results (*.csv);;All files (*.*)"));

    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        out << QLatin1String("Noise file: ") << d->ui.noiseFileEdit->text()
            << '\n';
        out << QLatin1String("Tone file: ") << d->ui.toneFileEdit->text()
            << '\n';
        out << QLatin1String("Masker file: ") << d->ui.maskerFileEdit->text()
            << '\n';
        out << QLatin1String("Tone gain: ") << d->ui.toneGainSpin->value()
            << '\n';
        if (d->maskerEnabled)
            out << QLatin1String("Masker gain: ")
                << d->ui.maskerGainSpin->value() << '\n';
        else
            out << QLatin1String("Masker gain: NaN") << '\n';
        out << QLatin1String("Gain rate: ") << d->gainRate << '\n';
        Q_FOREACH (PtcResult r, d->results) {
            out << r.sample << '\t' << r.gain << '\n';
        }
    }
}

void MainWindow::selectNoiseFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open noise file"),
                                                    d->ui.noiseFileEdit->text(),
                                                    tr("WAV files (*.wav)"));
    if (!filename.isEmpty())
        d->ui.noiseFileEdit->setText(filename);
}

void MainWindow::selectToneFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open tone file"),
                                                    d->ui.toneFileEdit->text(),
                                                    tr("WAV files (*.wav)"));
    if (!filename.isEmpty())
        d->ui.toneFileEdit->setText(filename);
}

void MainWindow::selectMaskerFile()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Open masker file"), d->ui.maskerFileEdit->text(),
        tr("WAV files (*.wav)"));
    if (!filename.isEmpty())
        d->ui.maskerFileEdit->setText(filename);
}

void MainWindow::calibrate()
{
    static QScopedPointer<ExperimentProcessor> proc;

    if (d->state == Idle) {
        QString filename = QFileDialog::getOpenFileName(
            this, tr("Select calibration file"), d->ui.toneFileEdit->text(),
            tr("WAV files (*.wav)"));
        if (filename.isEmpty())
            return;

        ExperimentData expdata =
            d->basicExperiment(filename, QString(), QString());

        /*        expdata.addConnection(
                        ConnectionData(QLatin1String("tonereader"),
           QLatin1String("output-1"),
                                       QLatin1String("toneamp"),
           QLatin1String("input-1")) );
                expdata.addConnection(
                        ConnectionData(QLatin1String("toneamp"),
           QLatin1String("output-1"),
                                       QLatin1String("soundcard"),
           QLatin1String("playback-1")) );*/

        try {
            proc.reset(new ExperimentProcessor(expdata));
            d->state = Loaded;
            proc->prepare();
            proc->start();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, QLatin1String("Error"),
                                  QString::fromLocal8Bit(e.what()));
            return;
        }

        d->state = Calibrating;

        d->ui.calibrationButton->setText(tr("Stop calibration"));
        d->ui.startButton->setEnabled(false);

    } else if (d->state == Calibrating) {
        proc->release();
        d->state = Idle;
        d->ui.calibrationButton->setText(tr("Calibrate"));
        d->ui.startButton->setEnabled(true);
    } else {
        qFatal("Invalid state for calibration");
    }
}

void MainWindow::playTone()
{
    if (d->state == Idle) {
        QString toneFile(d->ui.toneFileEdit->text());

        ExperimentData expdata =
            d->basicExperiment(toneFile, QString(), QString());

        d->ui.noiseGainSpin->setEnabled(false);
        d->ui.toneGainSpin->setEnabled(true);
        d->ui.startButton->setEnabled(false);
        d->ui.stopButton->setEnabled(false);
        d->ui.calibrationButton->setEnabled(false);

        float toneGain = d->ui.toneGainSpin->value();

        d->processor.reset(new ExperimentProcessor(expdata));
        d->state = TonePlaying;

        d->processor->setParameterValue(QLatin1String("toneamp"),
                                        QLatin1String("gain"), toneGain);

        try {
            d->processor->prepare();
            d->processor->start();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, QLatin1String("Error"),
                                  QString::fromLocal8Bit(e.what()));
            return;
        }

        d->state = Calibrating;

        d->ui.playToneButton->setText(tr("Stop tone"));

        d->state = TonePlaying;

    } else if (d->state == TonePlaying) {
        d->processor->release();
        d->state = Idle;
        d->ui.playToneButton->setText(tr("Play tone"));
        d->ui.startButton->setEnabled(true);
        d->ui.noiseGainSpin->setEnabled(true);
    }
}

void MainWindow::changeToneGain(double newvalue)
{
    if (d->state != Idle)
        d->processor->setParameterValue(QLatin1String("toneamp"),
                                        QLatin1String("gain"), newvalue);
}

void MainWindow::changeMaxGain(double newvalue)
{
    d->ui.noiseGainSpin->setMaximum(newvalue);
    d->maxGain = newvalue;
}

void MainWindow::enableMasker(int state)
{
    d->maskerEnabled = (state == Qt::Checked);
    d->ui.maskerGainSpin->setEnabled(d->maskerEnabled);
}

void MainWindow::resetExperiment()
{
    if (QMessageBox::question(
            this, QLatin1String("Reset experiment?"),
            QLatin1String("Do you want to reset the experiment? "
                          "The measurement will be lost."),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    d->processor->release();
    d->state = Idle;

    d->ui.noiseGainSpin->setEnabled(true);
    d->ui.toneGainSpin->setEnabled(true);
    d->ui.maxNoiseGainSpin->setEnabled(true);
    d->ui.maskerEnabledCheckBox->setEnabled(true);
    enableMasker(d->ui.maskerEnabledCheckBox->checkState());
    d->ui.startButton->setEnabled(true);
    d->ui.stopButton->setEnabled(false);
    d->ui.calibrationButton->setEnabled(true);
    d->ui.playToneButton->setEnabled(true);

    d->gainTimer.stop();
    d->pollBerthaTimer.stop();
    d->results.empty();

    releaseKeyboard();
}

void MainWindow::updateGain()
{
    static bool lastIncreaseGain = false;

    if (d->increaseGain)
        d->currentGain += d->gainStep;
    else
        d->currentGain -= d->gainStep;

    if (d->currentGain > d->maxGain)
        d->currentGain = d->maxGain;

    d->processor->setParameterValue(QLatin1String("noiseamp"),
                                    QLatin1String("gain"), d->currentGain);
    d->ui.noiseGainSpin->setValue(d->currentGain);

    if (lastIncreaseGain != d->increaseGain) { // save reversal
        int currentBuffer = d->processor->currentBuffer();
        d->results.push_back(
            PtcResult(currentBuffer * d->blockSize, d->currentGain));
    }
    lastIncreaseGain = d->increaseGain;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == d->userKey) {
        if (!event->isAutoRepeat()) { // we ignore repetitions
            event->accept();
            // d->ui.keyLabel->setText(QLatin1String("Key down"));
            d->increaseGain = true;
        } else {
            event->ignore();
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == d->userKey) {
        if (!event->isAutoRepeat()) {
            event->accept();
            // d->ui.keyLabel->setText(QLatin1String("Key up"));
            d->increaseGain = false;
        } else {
            event->ignore();
        }
    } else {
        QMainWindow::keyReleaseEvent(event);
    }
}
