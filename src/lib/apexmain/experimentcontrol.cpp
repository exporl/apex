#include "apexdata/experimentdata.h"

#include "apexdata/parameters/generalparameters.h"

#include "apexdata/procedure/procedureinterface.h"
#include "apexdata/procedure/trial.h"

#include "apexdata/result/resultparameters.h"

#include "apexdata/screen/screensdata.h"

#include "apexdata/stimulus/stimulusparameters.h"

#include "apextools/apexpaths.h"
#include "apextools/exceptions.h"

#include "device/controldevice.h"
#include "device/plugincontroller.h"

#include "procedure/procedureapiimplementation.h"

#include "randomgenerator/randomgenerators.h"

#include "resultsink/apexresultsink.h"
#include "resultsink/resultviewer.h"
#include "resultsink/rtresultsink.h"

#include "runner/experimentrundelegate.h"

#include "stimulus/stimulusoutput.h"

#include "timer/apextimer.h"

#include "trial/trialstarttime.h"

#include "wavstimulus/wavdevice.h"
#include "wavstimulus/wavdeviceio.h"

#include "apexcontrol.h"
#include "experimentcontrol.h"
#include "experimentio.h"

#include <QDebug>
#include <QFinalState>
#include <QHistoryState>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QScopedPointer>
#include <QSignalTransition>
#include <QState>
#include <QStateMachine>
#include <QTimer>

#ifdef Q_OS_ANDROID
#include "apexandroidnative.h"
#endif

//#define PRINT_EXPERIMENT_STATES
static const char *ERROR_SOURCE = "ExperimentControl";

namespace apex
{

class ExperimentControlPrivate : public QObject
{
    Q_OBJECT

    friend class ExperimentControl; // FIXME remove

public:
    ExperimentControlPrivate(ExperimentControl *qq,
                             ExperimentControl::Flags flags);

    ~ExperimentControlPrivate();

    void loadExperiment(ExperimentRunDelegate *runDelegate);
    void start();

    QStateMachine machine;

public Q_SLOTS:

    void waitForStart();
    void showIntro();
    void startNextTrial();
    void restartTrial();
    void afterTrial();
    void showNextScreen();
    void playNextStimulus();
    void waitAfterStimulus();
    void checkResponseNeededAfterStimuli();
    void checkResponseNeededDuringStimuli();
    void checkPauseNeeded();
    void postponePause();
    void showInterTrialScreen();
    void showFeedback();
    void showOutro();
    void showResult();
    void afterExperiment();
    void startResponseTimer();
    void stopResponseTimer();

    void showMessageBox(const QString title, const QString text) const;

    void debug()
    {
        qCDebug(APEX_RS) << "debug slot called";
    }

signals:

    void introDone();
    void trialsDone();
    void screensDone();
    void stimuliDone();
    void interTrialScreenDone();
    void stimulusTimeoutDone();
    void needResponse();
    void dontNeedResponse();
    void responseGiven();
    void needPause();
    void dontNeedPause();
    void outroDone();
    void errorMessage(const QString &source, const QString &message);
    void doSignalExperimentFinished();
    void resultsShowed();
    /**
     * @brief parametersLoaded sends out the stimulus' params map
     *        after it's prepared in playNextStimulus
     * @param params QvariantMap with <QString,QVariant>
     */
    void parametersLoaded(const QVariantMap &params);
    void trialStarted();

private:
    ExperimentControl *q;

    ExperimentControl::Flags flags;
    ExperimentRunDelegate *rd;
    ExperimentIo *io;
    ProcedureInterface *procedure;
    ProcedureApiImplementation *api;

    data::Trial currentTrial;
    int currentScreen;
    int currentStimulus;

    ResultHighlight lastResponse;

    bool pauseAfterTrial;
    bool isFirstTrial;
    bool stopped;

    void setupStates();
    void setupRunningState(QState *running);
    void setupRunningTrialState(QState *runningTrial);
    void setupRunningScreenState(QState *runningScreen);
    void setupResponseState(QState *response);
    void setupStimuliState(QState *stimuli);
    void setupPlayingStimuliState(QState *playingStimuli);

    void animateTimeout(int msec, const char *property, const char *signal = 0);
    void stopStimulus();

    QFinalState *runningDone;
    QState *crashed;

private Q_SLOTS:

