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

#include "apexdata/experimentdata.h"

#include "apexdata/screen/buttongroup.h"
#include "apexdata/screen/screen.h"
#include "apexdata/screen/screenelement.h"
#include "apexdata/screen/screenresult.h"
#include "apexdata/screen/screensdata.h"

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "apextools/status/screenstatusreporter.h"

#include "feedback/feedback.h"

#include "gui/guidefines.h"

#include "runner/experimentrundelegate.h"

#include "screen/screenelementrundelegate.h"
#include "screen/screenrundelegate.h"

#include "apexcontrol.h"
#include "centralwidget.h"
#include "errorhandler.h"
#include "mainwindow.h"
#include "mru.h"
#include "panel.h"
#include "plugindialog.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QStatusBar>

using namespace apex;
using namespace apex::gui;
using apex::data::gc_eChild;
using apex::data::gc_eNormal;

const QString defaultStyleSheet(
    //        "QPushButton[role=\"none\"]{background-color: yellow}"
    "QPushButton[role=\"highlight\"]{background-color: #ffff90}\n"
    "QPushButton[role=\"positive\"]{background-color: green}\n"
    "QPushButton[role=\"negative\"]{background-color: red}\n");

ApexMainWindow::ApexMainWindow(QWidget * /*a_pParent*/)
    : ApexMainWndBase(),
      m_rd(0),
      m_config(0),
      m_pMRU(0),
      m_pPanel(0),
      m_pCentral(0),
      paused(false),
      m_sSavefileName(),
      runningScreen(),
      m_pCurFeedback()
{
    // setup
    statusBar()->hide();

    // create the mru list
    m_pMRU.reset(new MRU(fileMenu));

    // create the main widget
    m_pCentral.reset(new ApexCentralWidget(this));
    setCentralWidget(m_pCentral.data());

    // setup status reporting
    // FIXME I don't think this should be here
    screenStatus = new ScreenStatusReporter();
    connect(ErrorHandler::instance(), SIGNAL(itemAdded(apex::StatusItem)),
            screenStatus, SLOT(addItem(apex::StatusItem)));
    connect(this, SIGNAL(statusReportingChanged()), this,
            SLOT(updateStatusReporting()));
#ifdef Q_OS_ANDROID
    screenStatus->showMaximized();

    QFont appFont = QApplication::font();
    appFont.setPixelSize(appFont.pixelSize() *
                         QApplication::primaryScreen()->devicePixelRatio());
    QApplication::setFont(appFont);
#endif

    // ready
    Initted();
}

void ApexMainWindow::setExperimentRunDelegate(ExperimentRunDelegate *p_rd)
{
    m_rd = p_rd;
    connect(m_rd, SIGNAL(showMessage(QString)), this,
            SLOT(AddStatusMessage(QString)));
}

ApexMainWindow::~ApexMainWindow()
{
    m_pPanel.reset();
    delete screenStatus;
}

void ApexMainWindow::SetIcon(const QString &ac_sIcon)
{
    setWindowIcon(QPixmap(ac_sIcon));
}

