#include "calibration/calibrationdata.h"
#include "calibration/calibrationdatabase.h"
#include "calibration/calibrator.h"

#include "corrector/corrector.h"

#include "device/controllers.h"

#include "error/errorevent.h"

#include "gui/calibrationdialog.h"
#include "gui/connectiondialog.h"
#include "gui/mainwindow.h"
//#include "gui/mainwindowconfig.h"
#include "gui/mru.h"
#include "screen/screenrundelegate.h"
#include "gui/plugindialog.h"
#include "gui/startupdialog.h"

//#include "stimulus/stimulusparameters.h"
#include "parameters/generalparameters.h"

#include "procedure/adaptiveprocedure.h"
#include "procedure/apexprocedure.h"
#include "procedure/apexprocedureconfig.h"

#include "randomgenerator/randomgenerators.h"

#include "result/resultparameters.h"
#include "result/resultviewer.h"

#include "resultsink/apexresultsink.h"
#include "resultsink/rtresultsink.h"

#include "runner/experimentrundelegate.h"
#include "runner/pluginrunner.h"
#include "runner/simplerunner.h"

#include "screen/apexscreen.h"
#include "screen/screensdata.h"
#include "screen/screenelementrundelegate.h"

#include "services/application.h"
#include "services/mainconfigfileparser.h"
#include "services/paths.h"
#include "services/pluginloader.h"

#include "stimulus/datablock.h"
#include "stimulus/stimulus.h"
#include "stimulus/stimulusoutput.h"

#include "stimulus/wav/outputtest.h"
#include "stimulus/wav/soundcarddisplayer.h"

#include "feedback/feedback.h"
#include "timer/apextimer.h"

#include "apexcontrol.h"
#include "apexcontrolthread.h"
#include "outputrecorder.h"
//#include "apexmessagewindow.h"
#include "apexmodule.h"
#include "apextools.h"
#include "desktoptools.h"
#include "pathtools.h"
#include "experimentparser.h"
#include "version.h"

//#include "optionutils.h"
//#include <boost/program_options.hpp>
//namespace po = boost::program_options;
#include "programoptions/optioncontext.h"

#include <appcore/deleter.h>

#include <appcore/events/eventqueuethread.h>
#include <appcore/events/qt_events.h>

#include <appcore/qt_utils.h>

//from libdata
#include "experimentdata.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegExp>
#include <QSettings>
#include <QTimer>
#include <QFileDialog>


#include <algorithm>

//from libwriters
#include "experimentwriter.h"

using namespace apex;
using namespace apex::gui;
using namespace apex::device;
using namespace apex::data;

ApexControl::tState::tState() :
        m_eState( mc_eIdle ),
        m_bResult( false ),
        m_bError( false )
{
    mp_SetState( mc_eIdle );
}

void ApexControl::tState::mp_Reset()
{
    mp_SetState( mc_eIdle );
    m_bError = false;
}

void ApexControl::tState::mp_SetError()
{
    m_bError = true;
}

void ApexControl::tState::mp_SetState( const ApexControl::tState::mt_eState ac_eState )
{
    m_ePreviousState = m_eState;
    m_eState = ac_eState;
#ifdef PRINTSTATE
    mf_PrintState();
#endif
}

void ApexControl::tState::mp_RestoreState( const mt_eState ac_eIntermediateState )
{
    const mt_eState ePrev = m_ePreviousState;
    mp_SetState( ac_eIntermediateState );
    mp_SetState( ePrev );
}

void ApexControl::tState::mf_PrintState() const
{
    switch ( m_eState )
    {
    case mc_eIdle         : qDebug( "state = mc_eIdle           " ); break;
    case mc_eLoaded       : qDebug( "state = mc_eLoaded         " ); break;
    case mc_eIntro        : qDebug( "state = mc_eIntro          " ); break;
    case mc_eIntroDone    : qDebug( "state = mc_eIntroDone      " ); break;
    case mc_eStarted      : qDebug( "state = mc_eStarted        " ); break;
    case mc_eStopped      : qDebug( "state = mc_eStopped        " ); break;
    case mc_eStim         : qDebug( "state = mc_eStim           " ); break;
    case mc_eStimDone     : qDebug( "state = mc_eStimDone       " ); break;
    case mc_eFeedback     : qDebug( "state = mc_eFeedBack       " ); break;
    case mc_eFeedbackDone : qDebug( "state = mc_eFeedBackDone   " ); break;
    case mc_ePaused       : qDebug( "state = mc_ePaused         " ); break;
    case mc_eIti          : qDebug( "state = mc_eIti            " ); break;
    case mc_eItiDone      : qDebug( "state = mc_eItiDone        " ); break;
    case mc_eOutro        : qDebug( "state = mc_eOutro          " ); break;
    case mc_eOutroDone    : qDebug( "state = mc_eOutroDone      " ); break;
    case mc_eQuitting     : qDebug( "state = mc_eQuitting       " ); break;
    }
}

ApexControl::ApexControl() :
        m_app ( Application::Get().GetApplication() ),
/*        mod_timer( 0 ),
        experimentRunDelegate->modScreen()( 0 ),
        experimentRunDelegate->modProcedure()( 0 ),
        mod_corrector( 0 ),
        mod_resultsink( 0 ),
        mod_randomgenerators( 0 ),*/
        mod_experimentselector (new SimpleRunner),