    void processResponse(const ScreenResult *result);
    void handleStopped();
// void setPauseAfterTrial();

#ifdef PRINT_EXPERIMENT_STATES
    void printEnterState();
    void printLeaveState();
#endif
};

ExperimentControlPrivate::ExperimentControlPrivate(ExperimentControl *qq,
                                                   ExperimentControl::Flags f)
    : q(qq),
      flags(f),
      io(new ExperimentIo()),
      procedure(0),
      api(0),
      pauseAfterTrial(false),
      isFirstTrial(true),
      stopped(false)
{
    connect(io, SIGNAL(responseGiven(const ScreenResult *)), this,
            SLOT(processResponse(const ScreenResult *)));
    connect(io, SIGNAL(skipRequest()), io, SLOT(finishTimers()));
    connect(this, SIGNAL(errorMessage(QString, QString)), qq,
            SIGNAL(errorMessage(QString, QString)));

#ifdef PRINT_EXPERIMENT_STATES
    Q_FOREACH (QAbstractState *state, machine.findChildren<QAbstractState *>())
        connect(state, SIGNAL(entered()), this, SLOT(printEnterState()));

    Q_FOREACH (QAbstractState *state, machine.findChildren<QAbstractState *>())
        connect(state, SIGNAL(exited()), this, SLOT(printLeaveState()));
#endif
}

ExperimentControlPrivate::~ExperimentControlPrivate()
{
}

void ExperimentControlPrivate::setupStates()
{
    // while waiting for the user to start the procedure
    QState *waitForStart = new QState(&machine);
    // the main running state
    QState *running = new QState(&machine);
    // while showing the result
    QState *showingResult = new QState(&machine);
    // while pausing
    QState *pause = new QState(&machine);
    // history state for restoring the state before paused
    QHistoryState *resume =
        new QHistoryState(QHistoryState::DeepHistory, running);
    // the final state of the procedure
    QFinalState *done = new QFinalState(&machine);

    waitForStart->setObjectName("WaitForStart");
    running->setObjectName("Running");
    showingResult->setObjectName("ShowingResult");
    pause->setObjectName("Pause");
    resume->setObjectName("Resume");
    done->setObjectName("Done");

    machine.setGlobalRestorePolicy(QStateMachine::RestoreProperties);
    machine.setInitialState(waitForStart);

    connect(waitForStart, SIGNAL(entered()), io, SLOT(enableStartStop()));
    connect(waitForStart, SIGNAL(entered()), io,
            SLOT(disableSelectSoundcard()));
    connect(running, SIGNAL(entered()), io, SLOT(disableStart()));

    if ((flags & ExperimentControl::AutoStart) ||
        rd->GetData().generalParameters()->GetAutoStart())
        waitForStart->addTransition(running);
    else {
        waitForStart->addTransition(io, SIGNAL(startRequest()), running);
        waitForStart->addTransition(io, SIGNAL(stopRequest()), done);
    }

    connect(waitForStart, SIGNAL(entered()), this, SLOT(waitForStart()));

    // [Tom] Removed property setting: leads to unpredictable results when
    // leaving states
    //    running->assignProperty(io, "stopEnabled", true);

    running->addTransition(running, SIGNAL(finished()), showingResult);
    running->addTransition(this, SIGNAL(needPause()), pause);
    setupRunningState(running);

    if (!(flags & ExperimentControl::NoResults))
        connect(showingResult, SIGNAL(entered()), this, SLOT(showResult()));

    if (rd->GetData().resultParameters()->showResultsAfter())
        showingResult->addTransition(this, SIGNAL(resultsShowed()), done);
    else
        showingResult->addTransition(done);

    connect(done, SIGNAL(entered()), this, SLOT(afterExperiment()));

    pause->assignProperty(io, "pauseEnabled", true);
    pause->assignProperty(io, "paused", true);
    pause->addTransition(io, SIGNAL(pauseRequest()), resume);
    connect(io, SIGNAL(pauseRequest()), this, SLOT(debug()));
}

void ExperimentControlPrivate::setupRunningState(QState *running)
{
    // while showing the intro
    QState *intro = new QState(running);
    // the main state while running a trial
    QState *runningTrial = new QState(running);
    // state for checking whether the user wanted to pause after the trial
    QState *checkingPause = new QState(running);
    // while showing the inter trial screen
    QState *interTrial = new QState(running);
    // while showing the outro
    QState *outro = new QState(running);
    // if an error is occured while running the trial
    QState *crashed = new QState(running);
    // the final state of the main running state
    QFinalState *runningDone = new QFinalState(running);

    intro->setObjectName("Intro");
    runningTrial->setObjectName("RunningTrial");
    checkingPause->setObjectName("CheckingPause");
    interTrial->setObjectName("InterTrial");
    outro->setObjectName("Outro");
    crashed->setObjectName("Crashed");
    runningDone->setObjectName("RunningDone");

    running->setInitialState(intro);

    intro->assignProperty(io, "skipEnabled", true);
    intro->addTransition(this, SIGNAL(introDone()), runningTrial);
    connect(intro, SIGNAL(entered()), this, SLOT(showIntro()));

    runningTrial->addTransition(runningTrial, SIGNAL(finished()),
                                checkingPause);
    runningTrial->addTransition(this, SIGNAL(trialsDone()), outro);
    setupRunningTrialState(runningTrial);

    checkingPause->addTransition(this, SIGNAL(dontNeedPause()), interTrial);
    connect(checkingPause, SIGNAL(entered()), this, SLOT(checkPauseNeeded()));

    interTrial->assignProperty(io, "skipEnabled", true);
    interTrial->addTransition(this, SIGNAL(interTrialScreenDone()),
                              runningTrial);
    connect(interTrial, SIGNAL(entered()), this, SLOT(showInterTrialScreen()));

    outro->assignProperty(io, "skipEnabled", true);
    outro->addTransition(this, SIGNAL(outroDone()), runningDone);
    connect(outro, SIGNAL(entered()), this, SLOT(showOutro()));

    this->runningDone = runningDone;
    this->crashed = crashed;
}

void ExperimentControlPrivate::setupRunningTrialState(QState *runningTrial)
{
    // while starting a trial
    QState *startingTrial = new QState(runningTrial);
    // while restarting a trial
    QState *restartingTrial = new QState(runningTrial);
    // while showing a screen
    QState *showingScreen = new QState(runningTrial);
    // the main state while running a screen (stimuli, response, feedback)
    QState *runningScreen = new QState(runningTrial);
    // the final state of a trial
    QFinalState *trialDone = new QFinalState(runningTrial);
    QFinalState *trialStopped = new QFinalState(runningTrial);

    startingTrial->setObjectName("StartingTrial");
    restartingTrial->setObjectName("RestartingTrial");
    showingScreen->setObjectName("ShowingScreen");
    runningScreen->setObjectName("RunningScreen");
    trialDone->setObjectName("TrialDone");
    trialStopped->setObjectName("TrialStopped");

    runningTrial->setInitialState(startingTrial);
    runningTrial->addTransition(io, SIGNAL(repeatTrialRequest()),
                                restartingTrial);

    // [Tom] changed, because when stop is clicked, no results should be
    // collected for the last trial
    // connect(runningTrial, SIGNAL(exited()), this, SLOT(afterTrial()));

    runningTrial->addTransition(io, SIGNAL(stopRequest()), trialStopped);
    connect(trialStopped, SIGNAL(entered()), this, SLOT(handleStopped()));
    connect(trialDone, SIGNAL(exited()), this, SLOT(afterTrial()));

    startingTrial->addTransition(showingScreen);
    connect(startingTrial, SIGNAL(entered()), this, SLOT(startNextTrial()));

    restartingTrial->addTransition(showingScreen);
    connect(restartingTrial, SIGNAL(entered()), this, SLOT(restartTrial()));

    showingScreen->addTransition(io, SIGNAL(screenShown()), runningScreen);
    showingScreen->addTransition(this, SIGNAL(screensDone()), trialDone);
    connect(showingScreen, SIGNAL(entered()), this, SLOT(showNextScreen()));

    runningScreen->addTransition(runningScreen, SIGNAL(finished()),
                                 showingScreen);
    setupRunningScreenState(runningScreen);
}

void ExperimentControlPrivate::setupRunningScreenState(QState *runningScreen)
{
    // helper state to contain two parallel states
    // TODO [job] change this setup once Qt bugreport gets fixed
    QState *helper = new QState(QState::ParallelStates, runningScreen);
    // the state in which response is given (or not)
    QState *response = new QState(helper);
    // the state in which stimuli are handled
    QState *stimuli = new QState(helper);
    // the final state of the running screen state
    QFinalState *screenDone = new QFinalState(runningScreen);

    helper->setObjectName("Helper");
    response->setObjectName("Response");
    stimuli->setObjectName("Stimuli");
    screenDone->setObjectName("ScreenDone");

    runningScreen->setInitialState(helper);

    // response->addTransition(response, SIGNAL(finished()), screenDone);
    response->addTransition(helper, SIGNAL(finished()), screenDone);
    setupResponseState(response);
    connect(response, SIGNAL(finished()), this, SLOT(debug()));
    connect(runningScreen, SIGNAL(entered()), io, SLOT(disableRepeat()));

    setupStimuliState(stimuli);
}

void ExperimentControlPrivate::setupResponseState(QState *response)
{
    // checking whether we need response during stimuli
    QState *checkingResponseNeeded = new QState(response);
    // while waiting for user response
    QState *waitingForResponse = new QState(response);
    // if the user clicked pause while waiting for response
    QState *postponingPause = new QState(response);
    // while showing feedback
    QState *showingFeedback = new QState(response);
    // final state of response state
    QFinalState *responseDone = new QFinalState(response);

    checkingResponseNeeded->setObjectName("CheckingResponseNeeded");
    waitingForResponse->setObjectName("WaitingForResponse");
    postponingPause->setObjectName("PostponingPause");
    showingFeedback->setObjectName("ShowingFeedback");
    responseDone->setObjectName("ResponseDone");

    response->setInitialState(checkingResponseNeeded);

    connect(waitingForResponse, SIGNAL(entered()), this,
            SLOT(startResponseTimer()));
    connect(this, SIGNAL(responseGiven()), this, SLOT(stopResponseTimer()));

    checkingResponseNeeded->addTransition(this, SIGNAL(needResponse()),
                                          waitingForResponse);
    checkingResponseNeeded->addTransition(this, SIGNAL(dontNeedResponse()),
                                          responseDone);
    connect(checkingResponseNeeded, SIGNAL(entered()), this,
            SLOT(checkResponseNeededDuringStimuli()));

    // [Tom] Removed property setting: property is unset when leaving state, but
    // this leads
    // to weird/unpredictable results when using parallel states etc
    // waitingForResponse->assignProperty(io, "responseEnabled", true);
    // waitingForResponse->assignProperty(io, "pauseEnabled", true);
    // waitingForResponse->assignProperty(io, "repeatEnabled", true);
    connect(waitingForResponse, SIGNAL(entered()), io,
            SLOT(enableResponseStopPause()));
    connect(waitingForResponse, SIGNAL(entered()), io, SLOT(enableRepeat()));
    connect(showingFeedback, SIGNAL(entered()), io, SLOT(disableStop()));

    waitingForResponse->addTransition(io, SIGNAL(pauseRequest()),
                                      postponingPause);
    waitingForResponse->addTransition(this, SIGNAL(responseGiven()),
                                      showingFeedback);

    postponingPause->addTransition(waitingForResponse);
    connect(postponingPause, SIGNAL(entered()), this, SLOT(postponePause()));

    showingFeedback->addTransition(io, SIGNAL(feedbackDone()), responseDone);
    connect(showingFeedback, SIGNAL(entered()), this, SLOT(showFeedback()));
}

void ExperimentControlPrivate::setupStimuliState(QState *stimuli)
{
    // the main state while playing a group of stimuli
    QState *playingStimuli = new QState(stimuli);
    // the state after all stimuli have been played. check whether response is
    // needed
    // QState* afterStimuli = new QState(stimuli);
    QFinalState *afterStimuli = new QFinalState(stimuli);

    playingStimuli->setObjectName("PlayingStimuli");
    afterStimuli->setObjectName("AfterStimuli");

    stimuli->setInitialState(playingStimuli);

    playingStimuli->addTransition(playingStimuli, SIGNAL(finished()),
                                  afterStimuli);
    setupPlayingStimuliState(playingStimuli);

    connect(afterStimuli, SIGNAL(entered()), this,
            SLOT(checkResponseNeededAfterStimuli()));
}

void ExperimentControlPrivate::setupPlayingStimuliState(QState *playingStimuli)
{
    // while playing a stimulus
    QState *playingStimulus = new QState(playingStimuli);
    // while waiting after a stimulus
    QState *waitAfterStimulus = new QState(playingStimuli);
    // the final state of the playing stimuli state
    QFinalState *stimuliDone = new QFinalState(playingStimuli);

    playingStimulus->setObjectName("PlayingStimulus");
    waitAfterStimulus->setObjectName("WaitAfterStimulus");
    stimuliDone->setObjectName("StimuliDone");

    playingStimuli->setInitialState(playingStimulus);

    playingStimulus->addTransition(io, SIGNAL(stimulusPlayed()),
                                   waitAfterStimulus);
    playingStimulus->addTransition(this, SIGNAL(stimuliDone()), stimuliDone);
    connect(playingStimulus, SIGNAL(entered()), this, SLOT(playNextStimulus()));

    waitAfterStimulus->assignProperty(io, "skipEnabled", true);
    waitAfterStimulus->addTransition(this, SIGNAL(stimulusTimeoutDone()),
                                     playingStimulus);
    connect(waitAfterStimulus, SIGNAL(entered()), this,
            SLOT(waitAfterStimulus()));
}

void ExperimentControlPrivate::loadExperiment(
    ExperimentRunDelegate *runDelegate)
{
    rd = runDelegate;
    QString error;

    // keep scoped pointer of all data so they will all be freed
    // when something goes wrong here
    QScopedPointer<ProcedureApiImplementation> api;
    QScopedPointer<ProcedureInterface> procedure;

    currentScreen = -1;
    currentStimulus = -1;
    isFirstTrial = true;

    try {
        api.reset(new ProcedureApiImplementation(
            *rd, flags & ExperimentControl::Deterministic));
        connect(api.data(), SIGNAL(showMessageBox(QString, QString)), this,
                SLOT(showMessageBox(QString, QString)));
        connect(api.data(), SIGNAL(stoppedWithError(QString, QString)), q,
                SIGNAL(errorMessage(QString, QString)));

        if (flags & ExperimentControl::Deterministic)
            rd->modRandomGenerators()->doDeterministicGeneration();

        procedure.reset(
            rd->makeProcedure(api.data(), rd->GetData().procedureData()));

        if (procedure.isNull()) {
            QString procedureType = rd->GetData().procedureData()->name();
            error = tr("Cannot find a suitable procedure creator plugin for "
                       "type %1")
                        .arg(procedureType);
        }

        //  OUTPUTDEVICE X CONTROLDEVICE SYNCHRONIZATION
        stimulus::tDeviceMap::Iterator devIt;
        stimulus::tDeviceMap deviceMap = rd->GetDevices();

        device::tControllerMap::Iterator conIt;
        device::tControllerMap controlMap = rd->GetControllers();

        for (devIt = deviceMap.begin(); devIt != deviceMap.end(); ++devIt) {
            for (conIt = controlMap.begin(); conIt != controlMap.end();
                 ++conIt) {
                qCDebug(APEX_RS) << "Connecting outputdevice " << devIt.key()
                                 << " with controldevice" << conIt.key();

                connect(devIt.value(), SIGNAL(stimulusStarted()), conIt.value(),
                        SLOT(syncControlDeviceOutput()));
            }
        }
    } catch (const std::exception &e) {
        error = e.what();
    }

    if (!error.isEmpty()) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(ERROR_SOURCE, error)));
        machine.stop();
    } else {
        this->procedure = procedure.take();
        this->api = api.take();

        setupStates();

        crashed->addTransition(
            this->api, SIGNAL(stoppedWithError(QString, QString)), runningDone);
    }
}

