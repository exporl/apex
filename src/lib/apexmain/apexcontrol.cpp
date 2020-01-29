#include "apexdata/device/devicesdata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/parameters/generalparameters.h"

#include "apextools/apexpaths.h"
#include "apextools/apextools.h"
#include "apextools/desktoptools.h"
#include "apextools/exceptions.h"
#include "apextools/version.h"

#include "apexwriters/experimentwriter.h"

#include "calibration/calibrationdatabase.h"
#include "calibration/calibrator.h"

#include "apextools/settingsfactory.h"
#include "common/paths.h"

#include "device/soundcardsettings.h"

#include "study/studymanager.h"

#include "gui/connectiondialog.h"
#include "gui/mainwindow.h"
#include "gui/mru.h"
#include "gui/plugindialog.h"
#include "gui/soundcardsdialog.h"
#include "gui/startupdialog.h"

#include "runner/experimentrundelegate.h"
#include "runner/flowrunner.h"
#include "runner/pluginrunner.h"
#include "runner/simplerunner.h"

#include "screen/apexscreen.h"

#include "stimulus/stimuluscontrol.h"
#include "stimulus/stimulusoutput.h"

#include "study/studymodule.h"

#include "wavstimulus/soundcarddisplayer.h"
#include "wavstimulus/wavdeviceio.h"

#include "apexcontrol.h"
#include "errorhandler.h"
#include "experimentcontrol.h"
#include "experimentio.h"
#include "mainconfigfileparser.h"
#include "outputrecorder.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#ifdef Q_OS_ANDROID
#include "apexandroidnative.h"
#endif

using namespace cmn;
using namespace apex;

ApexControl *ApexControl::instance = NULL;

ApexControl::ApexControl(bool launchStandalone)
    : mod_experimentselector(new SimpleRunner),
      m_Wnd(new gui::ApexMainWindow()),
      recordExperiments(false),
      dummySoundcard(false),
      autoStartExperiments(false),
      deterministicRandom(false),
      noResults(false),
      autoSaveResults(false),
      exitAfter(false),
      useBertha(true),
      launchStandalone(launchStandalone)
{
    instance = this;

    ApexTools::InitRand();

    QApplication::setWindowIcon(QIcon(
        Paths::searchFile(QL1S("apex_icon.svg"), Paths::iconDirectories())));

    connect(m_Wnd, SIGNAL(fileExit()), this, SLOT(fileExit()));
    connect(m_Wnd, SIGNAL(fileOpen()), this, SLOT(fileOpen()));
    connect(m_Wnd, SIGNAL(saveExperiment()), this, SLOT(saveExperiment()));
    connect(m_Wnd, SIGNAL(startPluginRunner()), this,
            SLOT(startPluginRunner()));
    connect(m_Wnd, SIGNAL(selectSoundcard()), this, SLOT(selectSoundcard()));
    connect(m_Wnd->GetMru(), SIGNAL(Released(QString)), this,
            SLOT(fileOpen(QString)));
    connect(m_Wnd, SIGNAL(StopOutput()), this, SLOT(StopOutput()));
    connect(m_Wnd, SIGNAL(autoAnswerClicked()), this, SLOT(enableAutoAnswer()));
    connect(m_Wnd, SIGNAL(showStimulus()), this, SLOT(ShowStimulus()));
    connect(m_Wnd, SIGNAL(recalibrateClicked()), this, SLOT(calibrate()));
#ifdef Q_OS_ANDROID
    connect(m_Wnd, SIGNAL(createShortcutToFile()),
            apex::android::ApexAndroidBridge::instance(),
            SLOT(createShortcutToFile()));
    connect(m_Wnd, SIGNAL(createShortcutToRunner()),
            apex::android::ApexAndroidBridge::instance(),
            SLOT(createShortcutToRunner()));
    connect(m_Wnd, SIGNAL(enableKioskMode()),
            apex::android::ApexAndroidBridge::instance(),
            SLOT(enableKioskMode()));
    connect(m_Wnd, SIGNAL(disableKioskMode()),
            apex::android::ApexAndroidBridge::instance(),
            SLOT(disableKioskMode()));
#endif
    connect(m_Wnd, SIGNAL(startGdbServer()), this, SLOT(startGdbServer()));
    connect(m_Wnd, SIGNAL(openStudyManager()), this, SLOT(openStudyManager()));
    connect(mod_experimentselector.data(),
            SIGNAL(selected(data::ExperimentData *)), this,
            SLOT(newExperiment(data::ExperimentData *)));
    connect(mod_experimentselector.data(),
            SIGNAL(errorMessage(QString, QString)), this,
            SLOT(errorMessage(QString, QString)));
    connect(mod_experimentselector.data(), SIGNAL(setResultsFilePath(QString)),
            this, SLOT(setResultsFilePath(QString)));

    // make sure to quit when everything is closed
    connect(QCoreApplication::instance(), SIGNAL(lastWindowClosed()), this,
            SLOT(fileExit()));

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() +
                                     "/plugins");

    if (!configure())
        return;

    qCDebug(APEX_RS) << "Configure done";

    // Break service dependency loop by loading after the QT main loop has been
    // started
    QTimer::singleShot(0, this, SLOT(StartUp()));
}

