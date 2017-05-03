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

#include "apextools/global.h"

#include "gui/guidefines.h"

#include "apexmainwndbase.h"

#include <QAction>
#include <QImage>
#include <QLayout>
#include <QMenuBar>
#include <QPixmap>
#include <QToolBar>
#include <QToolTip>
#include <QVariant>
#include <QWhatsThis>

/*
 *  Constructs a ApexMainWndBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
ApexMainWndBase::ApexMainWndBase( QWidget* parent, const char* name )
    : QMainWindow (parent)
{
    statusBar();
    setObjectName (name ? name : "ApexMainWndBase");
    setAcceptDrops( false );

    // actions
    fileOpenAction = new QAction ("fileOpenAction", this);
    startPluginRunnerAction = new QAction("startPluginRunnerAction", this);
    selectSoundcardAction = new QAction("selectSouncardAction", this);
    fileSaveAsAction = new QAction ("fileSaveAsAction", this);
    fileExitAction = new QAction ("fileExitAction", this);
    helpContentsAction = new QAction ("helpContentsAction", this);
    helpDeletePluginCacheAction = new QAction ("helpDeletePluginCacheAction", this);
    helpShowPluginDialogAction = new QAction ("helpShowPluginDialogAction", this);
    helpAboutAction = new QAction ("helpAboutAction", this);
    helpEditApexconfigAction = new QAction ("helpEditApexconfigAction", this);
    experimentStartAction = new QAction ("experimentStartAction", this);
    experimentPauseAction = new QAction ("experimentPauseAction", this);
    experimentStopAction = new QAction ("experimentStopAction", this);
    experimentShowStimulusAction = new QAction ("experimentShowStimulus", this);
    recalibrateAction = new QAction ("recalibrateAction", this);
    experimentAutoAnswerAction = new QAction ("autoAnswerAction", this);
    experimentSkipAction = new QAction ("skipAction", this);
    stimulusRepeatAction = new QAction("stimulusRepeatAction", this);
    saveExperimentAction = new QAction("saveExperimentAction", this);
    showMessageWindowAction = new QAction("showMessageWindowAction", this);
    createShortcutToFileAction =
        new QAction("createShortcutToFileAction", this);

    // menubar
    MenuBar = menuBar();

    fileMenu = new QMenu (tr("&File"), this);
    MenuBar->addMenu (fileMenu);
    fileMenu->addAction (fileOpenAction);
    fileMenu->addAction (startPluginRunnerAction);
    fileMenu->addAction (selectSoundcardAction);
    fileMenu->addAction (fileSaveAsAction);
    fileMenu->addAction (createShortcutToFileAction);
    fileMenu->addAction(showMessageWindowAction);
    fileMenu->addSeparator();
    fileMenu->addAction (fileExitAction);

    experimentMenu = new QMenu (tr("&Experiment"), this);
    MenuBar->addMenu (experimentMenu);
    experimentMenu->addAction (experimentStartAction);
    experimentMenu->addAction (experimentPauseAction);
    experimentMenu->addAction (experimentStopAction);
    experimentMenu->addAction (stimulusRepeatAction);
    experimentMenu->addAction (experimentAutoAnswerAction);
    experimentMenu->addAction (experimentSkipAction);
    experimentMenu->addSeparator();
    experimentMenu->addAction (experimentShowStimulusAction);
    experimentMenu->addSeparator();
    experimentMenu->addAction (saveExperimentAction);

    calibrateMenu = new QMenu (tr("&Calibrate"), this);
    MenuBar->addMenu (calibrateMenu);
    calibrateMenu->addAction (recalibrateAction);

    hardwareSetupMenu = new QMenu (tr ("&Hardware setup"),
            calibrateMenu);

    helpMenu = new QMenu (tr("&Help"), this);
    MenuBar->addMenu (helpMenu);
    helpMenu->addAction (helpContentsAction);
    helpMenu->addSeparator();
    helpMenu->addAction(helpShowPluginDialogAction );
    helpMenu->addAction (helpDeletePluginCacheAction);
    helpMenu->addAction(helpEditApexconfigAction);
    helpMenu->addSeparator();
    helpMenu->addAction (helpAboutAction);

        // add actions to main window, so they also work without menu shown
    addActions(experimentMenu->actions());
    addActions(fileMenu->actions());

    languageChange();
    resize (QSize (806, 574).expandedTo (minimumSizeHint()));

    // signals and slots connections
    connect (fileOpenAction, SIGNAL (triggered()),
            this, SIGNAL (fileOpen()));
    connect (startPluginRunnerAction, SIGNAL (triggered()),
             this, SIGNAL (startPluginRunner()));
    connect (selectSoundcardAction, SIGNAL(triggered()),
            this, SIGNAL(selectSoundcard()));
    connect (fileSaveAsAction, SIGNAL (triggered()),
            this, SIGNAL (fileSaveAs()));
    connect (fileExitAction, SIGNAL (triggered()),
            this, SIGNAL (fileExit()));
    connect(saveExperimentAction, SIGNAL(triggered()),
            this, SIGNAL(saveExperiment()));
    connect (experimentStopAction, SIGNAL (triggered()),
            this, SIGNAL (stopClicked()));
    connect (experimentStartAction, SIGNAL (triggered()),
            this, SIGNAL (startClicked()));
    connect (experimentPauseAction, SIGNAL (triggered()),
            this, SIGNAL (pauseClicked()));
    connect (experimentShowStimulusAction, SIGNAL(triggered()),
             this, SIGNAL (showStimulus()));
    connect (stimulusRepeatAction, SIGNAL(triggered()),
             this, SIGNAL (repeatTrial()));
    connect (experimentAutoAnswerAction, SIGNAL (toggled(bool)),
            this, SIGNAL (autoAnswerClicked(bool)));
    connect (experimentSkipAction, SIGNAL (triggered()),
            this, SIGNAL (skipClicked()));
    connect (recalibrateAction, SIGNAL (triggered()),
            this, SIGNAL (recalibrateClicked()));
    connect (helpContentsAction, SIGNAL (triggered()),
            this, SLOT (helpContents()));
    connect (helpDeletePluginCacheAction, SIGNAL (triggered()),
            this, SLOT (helpDeletePluginCache()));
    connect (helpEditApexconfigAction, SIGNAL (triggered()),
             this, SLOT (helpEditApexconfig()));
     connect (helpShowPluginDialogAction, SIGNAL (triggered()),
            this, SLOT (helpShowPluginDialog()));
    connect (helpAboutAction, SIGNAL (triggered()),
            this, SLOT (helpAbout()));
    connect(showMessageWindowAction, SIGNAL(triggered()),
            this, SIGNAL(statusReportingChanged()));
    connect(createShortcutToFileAction, SIGNAL(triggered()),
            this, SIGNAL(createShortcut()));

#ifndef Q_OS_ANDROID
    createShortcutToFileAction->setVisible(false);
#endif
}

/*
 *  Destroys the object and frees any allocated resources
 */