void ExperimentControlPrivate::start()
{
    io->start();
    machine.start();
}

void ExperimentControlPrivate::waitForStart()
{
    const data::ScreensData *data = rd->GetData().screensData();

    if (data->hasIntroScreen())
        io->showScreen(data->introScreen());
    else
        io->showScreen(procedure->firstScreen());

    if (rd->modRTResultSink()) {
        rd->modRTResultSink()->show();
        //  Let rv know when trial starts
        connect(this, SIGNAL(trialStarted()), rd->modRTResultSink(),
                SLOT(trialStarted()));
        //  Send params to resultsviewer when they are loaded
        connect(this, SIGNAL(parametersLoaded(const QVariantMap &)),
                rd->modRTResultSink(),
                SLOT(newStimulusParameters(const QVariantMap &)));
        //  let rv know when device does playStimulus
        stimulus::tDeviceMap deviceMap = rd->GetDevices();
        for (auto devIt = deviceMap.begin(); devIt != deviceMap.end();
             ++devIt) {
            connect(devIt.value(), SIGNAL(stimulusStarted()),
                    rd->modRTResultSink(), SLOT(stimulusStarted()));
        }
    }
}

void ExperimentControlPrivate::showIntro()
{
    const data::ScreensData *data = rd->GetData().screensData();

    if (data->hasIntroScreen()) {
        // make sure the timer starts once the screen is shown
        TimerDescription timer;
        timer.msec = data->introLength();
        timer.startObject = io;
        timer.startSignal = SIGNAL(screenShown());
        timer.endObject = this;
        timer.endMember = SIGNAL(introDone());
        io->setTimer(timer);

        // and show the screen
        io->showScreen(data->introScreen());
    } else
        QTimer::singleShot(0, this, SIGNAL(introDone()));
}

