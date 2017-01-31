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

#ifndef APEXAPEXCONTROL_H
#define APEXAPEXCONTROL_H

#include <QDateTime>
#include <QApplication>

#include <memory>

#include "runner/experimentrunner.h"
#include "services/errorhandler.h"
#include "apextypedefs.h"
#include "services/servicemanager.h"

namespace apex
{
    class MainConfigFileParser;
    class ExperimentParser;
    class ApexModule;
    class Calibrator;
    class ApexProcedureConfig;
    class Corrector;
    class ApexResultSink;
    class ApexProcedure;
    class ApexTimer;
    class ApexScreen;
    class RandomGenerators;
    class ParameterDialogResults;
    class ScreenResult;
    class ApexControlThread;
    class ExperimentRunDelegate;
    class ExperimentRunner;

    namespace stimulus
    {
        class Stimulus;
        class DataBlock;
        class StimulusOutput;
    }

    namespace device
    {
        class Controllers;
    }

    namespace data
    {
        class StimuliData;
        class ExperimentData;
        class ApexTrial;
        class ResultParameters;
    }

    namespace gui
    {
        class ApexMainWindow;
    }


    /**
    Main apex control class: initializes and connects all objects

    @author Tom Francart,,,
    */
    class ApexControl: public QObject, public Service<ApexControl>
    {
        Q_OBJECT
    public:
        ApexControl();
        ~ApexControl();
        const char* Name()
        {
            return "ApexControl";
        }
        int exec()
        {
            return m_app->exec();
        }

    // API
  const data::ExperimentData&             GetCurrentExperiment();
  const ExperimentRunDelegate&  GetCurrentExperimentRunDelegate();

  gui::ApexMainWindow*          GetMainWnd();       // FIXME: remove
  gui::ApexMainWindow*          mainWindow();
  const gui::ScreenRunDelegate*            GetCurrentScreen() const;

  stimulus::StimulusOutput*     GetOutput() ;

  data::ApexTrial*                    pGetCurrentTrial();
  const data::ApexTrial*              pGetCurrentTrial() const;

  const QString                 GetCurrentTrial() const;
  const QString                 GetCurrentAnswerElement() const;

  const QDateTime&              GetStartTime() const;  //! get time of experiment start

  const QString& saveFilename() const;

  // methods replacing slots
  void SetCurrentTrial( const QString& ac_sName );      // used by procedure
  void StartTimer();

  void ConnectProcedures( ApexProcedure* newproc, ApexProcedure* proc );    //! connect signals of child procedures

  ApexControlThread* mf_pGetControlThread() const { return mc_pControlThread; }

  int mf_nGetScreenElementFinishedEvent() const;


public slots:
  void StartUp(); // do one-time initialisations that should not occure in the constructor
  void Start();   // start the experiment
  void Stop();    // stop the experiment
  void RepeatOutput();
  void StopOutput();
  void DoPause();
  void Finished();
  void setAutoAnswer(bool);
  void ShowStimulus();
  void SetScreen( const QString& ac_sID );
  void KillWait();

  void ExperimentFinished();      // after everything is finished, this is called and ApexControl can quit or start a new experiment
  void fileExit();
  void fileOpen();
  void saveExperiment();
  void mruFileOpen(const QString& filename);
  void startPluginRunner();

  void Calibrate();
  bool newExperiment(data::ExperimentData* data);

  // replacement slots, we replace these slots to control the order of events
  void SetCorrectAnswer(unsigned);
  void NewStimulus( const QString& );     //load a new stimulus
  void Answered( const ScreenResult* );