void ApexMainWindow::ApplyConfig(const data::ScreensData *c)
{
    qApp->setStyleSheet(c->apexStyle());

    m_config = c;
    const data::ScreensData &config = *c;

    m_pPanel.reset(0);

    if (config.hasPanelEnabled()) {
        if (config.m_eMode == data::gc_eNormal || !config.hasPanelMovie()) {
            Panel *p = new Panel(centralWidget(), c);
            if (!p->mp_bSetConfig(config)) // FIXME memory leak!!!
                throw ApexStringException(
                    "ApexMainWindow::ApplyConfig: couldn't create the panel.");
            m_pPanel.reset(p);
            m_pPanel->mp_SetProgressSteps(100);
            m_pCentral->mp_SetPanel(p);
            connect(p->getSignalSlotProxy(), SIGNAL(ms_Start()),
                    SIGNAL(startClicked()));
            connect(p->getSignalSlotProxy(), SIGNAL(ms_Stop()),
                    SIGNAL(stopClicked()));
            connect(p->getSignalSlotProxy(), SIGNAL(ms_Pause()),
                    SIGNAL(pauseClicked()));
            connect(p->getSignalSlotProxy(), SIGNAL(ms_Panic()),
                    SIGNAL(StopOutput()));
            connect(p->getSignalSlotProxy(), SIGNAL(ms_Repeat()),
                    SIGNAL(RepeatOutput()));
        } else {
#ifdef FLASH
            ChildModePanel *p = new ChildModePanel(centralWidget());
            if (!p->mp_bSetConfig(config))
                throw ApexStringException(
                    "ApexMainWindow::ApplyConfig: couldn't create the panel.");
            m_pPanel.reset(p);
            m_pCentral->mp_SetPanel(p);
#else
            throw ApexStringException("ApexMainWindow::ApplyConfig: can't do "
                                      "childmode with panel when FLASH is not "
                                      "defined");
#endif
        }
    } else {
        m_pCentral->mp_SetPanel(0);
    }

    if (config.m_eMode == gc_eChild) {
        m_pCentral->setBackgroundColor(Qt::white);
        m_pCentral->mp_SetHighColor(sc_DefaultBGColor);
    } else {
        m_pCentral->setBackgroundColor(sc_DefaultBGColor);
        m_pCentral->mp_SetHighColor(Qt::white);
    }

    if (!config.hasMenuEnabled())
        menuBar()->hide();

    if (config.hasFullScreenEnabled())
        setWindowState(windowState() | Qt::WindowFullScreen);

    //  m_pCentral->setStyle

    m_pCentral->mp_LayoutNow();
    if (m_pPanel)
        m_pPanel->mp_Show(config.hasPanelEnabled());

    m_pCentral->repaint();
}

void ApexMainWindow::SetScreen(gui::ScreenRunDelegate *ac_Screen)
{
    // do nothing if it's the same screen
    if (runningScreen == ac_Screen) {
        runningScreen->clearText();
        return;
    }

    // show, connect and lay out the new screen
    if (runningScreen) {
        runningScreen->getWidget()->setParent(0);
        ClearScreen();
    }

    runningScreen = ac_Screen;

    // m_pCentral->mp_SetScreen( runningScreen->getLayout() );
    runningScreen->getWidget()->setStyleSheet(
        defaultStyleSheet + m_rd->GetData().screensData()->style());

    m_pCentral->setScreenWidget(runningScreen->getWidget());

    m_pCentral->mp_LayoutNow();
    EnableScreen(false);
    runningScreen->clearText();
    runningScreen->showWidgets();
}

gui::ScreenRunDelegate *ApexMainWindow::setScreen(const QString &id)
{
    gui::ScreenRunDelegate *s = m_rd->GetScreen(id);
    SetScreen(s);
    return s;
}

ScreenRunDelegate *ApexMainWindow::currentScreen() const
{
    return runningScreen;
}

void ApexMainWindow::setAnswer(const QString &answer)
{
    if (runningScreen != 0)
        runningScreen->setAnswer(answer);
}

void ApexMainWindow::EnableScreen(const bool ac_bEnable /*= true*/)
{
    if (runningScreen) {
        runningScreen->enableWidgets(ac_bEnable);

        ReclaimFocus();

        if (ac_bEnable &&
            ApexControl::Get()
                .GetCurrentExperimentRunDelegate()
                .GetAutoAnswer()) {

            const data::ButtonGroup *b =
                runningScreen->getScreen()->getButtonGroup();
            if (!b) {
                QMessageBox::warning(0, tr("No buttongroup found"),
                                     tr("Error : no buttongroup was found, "
                                        "can't automatically select a button"),
                                     QString(tr("OK")));
                return;
            }
            data::ButtonGroup::const_iterator i = b->begin();
            int number = randomGenerator.nextDouble() * b->size();

            for (int c = 0; c < number; ++c)
                ++i;

            QString elem = (*i);
            qCDebug(APEX_RS, "Automatically selecting %s", qPrintable(elem));

            static ScreenResult result;
            result.clear();

            result[runningScreen->getScreen()->getButtonGroup()->getID()] =
                elem;

            runningScreen->addInterestingTexts(result);

            // QMetaObject::invokeMethod (this, "Answered",
            // Qt::QueuedConnection, Q_ARG (const ScreenResult*, &result));

            Q_EMIT Answered(&result);
        }
    }
}