/*        experimentRunDelegate->modCalibrator()( 0 ),
        experimentRunDelegate->modOutput()( 0 ),*/
        experiment( 0 ),
        m_Wnd( new ApexMainWindow() ),
        mc_pControlThread( new ApexControlThread( this ) ),
        m_error( ErrorHandler::Get() )

{
    ApexTools::InitRand();

    //setup mainwindow

    QString logopath;
    try {
        logopath= Paths::Get().GetMainLogoPath();
    } catch (ApexStringException& e) {
        QMessageBox::critical(0, tr("Installation error"), e.what());
        QCoreApplication::quit();
    }
    QIcon mainIcon( logopath );
    QApplication::setWindowIcon( mainIcon );  //[ stijn ] set the icon for all windows

    connect (m_Wnd, SIGNAL (fileExit()), this, SLOT(fileExit()));
    connect (m_Wnd, SIGNAL (fileOpen()), this, SLOT(fileOpen()));
    connect(m_Wnd, SIGNAL(saveExperiment()), this, SLOT(saveExperiment()));
    connect (m_Wnd, SIGNAL (startPluginRunner()),
             this, SLOT(startPluginRunner()));
    connect (m_Wnd->GetMru(), SIGNAL (Released (const QString&)),
             this, SLOT (mruFileOpen(const QString&)));
    connect( m_Wnd, SIGNAL( startClicked() ), this, SLOT( Start()) );
    connect( m_Wnd, SIGNAL( stopClicked() ), this, SLOT( Stop()) );
    connect( m_Wnd, SIGNAL( StopOutput() ), this, SLOT( StopOutput()) );
    connect( m_Wnd, SIGNAL( RepeatOutput() ), this, SLOT( RepeatOutput()) );
    connect( m_Wnd, SIGNAL( pauseClicked() ), this, SLOT( DoPause() ) );
    connect( m_Wnd, SIGNAL( skipClicked() ), this, SLOT( KillWait() ) );
    connect( m_Wnd, SIGNAL( recalibrateClicked() ), this, SLOT( Calibrate() ) );
    connect( m_Wnd, SIGNAL( autoAnswerClicked(bool) ), this, SLOT( setAutoAnswer(bool) ) );
    connect( m_Wnd, SIGNAL(repeatStimulus()), this, SLOT( RepeatOutput()));
    connect( m_Wnd, SIGNAL( showStimulus() ), this, SLOT( ShowStimulus() ) );
    connect( this , SIGNAL( NewScreen( gui::ScreenRunDelegate*) ), m_Wnd, SLOT( SetScreen( gui::ScreenRunDelegate*) ) );
    connect(mod_experimentselector.get(),
            SIGNAL (Selected (data::ExperimentData*)), this,
            SLOT (newExperiment (data::ExperimentData*)));

    //make sure to quit when everything is closed
    connect (m_app, SIGNAL (lastWindowClosed()), this, SLOT (fileExit()));

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()+
            "/plugins");
    qDebug("Qt Library paths:");
    foreach (QString path, QCoreApplication::libraryPaths () )
        qDebug("%s", qPrintable(path));

    //start thread
    mc_pControlThread->mp_Start();

//    qDebug("Thread started");

    if (!configure())
        return;

    qDebug("Configure done");

#ifndef NOMAXIMIZE
    m_Wnd->showMaximized();
#else
    m_Wnd->show();
#endif

    // Break service dependency loop by loading after the QT main loop has been
    // started
    QTimer::singleShot (0, this, SLOT (StartUp()));
}

ApexControl::~ApexControl()
{
    if (mc_pControlThread)
        mc_pControlThread->mp_Stop( 2000 );
//    deleteModules();
    // Do this manually before the window is destroyed. Otherwise the screen
    // delegates can not cope with the deleted widgets.
    experimentRunDelegate.reset();
    if (mc_pControlThread)
        delete mc_pControlThread;
    mod_experimentselector.reset();
    if (m_Wnd)
        delete m_Wnd;
}

void ApexControl::StartUp()
{
    parsecmdline();
    qDebug("Parse commandline done");

    qDebug ("Shown MainWindow");
    //ask for experiment now if not in command line
    if ( m_state.m_eState == tState::mc_eIdle && !m_state.m_bError )
        showStartupDialog();
}

void ApexControl::showStartupDialog()
{
    StartupDialog dialog(m_Wnd->GetMru()->items(), m_Wnd);
    if (dialog.exec() != QDialog::Accepted)
        return;

    dialog.hide();

    if (dialog.selectedFile().isEmpty())
        fileOpen();
    else
        mruFileOpen(dialog.selectedFile());
}

