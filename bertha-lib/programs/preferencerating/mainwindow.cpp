#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QTimer>

#include <algorithm>
#include <ctime>

using namespace bertha;
using namespace cmn;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialise some condition variables.
    isSelected = isRated = 0;
    nExp = 5;
    expIdx = 0;

    isPlayingA = isPlayingB = false; // No sounds are initially playing.

    // Set up experiment particular parameters...
    // 1. Number of experiments.
    bool ok = false;

    while (!ok) {
        nExp = QInputDialog::getInt(this, tr("Enter number of trials"),
                                    tr("Enter valid number (>1,<50):"), 30, 1,
                                    100, 1, &ok);
    }

    // 2. Subject parameters.
    ok = false;
    while (!ok || subjName.isEmpty()) {
        subjName = QInputDialog::getText(this, tr("Enter subject particulars"),
                                         tr("User name:"), QLineEdit::Normal,
                                         tr("Mustermann"), &ok);
    }

    // Create the output file to which the results will be dumped.
    // By default this will be created in the home directory of the user.
    outFile = new QFile(
        QDir::toNativeSeparators(QDir::homePath() + QLatin1String("/")) +
        subjName + QLatin1String(".txt"));
    if (!outFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
        QMessageBox::critical(this, tr("Error"), tr("File cannot be opened"),
                              QMessageBox::Ok);
        qApp->exit(0);
    }

    QTextStream out(outFile);

    // Time stamp preparation.
    QDateTime timeNow(QDateTime::currentDateTime());
    //    QString timeNow = QDateTime::toString("hh:mm,
    //    dd/MM/yyyy")(QDateTime::currentDateTime());

    // Initalisation of the experiment index.
    srand(unsigned(time(NULL)));

    arrExpIdx = new int[nExp];
    arrSigIdx = new int[nExp];

    arrResult[0] = new int[nExp];
    arrResult[1] = new int[nExp];
    int dummy[] = {1, 2};

    for (int count = 0; count < nExp; count++) {
        arrExpIdx[count] = count + 1;
        std::random_shuffle(dummy, dummy + 2);
        arrSigIdx[count] = dummy[0];
        arrResult[0][count] = 0;
        arrResult[1][count] = 0;
    }

    std::random_shuffle(arrExpIdx, arrExpIdx + nExp);

    // For reference, store the experiment index and signal presentation
    // index into the results file.

    QString strExpIdx, strSigIdx;
    for (int count = 0; count < nExp; count++) {
        strExpIdx += tr("%1 ").arg(arrExpIdx[count]);
        strSigIdx += tr("%1 ").arg(arrSigIdx[count]);
    }
    out << "Time stamp:" << timeNow.toString(QLatin1String("hh:mm, dd/MM/yyyy"))
        << "\n"
        << "Experiment index " << strExpIdx << "\n"
        << "Signal index " << strSigIdx << "\n";

    //    outFile->close();

    // Initalise the main window name and begin the experiment.
    this->setWindowTitle(tr("Preference rating: %1").arg(subjName));

    // Initalise the tool tip.
    ui->lblTip->setText(
        tr("<p> Listen to A and B by clicking on the buttons on the left "
           "panel. Then select the one you think sounds better. <p><b>Note</b> "
           "Clicking twice on a sound that is playing will stop play."));
    ui->lblTip->setWordWrap(true);

    // Initalise the information on butNext
    ui->butNext->setText(tr("Next (%1/%2)").arg(expIdx + 1).arg(nExp));

    // Perform the connections between the signals and slots.
    // 1. For algorithm selection:
    connect(ui->chkAlgA, SIGNAL(clicked()), this, SLOT(onAlgSelected()));
    connect(ui->chkAlgB, SIGNAL(clicked()), this, SLOT(onAlgSelected()));

    // 2. For the preference rating:
    connect(ui->chkPref1, SIGNAL(clicked()), this, SLOT(onPrefSelected()));
    connect(ui->chkPref2, SIGNAL(clicked()), this, SLOT(onPrefSelected()));
    connect(ui->chkPref3, SIGNAL(clicked()), this, SLOT(onPrefSelected()));
    connect(ui->chkPref4, SIGNAL(clicked()), this, SLOT(onPrefSelected()));
    connect(ui->chkPref5, SIGNAL(clicked()), this, SLOT(onPrefSelected()));

    // Initialise the first input to the experiment.
    initFileNames();
    // Start the sound delivery.
    start();
}

