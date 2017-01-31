#include "apexdata/experimentdata.h"

#include "apexdata/parameters/generalparameters.h"
#include "apexdata/device/devicesdata.h"

#include "apextools/apextools.h"
#include "apextools/desktoptools.h"
#include "apextools/exceptions.h"
#include "apextools/pathtools.h"

#include "apextools/services/application.h"
#include "apextools/services/paths.h"

#include "apextools/status/statusreporter.h"

#include "apextools/version.h"

#include "apexwriters/experimentwriter.h"

#include "calibration/calibrator.h"

#include "common/optioncontext.h"

#include "gui/connectiondialog.h"
#include "gui/mainwindow.h"
#include "gui/mru.h"
#include "gui/plugindialog.h"
#include "gui/startupdialog.h"
#include "gui/soundcardsdialog.h"

#include "runner/experimentrundelegate.h"
#include "runner/pluginrunner.h"
#include "runner/simplerunner.h"

// TODO ANDROID flowrunner uses webkitwidgets
#ifndef Q_OS_ANDROID
#include "runner/flowrunner.h"
#endif

#include "screen/apexscreen.h"

#include "services/errorhandler.h"
#include "services/mainconfigfileparser.h"

#include "stimulus/stimuluscontrol.h"
#include "stimulus/stimulusoutput.h"

#include "wavstimulus/soundcarddisplayer.h"
#include "wavstimulus/wavdeviceio.h"
#include "device/soundcardsettings.h"

#include "apexcontrol.h"
#include "experimentcontrol.h"
#include "experimentio.h"
#include "outputrecorder.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include <iostream>

using namespace cmn;
using namespace apex;

ApexControl::ApexControl() :
        mod_experimentselector(new SimpleRunner),
        m_Wnd(new gui::ApexMainWindow()),
        m_error(ErrorHandler::Get())
{
    ApexTools::InitRand();

    //experimentControl->addErrorListener(&ErrorHandler::Get());

    //setup mainwindow

    try
    {
        QApplication::setWindowIcon(QIcon(Paths::Get().GetMainLogoPath()));
    }
    catch (ApexStringException& e)
    {
        m_error.addWarning("ApexControl", tr("Unable to set main logo."));
        m_error.addWarning("ApexControl", e.what());
    }

    connect(m_Wnd, SIGNAL(fileExit()), this, SLOT(fileExit()));
    connect(m_Wnd, SIGNAL(fileOpen()), this, SLOT(fileOpen()));
    connect(m_Wnd, SIGNAL(saveExperiment()), this, SLOT(saveExperiment()));
    connect(m_Wnd, SIGNAL(startPluginRunner()), this, SLOT(startPluginRunner()));
    connect(m_Wnd, SIGNAL(selectSoundcard()), this, SLOT(selectSoundcard()));
    connect(m_Wnd->GetMru(), SIGNAL(Released(QString)),
            this, SLOT(fileOpen(QString)));
    connect(m_Wnd, SIGNAL(StopOutput()), this, SLOT(StopOutput()));
    connect(m_Wnd, SIGNAL(autoAnswerClicked(bool)),
            this, SLOT(setAutoAnswer(bool)));
    connect(m_Wnd, SIGNAL(showStimulus()), this, SLOT(ShowStimulus()));
    connect(m_Wnd, SIGNAL(recalibrateClicked()), this, SLOT(calibrate()));
    connect(mod_experimentselector.data(), SIGNAL(selected(data::ExperimentData*)),
            this, SLOT(newExperiment(data::ExperimentData*)));
    connect(mod_experimentselector.data(), SIGNAL(errorMessage(QString, QString)),
            this, SLOT(errorMessage(QString,QString)));

    //make sure to quit when everything is closed
    connect(Application::Get().GetApplication(), SIGNAL(lastWindowClosed()),
            this, SLOT(fileExit()));

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
    runDelegate.reset();
    experimentData.reset();
    stimulusControl.reset();
    // Do this manually before the window is destroyed. Otherwise the screen
    // delegates can not cope with the deleted widgets.
    //experimentRunDelegate.reset();
    mod_experimentselector.reset();
    delete m_Wnd;
}

const char* ApexControl::Name()
{
    return "ApexControl";
}

int ApexControl::exec()
{
    return Application::Get().GetApplication()->exec();
}