ApexControl::~ApexControl()
{
    instance = NULL;
    stimulusControl.reset();
    runDelegate.reset();
    experimentData.reset();
    // Do this manually before the window is destroyed. Otherwise the screen
    // delegates can not cope with the deleted widgets.
    // experimentRunDelegate.reset();
    mod_experimentselector.reset();
    delete m_Wnd;
}

ApexControl &ApexControl::Get()
{
    return *instance;
}

void ApexControl::StartUp()
{
#ifndef NOMAXIMIZE
    m_Wnd->showMaximized();
#else
    m_Wnd->show();
#endif
    qCDebug(APEX_RS) << "Shown MainWindow";

    if (!launchStandalone) {
        parseCommandLine();
        qCDebug(APEX_RS) << "Parse commandline done";
    }

    ensureSshKeypair();

#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::signalApexInitialized();
#endif
    Q_EMIT apexInitialized();
}

void ApexControl::ensureSshKeypair()
{
    QDir keyDirectory = ApexPaths::GetSshKeyDirectory();
    keyDirectory.mkpath(QSL("."));
    if (!keyDirectory.exists(keyDirectory.filePath(QSL("id_rsa")))) {
        ApexTools::generateKeyPair(
            keyDirectory.filePath(QSL("id_rsa")),
            StudyManager::instance()->getUsernameAndEmail().second);
    }
}

// parse mainconfig
bool ApexControl::configure()
{
    qCInfo(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(
                              "APEX", tr("Apex version %1, %2")
                                          .arg(APEX_SCHEMA_VERSION)
                                          .arg(ApexTools::fetchVersion()))));
    qCInfo(APEX_RS, "%s",
           qPrintable(QSL("%1: %2").arg(
               "APEX", tr("Compiled with Qt version %1").arg(QT_VERSION_STR))));
    qCInfo(APEX_RS, "%s",
           qPrintable(QSL("%1: %2").arg(
               "APEX", tr("Running with Qt version %1").arg(qVersion()))));
    qCInfo(
        APEX_RS, "%s",
        qPrintable(QSL("%1: %2").arg(
            "APEX",
            tr("The APEX 4 binaries use the Nokia Qt framework available\n"
               "under the GNU General Public License (GPL version 2) on:\n"
               "http://qt.nokia.com/. The APEX 4 source code and\n"
               "binaries are available under the GNU General Public License\n"
               "(GPL version 2) on http://www.kuleuven.be/exporl/apex."))));

    qCInfo(APEX_RS, "%s",
           qPrintable(QSL("%1: %2").arg(
               "ApexControl", tr("Parsing Apex config file %1\nwith schema %2")
                                  .arg(ApexPaths::GetApexConfigFilePath())
                                  .arg(ApexPaths::GetApexConfigSchemaPath()))));

    try {
        MainConfigFileParser::Get().parse();
    } catch (const std::exception &e) {
        qCCritical(
            APEX_RS, "%s",
            qPrintable(QSL("%1: %2").arg(
                "ApexControl",
                tr("Parsing of main config file failed: %1").arg(e.what()))));
        return false;
    }

    // check if experiment schema file exists
    QString experimentSchema = ApexPaths::GetExperimentSchemaPath();
    if (!QFile::exists(experimentSchema)) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ApexControl", tr("Experiment schema file not found: "
                                         "%1\nCheck main config file")
                                          .arg(experimentSchema))));
        return false;
    }

    if (MainConfigFileParser::Get()
            .data()
            .areInstallationWideSettingsEnabled()) {
        SettingsFactory::initializeForInstallationWideSettings();
    } else {
        SettingsFactory::initializeForUserSpecificSettings();
    }

    qCInfo(APEX_RS, "ApexControl: Done");
    m_Wnd->SetOpenDir(
        Paths::searchDirectory(QL1S("examples"), Paths::dataDirectories()));
    m_Wnd->MruLoad(); // will override sOpenDir if specified

    useBertha = MainConfigFileParser::Get().data().useBertha();

    return true;
}

