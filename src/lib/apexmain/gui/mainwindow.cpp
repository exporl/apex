/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "runner/experimentrundelegate.h"

#include "screen/buttongroup.h"
#include "screen/screen.h"
#include "screen/screensdata.h"
#include "screen/screenelement.h"
#include "screen/screenelementrundelegate.h"
#include "screen/apexscreenresult.h"

#include "services/application.h"
#include "services/paths.h"

#include "screen/screenrundelegate.h"
#include "apexcontrol.h"
#include "apextools.h"
#include "centralwidget.h"
#include "exceptions.h"
#include "gui/guidefines.h"
#include "mainwindow.h"
//#include "mainwindowconfig.h"
#include "mru.h"
#include "panel.h"

#include <appcore/qt_utils.h>

#include "status/screenstatusreporter.h"
#include "status/consolestatusreporter.h"

#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>

//from libdata
#include "experimentdata.h"

//#ifndef WIN32
#include <QApplication>
//#endif
#include <feedback/feedback.h>

using namespace apex;
using namespace apex::gui;
using apex::data::gc_eChild;
using apex::data::gc_eNormal;

ApexMainWindow::ApexMainWindow( QWidget* /*a_pParent*/ ) :
        ApexMainWndBase(),
        m_rd(0),
        m_config(0),
        m_pMRU( 0 ),
        m_pPanel( 0 ),
        m_pCentral( 0 ),
        m_sSavefileName(),
        runningScreen(),
        m_pCurFeedback()
{
    //setup
    statusBar()->hide();

    //create the mru list
    m_pMRU.reset( new MRU( fileMenu ) );


    //create the main widget
    m_pCentral.reset( new ApexCentralWidget( this ) );
    setCentralWidget( m_pCentral.data() );

    //setup status reporting
    //FIXME I don't think this should be here
    consoleStatus.reset( new ConsoleStatusReporter() );
    screenStatus.reset( new ScreenStatusReporter() );
    showScreenStatusAction->setChecked(true);
    showMessageWindowAction->setChecked(true);
    showConsoleStatusAction->setChecked(true);
    updateStatusReporting();
    connect(this, SIGNAL(statusReportingChanged()),
            this, SLOT(updateStatusReporting()));

    //ready
    Initted();
}

void ApexMainWindow::setExperimentRunDelegate(ExperimentRunDelegate* p_rd) {
    m_rd=p_rd;
}

ApexMainWindow::~ApexMainWindow()
{
    m_pPanel.reset();
    ErrorHandler::Get().removeReporter(screenStatus.data());
    ErrorHandler::Get().removeReporter(consoleStatus.data());
    screenStatus.reset();
    consoleStatus.reset();
}

void ApexMainWindow::SetIcon( const QString& ac_sIcon )
{
    setWindowIcon (QPixmap (ac_sIcon));
}

void ApexMainWindow::ApplyConfig(const data::ScreensData* c)
{
    Application::Get().GetApplication()->setStyleSheet(c->apexStyle());

    m_config=c;
    const data::ScreensData& config = *c;



    m_pPanel.reset(0);

    if ( config.hasPanelEnabled() )
    {
        if ( config.m_eMode == data::gc_eNormal || !config.hasPanelMovie() )
        {
            Panel* p = new Panel( centralWidget(), c );
            if ( !p->mp_bSetConfig( config ) )
                throw ApexStringException( "ApexMainWindow::ApplyConfig: couldn't create the panel." );
            m_pPanel.reset( p );
            m_pCentral->mp_SetPanel( p );
            connect( p->getSignalSlotProxy(), SIGNAL( ms_Start() ), SIGNAL( startClicked() ) );
            connect( p->getSignalSlotProxy(), SIGNAL( ms_Stop() ), SIGNAL( stopClicked() )  );
            connect( p->getSignalSlotProxy(), SIGNAL( ms_Pause() ), SIGNAL( pauseClicked() ) );
            connect( p->getSignalSlotProxy(), SIGNAL( ms_Panic() ), SIGNAL( StopOutput() ) );
            connect( p->getSignalSlotProxy(), SIGNAL( ms_Repeat() ), SIGNAL( RepeatOutput() ) );
        }
        else
        {
#ifdef FLASH
            ChildModePanel* p = new ChildModePanel( centralWidget() );
            if ( !p->mp_bSetConfig( config ) )
                throw ApexStringException( "ApexMainWindow::ApplyConfig: couldn't create the panel." );
            m_pPanel.reset( p );
            m_pCentral->mp_SetPanel( p );
#else
            throw ApexStringException( "ApexMainWindow::ApplyConfig: can't do childmode with panel when FLASH is not defined" );
#endif
        }
    }
    else
    {
        m_pCentral->mp_SetPanel( 0 );
    }

    if ( config.m_eMode == gc_eChild )
    {
        m_pCentral->setBackgroundColor( Qt::white );
        m_pCentral->mp_SetHighColor( sc_DefaultBGColor );
    }
    else
    {
        m_pCentral->setBackgroundColor( sc_DefaultBGColor );
        m_pCentral->mp_SetHighColor( Qt::white );
    }

    if (!config.hasMenuEnabled())
        menuBar()->hide();

    if (config.hasFullScreenEnabled())
        setWindowState(windowState() | Qt::WindowFullScreen);

//  m_pCentral->setStyle

    m_pCentral->mp_LayoutNow();
    if (m_pPanel)
        m_pPanel->mp_Show( config.hasPanelEnabled() );

    m_pCentral->repaint();
}