//parse mainconfig
bool ApexControl::configure()
{
    Paths& pPaths = Paths::Get();

    m_error.addMessage( "APEX", tr("Apex version ") + APEX_VERSION + tr(", ") + SVN_REVISION );
    m_error.addMessage("APEX", tr("Compiled with Qt version ") + QT_VERSION_STR);
    m_error.addMessage("APEX", tr("Running with Qt version ") + qVersion());
    m_error.addMessage("APEX",
                       "The APEX 3 binaries use the Trolltech Qt framework available \n"
                               "under the GNU General Public License (GPL version 2) on: \n"
                               "http://www.trolltech.com. The APEX 3 source code and \n"
                               "binaries are available under the GNU General Public License \n"
                               "(GPL version 2) on http://www.kuleuven.be/exporl/apex.");
    m_error.addMessage( "ApexControl",
            tr("Parsing apexconfig file %1\nwith schema %2")
            .arg(Paths::Get().GetApexConfigFilePath())
            .arg(Paths::Get().GetApexConfigSchemaPath()) );

    try
    {
        MainConfigFileParser& mainConfigFileParser = MainConfigFileParser::Get();
        mainConfigFileParser.LogErrorsToApex (&m_error);
//        mainConfigFileParser.SetPrefix (pPaths.mf_sBasePath());
        mainConfigFileParser.CFParse();

        //check if file exists:
        const QString schema( pPaths.GetExperimentSchemaPath() );
        QFile f( schema );
        if (!f.exists())
        {
            m_error.addError("ApexControl" , "Schema file not found: " + schema +"\n" + "Check main config file" );
            throw(0);
        }
    }
    catch (ApexStringException& e) {
        m_error.addError( "ApexControl" ,
                          QString(tr( "Parsing of main config file failed: %1" ) ).arg(e.what()));
        return false;
    } catch (...)
    {
        m_error.addError( "ApexControl" , tr( "Parsing of main config file failed" ) );
        return false;
    }

    m_error.addMessage( "ApexControl" , "Done" );
    m_Wnd->SetOpenDir( pPaths.GetExamplesPath() );
    m_Wnd->MruLoad( pPaths.GetMruIniPath() );   //will override sOpenDir if specified
    return true;
}

void ApexControl::deletePluginCache()
{
    QSettings settings (QSettings::UserScope, QLatin1String ("Trolltech"));
    const QString key = QString::fromLatin1 ("Qt Plugin Cache %1.%2.%3")
        .arg ((QT_VERSION & 0xff0000) >> 16)
        .arg ((QT_VERSION & 0x00ff00) >>  8);

    settings.remove (key.arg ("false"));
    settings.remove (key.arg ("debug"));
}

void ApexControl::editApexconfig()
{
    // if necessary copy apexconfig file to user path
    QString userApexConfig(
            PathTools::GetUserConfigFilePath() + "/" + Paths::cApexConfig);
    if (!QFile::exists(userApexConfig)) {
        qDebug("Copying file");
        QFile source(Paths::Get().GetApexConfigFilePath());
        bool result = source.copy(userApexConfig);
        if (!result) {
            m_error.addError(tr("Cannot copy apexconfig from %1 to %2")
                    .arg(Paths::Get().GetApexConfigFilePath())
                    .arg(userApexConfig), "ApexControl");
            return;
        }
    }

    bool success=DesktopTools::editFile(userApexConfig);
	if (!success) {
		QMessageBox::critical(m_Wnd, tr("Error editing ApexConfig"),
			tr("Could not open ApexConfig.xml for editing. Please open the file \n%1\n yourself.")
			.arg(userApexConfig));
	}
}

void ApexControl::showPluginDialog()
{
    PluginDialog d(m_Wnd);
    d.exec();
}

void ApexControl::parsecmdline()
{
    // use boost getopts
   QMap<QString, QString> params;
   QString experimentfile;
   QStringList files;
   QString pluginRunner;
   bool listSoundcards = false;
   recordExperiments = false;
   autoStartExperiments = false;
   autoAnswer = false;
   deterministicRandom = false;
   noResults = false;
   exitAfter = false;

   OptionContext context(tr("[OPTION...] experimentfile"),
                         OptionGroup (QString(), tr("Program Options"), QString(), QString(),
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
                                      << OptionEntry(tr("record"), QLatin1Char('e'), 0,
                                                     OptionEntry::NoArgument, &recordExperiments,
                                                     tr("Record output of experiments"))
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
                                      << OptionEntry(tr("exitafter"), QLatin1Char('n'), 0,
                                                     OptionEntry::NoArgument, &exitAfter,
                                                     tr("Quit Apex after an experiment is done"))
                                      ));


   try {
       files = context.parse(m_app->arguments());
       if (!files.isEmpty())
           experimentfile = files[0];
   } catch (const std::exception &e) {
       m_error.addWarning("ApexControl", tr("Error parsing commandline: %1").
                          arg(e.what()));
       return;
   }

   if (listSoundcards) {
       ShowSoundcards();
       return;
   }


   if (!pluginRunner.isEmpty()) {
       mod_experimentselector.reset( new PluginRunner() );

       connect(mod_experimentselector.get(),
               SIGNAL(Selected(data::ExperimentData*)),
               this, SLOT(newExperiment(data::ExperimentData*)));
       connect(mod_experimentselector.get(),
               SIGNAL(errorMessage(const QString&, const QString&)),
               this, SLOT(errorMessage(const QString&, const QString&)));

       mod_experimentselector->Select(pluginRunner);
       return;
   }

   if (!experimentfile.isEmpty())
   {
       QFileInfo url(experimentfile);
       if (url.isFile())
       {
           m_error.addMessage ("ApexControl" ,"Using filename given on the command line");
           mod_experimentselector->Select(url.absoluteFilePath());
       }
       else if (url.isDir())
       {
           m_Wnd->SetOpenDir (url.absoluteFilePath());
           fileOpen();
       }
   }

   return;
}

