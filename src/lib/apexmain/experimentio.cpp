#include "experimentio.h"

#include "gui/mainwindow.h"
#include "runner/experimentrundelegate.h"
#include "screen/apexscreen.h"
#include "stimulus/stimulusoutput.h"

#include <QProgressBar>
#include <QTimer>
#include <QThread>
#include <QDebug>

#define NOT_IMPL qFatal("%s: not implemented yet", Q_FUNC_INFO)

namespace apex
{

//class ExperimentIoPrivate ***************************************************

struct ExperimentIoPrivate
{
    ExperimentIoPrivate();

    gui::ApexMainWindow* gui;

    QString currentAnswer;
    gui::ScreenRunDelegate* currentScreen;
};

ExperimentIoPrivate::ExperimentIoPrivate() : gui(0), currentScreen(0)
{
}

//class TimerDescription ******************************************************

TimerDescription::TimerDescription() : msec(-1), startObject(0), startSignal(0),
                                                 endObject(0),   endMember(0)
{
}

//class ExperimentIo **********************************************************

ExperimentIo::ExperimentIo() : d(new ExperimentIoPrivate())
{
}

ExperimentIo::~ExperimentIo()
{
    delete d;
}

void ExperimentIo::setGui(gui::ApexMainWindow* gui)
{
    if (d->gui == gui)
        return;

    if (d->gui != 0)
    {
        d->gui->disconnect(this);
        disconnect(d->gui);
    }

    d->gui = gui;

    connect(gui, SIGNAL(Answered(const ScreenResult*)),
            this, SIGNAL(responseGiven(const ScreenResult*)));
    connect(gui, SIGNAL(startClicked()), this, SIGNAL(startRequest()));
    connect(gui, SIGNAL(stopClicked()), this, SIGNAL(stopRequest()));
    connect(gui, SIGNAL(pauseClicked()), this, SIGNAL(pauseRequest()));
    connect(gui, SIGNAL(skipClicked()), this, SIGNAL(skipRequest()));
    connect(gui, SIGNAL(repeatTrial()), this, SIGNAL(repeatTrialRequest()));
    connect(this, SIGNAL(feedbackDone()), this, SLOT(endFeedback()));
}

void ExperimentIo::showScreen(const QString& screen)
{
    d->currentScreen = d->gui->setScreen(screen);
    d->gui->setAnswer(d->currentAnswer);
    emit screenShown();
}

/*void ExperimentIo::updateScreen(const QString &stimulus)
{
    Q_ASSERT(! d->currentScreen);
    d->currentScreen->newStimulus();
}*/

void ExperimentIo::playStimulus(const QString& stimulus, double silenceBefore)
{
    emit newStimulus(stimulus, silenceBefore);
}

void ExperimentIo::onStimulusPlayed()
{
    qCDebug(APEX_RS) << "stimulus played";
    emit stimulusPlayed();
}

void ExperimentIo::showFeedback(rundelegates::ScreenElementRunDelegate::FeedbackMode mode,
                                const QString& screenElement, int msec)
{
    d->gui->feedback(mode, screenElement);

    if (msec >= 0)
    {
        TimerDescription timer;
        timer.msec = msec;
        timer.endObject = this;
        timer.endMember = SIGNAL(feedbackDone());
        setTimer(timer);
    }
}

void ExperimentIo::endFeedback()
{
    d->gui->EndFeedBack();
}

void ExperimentIo::setProgress(double percentage)
{
    d->gui->setProgress(percentage);
}

void apex::ExperimentIo::setAnswer(const QString& answer)
{
    if (answer != d->currentAnswer)
    {
        d->currentAnswer = answer;
        d->gui->setAnswer(answer);
    }
}

void ExperimentIo::start()
{
    d->gui->ExperimentLoaded();
    setAllEnabled(false);
}

void ExperimentIo::finish()
{
    emit finished();
}

void ExperimentIo::disableStart()
{
    setStartEnabled(false);
}

void ExperimentIo::enableStartStop()
{
    setStartEnabled(true);
    setStopEnabled(true);
}

void ExperimentIo::enableResponseStopPause()
{
    setResponseEnabled(true);
    setStopEnabled(true);
    setPauseEnabled(true);
}

void ExperimentIo::disableResponseStopPause()
{
    setResponseEnabled(false);
    setStopEnabled(false);
    setPauseEnabled(false);
}

void ExperimentIo::enableSelectSoundcard()
{
    d->gui->EnableSelectSoundcard(true);
}

void ExperimentIo::disableSelectSoundcard()
{
     d->gui->EnableSelectSoundcard(false);
}

bool ExperimentIo::startEnabled() const
{
    return d->gui->startEnabled();
}

bool ExperimentIo::pauseEnabled() const
{
    return d->gui->pauseEnabled();
}

bool ExperimentIo::stopEnabled() const
{
    return d->gui->stopEnabled();
}

bool ExperimentIo::skipEnabled() const
{
    return d->gui->skipEnabled();
}

bool ExperimentIo::repeatEnabled() const
{
    return d->gui->repeatEnabled();
}

bool ExperimentIo::responseEnabled() const
{
    return d->gui->screenEnabled();
}

bool ExperimentIo::allEnabled() const
{
    return startEnabled() && pauseEnabled() && stopEnabled() &&
           skipEnabled() && repeatEnabled() && responseEnabled();
}

bool ExperimentIo::isPaused() const
{
    return d->gui->isPaused();
}

int ExperimentIo::stimulusTimeout() const
{
    NOT_IMPL;
    return 0;
}

int ExperimentIo::interTrialTimeout() const
{
    NOT_IMPL;
    return 0;
}

void ExperimentIo::setStartEnabled(bool enable)
{
    d->gui->EnableStart(enable);
}

void ExperimentIo::setPauseEnabled(bool enable)
{
    d->gui->EnablePause(enable);
}

void ExperimentIo::setStopEnabled(bool enable)
{
    d->gui->EnableStop(enable);
}

void ExperimentIo::setSkipEnabled(bool enable)
{
    d->gui->EnableSkip(enable);
}

void ExperimentIo::setRepeatEnabled(bool enable)
{
    d->gui->EnableRepeat(enable);
}

void ExperimentIo::setResponseEnabled(bool enable)
{
    qCDebug(APEX_RS) << QString("ExperimentIo::setResponseEnabled(%1)").arg(enable?"true":"false");
    d->gui->EnableScreen(enable);
}

void ExperimentIo::setAllEnabled(bool enable)
{
    setStartEnabled(enable);
    setPauseEnabled(enable);
    setStopEnabled(enable);
    setSkipEnabled(enable);
    setRepeatEnabled(enable);
    setResponseEnabled(enable);
}

void ExperimentIo::setPaused(bool paused)
{
    d->gui->setPaused(paused);
}

void ExperimentIo::setStimulusTimeout(int timeout)
{
    Q_UNUSED(timeout);
    NOT_IMPL;
}

void ExperimentIo::setInterTrialTimeout(int timeout)
{
    Q_UNUSED(timeout);
    NOT_IMPL;
}

void ExperimentIo::setTimer(TimerDescription description)
{
    if (description.endObject == 0 || description.endMember == 0)
    {
        //no slot to call at end of timer so just call our own timeout() signal
        description.endObject = this;
        description.endMember = SIGNAL(timeout());
    }

    if (description.msec > 0)
    {
        QTimer* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(description.msec);
        connect(timer, SIGNAL(timeout()), description.endObject, description.endMember);
        connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));

        if (description.startObject != 0 && description.startSignal != 0)
            connect(description.startObject, description.startSignal, timer,  SLOT(start()));
        else
            timer->start();
    }
    else //timeout <= 0 so just call endMember
        QTimer::singleShot(0, description.endObject, description.endMember);
}

void ExperimentIo::setTimer(uint msec)
{
    TimerDescription description;
    description.msec = msec;
    setTimer(description);
}

void ExperimentIo::killTimers()
{
    qDeleteAll(allTimers());
}

void ExperimentIo::finishTimers()
{
    Q_FOREACH (QTimer* timer, allTimers())
        timer->setInterval(0);
}

QList<QTimer*> ExperimentIo::allTimers()
{
    return findChildren<QTimer*>();
}

void ExperimentIo::setTimeout(int timeout, QProgressBar* bar)
{
    if (timeout == 0)
    {
        //timeout done
        bar->setEnabled(false);
    }
    else if (bar->value() == 0)
    {
        //timeout started
        bar->setEnabled(true);
        bar->setMaximum(timeout);
    }

    bar->setValue(timeout);
}

}//ns apex