void ApexMainWindow::SetScreen( gui::ScreenRunDelegate* ac_Screen )
{
    //do nothing if it's the same screen
    if ( runningScreen == ac_Screen )
    {
        runningScreen->clearText();
        return;
    }

    //show, connect and lay out the new screen
    if ( runningScreen ) {
        runningScreen->getWidget()->setParent(0);
        ClearScreen();
    }

    runningScreen = ac_Screen;

    //m_pCentral->mp_SetScreen( runningScreen->getLayout() );
    runningScreen->getWidget()->setStyleSheet(
    m_rd->GetData().screensData()->style()
    );

    m_pCentral->setScreenWidget(runningScreen->getWidget());

    m_pCentral->mp_LayoutNow();
    EnableScreen( false );
    runningScreen->clearText();
    runningScreen->showWidgets();
}

void ApexMainWindow::EnableScreen( const bool ac_bEnable /*= true*/ )
{
    if ( runningScreen )
    {
        runningScreen->enableWidgets( ac_bEnable );

        ReclaimFocus();

        if (ac_bEnable && ApexControl::Get().GetCurrentExperimentRunDelegate().GetAutoAnswer()) {

            const data::ButtonGroup* b = runningScreen->getScreen()->getButtonGroup();
            if (!b) {
                QMessageBox::warning(0, tr("No buttongroup found"), tr("Error : no buttongroup was found, can't automatically select a button"), QString(tr("OK")));
                return;

            }
            data::ButtonGroup::const_iterator i = b->begin();
            int number = randomGenerator.nextDouble() * b->size();

            for (int c = 0; c < number; ++c)
                ++i;

            QString elem=(*i);
            qDebug ("Automatically selecting %s", qPrintable (elem));


            static ApexScreenResult result;
            result.clear();

            result[ runningScreen->getScreen()->getButtonGroup()->getID() ] = elem;

            runningScreen->addInterestingTexts( result );


            //QMetaObject::invokeMethod (this, "Answered", Qt::QueuedConnection, Q_ARG (const ApexScreenResult*, &result));

            emit Answered( &result );

        }

    }
}

void ApexMainWindow::doDeterministicAutoAnswer()
{
    randomGenerator.setSeed(0);
}

void ApexMainWindow::ReclaimFocus()
{
    //if ( ApexControl::Get().GetCurrentExperiment().GetMainWindowConfig().m_eMode == gc_eChild )
    if (QApplication::focusWidget()==0)
        MenuBar->setFocus();
}

/******************************************** FEEDBACK ***************************************************************************/

void ApexMainWindow::FeedBack( const ScreenElementRunDelegate::mt_eFeedBackMode& ac_eMode, const QString& ac_sID )
{
    //the element
    if ( runningScreen )
    {

        if ( ac_eMode != ScreenElementRunDelegate::mc_eHighLight ||
            m_rd->GetData().screensData()->hasShowCurrentEnabled() ) {

            runningScreen->feedBack( ac_eMode, ac_sID );
            if (runningScreen->getFeedBackElement())
                m_rd->modFeedback()->
                highLight(runningScreen->getFeedBackElement()->getID() );
        }
        m_pCurFeedback = runningScreen->getFeedBackElement();
        m_pCentral->mp_SetFeedBackElement( m_pCurFeedback );
    }

    //the panel
    if ( m_pPanel && ac_eMode != ScreenElementRunDelegate::mc_eHighLight )
        m_pPanel->feedBack( ac_eMode );

    //make sure to repaint before feedback period is over
    m_pCentral->repaint();

    QApplication::flush();
}

void ApexMainWindow::HighLight( const QString& ac_sID )
{
    FeedBack( ScreenElementRunDelegate::mc_eHighLight, ac_sID );
}