void ExperimentControlPrivate::startNextTrial()
{
    rd->trialStartTime()->setDateTime();
    if (isFirstTrial)
        rd->modResultSink()->setExperimentStartTime(
            QDateTime::currentDateTime());

    io->setProgress(procedure->progress());
    try {
        Q_EMIT trialStarted();
        currentTrial = procedure->setupNextTrial();
#ifdef Q_OS_ANDROID
        android::ApexAndroidBridge::signalTrialStarted();
#endif
    } catch (const std::exception &e) {
        Q_EMIT errorMessage(ERROR_SOURCE, e.what());
        Q_EMIT trialsDone();
    }
    io->setAnswer(currentTrial.answer());

    if (!currentTrial.isValid())
        Q_EMIT trialsDone();
}

void ExperimentControlPrivate::restartTrial()
{
    // qCDebug(APEX_RS) << "restarting trial";
    currentScreen = -1;
    currentStimulus = -1;
}

void ExperimentControlPrivate::afterTrial()
{
    if (currentTrial.isValid()) {
        QString resultXml = procedure->resultXml();

        // FIXME check for a better way to collect results
        if (!resultXml.isEmpty())
            rd->modResultSink()->collectResults(currentTrial.id(), resultXml);
    }

    isFirstTrial = false;
}