ApexMainWndBase::~ApexMainWndBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ApexMainWndBase::languageChange()
{
    setWindowTitle (tr ("APEX 3" ));

    fileOpenAction->setText (tr ("&Open"));
    fileOpenAction->setShortcut (tr ("Ctrl+O"));

    startPluginRunnerAction->setText(tr("Load &runner"));
    selectSoundcardAction->setText(tr("Select default &soundcard"));

    fileSaveAsAction->setText (tr ("Save Results &As..."));

    saveExperimentAction->setText(tr("Save &experiment as..."));

    fileExitAction->setText (tr ("&Exit"));
    fileExitAction->setShortcut (Qt::CTRL + Qt::Key_P);

    helpContentsAction->setText (tr ("&Contents..."));
    helpDeletePluginCacheAction->setText (tr ("&Delete QT Plugin Cache"));
    helpShowPluginDialogAction->setText(tr("&List plugins"));
    helpAboutAction->setText (tr ("&About"));
    helpEditApexconfigAction->setText(tr("&Edit Apexconfig file"));

    experimentStartAction->setText (tr ("Start"));
    experimentStartAction->setShortcut (Qt::Key_F5);

    experimentPauseAction->setText (tr ("Pause"));
    experimentPauseAction->setShortcut (Qt::Key_F6);

    experimentStopAction->setText (tr ("Stop"));
    experimentStopAction->setShortcut (Qt::Key_F4);

    experimentShowStimulusAction->setText (tr ("Show Stimulus Connections"));

    stimulusRepeatAction->setText(tr("Repeat last trial"));
    stimulusRepeatAction->setEnabled(false);
    stimulusRepeatAction->setShortcut( Qt::Key_F9 );

    experimentAutoAnswerAction->setText (tr("AutoAnswer"));
    experimentAutoAnswerAction->setCheckable(true);
    experimentAutoAnswerAction->setChecked(false);

    recalibrateAction->setText (tr ("Recalibrate"));

    experimentSkipAction->setText (tr ("Skip Wait"));
    experimentSkipAction->setShortcut (Qt::Key_F7);

    showMessageWindowAction->setText(tr("Show message window"));

    createShortcutToFileAction->setText(tr("Create Shortcut"));
}

void ApexMainWndBase::helpContents()
{
    qCWarning(APEX_RS, "ApexMainWndBase::helpContents(): Not implemented yet");
}

void ApexMainWndBase::helpAbout()
{
    qCWarning(APEX_RS, "ApexMainWndBase::helpAbout(): Not implemented yet");
}