void ApexMainWindow::doDeterministicAutoAnswer()
{
    randomGenerator.setSeed(0);
}

bool ApexMainWindow::screenEnabled() const
{
    return runningScreen->widgetsEnabled();
}

void ApexMainWindow::ReclaimFocus()
{
    if (QApplication::focusWidget() == 0)
        MenuBar->setFocus();
}

/******************************************** FEEDBACK ************************/

void ApexMainWindow::feedback(ScreenElementRunDelegate::FeedbackMode mode,
                              const QString &elementId)
{
    // the element
    if (runningScreen != 0) {
        runningScreen->feedback(mode, elementId);
        m_rd->modFeedback()->highLight(
            elementId); // TODO: implement positive and negative feedback
        m_pCurFeedback = runningScreen->getFeedBackElement();
    }

    // the panel
    if (m_pPanel != 0 && mode != ScreenElementRunDelegate::HighlightFeedback)
        m_pPanel->feedBack(mode);
}

void ApexMainWindow::HighLight(const QString &ac_sID)
{
    feedback(ScreenElementRunDelegate::HighlightFeedback, ac_sID);
}

void ApexMainWindow::EndFeedBack()
{
    if (m_pCurFeedback)
        runningScreen->feedback(ScreenElementRunDelegate::NoFeedback,
                                m_pCurFeedback);
    if (m_pPanel)
        m_pPanel->feedBack(ScreenElementRunDelegate::NoFeedback);
    m_rd->modFeedback()->clear();
    m_pCentral->repaint();
}

void ApexMainWindow::EndHighLight()
{
    if (m_pCurFeedback)
        runningScreen->feedback(ScreenElementRunDelegate::NoFeedback,
                                m_pCurFeedback);
    m_rd->modFeedback()->clear();
    m_pCentral->repaint();
}

void ApexMainWindow::AnswerFromElement(ScreenElementRunDelegate *ac_Element)
{
    static ScreenResult result;
    result.clear();

    // add element id if buttongroup
    if (runningScreen->getScreen()->getButtonGroup()) {
        if (runningScreen->getScreen()->getButtonGroup()->IsElement(
                ac_Element->getID()))
            result[runningScreen->getScreen()->getButtonGroup()->getID()] =
                ac_Element->getID();
    }

    if (runningScreen->getScreen()
                ->elementById(ac_Element->getID())
                ->elementType() == ScreenElement::Picture ||
        runningScreen->getScreen()
                ->elementById(ac_Element->getID())
                ->elementType() == ScreenElement::Button) {
        result[ac_Element->getID()] = QString();
    }

    runningScreen->addInterestingTexts(result);
    runningScreen->addScreenParameters(result);
    result.setLastClickPosition(ac_Element->getClickPosition());

    qCDebug(APEX_RS) << "***Answered" << result;
    Q_EMIT Answered(&result);
}

/******************************************** STATE
 * ***************************************************************************/

// TODO remove
void ApexMainWindow::Paused()
{
    const data::ScreensData *config = m_rd->GetData().screensData();
    if (m_pPanel && config->m_eMode == gc_eNormal)
        ((Panel *)m_pPanel.data())->mp_Paused();
}

void ApexMainWindow::setPaused(bool paused)
{
    if (paused != this->paused) {
        this->paused = paused;
        static_cast<Panel *>(m_pPanel.data())->setPaused(paused);
    }
}