void ExperimentControlPrivate::showNextScreen()
{
    // this happens after the last trial: we enter the trial state for a last
    // time
    // and, although the trialsDone signal has been emitted, we still seem to
    // enter
    // its initial state (showing screen) before exiting it...
    if (!currentTrial.isValid())
        return;

    if (++currentScreen >= currentTrial.screenCount()) {
        currentScreen = -1;
        if (!stopped) {
            Q_EMIT screensDone();
        }
    } else
        io->showScreen(currentTrial.screen(currentScreen));
}

void ExperimentControlPrivate::playNextStimulus()
{
    Q_ASSERT(currentTrial.isValid() &&
             currentScreen < currentTrial.screenCount());

    if (++currentStimulus >= currentTrial.stimulusCount(currentScreen)) {
        currentStimulus = -1;
        Q_EMIT stimuliDone();
    } else {
        QString stimulus =
            currentTrial.stimulus(currentScreen, currentStimulus);

        // set all parameters for the stimulus
        ParameterManager *paramMgr = rd->GetParameterManager();

        // set stimulus variable parameters
        Stimulus *stim = rd->GetStimulus(stimulus);
        const QVariantMap &varparams(stim->GetVarParameters()->map());
        for (QVariantMap::const_iterator it = varparams.begin();
             it != varparams.end(); ++it) {
            paramMgr->setParameter(it.key(), it.value(), true);
        }
        QVariantMap allparams(varparams);

        const QVariantMap &fixparams(stim->GetFixParameters()->map());
        for (QVariantMap::const_iterator it = fixparams.begin();
             it != fixparams.end(); ++it) {
            paramMgr->setParameter(it.key(), it.value(), true);
            allparams.insert(it.key(), it.value());
        }

        // set parameters obtained from procedure
        QMap<QString, QVariant> parameters =
            currentTrial.parameters(currentScreen, currentStimulus);

        QMap<QString, QVariant>::const_iterator it;
        for (it = parameters.begin(); it != parameters.end(); ++it) {
            paramMgr->setParameter(it.key(), it.value(), true);
            allparams.insert(it.key(), it.value());
        }

        if (allparams.size() > 0) {
            Q_EMIT parametersLoaded(allparams);
        }

        // run random parameter generators
        rd->modRandomGenerators()->ApplyGenerators();

        // Set labels etc. on screen that depend on parameter settings
        // io->updateScreen();

        // play the stimulus

        double silence = 0.0;

        if (isFirstTrial) {
            silence = rd->GetData().procedureData()->timeBeforeFirstStimulus();
        }

        qCDebug(
            APEX_RS, "Calling playNextStimulus at time: %s",
            qPrintable(QString::number(QDateTime::currentMSecsSinceEpoch())));
        io->playStimulus(stimulus, silence);

        // highlight current screen element if needed
        if (rd->GetData().screensData()->hasShowCurrentEnabled()) {
            // no timeout on feedback; we stop is when stimulus is done
            io->showFeedback(
                rundelegates::ScreenElementRunDelegate::HighlightFeedback,
                currentTrial.highlightedElement(currentScreen, currentStimulus),
                -1);
        }
    }
}