void ApexControl::editApexconfig()
{
    // if necessary copy apexconfig file to user path
    QString userApexConfig(ApexPaths::GetUserConfigFilePath() +
                           QL1S("/apexconfig.xml"));

    if (!QFile::exists(userApexConfig)) {
        qCDebug(APEX_RS) << "Copying file";
        QFile source(ApexPaths::GetApexConfigFilePath());

        if (!source.copy(userApexConfig)) {
            qCCritical(
                APEX_RS, "%s",
                qPrintable(QSL("%1: %2").arg(
                    "ApexControl", tr("Cannot copy apexconfig from %1 to %2")
                                       .arg(source.fileName())
                                       .arg(userApexConfig))));
            return;
        }
    }

    if (!DesktopTools::editFile(userApexConfig)) {
        QMessageBox::critical(m_Wnd, tr("Error editing ApexConfig"),
                              tr("Could not open ApexConfig.xml for editing."
                                 " Please open the file \n%1\n yourself.")
                                  .arg(userApexConfig));
    }
}

void ApexControl::showPluginDialog()
{
    try {
        PluginDialog *pluginDialog = new PluginDialog(m_Wnd);
        pluginDialog->show();
    } catch (const std::exception &e) {
        QMessageBox::critical(m_Wnd, tr("Plugin Error"),
                              tr("Unable to display available plugins\n%1")
                                  .arg(QString::fromLocal8Bit(e.what())));
    }
}