bool ApexControl::newExperiment(data::ExperimentData* data)
{
    if (data == 0)
        return false;

    experiment.reset();

    m_error.clearCounters();
    m_Wnd->Clear();
    m_state.mp_Reset();

    m_Wnd->MruFile(data->fileName());

    try
    {
        if (recordExperiments)
        {
            OutputRecorder rec;

            if (!rec.setupRecording(data))
                m_error.addItems(rec.logger().items());
        }

        if (exitAfter)
            data->generalParameters()->setExitAfter(true);

        experiment.reset(data);
        experimentRunDelegate.reset (new ExperimentRunDelegate (*experiment, m_Wnd->centralWidget(), deterministicRandom ));
        m_Wnd->setExperimentRunDelegate( experimentRunDelegate.get());
        m_error << experimentRunDelegate->logger().items();
        m_Wnd->ApplyConfig(&(*experiment->screensData())); //make sure to keep this before makeModules
        m_Wnd->EnableSkip( experiment->generalParameters()->GetAllowSkip() );
        experimentRunDelegate->makeModules(this);
        m_state.mp_SetState( tState::mc_eLoaded );

        if (deterministicRandom)
            experimentRunDelegate->modRandomGenerators()->doDeterministicGeneration();

        if (autoAnswer)
        {
            if (deterministicRandom)
                m_Wnd->doDeterministicAutoAnswer();

            setAutoAnswer(true);
        }
    }
    catch (std::exception &e)
    {
        m_error.addError("ApexControl" , e.what());
        m_state.mp_SetError();
        mod_experimentselector->Finished();
    }
    catch (...)
    {
        m_error.addError("ApexControl" , "FAILED" );
        m_state.mp_SetError();
        mod_experimentselector->Finished();
    }
    m_Wnd->SetOpenDir( f_sDirectoryFromFile( experiment->fileName() ) );

    int nWarnings=m_error.numberOfWarnings();
    m_error.addWarning( "", QString::number( m_error.numberOfErrors() ) + tr(" Error") );
    if ( m_error.numberOfErrors() )
        m_error.addWarning( "", tr("Warning: errors after the first one could be generated by earlier errors. Try to fix the first error first.") );
    m_error.addWarning( "", QString::number( nWarnings ) + " Warning(s)" );

//     delete ac_pExperiment;

    if (m_error.numberOfErrors()==0)
    {
        m_error.addWarning("ApexControl" , "DONE" );
        PrepareExperiment();

        if (autoStartExperiments)
            Start();

        return true;
    }
    else
    {
        return false;
    }
}



void ApexControl::ConnectProcedures(ApexProcedure* newproc, ApexProcedure* proc)
{
    // connect signals
    connect (newproc, SIGNAL(NewScreen(QString)),
             proc, SIGNAL(NewScreen(QString)));
    connect (newproc, SIGNAL(EnableScreen(bool)),
             proc, SIGNAL(EnableScreen(bool)));
    connect (newproc, SIGNAL(SetCorrectAnswer(unsigned)),
             proc, SIGNAL(SetCorrectAnswer(unsigned)));
    connect (newproc, SIGNAL(NewStimulus(stimulus::Stimulus* )),
             proc, SIGNAL(NewStimulus(stimulus::Stimulus*)));
    connect (newproc, SIGNAL(SendMessage(QString)),
             proc, SIGNAL(SendMessage(QString)));
    connect (newproc, SIGNAL(NewScreenElement(QString)),
             proc, SIGNAL(NewScreenElement(QString)));
    connect (newproc, SIGNAL(NewStimulus(QString)),
             proc, SIGNAL(NewStimulus(QString)));

}



void ApexControl::PrepareExperiment()
{
    const ScreensData* p = experiment->screensData();
    ScreenWait( p->introScreen(), p->introLength(), tState::mc_eIntro, tState::mc_eIntroDone );

    if (experimentRunDelegate->modRTResultSink())
        experimentRunDelegate->modRTResultSink()->show();
}

void ApexControl::ExperimentFinished()
{
    if ( m_state.m_eState != tState::mc_eQuitting )
        m_state.mp_Reset();
    experimentRunDelegate->modOutput()->StopDevices();
    m_Wnd->Finished();
    m_error.addMessage( "ApexControl" , "Experiment Finished" );
    m_error.addMessage( "", "" ); //skip a line
}

void ApexControl::fileOpen()
{
    mod_experimentselector.reset(new SimpleRunner);
    connect(mod_experimentselector.get(), SIGNAL(Selected(data::ExperimentData*)),
            this, SLOT(newExperiment(data::ExperimentData*)));

    mod_experimentselector->SelectFromDir (m_Wnd->GetOpenDir());
}

void ApexControl::saveExperiment()
{
    if (experiment.get() == 0)
        return;

    QFileDialog dlg(m_Wnd, tr("Save experiment file"));
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setFilter(tr("Apex experiment files (*.apx *.xml)"));
    dlg.setDefaultSuffix("apx");
    dlg.setAcceptMode(QFileDialog::AcceptSave);

    QString loadedFile = experiment->fileName();
    if (!loadedFile.isNull())
        dlg.selectFile(loadedFile);

    if (dlg.exec() != QDialog::Accepted)
        return;

    QString file = dlg.selectedFiles().first();

    writer::ExperimentWriter::write(*experiment, file);
}

void ApexControl::mruFileOpen(const QString& filename)
{
    mod_experimentselector.reset(new SimpleRunner);
    connect(mod_experimentselector.get(), SIGNAL(Selected(data::ExperimentData*)),
            this, SLOT(newExperiment(data::ExperimentData*)));

    mod_experimentselector->Select (filename);
}

void ApexControl::fileExit()
{
    if ( m_state.m_eState != tState::mc_eQuitting )
    {
        SaveAndClose();
        m_state.mp_SetState( tState::mc_eQuitting );
        m_app->quit();
    }
}