void ExperimentControlPrivate::stopStimulus()
{
    rd->modOutput()->UnLoadStimulus();
}

void ExperimentControlPrivate::waitAfterStimulus()
{
    // stop any feedback
    io->endFeedback();

    if (!currentTrial.doWaitAfter(currentScreen, currentStimulus)) {
        Q_EMIT stimulusTimeoutDone();
        return;
    }

    // start timer for wait after stimulus
    TimerDescription timer;
    timer.msec = 1000 * currentTrial.waitAfter(currentScreen, currentStimulus);
    timer.endObject = this;
    timer.endMember = SIGNAL(stimulusTimeoutDone());
    io->setTimer(timer);
}

void ExperimentControlPrivate::checkResponseNeededAfterStimuli()
{
    if (rd->GetData().procedureData()->inputDuringStimulus())
        return;

    if (currentTrial.acceptResponse(currentScreen) &&
        !rd->GetData().procedureData()->inputDuringStimulus())
        Q_EMIT needResponse();
    else
        Q_EMIT dontNeedResponse();
}

void ExperimentControlPrivate::checkResponseNeededDuringStimuli()
{
    if (rd->GetData().procedureData()->inputDuringStimulus() &&
        currentTrial.acceptResponse(currentScreen)) {
        Q_EMIT needResponse();
    }
}

