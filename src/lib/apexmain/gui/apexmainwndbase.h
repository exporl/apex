/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/
#ifndef APEXMAINWNDBASE_H
#define APEXMAINWNDBASE_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QAction;
class QActionGroup;
class QToolBar;
class QMenu;

  /**
    * File originally generated from mainwndbase.ui.
    * DO NOT UIC the .ui file, since this file is modified to apex' needs.
    */

class ApexMainWndBase : public QMainWindow
{
    Q_OBJECT

public:
    ApexMainWndBase( QWidget* parent = 0, const char* name = 0 );
    ~ApexMainWndBase();

    QMenuBar *MenuBar;
    QMenu *fileMenu;
    QMenu *experimentMenu;
    QMenu *calibrateMenu;
    QMenu *hardwareSetupMenu;
    QMenu *helpMenu;
    QMenu* viewMenu;
    QAction* fileOpenAction;
    QAction* fileSaveAsAction;
    QAction* fileExitAction;
    QAction* helpContentsAction;
    QAction* helpDeletePluginCacheAction;
    QAction* helpShowPluginDialogAction;
    QAction* helpAboutAction;
    QAction* helpEditApexconfigAction;
    QAction* experimentStartAction;
    QAction* experimentPauseAction;
    QAction* experimentStopAction;
    QAction* experimentShowStimulusAction;
    QAction* experimentSkipAction;
    QAction* stimulusRepeatAction;
    QAction* experimentAutoAnswerAction;
    QAction* recalibrateAction;
    QAction* startPluginRunnerAction;
    QAction* saveExperimentAction;
    QAction* showMessageWindowAction;
    QAction* showScreenStatusAction;
    QAction* showConsoleStatusAction;

Q_SIGNALS:
    void fileOpen();
    void startPluginRunner();
    void fileSaveAs();
    void fileExit();
    void startClicked();
    void pauseClicked();
    void stopClicked();
    void skipClicked();
    void recalibrateClicked();
    void autoAnswerClicked(bool);
    void showStimulus();
    void repeatStimulus();
    void saveExperiment();
    void statusReportingChanged();

public Q_SLOTS:
    virtual void helpContents();
    virtual void helpDeletePluginCache() = 0;
    virtual void helpShowPluginDialog() = 0;
    virtual void helpAbout();
    virtual void helpEditApexconfig() = 0;

protected Q_SLOTS:
    virtual void languageChange();
};

#endif // APEXMAINWNDBASE_H