// FIXME this method should ONLY parse!
void ApexControl::parseCommandLine()
{
    qCDebug(APEX_RS) << "Parsing command line";

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QSL("experiment"),
                                 tr("Experiment to load on startup"));
    parser.addOption(QCommandLineOption(QSL("soundcards"),
                                        tr("List soundcards in the system")));
    parser.addOption(
        QCommandLineOption(QStringList() << QSL("resultsfile") << QSL("r"),
                           tr("Name of the results file"), tr("resultfile")));
    parser.addOption(QCommandLineOption(
        QSL("pluginrunner"), tr("Load plugin runner"), tr("pluginrunner")));
    parser.addOption(
        QCommandLineOption(QSL("record"), tr("Record output of experiments")));
    parser.addOption(QCommandLineOption(
        QSL("virtual-soundcard"), tr("Use a virtual soundcard that will not "
                                     "play any sound and will not wait "
                                     "for the sampling time of each sample, "
                                     "thus running much faster than an "
                                     "actual sound card")));
    parser.addOption(QCommandLineOption(
        QSL("autostart"),
        tr("Automatically start an experiment when it is loaded")));
    parser.addOption(QCommandLineOption(QSL("autoanswer"),
                                        tr("Automatically answer trials")));
    parser.addOption(QCommandLineOption(
        QSL("deterministic"), tr("Make all random stuff deterministic")));
    parser.addOption(QCommandLineOption(
        QSL("noresults"),
        tr("Do not handle results in any way after an experiment")));
    parser.addOption(QCommandLineOption(
        QSL("autosaveresults"),
        tr("Automatically save results after the experiment")));
    parser.addOption(QCommandLineOption(
        QSL("exitafter"), tr("Quit Apex after an experiment is done")));
    parser.addOption(
        QCommandLineOption(QSL("disable-bertha"),
                           tr("Disable bertha as audio backend for acoustic "
                              "experiments and use the old system")));
    parser.addOption(QCommandLineOption(
        QSL("calibrationsetup"),
        tr("Specify calibration setup to be used in the experiment"), "Calib"));

    parser.process(*qApp);

    QString experimentfile = parser.positionalArguments().value(0);
    bool listSoundcards = parser.isSet(QSL("soundcards"));
    mSaveFilename = parser.value(QSL("resultsfile"));
    bool pluginRunner = parser.isSet(QSL("pluginrunner"));
    recordExperiments = parser.isSet(QSL("record"));
    dummySoundcard = parser.isSet(QSL("virtual-soundcard"));
    autoStartExperiments = parser.isSet(QSL("autostart"));
    autoAnswer = parser.isSet(QSL("autoanswer"));
    deterministicRandom = parser.isSet(QSL("deterministic"));
    noResults = parser.isSet(QSL("noresults"));
    autoSaveResults = parser.isSet(QSL("autosaveresults"));
    exitAfter = parser.isSet(QSL("exitafter"));
    bool useCalibrationSetup = parser.isSet(QSL("calibrationsetup"));
    calibrationSetup = parser.value("calibrationsetup");

    /* command line argument overwrites config */
    useBertha = parser.isSet(QSL("disable-bertha")) ? false : useBertha;

    if (useCalibrationSetup) {
        if (CalibrationDatabase().hardwareSetups().contains(calibrationSetup)) {
            qCInfo(APEX_RS, "%s", qPrintable(QSL("%1").arg(
                                      tr("Setting calibration to setup \"%1\"")
                                          .arg(calibrationSetup))));
            CalibrationDatabase().setCurrentHardwareSetup(calibrationSetup);
        } else {
            qCInfo(
                APEX_RS, "%s",
                qPrintable(QSL("%1").arg(
                    tr("Calibration setup \"%1\" does not exist! Reverting to "
                       "default: \"%2\"")
                        .arg(calibrationSetup,
                             CalibrationDatabase().currentHardwareSetup()))));
        }
    }

    if (autoStartExperiments)
        flags |= ExperimentControl::AutoStart;
    if (noResults)
        flags |= ExperimentControl::NoResults;
    if (deterministicRandom) {
        flags |= ExperimentControl::Deterministic;
        Random::setDeterministic(true);
        ApexTools::InitRand(0);
    }

    if (listSoundcards) {
        stimulus::SoundCardDisplayer::ShowSoundCards();
        QCoreApplication::quit();
        return;
    }

    if (pluginRunner) {
        mod_experimentselector.reset(new PluginRunner());

        connect(mod_experimentselector.data(),
                SIGNAL(selected(data::ExperimentData *)), this,
                SLOT(newExperiment(data::ExperimentData *)));
        connect(mod_experimentselector.data(), SIGNAL(selected(QString)), this,
                SLOT(fileOpen(QString)));
        connect(mod_experimentselector.data(),
                SIGNAL(errorMessage(const QString &, const QString &)), this,
                SLOT(errorMessage(const QString &, const QString &)));

        mod_experimentselector->select(parser.value(QSL("pluginrunner")));
        return;
    }

    if (!experimentfile.isEmpty()) {
        QFileInfo url(experimentfile);
        if (url.isFile()) {
            qCInfo(APEX_RS,
                   "ApexControl: Using filename given on the command line");
            if (!mod_experimentselector->select(url.absoluteFilePath()) &&
                exitAfter)
                QCoreApplication::quit();
        } else if (url.isDir()) {
            m_Wnd->SetOpenDir(url.absoluteFilePath());
            fileOpen();
        }
    }
#ifndef Q_OS_ANDROID
    else
        showStartupDialog();
#endif
}