void ExperimentControlPrivate::checkPauseNeeded()
{
    if (pauseAfterTrial) {
        pauseAfterTrial = false;
        Q_EMIT needPause();
    } else
        Q_EMIT dontNeedPause();
}

void ExperimentControlPrivate::postponePause()
{
    pauseAfterTrial = true;
}

void ExperimentControlPrivate::showInterTrialScreen()
{
    const data::ScreensData *data = rd->GetData().screensData();

    if (data->hasInterTrialScreen()) {
        // FIXME inter trial screen is shown after last trial: fix sometime
        TimerDescription timer;
        timer.msec = data->interTrialLength();
        timer.startObject = io;
        timer.startSignal = SIGNAL(screenShown());
        timer.endObject = this;
        timer.endMember = SIGNAL(interTrialScreenDone());
        io->setTimer(timer);

        io->showScreen(data->interTrialScreen());
    } else
        QTimer::singleShot(0, this, SIGNAL(interTrialScreenDone()));
}

void ExperimentControlPrivate::showFeedback()
{
    uint msec = rd->GetData().screensData()->feedbackLength();
    rundelegates::ScreenElementRunDelegate::FeedbackMode mode;

    if ((lastResponse.overrideCorrectFalse == false &&
         rd->GetData().screensData()->hasFeedbackEnabled()) ||
        (lastResponse.overrideCorrectFalse && lastResponse.showCorrectFalse)) {
        if (lastResponse.correct)
            mode = rundelegates::ScreenElementRunDelegate::PositiveFeedback;
        else
            mode = rundelegates::ScreenElementRunDelegate::NegativeFeedback;
    } else
        mode = rundelegates::ScreenElementRunDelegate::NoFeedback;

    io->showFeedback(mode, lastResponse.highlightElement, msec);
}

void ExperimentControlPrivate::showOutro()
{
    const data::ScreensData *data = rd->GetData().screensData();

    if (data->hasOutroScreen()) {
        // make sure the timer starts once the screen is shown
        TimerDescription timer;
        timer.msec = data->outroLength();
        timer.startObject = io;
        timer.startSignal = SIGNAL(screenShown());
        timer.endObject = this;
        timer.endMember = SIGNAL(outroDone());
        io->setTimer(timer);

        // and show the screen
        io->showScreen(data->introScreen());
    } else
        QTimer::singleShot(0, this, SIGNAL(outroDone()));
}