void ApexControl::SetCurrentTrial( const QString & ac_sName )
{
#ifdef SHOWSLOTS
    qDebug("notSLOT ApexControl::NewTrial");
#endif

//    qDebug() << "ApexControl: Setting current trial to" << ac_sName;

    m_state.m_sTrial = ac_sName;

    emit (NewAnswer( pGetCurrentTrial()->GetAnswer()));
}

void ApexControl::SetCorrectAnswer(unsigned answer)
{
    experimentRunDelegate->modCorrector()->SetCorrectAnswer (answer);
    emit ( NewAnswer( experimentRunDelegate->modCorrector()->GetCorrectAnswer(answer) ) );
}

void ApexControl::StartTimer()
{
    experimentRunDelegate->modTimer()->start();
}

void ApexControl::Answered( const ApexScreenResult* ac_Answer )
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexControl::Answered");
#endif

    if ( m_state.m_eState >= m_state.mc_eStarted &&
            m_state.m_eState < m_state.mc_eOutro ) //in the middle of an experiment
    {
        experimentRunDelegate->modTimer()->stop();

        if ( experimentRunDelegate->modProcedure()->mf_bHasOutputLeft() )
            experimentRunDelegate->modProcedure()->mp_ClearOutputList();

        try {
            m_state.m_bResult = experimentRunDelegate->modCorrector()->Correct( *ac_Answer );
        } catch (ApexStringException& e) {
            m_error.addError(tr("Corrector"), e.what());
            return;
        }
        experimentRunDelegate->modResultSink()->CollectResults();
        /*if ( experimentRunDelegate->modRTResultSink())
            experimentRunDelegate->modRTResultSink()->
                    newAnswer( ac_Answer, m_state.m_bResult  );*/


        m_state.mp_SetState( tState::mc_eFeedback );
        int nFeedBackLength = 0;
        QString highlight_element;
        if ( experiment->screensData()->feedbackOn() == HIGHLIGHT_CORRECT)
            highlight_element = experimentRunDelegate->modCorrector()->GetCorrectAnswer();
        else if ( experiment->screensData()->feedbackOn() == HIGHLIGHT_ANSWER)
            highlight_element = experimentRunDelegate->modScreen()->GetLastResult()->value(
                                    GetCurrentAnswerElement())  ;

        if ( experiment->screensData()->hasFeedbackEnabled() )
        {

            m_Wnd->FeedBack( m_state.m_bResult ?
                             ScreenElementRunDelegate::mc_ePositive :
                             ScreenElementRunDelegate::mc_eNegative,
                             highlight_element);
            nFeedBackLength = experiment->screensData()->feedbackLength();
            if ( !nFeedBackLength )
                nFeedBackLength = -1;
        }
        else /*if ( experiment->GetScreens().hasFeedbackEnabled())*/
        {
            nFeedBackLength=experiment->screensData()->feedbackLength();
        }

        if (m_state.m_bResult)
            experimentRunDelegate->modFeedback()->showPositive();
        else
            experimentRunDelegate->modFeedback()->showNegative();
        experimentRunDelegate->modFeedback()->highLight(highlight_element);

        m_Wnd->ReclaimFocus();

        if ( nFeedBackLength >= 0 ) {
            qDebug("Feedback: waiting for %dms", nFeedBackLength);
            mc_pControlThread->mp_StartWaitOnTimer( nFeedBackLength, tState::mc_eFeedbackDone );
        }
    }
    else
    {
        qDebug( "answered but experiment not running" );
        //KillWait();
    }
}