bool ApexControl::newExperiment(data::ExperimentData *data)
{
    if (data == 0)
        return false;

    if (experimentControl && experimentControl->isRunning())
        return false;

    experimentControl.reset();
    experimentControl.reset(new ExperimentControl(flags));
    connect(experimentControl.data(), SIGNAL(errorMessage(QString, QString)),
            this, SLOT(errorMessage(QString, QString)));
    connect(experimentControl.data(), &ExperimentControl::experimentClosed,
            mod_experimentselector.data(), &ExperimentRunner::experimentDone);

    ErrorHandler::instance()->clearCounters();

    try {
        if (recordExperiments)
            setupOutputRecording(data, useBertha);

        if (dummySoundcard) {
            const data::DevicesData *devices = data->devicesData();
            data::DevicesData::const_iterator devicesIt = devices->begin();
            data::DeviceData *device = devicesIt.value();

            device->setSilent();
        }

        if (exitAfter)
            data->generalParameters()->setExitAfter(true);
        if (autoSaveResults)
            data->generalParameters()->setAutoSave(true);

        experimentData.reset(data);
        runDelegate.reset(new ExperimentRunDelegate(
            *experimentData, MainConfigFileParser::Get().data(), 0,
            deterministicRandom, useBertha));
        setupIo();
        experimentControl->loadExperiment(runDelegate.data());

        connect(experimentControl.data(), SIGNAL(experimentDone()), this,
                SLOT(afterExperiment()));

        m_Wnd->ApplyConfig(experimentData->screensData());
        m_Wnd->EnableSkip(experimentData->generalParameters()->GetAllowSkip());

        if (autoAnswer) {
            if (deterministicRandom)
                m_Wnd->doDeterministicAutoAnswer();

            enableAutoAnswer();
        }
    } catch (std::exception &e) {
        qCCritical(APEX_RS, "ApexControl: %s", e.what());
    } catch (...) {
        qCCritical(APEX_RS, "ApexControl: FAILED");
    }

    if (!StudyManager::instance()->belongsToActiveStudy(data->fileName()))
        m_Wnd->SetOpenDir(QFileInfo(experimentData->fileName()).absolutePath());

    if (ErrorHandler::instance()->numberOfErrors())
        return false;

    qCInfo(APEX_RS, "ApexControl: DONE");
    experimentControl->start();
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::signalExperimentStarted();
#endif
    return true;
}

void ApexControl::afterExperiment()
{
    m_Wnd->Finished();
    bool quit = experimentData->generalParameters()->GetExitAfter();

    mod_experimentselector->makeVisible();

    disconnect(experimentControl.data(), SIGNAL(experimentDone()), this,
               SLOT(afterExperiment()));

    runDelegate.reset();
    experimentData.reset();
    stimulusControl.reset();

#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::signalExperimentFinished();
#endif

    if (quit)
        QCoreApplication::quit();
}

void ApexControl::setupIo()
{
    m_Wnd->setExperimentRunDelegate(runDelegate.data());
    runDelegate->makeModules();

    ExperimentIo *io = experimentControl->io();
    io->setGui(m_Wnd);

    stimulusControl.reset(
        new stimulus::StimulusControl(runDelegate->modOutput()));

    connect(io, SIGNAL(responseGiven(const ScreenResult *)),
            runDelegate->modScreen(), SLOT(iAnswered(const ScreenResult *)));
    connect(io, SIGNAL(newStimulus(QString, double)), stimulusControl.data(),
            SLOT(playStimulus(QString, double)));
    connect(stimulusControl.data(), SIGNAL(stimulusPlayed()), io,
            SLOT(onStimulusPlayed()));
    connect(experimentControl.data(), SIGNAL(experimentClosed(QString)),
            runDelegate->modStudy(), SLOT(savedResultfile(QString)));
}

bool ApexControl::isExperimentRunning() const
{
    return experimentControl && experimentControl->isRunning();
}

void ApexControl::fileOpen(const QString &file)
{
    mod_experimentselector.reset(new SimpleRunner());

    connect(mod_experimentselector.data(),
            SIGNAL(selected(data::ExperimentData *)), this,
            SLOT(newExperiment(data::ExperimentData *)));
    connect(mod_experimentselector.data(),
            SIGNAL(errorMessage(QString, QString)), this,
            SLOT(errorMessage(QString, QString)));
    connect(mod_experimentselector.data(), SIGNAL(setResultsFilePath(QString)),
            this, SLOT(setResultsFilePath(QString)));
    if (!StudyManager::instance()->belongsToActiveStudy(file))
        connect(mod_experimentselector.data(), SIGNAL(opened(QString)),
                m_Wnd->GetMru(), SLOT(addAndSave(QString)));

    if (file.isEmpty())
        mod_experimentselector->selectFromDir(m_Wnd->GetOpenDir());
    else if (QFileInfo(file).isDir())
        mod_experimentselector->selectFromDir(file);
    else
        mod_experimentselector->select(file);
}

void ApexControl::saveExperiment()
{
    if (!experimentData)
        return;

    QFileDialog dlg(m_Wnd, tr("Save experiment file"));
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setNameFilter(tr("Apex experiment files (*.apx *.xml)"));
    dlg.setDefaultSuffix("apx");
    dlg.setAcceptMode(QFileDialog::AcceptSave);

    QString loadedFile = experimentData->fileName();
    if (!loadedFile.isNull()) {
        dlg.selectFile(loadedFile);
    } else {
        QStringList docLocations = QStandardPaths::standardLocations(
            QStandardPaths::DocumentsLocation);
        if (loadedFile.isEmpty() && !docLocations.isEmpty())
            dlg.setDirectory(docLocations.first());
    }
    ApexTools::expandWidgetToWindow(&dlg);
    if (dlg.exec() == QDialog::Accepted) {
        QString file = dlg.selectedFiles().first();
        writer::ExperimentWriter::write(*experimentData, file);
    }
}