void ApexControl::StartUp()
{
#ifndef NOMAXIMIZE
    m_Wnd->showMaximized();
#else
    m_Wnd->show();
#endif
    qCDebug(APEX_RS) << "Shown MainWindow";

    parseCommandLine();
    qCDebug(APEX_RS) << "Parse commandline done";
}

//parse mainconfig
bool ApexControl::configure()
{
    Paths& paths = Paths::Get();

    m_error.addMessage("APEX", tr("Apex version %1, %2").arg(APEX_VERSION).arg(ApexTools::fetchVersion()));
    m_error.addMessage("APEX", tr("Compiled with Qt version %1").arg(QT_VERSION_STR));
    m_error.addMessage("APEX", tr("Running with Qt version %1").arg(qVersion()));
    m_error.addMessage("APEX", tr(
                       "The APEX 3 binaries use the Nokia Qt framework available\n"
                       "under the GNU General Public License (GPL version 2) on:\n"
                       "http://qt.nokia.com/. The APEX 3 source code and\n"
                       "binaries are available under the GNU General Public License\n"
                       "(GPL version 2) on http://www.kuleuven.be/exporl/apex." ));

    m_error.addMessage("ApexControl",
                       tr("Parsing Apex config file %1\nwith schema %2")
                            .arg(paths.GetApexConfigFilePath())
                            .arg(paths.GetApexConfigSchemaPath()));

    try
    {
        MainConfigFileParser& mainConfigFileParser = MainConfigFileParser::Get();
        mainConfigFileParser.LogErrorsToApex(&m_error);
        mainConfigFileParser.CFParse();
    }
    catch (ApexStringException& e)
    {
        m_error.addError("ApexControl",
                         tr("Parsing of main config file failed: %1" )
                            .arg(e.what()));
        return false;
    }
    catch (...)
    {
        qFatal("Stop throwing garbage!");
    }

    //check if experiment schema file exists
    QString experimentSchema = paths.GetExperimentSchemaPath();
    if (!QFile::exists(experimentSchema))
    {
        m_error.addError("ApexControl",
                         tr("Experiment schema file not found: %1\nCheck main config file")
                                .arg(experimentSchema));

        return false;
    }

    m_error.addMessage("ApexControl", "Done");
    m_Wnd->SetOpenDir(paths.GetExamplesPath());
    m_Wnd->MruLoad();  //will override sOpenDir if specified

    return true;
}

void ApexControl::deletePluginCache()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    QString key = QString::fromLatin1("Qt Plugin Cache %1.%2.%3")
                        .arg((QT_VERSION & 0xff0000) >> 16)
                        .arg((QT_VERSION & 0x00ff00) >>  8);

    settings.remove(key.arg("false"));
    settings.remove(key.arg("debug"));
}