//[ controlthread ]
void ApexControl::customEvent( QEvent* e )
{

    try
    {


        if ( e->type() == appcore::QDispatchedEvent<>::sc_QDispatchedEventType )
        {
            const int nEventType = ( (appcore::QDispatchedEvent<>*)e )->mc_nEventType;

            if ( nEventType == tState::mc_eIntroDone )
            {
                //start the first trial, this completes PrepareExperiment()
                m_state.mp_SetState( tState::mc_eIntroDone );
                m_Wnd->ClearScreen();
                experimentRunDelegate->modProcedure()->FirstTrial();
                m_Wnd->ExperimentLoaded();
            }
            else if ( nEventType == tState::mc_eStarted )
            {
                if ( m_state.m_eState == tState::mc_eIntroDone )
                {
                    //first trial
                    m_state.mp_SetState( tState::mc_eStarted );
                    m_Wnd->Started();
                    m_Wnd->EnablePause( experimentRunDelegate->modProcedure()->CanPause() );
                    emit( NewAnswer( pGetCurrentTrial()->GetAnswer() ) );
                    m_state.m_StartTime = QDateTime::currentDateTime();
                    experimentRunDelegate->modProcedure()->Start();
                }
                else
                {
                    //start invoked, continue with next trial
                    m_Wnd->EnableStart( false );
                    if ( m_state.m_eState != tState::mc_ePaused )
                        experimentRunDelegate->modProcedure()->StartOutput();
                    //experimentRunDelegate->modProcedure()->NextTrial( m_state.m_bResult );
                }
            }
            else if ( nEventType == tState::mc_ePaused )
            {
                if ( m_state.m_eState == tState::mc_ePaused )
                {
                    m_state.mp_SetState( tState::mc_eStarted );
                    if ( experimentRunDelegate->modProcedure()->NextTrial(experimentRunDelegate->modCorrector()->GetLastResult(), experimentRunDelegate->modScreen()->GetLastResult()) )
                    {
                        experimentRunDelegate->modProcedure()->StartOutput();
                        experimentRunDelegate->modOutput()->ResumeDevices();
                    }
                    m_Wnd->EnablePause( true );
                }
                else
                {
                    m_state.mp_SetState( tState::mc_ePaused );
                    QMessageBox::question(0, tr("Pause"), tr("Please respond to the last trial before taking a pause"), tr("I'll answer now"));
                    experimentRunDelegate->modOutput()->StopDevices();
                    m_Wnd->EnableStop( false ); //don't allow stopping if paused, an answer must be given first
                    m_Wnd->EnablePause( false );
                    m_Wnd->EnableRepeat( false );
                }
            }
            else if ( nEventType == tState::mc_eFeedbackDone )
            {
                m_state.mp_RestoreState( tState::mc_eFeedbackDone );
                const data::ScreensData* pCfg = experiment->screensData();

                //this event means an answer was given and the feedback period completed
                //so, end feedback
                if ( pCfg->hasFeedbackEnabled() )
                    m_Wnd->EndFeedBack();
                experimentRunDelegate->modFeedback()->clear();

                //disable buttons
                m_Wnd->EnableStop( false );
                m_Wnd->EnableRepeat( false );
                if ( m_state.m_eState == tState::mc_ePaused && experimentRunDelegate->modProcedure()->CanPause() )
                    m_Wnd->EnablePause( true );
                else
                    m_Wnd->EnablePause( false );

                //and do iti
                ScreenWait( pCfg->interTrialScreen(), pCfg->interTrialLength(), tState::mc_eIti, tState::mc_eItiDone );
            }
            else if ( nEventType == tState::mc_eItiDone )
            {
                m_state.mp_RestoreState( tState::mc_eItiDone );
                if ( !experiment->screensData()->interTrialScreen().isEmpty() )
                    m_Wnd->ClearScreen();

                //do next trial if required
                if ( experimentRunDelegate->modProcedure()->isFinished() == false )
                {
                    bool procresult=true;
                    if ( m_state.m_eState != tState::mc_ePaused )
                        procresult=experimentRunDelegate->modProcedure()->NextTrial( m_state.m_bResult, experimentRunDelegate->modScreen()->GetLastResult() );

                    if ( !experiment->generalParameters()->GetWaitForStart())
                    {
                        if (procresult)
                            experimentRunDelegate->modProcedure()->StartOutput();
                    }
                    else
                    {
                        //do nothing, but wait for start    FIXME it's a bit stupid to wait for start if it was the last trial?
                        m_Wnd->EnableStart( true );
                    }
                }
            }
            else if ( nEventType == tState::mc_eStimDone )
            {
                m_state.mp_RestoreState( tState::mc_eStimDone );
                experimentRunDelegate->modOutput()->UnLoadStimulus();
                if ( experiment->screensData()->hasShowCurrentEnabled() )
                    m_Wnd->EndHighLight();
                m_Wnd->EnableStop( true );
                m_Wnd->EnableRepeat( true );
                m_Wnd->EnablePause( experimentRunDelegate->modProcedure()->CanPause() );
                experimentRunDelegate->modProcedure()->StimulusDone();
            }
            else if ( nEventType == tState::mc_eStopped )
            {
                m_state.mp_SetState( tState::mc_eStopped );
                m_Wnd->Stopped();
                SaveAndClose();
            }
            else if ( nEventType == tState::mc_eOutroDone )
            {
                m_state.mp_SetState( tState::mc_eOutroDone );
                m_Wnd->ClearScreen();
                SaveAndClose();
            }
            /*else if( nEventType == tState::mc_eWaitingOnRepeat )
            {
              experimentRunDelegate->modProcedure()->RepeatOutput();
            }*/
        }
        else if ( e->type() == ErrorEvent::sc_tEventNumber )
        {
            ErrorEvent* p = (ErrorEvent*) e;
            ErrorHandler::Get().addItem( p->m_Error );
            e->accept();
        }


    }
    catch (ApexStringException& e)
    {
        ErrorHandler::Get().addItem(StatusItem(StatusItem::ERROR,"Control thread",  e.what()) );
    }

}

void ApexControl::ShowStimulus()
{
    try
    {
        ConnectionDialog dialog (m_Wnd);
        dialog.setDelegate (*experimentRunDelegate);
        dialog.exec();
    }
    catch (std::exception &e)
    {
        QMessageBox::critical (m_Wnd, tr("Graph Display Error"),
                               tr ("Unable to display connection graph: %1").arg (e.what()),
                               QMessageBox::Ok);
    }
}

void ApexControl::SetScreen( const QString& ac_sID )
{
    emit NewScreen( experimentRunDelegate->GetScreen( ac_sID ) );
}

void ApexControl::setAutoAnswer(bool p)
{
    if (isExperimentRunning() && p)
        QMessageBox::information( 0, tr("Auto answer"), tr("You selected the auto answer option.\nAfter you answer to the current trial, a random button will be selected automatically from the buttongroup for each trial.\nWarning: this is an experimental feature!"), QMessageBox::Ok);
    experimentRunDelegate->SetAutoAnswer(p);
}

bool ApexControl::isExperimentRunning() const
{
    return m_state.m_eState >= tState::mc_eIntro &&
           m_state.m_eState <= tState::mc_eOutro;
}