void ApexControl::fileExit()
{
    QCoreApplication::quit();
}

void ApexControl::ShowStimulus()
{
    try {
        ConnectionDialog dialog(m_Wnd);
        dialog.setDelegate(*runDelegate);
        dialog.exec();
    } catch (std::exception &e) {
        QMessageBox::critical(
            m_Wnd, tr("Graph Display Error"),
            tr("Unable to display connection graph: %1").arg(e.what()));
    }
}

void ApexControl::enableAutoAnswer()
{
    if (isExperimentRunning()) {
        QMessageBox::information(
            m_Wnd, tr("Auto answer"),
            tr("You selected the auto answer option.\n"
               "After you answer to the current trial, a "
               "random button will be selected automatically "
               "from the buttongroup for each trial.\n"
               "Warning: this is an experimental feature!"));
    }

    runDelegate->enableAutoAnswer();
}

void ApexControl::StopOutput()
{
    // FIXME move to experimentcontrol
    // experimentControl->runDelegate()->modOutput()->StopDevices();

    QMessageBox::warning(m_Wnd, tr("Emergency button activated"),
                         tr("You have activated the emergency button. "
                            "This might result in unstable behavior and "
                            "corrupted test results. Therefore you will now "
                            "get the chance to save the results and Apex will "
                            "be closed."));

    fileExit();
}

const ExperimentRunDelegate &ApexControl::GetCurrentExperimentRunDelegate()
{
    return *runDelegate;
}

const data::ExperimentData &ApexControl::GetCurrentExperiment()
{
    return *experimentData;
}

void ApexControl::showStartupDialog()
{
    StartupDialog dialog(m_Wnd->GetMru()->items(true), m_Wnd);
    if (dialog.exec() != QDialog::Accepted)
        return;

    dialog.hide();

    if (dialog.selectedFile().isEmpty())
        fileOpen();
    else
        fileOpen(dialog.selectedFile());
}

void apex::ApexControl::startPluginRunner(const QString &runner)
{
    mod_experimentselector.reset(new PluginRunner());

    connect(mod_experimentselector.data(),
            SIGNAL(selected(data::ExperimentData *)), this,
            SLOT(newExperiment(data::ExperimentData *)));
    connect(mod_experimentselector.data(), SIGNAL(selected(QString)), this,
            SLOT(fileOpen(QString)));
    connect(mod_experimentselector.data(),
            SIGNAL(errorMessage(const QString &, const QString &)), this,
            SLOT(errorMessage(const QString &, const QString &)));

    mod_experimentselector->select(runner);
}

void ApexControl::selectSoundcard()
{
    stimulus::WavDeviceIO::showSoundcardDialog();
}

gui::ApexMainWindow *apex::ApexControl::mainWindow()
{
    return m_Wnd;
}

const QString &apex::ApexControl::saveFilename() const
{
    return mSaveFilename;
}

void apex::ApexControl::setResultsFilePath(QString filename)
{
    mSaveFilename = filename;
    autoSaveResults = true;
}

void apex::ApexControl::errorMessage(const QString &source,
                                     const QString &message)
{
    qCCritical(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(source, message)));
}

void apex::ApexControl::calibrate()
{
    Q_ASSERT(runDelegate->modCalibrator());

    if (runDelegate->modOutput()->IsOffLine()) {
        qCCritical(APEX_RS,
                   "ApexControl: cannot calibrate with offline devices");
        return;
    }

    if (runDelegate->modCalibrator()->calibrate(true))
        runDelegate->modCalibrator()->updateParameters();
}

void ApexControl::startGdbServer()
{
#ifdef Q_OS_ANDROID
    android::ApexAndroidBridge::startGdbServer();
#endif
}

void ApexControl::openStudyManager()
{
    StudyManager::instance()->showConfigurationDialog();
}

const ExperimentControl &ApexControl::getCurrentExperimentControl()
{
    return *experimentControl;
}