  void deletePluginCache();
  void editApexconfig();
  void showPluginDialog();
  void errorMessage(const QString& source, const QString& message);

signals:
  void NewAnswer( const QString& );
  void NewScreen( gui::ScreenRunDelegate* );
  void NewStimulus( stimulus::Stimulus* );  //for parameterlist

private:
  bool configure();         //parse mainconfig xml, configures apex
  void parsecmdline();      //parse cmd and load experiment if any
  void ShowSoundcards();    //! show all soundcards present and exit
  void makeModules();
  void deleteModules();
  void PrepareExperiment(); //additional actions before the experiment can start/exit
  void SaveAndClose();        //
  void showStartupDialog();

protected:
    /**
      * Override this to receive events from the thread.
      * @param e a QDispatchedEvent normally
      */
  void customEvent( QEvent* e );

private:
  QApplication* m_app;

  std::auto_ptr<ExperimentRunner>         mod_experimentselector;

  std::auto_ptr<data::ExperimentData>               experiment;
  // owned by us
  std::auto_ptr<ExperimentRunDelegate> experimentRunDelegate;

  QString mSaveFilename;            // parsed from commandline
  bool recordExperiments;
  bool autoStartExperiments;
  bool autoAnswer;
  bool deterministicRandom;
  bool noResults;
  bool autoSaveResults;
  bool exitAfter;

    /**
      * tState
      *   the current experiment's state
      */
  struct tState
  {
      /**
        * Constructor.
        * Resets state.
        */
    tState();

       /**
        * Reset state.
        */
    void mp_Reset();

      /**
        * Set error flag.
        */
    void mp_SetError();

      /**
        * Enum with the experiment state.
        * States are set either directly, or by
        * queueing the state vi a timer in ApexControlThread.
        * @note mc_eStimDone, mc_eFeedbackDone and mc_eItiDone
        * are just here for the sake of completeness and debugging:
        * they are never reached as a state, instead we swicth back
        * to the previous state (mc_eStarted/mc_ePasued)immedeately.
        */
    enum mt_eState
    {
      mc_eIdle,         //!> just started or experiment finished
      mc_eLoaded,       //!> an experiment is loaded
      mc_eIntro,        //!> about to start intro
      mc_eIntroDone,    //!> intro done, awaiting start buttonSetCorrectAnswer(unsigned)
      mc_eStarted,      //!> the experiment has started
      mc_eStopped,      //!> stopped
      mc_eStim,         //!> stimulus playing
      mc_eStimDone,     //!> stimulus completed
      mc_eFeedback,     //!> answer given, showing feedback
      mc_eFeedbackDone, //!> feedback finished
      mc_ePaused,       //!> pasued
      mc_eIti,          //!> inter trial interval
      mc_eItiDone,      //!> intertrial interval finished
      mc_eOutro,        //!> about to start outro
      mc_eOutroDone,    //!> outro done
      mc_eQuitting      //!> ...
    };

      /**
        * Set a new state.
        * The current state is saved in m_ePreviousState.
        * If PRINTSTATE is defined, mf_PrintState() is called to aid debugging.
        * @param ac_eState the new state
        */
    void mp_SetState( const mt_eState ac_eState );

      /**
        * Switch back to the previous state.
        * The parameter is just there for debugging.
        * @see mt_eState
        * @param ac_eIntermediateState printed when PRINTSTATE is defined
        */
    void mp_RestoreState( const mt_eState ac_eIntermediateState );

      /**
        * Print the current state.
        */
    void mf_PrintState() const;

    mt_eState   m_eState;
    mt_eState   m_ePreviousState;
    bool        m_bResult;   //!result of previous trial
    bool        m_bError;    //!error loading experiment (ignore all other flags)
    QDateTime   m_StartTime;
    QString     m_sTrial;
    QString     m_sStimulus;
  };

  void ScreenWait( const QString& ac_sScreen, const int ac_nTimeOut,
    const tState::mt_eState ac_eStateBefore, const tState::mt_eState ac_eStateAfter );

  bool isExperimentRunning() const;

  tState m_state;     // current state of experiment


  gui::ApexMainWindow* const m_Wnd;
  ApexControlThread* const mc_pControlThread;
  StatusReporter& m_error;
};

}

#endif