void ApexControl::NewStimulus( const QString& ac_sStimulus )
{
    m_Wnd->EnableStop( false );
    m_Wnd->EnableRepeat( false );
    m_Wnd->EnablePause( false );

    // [Tom]: do not restore parameters, this is problematic for continuous
    // filters
    experimentRunDelegate->modOutput()->LoadStimulus( ac_sStimulus, false );
    //experimentRunDelegate->modOutput()->LoadStimulus( ac_sStimulus, true );
    experimentRunDelegate->modOutput()->PlayStimulus();
    m_state.mp_SetState( tState::mc_eStim );
    mc_pControlThread->mp_StartWaitOnObject( -1, &experimentRunDelegate->modOutput()->GetStimulusEnd(), tState::mc_eStimDone );
}

void ApexControl::Start()
{
    if (m_state.m_eState == tState::mc_eIntroDone || autoStartExperiments)
    {
        //this is the actual start of the experiment
        mc_pControlThread->mp_StartWaitOnTimer( 0, tState::mc_eStarted );
    }
    else
    {
        //we already started. Maybe it's a waitforstart experiment?
        if ( experiment->generalParameters()->GetWaitForStart() )
            mc_pControlThread->mp_StartWaitOnTimer( 0, tState::mc_eStarted );
        else
            qDebug( "ApexControl::Start invoked but already started??" );
    }
}

void ApexControl::Stop()
{
    mc_pControlThread->mp_StartWaitOnTimer( 0, tState::mc_eStopped );
}

void ApexControl::StopOutput()
{
    experimentRunDelegate->modOutput()->StopDevices();
    QMessageBox::warning(0,
                         tr("Emergency button activated"),
                         tr("You have activated the emergency button. "
                            "This might result in unstable behaviour and "
                            "corrupted test results. Therefore you will now "
                            "get the chance to save the results and Apex will "
                            "be closed."));

    fileExit();

}

void ApexControl::DoPause( )
{
    m_Wnd->Paused();
    mc_pControlThread->mp_StartWaitOnTimer( 0, tState::mc_ePaused );
}

void ApexControl::KillWait()
{
    if ( m_state.m_eState == tState::mc_eIntro )
    {
        mc_pControlThread->mp_KillTimer( tState::mc_eIntroDone );
        mc_pControlThread->mp_StartWaitOnTimer(0,mf_nGetScreenElementFinishedEvent() );
    }
    else if ( m_state.m_eState == tState::mc_eOutro )
    {
        mc_pControlThread->mp_KillTimer( tState::mc_eOutroDone );
        mc_pControlThread->mp_StartWaitOnTimer(0,mf_nGetScreenElementFinishedEvent() );
    }
    else if ( m_state.m_eState == tState::mc_eIti )
    {
        mc_pControlThread->mp_KillTimer( tState::mc_eItiDone );
    }
    else if ( m_state.m_eState == tState::mc_eFeedback )
    {
        mc_pControlThread->mp_KillTimer( tState::mc_eFeedbackDone );
    }
    else
    {
        qDebug( "no timer to kill" );
    }
}

/**
 * Save current results if the user wants to, and close experiment
 * Also exit if exitafter is defined
 */
void ApexControl::SaveAndClose()
{
    Q_CHECK_PTR(m_Wnd);

    m_Wnd->MruSave( Paths::Get().GetMruIniPath() );

    if ( m_state.m_eState < tState::mc_eStarted )
        return;

    m_Wnd->EnableStop( false );
    m_Wnd->EnableRepeat( false );
    m_Wnd->EnablePause( false );

    experimentRunDelegate->modOutput()->StopDevices();        // stop devices so continuous filters stop
    experimentRunDelegate->modOutput()->CloseDevices();

    if (!noResults && experimentRunDelegate->modResultSink())
    {
        experimentRunDelegate->modResultSink()->Finished();
        if ( experimentRunDelegate->modResultSink()->IsSaved())
        {
            if (experiment->resultParameters()->showResultsAfter()
                || experiment->resultParameters()->GetSaveResults()
               )
            {
                ResultViewer rv(experiment->resultParameters(),
                                experimentRunDelegate->modResultSink()->GetFilename(),
                                Paths::Get().GetXsltScriptsPath()
                               );
                bool result=rv.ProcessResult();

                if (result)
                {
                    if (experiment->resultParameters()->showResultsAfter()) {
                        if ( experimentRunDelegate->modRTResultSink() )
                            experimentRunDelegate->modRTResultSink()->show(false);
                        rv.show(true);
                    }
                    if (experiment->resultParameters()->GetSaveResults())
                    {
                        rv.addtofile( experimentRunDelegate->modResultSink()->GetFilename() );
                    }
                }
            }
        }
    }
    else
        ExperimentFinished();

    if (experiment.get()!=0 && experiment->generalParameters()->GetExitAfter())
    {
        qDebug("ExitAfter defined, exiting...");
        m_app->exit();
    }

#ifdef WRITEWAVOUTPUT
    //test must be initialized before experiment data is gone
    stimulus::WavOutputTest* pTest = 0;
    if ( experiment->GetGeneralParameters()->RunOutputTest() )
        pTest = new stimulus::WavOutputTest( experiment->GetDatablocksData(), experiment->GetGeneralParameters()->OutputTestInput() );
#endif

    experimentRunDelegate.reset();
    experiment.reset();

    mod_experimentselector->Finished();

#ifdef WRITEWAVOUTPUT
    //test must be run after WavDevice is destructed, else the outputfile can't be opened
    if ( pTest )
    {
        pTest->mf_Run();
        delete pTest;
    }
#endif
}