void ApexMainWindow::EndFeedBack()
{
    if ( m_pCurFeedback )
        runningScreen->feedBack( ScreenElementRunDelegate::mc_eOff, m_pCurFeedback );
    if ( m_pPanel )
        m_pPanel->feedBack( ScreenElementRunDelegate::mc_eOff );
    m_rd->modFeedback()->clear();
    m_pCentral->repaint();
}

void ApexMainWindow::EndHighLight()
{
    if ( m_pCurFeedback )
        runningScreen->feedBack( ScreenElementRunDelegate::mc_eOff, m_pCurFeedback );
    m_rd->modFeedback()->clear();
    m_pCentral->repaint();
}

void ApexMainWindow::AnswerFromElement( ScreenElementRunDelegate* ac_Element )
{
    static ApexScreenResult result;
    result.clear();

    //add element id if buttongroup
    if ( runningScreen->getScreen()->getButtonGroup() )
    {
        if ( runningScreen->getScreen()->getButtonGroup()->IsElement( ac_Element->getID() ) )
            result[ runningScreen->getScreen()->getButtonGroup()->getID() ] = ac_Element->getID();
    }

    runningScreen->addInterestingTexts( result );
    runningScreen->addScreenParameters( result );

    emit Answered( &result );
}

/******************************************** STATE ***************************************************************************/

void ApexMainWindow::Paused()
{
    const data::ScreensData* config = m_rd->GetData().screensData();
    if ( m_pPanel && config->m_eMode == gc_eNormal )
        ( (Panel*) m_pPanel.data() )->mp_Paused();
}

void ApexMainWindow::Initted()
{
    EnableOpen        ( true  );
    EnableStart       ( false );
    EnableStop        ( false );
    EnableRepeat      ( false );
    EnablePause       ( false );
    EnableSave        ( false );
    EnableExperiment  ( false );
    EnableCalibration ( false );
}

void ApexMainWindow::Started()
{
    EnableStart       ( false );
    EnableStop        ( false );
    EnableRepeat      ( false );
    EnablePause       ( true  );
    EnableSave        ( true  );
}

void ApexMainWindow::Stopped()
{
    EnableStart       ( true  );
    EnableStop        ( false );
    EnableRepeat      ( false );
    EnablePause       ( false );
    EnableSave        ( false );
}

void ApexMainWindow::ExperimentLoaded()
{
    EnableOpen       ( false );
    EnableStart      ( true  );
    EnableStop       ( false );
    EnablePause      ( false );
    EnableSave       ( false );
    EnableExperiment ( true  );
    EnableAutoAnswer ( true  );
}

void ApexMainWindow::Finished()
{
    Initted();
    EnableScreen( false );
    m_sSavefileName = "";
    setWindowState(windowState() &~ Qt::WindowFullScreen);
    menuBar()->show();
}

void ApexMainWindow::ClearScreen()
{
    if ( runningScreen )
    {
        m_pCentral->mp_SetScreen( 0 );
        runningScreen->hideWidgets();
    }
    runningScreen = 0;
}

void ApexMainWindow::Clear()
{
    m_pCentral->mp_ClearContent();
    if ( m_pPanel )
        m_pPanel->mp_Show( false );
    // ClearScreen(); FIXME Michael Hofmann: Rename method to sth. useful,
    // clearscreen seems to not work because old experiment is not valid anymore
    runningScreen = 0; // Workaround
}

/******************************************** MRU ***************************************************************************/

MRU* ApexMainWindow::GetMru()
{
    return m_pMRU.data();
}

const QString& ApexMainWindow::GetOpenDir() const
{
//    qDebug("ApexMainWindow::GetOpenDir");
    return m_pMRU->mf_sGetOpenDir();
}

void ApexMainWindow::MruLoad( const QString& ac_sIniFile )
{
    m_pMRU->mp_LoadFromFile( ac_sIniFile );
}

void ApexMainWindow::MruSave( const QString& ac_sIniFile )
{
    m_pMRU->mp_SaveToFile( ac_sIniFile );
}

void ApexMainWindow::MruFile( const QString& ac_sFile )
{
    m_pMRU->mp_AddItem( ac_sFile );
    SetOpenDir( f_sDirectoryFromFile( ac_sFile ) );
}

void ApexMainWindow::MruClear()
{
    m_pMRU->mp_RemoveAllItems();
}

void ApexMainWindow::SetOpenDir( const QString& ac_sDir )
{
    m_pMRU->mp_SetOpenDir( ac_sDir );
}