bool ApexMainWindow::isPaused() const
{
    return paused;
}

void ApexMainWindow::Initted()
{
    EnableOpen(true);
    EnableStart(false);
    EnableStop(false);
    EnableRepeat(false);
    EnablePause(false);
    EnableSave(false);
    EnableExperiment(false);
    EnableCalibration(false);
}

void ApexMainWindow::Started()
{
    EnableStart(false);
    EnableStop(false);
    EnableRepeat(false);
    EnablePause(true);
    EnableSave(true);
}

void ApexMainWindow::Stopped()
{
    EnableStart(true);
    EnableStop(false);
    EnableRepeat(false);
    EnablePause(false);
    EnableSave(false);
}

void ApexMainWindow::ExperimentLoaded()
{
    EnableOpen(false);
    EnableStart(true);
    EnableStop(false);
    EnablePause(false);
    EnableSave(false);
    EnableExperiment(true);
}

void ApexMainWindow::Finished()
{
    Initted();
    Clear();
    EnableScreen(false);
    m_sSavefileName = "";
    setWindowState(windowState() & ~Qt::WindowFullScreen);
#ifndef Q_OS_ANDROID // the menuBar()->isHidden check would be cleaner but
                     // QMenuBar is always hidden on android
    menuBar()->show();
#endif
}

void ApexMainWindow::ClearScreen()
{
    if (runningScreen) {
        m_pCentral->mp_SetScreen(0);
        runningScreen->hideWidgets();
    }
    runningScreen = 0;
    m_pCurFeedback = 0;
}

void ApexMainWindow::Clear()
{
    m_pCentral->mp_ClearContent();
    if (m_pPanel)
        m_pPanel->mp_Show(false);
    ClearScreen(); // FIXME Michael Hofmann: Rename method to sth. useful,
    // clearscreen seems to not work because old experiment is not valid anymore
    runningScreen = 0; // Workaround
}

/******************************************** MRU
 * ***************************************************************************/

MRU *ApexMainWindow::GetMru()
{
    return m_pMRU.data();
}

const QString &ApexMainWindow::GetOpenDir() const
{
    //    qCDebug(APEX_RS, "ApexMainWindow::GetOpenDir");
    return m_pMRU->openDir();
}

void ApexMainWindow::MruLoad()
{
    m_pMRU->loadFromFile();
}

void ApexMainWindow::SetOpenDir(const QString &ac_sDir)
{
    m_pMRU->setOpenDir(ac_sDir);
}

/******************************************** ENABLERS
 * ***************************************************************************/

void ApexMainWindow::EnableStart(const bool ac_bEnable)
{
    if (m_pPanel)
        m_pPanel->mp_EnableStart(ac_bEnable);

    experimentStartAction->setEnabled(ac_bEnable);
}

bool ApexMainWindow::startEnabled() const
{
    return experimentStartAction->isEnabled();
}

void ApexMainWindow::EnableStop(const bool ac_bEnable)
{
    if (m_pPanel)
        m_pPanel->mp_EnableStop(ac_bEnable);

    experimentStopAction->setEnabled(ac_bEnable);
}

bool ApexMainWindow::stopEnabled() const
{
    return experimentStopAction->isEnabled();
}

void ApexMainWindow::EnableRepeat(const bool ac_bEnable)
{
    if (m_pPanel)
        m_pPanel->mp_EnableRepeat(ac_bEnable);
    stimulusRepeatAction->setEnabled(ac_bEnable);
}

void ApexMainWindow::EnableSkip(const bool ac_bEnable /* = true  */)
{
    experimentSkipAction->setEnabled(ac_bEnable);
}

void ApexMainWindow::EnableSelectSoundcard(const bool ac_bEnable)
{
    selectSoundcardAction->setEnabled(ac_bEnable);
}

void ApexMainWindow::EnablePause(const bool ac_bEnable)
{
    if (m_pPanel)
        m_pPanel->mp_EnablePause(ac_bEnable);

    experimentPauseAction->setEnabled(ac_bEnable);
}