void ApexControl::editApexconfig()
{
    // if necessary copy apexconfig file to user path
    QString userApexConfig(
        PathTools::GetUserConfigFilePath() + "/" + Paths::cApexConfig);

    if (!QFile::exists(userApexConfig))
    {
        qCDebug(APEX_RS) << "Copying file";
        QFile source(Paths::Get().GetApexConfigFilePath());

        if (!source.copy(userApexConfig))
        {
            m_error.addError("ApexControl",
                             tr("Cannot copy apexconfig from %1 to %2")
                                .arg(source.fileName()).arg(userApexConfig));
            return;
        }
    }

    if (!DesktopTools::editFile(userApexConfig))
    {
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

//FIXME this method should ONLY parse!
void ApexControl::parseCommandLine()
{
    qCDebug(APEX_RS) << "Parsing command line";
    QMap<QString, QString> params;
    QString experimentfile;
    QStringList files;
    QString pluginRunner;
    bool listSoundcards = false;
    recordExperiments = false;
    dummySoundcard = false;
    autoStartExperiments = false;
    autoAnswer = false;
    deterministicRandom = false;
    noResults = false;
    exitAfter = false;

    OptionContext context(Application::Get().GetApplication()->arguments(),
            tr("[OPTION...] experimentfile"),
            OptionGroup(QString(), tr("Program Options"), QString(), QString(),
                    QList<OptionEntry>()
                    << OptionEntry(tr("soundcards"), 0, 0,
                        OptionEntry::NoArgument, &listSoundcards,
                        tr("List soundcards in the system"))
                    << OptionEntry(tr("resultsfile"), QLatin1Char('r'), 0,
                        OptionEntry::RequiredArgument, &mSaveFilename,
                        tr("Name of the results file"))
                    << OptionEntry(tr("pluginrunner"), QLatin1Char('p'), 0,
                        OptionEntry::RequiredArgument, &pluginRunner,
                        tr("Load plugin runner"))
                    << OptionEntry(tr("set"), QLatin1Char('s'), 0,
                        OptionEntry::RequiredArgument, mapOptionValue(&params),
                        tr("Set additional parameters of the form key=value"))
                    << OptionEntry(tr("record"), 0, 0,
                        OptionEntry::NoArgument, &recordExperiments,
                        tr("Record output of experiments"))
                    << OptionEntry(tr("virtual-soundcard"), 0, 0,
                        OptionEntry::NoArgument, &dummySoundcard,
                        tr("Use a virtual soundcard that will not play any sound and will not wait "
                           "for the sampling time of each sample, thus running much faster than an "
                           "actual sound card"))
                    << OptionEntry(tr("autostart"), QLatin1Char('a'), 0,
                        OptionEntry::NoArgument, &autoStartExperiments,
                        tr("Automatically start an experiment when it is loaded"))
                    << OptionEntry(tr("autoanswer"), QLatin1Char('d'), 0,
                        OptionEntry::NoArgument, &autoAnswer,
                        tr("Automatically answer trials"))
                    << OptionEntry(tr("deterministic"), QLatin1Char('d'), 0,
                            OptionEntry::NoArgument, &deterministicRandom,
                            tr("Make all random stuff deterministic"))
                    << OptionEntry(tr("noresults"), QLatin1Char('n'), 0,
                            OptionEntry::NoArgument, &noResults,
                            tr("Do not handle results in any way after an experiment"))
                    << OptionEntry(tr("autosaveresults"), 0, 0,
                            OptionEntry::NoArgument, &autoSaveResults,
                            tr("Automatically save results after the experiment"))
                    << OptionEntry(tr("exitafter"), QLatin1Char('x'), 0,
                            OptionEntry::NoArgument, &exitAfter,
                            tr("Quit Apex after an experiment is done"))
                    ));


    try
    {
        files = context.parse();
        if (!files.isEmpty())
            experimentfile = files[0];
    }
    catch (const std::exception &e)
    {
        qCDebug(APEX_RS) << "Error parsing command line:" << e.what();
        m_error.addWarning("ApexControl", tr("Error parsing commandline: %1").
                            arg(e.what()));
        return;
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

    if (listSoundcards)
    {
        stimulus::SoundCardDisplayer::ShowSoundCards();
        Application::Get().GetApplication()->quit();
        return;
    }

    if (!pluginRunner.isEmpty())
    {
        mod_experimentselector.reset( new PluginRunner() );

        connect(mod_experimentselector.data(),
                SIGNAL(Selected(data::ExperimentData*)),
                this, SLOT(newExperiment(data::ExperimentData*)));
        connect(mod_experimentselector.data(),
                SIGNAL(errorMessage(const QString&, const QString&)),
                this, SLOT(errorMessage(const QString&, const QString&)));

        mod_experimentselector->select(pluginRunner);
        return;
    }

    if (!experimentfile.isEmpty())
    {
        QFileInfo url(experimentfile);
        if (url.isFile())
        {
            m_error.addMessage ("ApexControl" ,"Using filename given on the command line");
            mod_experimentselector->select (url.absoluteFilePath());
        }
        else if (url.isDir())
        {
            m_Wnd->SetOpenDir (url.absoluteFilePath());
            fileOpen();
        }
    }
    else
        showStartupDialog();
}

bool ApexControl::newExperiment(data::ExperimentData* data)
{
    if (data == 0)
        return false;

    if (experimentControl && experimentControl->isRunning())
        return false;

//     experimentData.reset();
//     experimentIo.reset();

    experimentControl.reset();
    experimentControl.reset(new ExperimentControl(flags));
    connect(experimentControl.data(), SIGNAL(errorMessage(QString, QString)),
            this, SLOT(errorMessage(QString, QString)));
    connect(experimentControl.data(), SIGNAL(savedResults(QString)),
            mod_experimentselector.data(), SIGNAL(savedFile(QString)));

    m_error.clearCounters();

    try
    {
        if (recordExperiments)
        {
            OutputRecorder rec;

            if (!rec.setupRecording(data))
                m_error.addItems(rec.logger().items());
        }
        if (dummySoundcard) {
            const data::DevicesData* devices = data->devicesData();
            data::DevicesData::const_iterator devicesIt = devices->begin();
            data::DeviceData* device = devicesIt.value();

            device->setSilent();
        }

        if (exitAfter)
            data->generalParameters()->setExitAfter(true);
        if (autoSaveResults)
            data->generalParameters()->setAutoSave(true);

        experimentData.reset(data);
        runDelegate.reset(new ExperimentRunDelegate(*experimentData,
                                                    MainConfigFileParser::Get().data(),0,
                                                    deterministicRandom,
                                                    &ErrorHandler::Get()));
        setupIo();
        experimentControl->loadExperiment(runDelegate.data());

        connect(experimentControl.data(), SIGNAL(experimentDone()),
                this, SLOT(afterExperiment()));

        m_error << experimentControl->logger().items();
        m_Wnd->ApplyConfig(experimentData->screensData());
        m_Wnd->EnableSkip(experimentData->generalParameters()->GetAllowSkip());

        if (autoAnswer)
        {
            if (deterministicRandom)
                m_Wnd->doDeterministicAutoAnswer();

            setAutoAnswer(true);
        }
    }
    catch (std::exception& e)
    {
        m_error.addError("ApexControl", e.what());
    }
    catch (...)
    {
        qFatal("Stop throwing garbage!");
    }

    m_Wnd->SetOpenDir(QFileInfo(experimentData->fileName()).absolutePath());

    m_error.addMessage("", tr("%n Error(s)", "", m_error.numberOfErrors()));

    if (m_error.numberOfErrors() > 0)
    {
        m_error.addMessage("", tr("Errors after the first one could be "
                                  "generated by earlier errors. Try to fix "
                                  "the first error first."));
    }

    m_error.addMessage("", tr("%n Warning(s)", "", m_error.numberOfWarnings()));

    if (m_error.numberOfErrors() == 0)
    {
        m_error.addMessage("ApexControl", "DONE");
        experimentControl->start();
        return true;
    }
    else
        return false;
}

void ApexControl::afterExperiment()
{
    m_Wnd->Finished();
    bool quit = experimentData->generalParameters()->GetExitAfter();

    mod_experimentselector->makeVisible();

    disconnect(experimentControl.data(), SIGNAL(experimentDone()),
            this, SLOT(afterExperiment()));

    runDelegate.reset();
    experimentData.reset();
    stimulusControl.reset();

    if (quit)
        Application::Get().GetApplication()->quit();
}

void ApexControl::setupIo()
{
    m_Wnd->setExperimentRunDelegate(runDelegate.data());
    runDelegate->makeModules();

    ExperimentIo* io = experimentControl->io();
    io->setGui(m_Wnd);

    stimulusControl.reset(new stimulus::StimulusControl(runDelegate->modOutput()));

    connect(io, SIGNAL(responseGiven(const ScreenResult*)),
            runDelegate->modScreen(), SLOT(iAnswered(const ScreenResult*)));
    connect(io, SIGNAL(newStimulus(QString, double)),
            stimulusControl.data(), SLOT(playStimulus(QString, double)));
    connect(stimulusControl.data(), SIGNAL(stimulusPlayed()), io, SLOT(onStimulusPlayed()));
}

bool ApexControl::isExperimentRunning() const
{
    return experimentControl && experimentControl->isRunning();
}

void ApexControl::fileOpen(const QString& file)
{
    if (file.endsWith(".apf")) {
    // TODO ANDROID flowrunner uses webkitwidgets
#ifndef Q_OS_ANDROID
        mod_experimentselector.reset(new FlowRunner());
        connect(mod_experimentselector.data(), SIGNAL(selected(data::ExperimentData*)),
                this, SLOT(newExperiment(data::ExperimentData*)));
        connect(mod_experimentselector.data(), SIGNAL(errorMessage(QString, QString)),
                this, SLOT(errorMessage(QString,QString)));
        connect(mod_experimentselector.data(), SIGNAL(opened(QString)),
                m_Wnd->GetMru(), SLOT(addAndSave(QString)));
        connect(mod_experimentselector.data(), SIGNAL(setResultsFilePath(QString)),
                this, SLOT(setResultsFilePath(QString)));
        mod_experimentselector->select(file);
#endif
    } else {
        mod_experimentselector.reset(new SimpleRunner());

        connect(mod_experimentselector.data(), SIGNAL(opened(QString)),
                m_Wnd->GetMru(), SLOT(addAndSave(QString)));
        connect(mod_experimentselector.data(), SIGNAL(selected(data::ExperimentData*)),
                this, SLOT(newExperiment(data::ExperimentData*)));
        connect(mod_experimentselector.data(), SIGNAL(errorMessage(QString, QString)),
                this, SLOT(errorMessage(QString,QString)));
        connect(mod_experimentselector.data(), SIGNAL(setResultsFilePath(QString)),
                this, SLOT(setResultsFilePath(QString)));

        if (file.isEmpty())
            mod_experimentselector->selectFromDir(m_Wnd->GetOpenDir());
        else if (QFileInfo(file).isDir())
            mod_experimentselector->selectFromDir(file);
        else
            mod_experimentselector->select(file);
    }
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
    if (!loadedFile.isNull())
        dlg.selectFile(loadedFile);

    if (dlg.exec() == QDialog::Accepted)
    {
        QString file = dlg.selectedFiles().first();
        writer::ExperimentWriter::write(*experimentData, file);
    }
}

void ApexControl::fileExit()
{
    Application::Get().GetApplication()->quit();
}

void ApexControl::ShowStimulus()
{
    try
    {
        ConnectionDialog dialog(m_Wnd);
        dialog.setDelegate(*runDelegate);
        dialog.exec();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(m_Wnd, tr("Graph Display Error"),
                              tr("Unable to display connection graph: %1").arg(e.what()));
    }
}

void ApexControl::setAutoAnswer(bool autoAnswer)
{
    if (isExperimentRunning() && autoAnswer)
    {
        QMessageBox::information(m_Wnd, tr("Auto answer"),
                                 tr("You selected the auto answer option.\n"
                                    "After you answer to the current trial, a "
                                    "random button will be selected automatically "
                                    "from the buttongroup for each trial.\n"
                                    "Warning: this is an experimental feature!"));
    }

    runDelegate->SetAutoAnswer(autoAnswer);
}

void ApexControl::StopOutput()
{
    //FIXME move to experimentcontrol
    //experimentControl->runDelegate()->modOutput()->StopDevices();

    QMessageBox::warning (m_Wnd,
                          tr("Emergency button activated" ),
                          tr("You have activated the emergency button. "
                             "This might result in unstable behavior and "
                             "corrupted test results. Therefore you will now "
                             "get the chance to save the results and Apex will "
                             "be closed."));

    fileExit();
}

QDateTime ApexControl::GetStartTime() const
{
    return experimentControl->startTime();
}

const ExperimentRunDelegate& ApexControl::GetCurrentExperimentRunDelegate()
{
    return *runDelegate;
}

const data::ExperimentData& ApexControl::GetCurrentExperiment()
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

void apex::ApexControl::startPluginRunner()
{
    mod_experimentselector.reset(new PluginRunner());

    connect(mod_experimentselector.data(), SIGNAL(Selected(data::ExperimentData*)),
            this, SLOT(newExperiment(data::ExperimentData*)));
    connect(mod_experimentselector.data(),
            SIGNAL(errorMessage(const QString&, const QString&)),
            this, SLOT(errorMessage(const QString&, const QString&)));

    mod_experimentselector->select(QString());
}

void ApexControl::selectSoundcard()
{
    stimulus::WavDeviceIO::showSoundcardDialog();
}

gui::ApexMainWindow* apex::ApexControl::GetMainWnd()
{
    return m_Wnd;
}

gui::ApexMainWindow* apex::ApexControl::mainWindow()
{
    return m_Wnd;
}

const QString& apex::ApexControl::saveFilename() const
{
    return mSaveFilename;
}

void apex::ApexControl::setResultsFilePath(QString filename) {
    mSaveFilename = filename;
    autoSaveResults = true;
}

void apex::ApexControl::errorMessage(const QString& source, const QString& message)
{
    m_error.addError(source, message);
}

void apex::ApexControl::calibrate()
{
    Q_ASSERT(runDelegate->modCalibrator());

    if (runDelegate->modOutput()->IsOffLine()) {
        m_error.addError("ApexControl", "cannot calibrate with offline devices");
        return;
    }

    if (runDelegate->modCalibrator()->calibrate(true))
        runDelegate->modCalibrator()->updateParameters();
}
