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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_APEXCONTROL_H_
#define _EXPORL_SRC_LIB_APEXMAIN_APEXCONTROL_H_

#include "apextools/services/servicemanager.h"

#include "experimentcontrol.h"

#include <QObject>

#include <memory>

class QDateTime;

namespace apex
{

class ExperimentIo;
class ExperimentRunner;
class ExperimentRunDelegate;
class ExperimentControl;
class StatusReporter;

namespace data
{
class ExperimentData;
}

namespace gui
{
class ApexMainWindow;
}

namespace stimulus
{
class StimulusControl;
}

/**
 * @brief Main apex control class: initializes and connects all objects
 *
 * @author Tom Francart,,,
 */
class ApexControl : public QObject, public Service<ApexControl>
{
        Q_OBJECT

    public:

        ApexControl();
        ~ApexControl();

        const char* Name();
        int exec();

        const data::ExperimentData&             GetCurrentExperiment();
        const ExperimentRunDelegate&  GetCurrentExperimentRunDelegate();

        gui::ApexMainWindow*          GetMainWnd();       // FIXME: remove
        gui::ApexMainWindow*          mainWindow();

        QDateTime GetStartTime() const;  //! get time of experiment start

        const QString& saveFilename() const;

    public slots:

        void StartUp(); // do one-time initialisations that should not occur in the constructor
        void StopOutput();
        void setAutoAnswer(bool);
        void ShowStimulus();
        void calibrate();

        void fileExit();
        void fileOpen(const QString& file = QString());
        void saveExperiment();
        void startPluginRunner();
        void selectSoundcard();

        bool newExperiment(data::ExperimentData* data);

        void deletePluginCache();
        void editApexconfig();
        void showPluginDialog();
        void errorMessage(const QString& source, const QString& message);

        void setResultsFilePath(QString filename);

    private slots:

        void afterExperiment();

    private:

        bool configure();         //parse mainconfig xml, configures apex
        void parseCommandLine();      //parse cmd and load experiment if any
        void showStartupDialog();
        void makeModules();
        void deleteModules();
        void setupIo();
        bool isExperimentRunning() const;

        QScopedPointer<ExperimentRunner> mod_experimentselector;
        gui::ApexMainWindow* const m_Wnd;
        QString mSaveFilename;            // parsed from commandline
        StatusReporter& m_error;
        ExperimentControl::Flags flags;

        QScopedPointer<ExperimentControl> experimentControl;
        QScopedPointer<data::ExperimentData> experimentData;
        QScopedPointer<stimulus::StimulusControl> stimulusControl;
        QScopedPointer<ExperimentRunDelegate> runDelegate;

        bool recordExperiments;
        bool dummySoundcard;
        bool autoStartExperiments;
        bool autoAnswer;
        bool deterministicRandom;
        bool noResults;
        bool autoSaveResults;
        bool exitAfter;
};

}

#endif