bool ApexMainWindow::pauseEnabled() const
{
    return experimentPauseAction->isEnabled();
}

bool ApexMainWindow::skipEnabled() const
{
    return experimentSkipAction->isEnabled();
}

bool ApexMainWindow::repeatEnabled() const
{
    return stimulusRepeatAction->isEnabled();
}

void ApexMainWindow::EnableSave(const bool ac_bEnable)
{
    fileSaveAsAction->setEnabled(ac_bEnable);
}

void ApexMainWindow::EnableOpen(const bool ac_bEnable)
{
    fileOpenAction->setEnabled(ac_bEnable);
    m_pMRU->enable(ac_bEnable);
}

void ApexMainWindow::EnableExperiment(const bool ac_bEnable)
{
    experimentMenu->setEnabled(ac_bEnable);
}

void ApexMainWindow::EnableCalibration(const bool ac_bEnable)
{
    calibrateMenu->setEnabled(ac_bEnable);
}

/******************************************** MISC SLOTS
 * ***************************************************************************/

// void ApexMainWindow::SetNumTrials( const unsigned ac_nTrials )
// {
//     if ( m_pPanel )
//         m_pPanel->mp_SetProgressSteps( ac_nTrials );
// }
//
// void ApexMainWindow::SetProgress( const unsigned ac_nTrials )
// {
//     if ( m_pPanel )
//         m_pPanel->mp_SetProgress( ac_nTrials );
// }

void ApexMainWindow::setProgress(double percentage)
{
    if (m_pPanel != 0)
        m_pPanel->mp_SetProgress(percentage);
}

void ApexMainWindow::AddStatusMessage(const QString &ac_sMessage)
{
    const data::ScreensData *config = m_rd->GetData().screensData();
    if (m_pPanel && config->m_eMode == gc_eNormal)
        m_pPanel->mp_SetText(ac_sMessage);
}

void ApexMainWindow::helpAbout()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("APEX 3"));
    msgBox.setText(
        tr("APEX v3.1 %1\n"
           "All copyrights ExpORL, KULeuven\n"
           "Contact tom.francart@med.kuleuven.be for more information")
            .arg(ApexTools::fetchVersion()));
    msgBox.setDetailedText(ApexTools::fetchDiffstat());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void ApexMainWindow::helpContents()
{
    QMessageBox::information(NULL, tr("APEX 3"),
                             tr("Sorry but there are no help files yet."));
}

void ApexMainWindow::helpDeletePluginCache()
{
    PluginDialog::refreshPluginCache();
}

void ApexMainWindow::helpShowPluginDialog()
{
    ApexControl::Get().showPluginDialog();
}

void ApexMainWindow::helpEditApexconfig()
{
    ApexControl::Get().editApexconfig();
}

/******************************************** MISC
 * ***************************************************************************/

void ApexMainWindow::closeEvent(QCloseEvent *evt)
{
    QMessageBox confirmBox;
    confirmBox.setText("Experiment in progress, are you sure?");
    confirmBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    confirmBox.setDefaultButton(QMessageBox::Cancel);

    if (ApexControl::Get().isExperimentRunning() &&
        confirmBox.exec() == QMessageBox::Cancel) {
        evt->ignore();
    } else {
        evt->accept();
        Q_EMIT fileExit();
    }
}

QWidget *apex::gui::ApexMainWindow::centralWidget()
{
    return m_pCentral.data();
}

// FIXME I think ApexControl should handle status reporting
void ApexMainWindow::updateStatusReporting()
{
    screenStatus->show();
    screenStatus->raise();
}

void ApexMainWindow::quickWidgetBugHide()
{
#ifdef Q_OS_ANDROID
    screenStatus->hide();
    this->hide();
#endif
}

void ApexMainWindow::quickWidgetBugShow()
{
#ifdef Q_OS_ANDROID
    this->show();
    this->raise();
#endif
}