void ExperimentControlPrivate::showResult()
{
    rd->modOutput()->CloseDevices();
    if (rd->modResultSink() != nullptr) {
        rd->modResultSink()->setStopCondition(stopped);
        rd->modResultSink()->setExtraXml(procedure->finalResultXml());
        rd->modResultSink()->setExperimentEndTime(QDateTime::currentDateTime());
        const QString resultfilePath = rd->modResultSink()->saveResultfile();

        if (!resultfilePath.isEmpty()) {
            if (rd->GetData().resultParameters()->showResultsAfter()) {
                ResultViewer *rv = new ResultViewer(resultfilePath);
                if (rd->modRTResultSink())
                    rd->modRTResultSink()->hide();

                connect(rv, SIGNAL(viewClosed()), rv, SLOT(deleteLater()));
                connect(rv, SIGNAL(viewClosed()), this,
                        SIGNAL(resultsShowed()));
                rv->show(
                    rd->GetData().resultParameters()->confirmShowResults());
            }

            this->disconnect(SIGNAL(doSignalExperimentFinished()));
            connect(this, &ExperimentControlPrivate::doSignalExperimentFinished,
                    [this, resultfilePath]() {
                        emit q->experimentClosed(resultfilePath);
                    });
        } else {
            Q_EMIT resultsShowed();
        }
    }
}

void ExperimentControlPrivate::afterExperiment()
{
    io->finish();
    rd->modOutput()->releaseClients();
    rd->modOutput()->StopDevices();

    delete procedure;
    delete api;
    procedure = 0;
    api = 0;

    qCInfo(APEX_RS, "%s",
           qPrintable(QSL("%1: %2").arg(ERROR_SOURCE, tr("Experiment done"))));

    io->enableSelectSoundcard();
}

void ExperimentControlPrivate::startResponseTimer()
{
    rd->modTimer()->start();
}

void ExperimentControlPrivate::stopResponseTimer()
{
    rd->modTimer()->stop();
}

void ExperimentControlPrivate::animateTimeout(int msec, const char *property,
                                              const char *signal)
{
    QPropertyAnimation *anim = new QPropertyAnimation(io, property);
    anim->setDuration(msec);
    anim->setStartValue(msec);
    anim->setEndValue(0);

    if (signal != 0)
        connect(anim, SIGNAL(finished()), this, signal);

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ExperimentControlPrivate::processResponse(const apex::ScreenResult *result)
{
    stopStimulus();
    lastResponse = procedure->processResult(result);

    ParameterManager *pm = this->rd->GetParameterManager();
    for (stimulusParamsT::const_iterator it =
             result->GetStimulusParameters().begin();
         it != result->GetStimulusParameters().end(); ++it) {
        pm->setParameter(it->first, it->second, true);
    }

    Q_EMIT responseGiven();
}

void ExperimentControlPrivate::handleStopped()
{
    stopped = true;
    Q_EMIT trialsDone();
    stopStimulus();
}

void ExperimentControlPrivate::showMessageBox(const QString title,
                                              const QString text) const
{
    QMessageBox::information(0, title, text);
}

#ifdef PRINT_EXPERIMENT_STATES
void ExperimentControlPrivate::printEnterState()
{
    QObject *state = sender();
    QString stateString = state->objectName();

    while ((state = state->parent()) != &machine)
        stateString = state->objectName() + "::" + stateString;

    qCDebug(APEX_RS) << ERROR_SOURCE << ": entering state" << stateString;
}

void ExperimentControlPrivate::printLeaveState()
{
    QObject *state = sender();
    QString stateString = state->objectName();

    while ((state = state->parent()) != &machine)
        stateString = state->objectName() + "::" + stateString;

    qCDebug(APEX_RS) << ERROR_SOURCE << ": leaving state" << stateString;
}
#endif

ExperimentControl::ExperimentControl(Flags flags)
    : d(new ExperimentControlPrivate(this, flags))
{
    connect(&d->machine, &QStateMachine::finished, d,
            &ExperimentControlPrivate::doSignalExperimentFinished);
    connect(d, &ExperimentControlPrivate::doSignalExperimentFinished,
            [this]() { emit experimentClosed(QString()); });
    connect(&d->machine, SIGNAL(finished()), this, SIGNAL(experimentDone()));
}

ExperimentControl::~ExperimentControl()
{
    delete d;
}

void ExperimentControl::loadExperiment(ExperimentRunDelegate *runDelegate)
{
    d->loadExperiment(runDelegate);
}

void ExperimentControl::start()
{
    if (isRunning())
        qCWarning(APEX_RS, "%s",
                  qPrintable(QSL("%1: %2").arg(
                      ERROR_SOURCE, tr("Experiment already running"))));
    else
        d->start();
}

bool ExperimentControl::isRunning() const
{
    return d->machine.isRunning();
}

ExperimentIo *ExperimentControl::io() const
{
    return d->io;
}

const QStateMachine *ExperimentControl::machine() const
{
    return &d->machine;
}

} // ns apex

#include "experimentcontrol.moc"