void apex::ApexControl::Finished()
{
    const data::ScreensData* p = experiment->screensData();
    ScreenWait( p->outroScreen(), p->outroLength(),
                tState::mc_eOutro, tState::mc_eOutroDone );
}

void ApexControl::Calibrate()
{
    Q_ASSERT (experimentRunDelegate->modCalibrator());

    if (experimentRunDelegate->modOutput()->IsOffLine())
    {
        m_error.addError ("ApexControl",
                          "cannot calibrate with offline devices");
        return;
    }

    if (experimentRunDelegate->modCalibrator()->calibrate (true))
        experimentRunDelegate->modCalibrator()->updateParameters();
}

// Getters =====================================================================

const gui::ScreenRunDelegate* ApexControl::GetCurrentScreen() const
{
    return experimentRunDelegate->GetScreen( pGetCurrentTrial()->GetScreen() );
}


const QString ApexControl::GetCurrentTrial() const
{
    //qDebug() << "ApexControl::GetCurrentTrial(): " << m_state.m_sTrial;
    return m_state.m_sTrial;
}

ApexTrial* ApexControl::pGetCurrentTrial()
{
    return experiment->trialById (GetCurrentTrial());
}

const ApexTrial* ApexControl::pGetCurrentTrial() const
{
    return experiment->trialById (GetCurrentTrial());
}

const QDateTime& ApexControl::GetStartTime() const
{
    return m_state.m_StartTime;
}

const ExperimentRunDelegate& ApexControl::GetCurrentExperimentRunDelegate()
{ return *experimentRunDelegate; }

const ExperimentData& ApexControl::GetCurrentExperiment()
{ return *experiment; }

const QString apex::ApexControl::GetCurrentAnswerElement() const
{
    QString answerElement = pGetCurrentTrial()->GetAnswerElement();

    if ( ! answerElement.isEmpty() )
        return answerElement;

    answerElement = GetCurrentScreen()->getDefaultAnswerElement();

    if ( ! answerElement.isEmpty() )
        return answerElement;

    m_error.addMessage(tr("ApexControl"), tr("No answer element defined for this trial (nor in the trial nor a default answer element in the screen"));
    return QString();
}

void apex::ApexControl::RepeatOutput( )
{
    experimentRunDelegate->modProcedure()->RepeatOutput();
}

void ApexControl::ScreenWait( const QString& ac_sScreen, const int ac_nTimeOut,
                              const tState::mt_eState ac_eStateBefore, const tState::mt_eState ac_eStateAfter )
{
    m_state.mp_SetState( ac_eStateBefore );
    m_Wnd->ReclaimFocus();

    int nWait = ac_nTimeOut;
    //check if there is a screen to set
    if ( !ac_sScreen.isEmpty() )
    {
        //if there is one, show it and wait for the end
        //if the length is non-zero we start a timer,
        //else the screen will do this
        SetScreen( ac_sScreen );
        if ( !ac_nTimeOut )
            nWait = -1;
    }

    if ( nWait >= 0 )
        mc_pControlThread->mp_StartWaitOnTimer( nWait, ac_eStateAfter );
}

/**
  * This will be called by movie screenelements to check if they
  * should start polling for their end.
  * @return the event to post when done, or zero if polling should not start
  */
int ApexControl::mf_nGetScreenElementFinishedEvent() const
{
    if ( m_state.m_eState == tState::mc_eIntro && experiment->screensData()->introLength() == 0 )
    {
        return tState::mc_eIntroDone;
    }
    else if ( m_state.m_eState == tState::mc_eOutro && experiment->screensData()->outroLength() == 0 )
    {
        return tState::mc_eOutroDone;
    }
    else if ( m_state.m_eState == tState::mc_eIti && experiment->screensData()->interTrialLength() == 0 )
    {
        return tState::mc_eItiDone;
    }
    else if ( m_state.m_eState == tState::mc_eFeedback && experiment->screensData()->feedbackLength() == 0 )
    {
        return tState::mc_eFeedbackDone;
    }
    return 0;
}

void apex::ApexControl::ShowSoundcards()
{
    std::cout << "Soundcards in this system: " << std::endl;
    std::cout << "--------------------------" << std::endl;
    SoundCardDisplayer::ShowSoundCards();
    std::cout << "--------------------------" << std::endl;
    m_app->exit();
}

void apex::ApexControl::startPluginRunner()
{
/*    if (mod_experimentselector)
        delete mod_experimentselector;*/
    mod_experimentselector.reset( new PluginRunner() );

    connect(mod_experimentselector.get(), SIGNAL(Selected(data::ExperimentData*)),
            this, SLOT(newExperiment(data::ExperimentData*)));

    mod_experimentselector->Select(QString());
}

stimulus::StimulusOutput*     apex::ApexControl::GetOutput() {
    return experimentRunDelegate->modOutput();
};

gui::ApexMainWindow*  apex::ApexControl::GetMainWnd()
{
    return m_Wnd;
}

gui::ApexMainWindow*  apex::ApexControl::mainWindow()
{
    return m_Wnd;
}


  const QString& apex::ApexControl::saveFilename() const {
       return mSaveFilename;
  }



void apex::ApexControl::errorMessage(const QString& source, const QString& message){
     m_error.addError(source,message);
}