MainWindow::~MainWindow()
{
    // Store the results in the output file, save it and then close.
    QTextStream out(outFile);
    for (int count = 0; count < nExp; count++)
        out << arrResult[0][count] << " " << arrResult[1][count] << "\n";

    outFile->close();
    delete ui;
    delete[] arrExpIdx;
    delete[] arrSigIdx;
    for (int count = 0; count < 2; count++)
        delete[] arrResult[count];
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// ONALGSELECTED(): an algorithm is selected in the checkbox. If this is the
// first selection,
// then enable the Choose button, to allow progression to next stage.
void MainWindow::onAlgSelected()
{
    if (isSelected == 0) {
        isSelected = 1; // Yes, one algorithm has been preferred.
        ui->butChoose->setEnabled(true);
    }

    // Code here to store the results.
    //    if (sender()->objectName() == QString("chkAlgA"))
    //        ui->chkPref1->setEnabled(true);
    //    else
    //        ui->chkPref2->setEnabled(true);
}

// ON_CHOOSE_CLICKED(): an algorithm choice has been confirmed. Gray out
// algorithm selection
// area and proceed to next stage of preference rating. Tool tip should be
// updated here.
// Using the `magical' connection properties of Qt here to connect the signal
// and slot.
void MainWindow::on_butChoose_clicked()
{
    // Gray out the area.
    ui->chkAlgA->setEnabled(false);
    ui->chkAlgB->setEnabled(false);
    ui->butChoose->setEnabled(false);

    // Activate preference area.
    ui->chkPref1->setEnabled(true);
    ui->chkPref2->setEnabled(true);
    ui->chkPref3->setEnabled(true);
    ui->chkPref4->setEnabled(true);
    ui->chkPref5->setEnabled(true);
    ui->lblTip->setText(
        tr("How much better is the chosen sound as compared to the other? You "
           "can listen to the signals again if you wish."));

    // Store the preference result in the out array.
    if (ui->chkAlgA->isChecked())
        arrResult[0][arrExpIdx[expIdx] - 1] = arrSigIdx[expIdx];
    else
        arrResult[0][arrExpIdx[expIdx] - 1] = 3 - arrSigIdx[expIdx];
}

// ONPREFSELECTED(): preference rating has been made. If this is the first
// selection,
// then enable butNext button, to allow progression to next stage.
void MainWindow::onPrefSelected()
{
    if (isRated == 0) {
        isRated = 1; // Yes, one preference atleast has been set.
        ui->butNext->setEnabled(true);
    }

    // Code here to store the results.
    //    if (sender()->objectName() == QString("chkAlgA"))
    //        ui->chkPref1->setEnabled(true);
    //    else
    //        ui->chkPref2->setEnabled(true);
}

// ON_NEXT_CLICKED(): the second stage (preference rating) is complete and we
// can move to next experiment.
// Load the new sound, gray out preference rating, update tool tip and progress
// bar, and gray out
// preference rating.
// Using the `magical' connection properties of Qt here to connect the signal
// and slot.
void MainWindow::on_butNext_clicked()
{
    // Stop playback first
    processor.reset();
    // First store the rating result in the output array.
    if (ui->chkPref1->isChecked())
        arrResult[1][arrExpIdx[expIdx] - 1] = 1;
    else if (ui->chkPref2->isChecked())
        arrResult[1][arrExpIdx[expIdx] - 1] = 2;
    else if (ui->chkPref3->isChecked())
        arrResult[1][arrExpIdx[expIdx] - 1] = 3;
    else if (ui->chkPref4->isChecked())
        arrResult[1][arrExpIdx[expIdx] - 1] = 4;
    else
        arrResult[1][arrExpIdx[expIdx] - 1] = 5;

    isRated = isSelected = 0;

    isPlayingA = isPlayingB = false;

    expIdx++; // Increment the experiment count.

    // If we are already done, quit!
    if (expIdx == nExp) {
        QMessageBox::information(this, tr("End"),
                                 tr("Thank you for your participation"),
                                 QMessageBox::Ok);
        qApp->exit(0);
    }

    // Initalise the new play list and start the processor again.
    initFileNames();
    start();

    // To uncheck all boxes, first allow non-exlusivity for both
    // algorithm selection and preference ratings.

    // Initalise all boxes to unchecked.
    ui->chkAlgA->setEnabled(true);
    ui->chkAlgA->setAutoExclusive(false);
    ui->chkAlgA->setChecked(false);

    ui->chkAlgB->setEnabled(true);
    ui->chkAlgA->setAutoExclusive(false);
    ui->chkAlgB->setChecked(false);

    ui->chkAlgA->setAutoExclusive(true);
    ui->chkAlgB->setAutoExclusive(true);

    ui->butChoose->setEnabled(false);

    ui->chkPref1->setEnabled(false);
    ui->chkPref1->setAutoExclusive(false);
    ui->chkPref1->setCheckState(Qt::Unchecked);

    ui->chkPref2->setEnabled(false);
    ui->chkPref2->setAutoExclusive(false);
    ui->chkPref2->setCheckState(Qt::Unchecked);

    ui->chkPref3->setEnabled(false);
    ui->chkPref3->setAutoExclusive(false);
    ui->chkPref3->setCheckState(Qt::Unchecked);

    ui->chkPref4->setEnabled(false);
    ui->chkPref4->setAutoExclusive(false);
    ui->chkPref4->setCheckState(Qt::Unchecked);

    ui->chkPref5->setEnabled(false);
    ui->chkPref5->setAutoExclusive(false);
    ui->chkPref5->setCheckState(Qt::Unchecked);

    // Re-enable exclusivity.
    ui->chkPref1->setAutoExclusive(true);
    ui->chkPref2->setAutoExclusive(true);
    ui->chkPref3->setAutoExclusive(true);
    ui->chkPref4->setAutoExclusive(true);
    ui->chkPref5->setAutoExclusive(true);

    // Disable butNext
    ui->butNext->setEnabled(false);
    ui->butNext->setText(tr("Next (%1/%2)").arg(expIdx + 1).arg(nExp));
    ui->lblTip->setText(tr("Listen to A and B. Which is better?"));
}

// CREATEEXPERIMENT() sets up the data for the audio device, initialises the
// processor block
// and begins to stream data in an infinite loop. Initial mixer settins are zero
// on all
// channels.
ExperimentData MainWindow::createExperiment(const QString &file1Name,
                                            const QString &file2Name)
{
    const QString file1Path =
        Paths::searchFile(file1Name, Paths::dataDirectories());
    const QString file2Path =
        Paths::searchFile(file2Name, Paths::dataDirectories());

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

    BlockData mixer(QLatin1String("mixer"), QLatin1String("Mixer"));
    mixer.setInputPorts(2);
    mixer.setOutputPorts(1);
    mixer.setParameter(QLatin1String("logarithmicGain"), false);
    mixer.setParameter(QLatin1String("matrix"), QLatin1String("[[0 0]]"));
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

// START() is the wrapper function to be called with the names of the sound
// files as input.
// It assigns the first file to button A and second to button B.
void MainWindow::start()
{
    try {
        ExperimentData expdata = this->createExperiment(file1Name, file2Name);

        //        this->processor.reset();
        this->processor.reset(new ExperimentProcessor(expdata));
        this->processor->prepare();
        this->processor->start();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, QLatin1String("Error"),
                              QString::fromLocal8Bit(e.what()));
    }
}

// ON_BUTPLYSNDA_CLICKED() changes the mixer settings in favour of sound A.
void MainWindow::on_butPlySndA_clicked()
{
    if (!isPlayingA) {
        processor->setParameterValue(QLatin1String("mixer"),
                                     QLatin1String("matrix"),
                                     QLatin1String("[[1 0]]"));
        isPlayingA = true;
        isPlayingB = false;
    } else {
        processor->setParameterValue(QLatin1String("mixer"),
                                     QLatin1String("matrix"),
                                     QLatin1String("[[0 0]]"));
        isPlayingA = false;
    }
}

// ON_BUTPLYSNDB_CLICKED() changes the mixer settings in favour of sound B.
void MainWindow::on_butPlySndB_clicked()
{
    if (!isPlayingB) {
        processor->setParameterValue(QLatin1String("mixer"),
                                     QLatin1String("matrix"),
                                     QLatin1String("[[0 1]]"));
        isPlayingA = false;
        isPlayingB = true;
    } else {
        processor->setParameterValue(QLatin1String("mixer"),
                                     QLatin1String("matrix"),
                                     QLatin1String("[[0 0]]"));
        isPlayingB = false;
    }
}

void MainWindow::initFileNames()
{
    file1Name = QString::fromLatin1("preftestdata/exp_%1_a%2.wav")
                    .arg(arrExpIdx[expIdx])
                    .arg(arrSigIdx[expIdx]);
    file2Name =
        QString::fromLatin1("preftestdata/exp_%1_a%2.wav")
            .arg(arrExpIdx[expIdx])
            .arg(3 -
                 arrSigIdx[expIdx]); // QLatin1String("testdata/sin1000Hz.wav");
    //    file1Name = QString::fromLatin1("testdata/sin%1Hz.wav").arg(1000);
    //        file2Name = QLatin1String("testdata/sin50Hz.wav");
    //        file1Name = QLatin1String("testdata/sin1000Hz.wav");
    //        file2Name = QLatin1String("testdata/sin50Hz.wav");
}