/******************************************** ENABLERS ***************************************************************************/

void ApexMainWindow::EnableStart( const bool ac_bEnable )
{
    if ( m_pPanel )
        m_pPanel->mp_EnableStart( ac_bEnable );

    experimentStartAction->setEnabled( ac_bEnable );
}

void ApexMainWindow::EnableStop ( const bool ac_bEnable )
{
    if ( m_pPanel )
        m_pPanel->mp_EnableStop( ac_bEnable );

    experimentStopAction->setEnabled( ac_bEnable );
}

void ApexMainWindow::EnableRepeat ( const bool ac_bEnable )
{
    if ( m_pPanel )
        m_pPanel->mp_EnableRepeat( ac_bEnable );
    stimulusRepeatAction->setEnabled(ac_bEnable);
}

void ApexMainWindow::EnableSkip( const bool ac_bEnable /* = true  */ )
{
    experimentSkipAction->setEnabled (ac_bEnable);
}

void ApexMainWindow::EnableAutoAnswer( const bool ac_bEnable /* = true  */ )
{
    experimentAutoAnswerAction->setEnabled (ac_bEnable);
}

void ApexMainWindow::EnablePause( const bool ac_bEnable )
{
    if ( m_pPanel )
        m_pPanel->mp_EnablePause( ac_bEnable );

    experimentPauseAction->setEnabled( ac_bEnable );
}

void ApexMainWindow::EnableSave( const bool ac_bEnable )
{
    fileSaveAsAction->setEnabled (ac_bEnable);
}

void ApexMainWindow::EnableOpen( const bool ac_bEnable )
{
    fileOpenAction->setEnabled (ac_bEnable);
    m_pMRU->mf_Enable( ac_bEnable );
}

void ApexMainWindow::EnableExperiment( const bool ac_bEnable )
{
    experimentMenu->setEnabled (ac_bEnable);
}

void ApexMainWindow::EnableCalibration( const bool ac_bEnable )
{
    calibrateMenu->setEnabled (ac_bEnable);
}

/******************************************** MISC SLOTS ***************************************************************************/

void ApexMainWindow::SetNumTrials( const unsigned ac_nTrials )
{
    if ( m_pPanel )
        m_pPanel->mp_SetProgressSteps( ac_nTrials );
}

void ApexMainWindow::SetProgress( const unsigned ac_nTrials )
{
    if ( m_pPanel )
        m_pPanel->mp_SetProgress( ac_nTrials );
}

void ApexMainWindow::AddStatusMessage( const QString& ac_sMessage )
{
    const data::ScreensData* config = m_rd->GetData().screensData();
    if ( m_pPanel && config->m_eMode == gc_eNormal )
        m_pPanel->mp_SetText( ac_sMessage );
}

void ApexMainWindow::helpAbout()
{
    QMessageBox::information (NULL, tr("APEX 3"),
            tr ("Apex v3.0\nAll copyrights Exp. ORL, KULeuven\n"
                "Contact tom.francart@med.kuleuven.be for more information"));
}

void ApexMainWindow::helpContents()
{
    QMessageBox::information (NULL, tr("APEX 3"),
            tr("Sorry but there are no help files yet."));
}

void ApexMainWindow::helpDeletePluginCache()
{
    ApexControl::Get().deletePluginCache();
}

void ApexMainWindow::helpShowPluginDialog()
{
    ApexControl::Get().showPluginDialog();
}

void ApexMainWindow::helpEditApexconfig()
{
    ApexControl::Get().editApexconfig();
}


/******************************************** MISC ***************************************************************************/

void ApexMainWindow::closeEvent( QCloseEvent* evt )
{
    evt->accept();
    emit( fileExit() );
}

QWidget* apex::gui::ApexMainWindow::centralWidget()
{
    return m_pCentral.data();
}

//FIXME I think ApexControl should handle status reporting
void ApexMainWindow::updateStatusReporting()
{
    if (showScreenStatusAction->isChecked())
    {
        showMessageWindowAction->setEnabled(true);
        ErrorHandler::Get().addReporter(screenStatus.data(), false);

        if (showMessageWindowAction->isChecked())
            screenStatus->showWindow();
        else
            screenStatus->hideWindow();
    }
    else
    {
        showMessageWindowAction->setEnabled(false);
        ErrorHandler::Get().removeReporter(screenStatus.data());
        screenStatus->hideWindow();
    }

    if (showConsoleStatusAction->isChecked())
        ErrorHandler::Get().addReporter(consoleStatus.data(), false);
    else
        ErrorHandler::Get().removeReporter(consoleStatus.data());
}


