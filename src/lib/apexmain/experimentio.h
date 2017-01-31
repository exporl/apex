#ifndef _EXPORL_SRC_LIB_APEXMAIN_EXPERIMENTIO_H_
#define _EXPORL_SRC_LIB_APEXMAIN_EXPERIMENTIO_H_

#include "screen/screenelementrundelegate.h"

#include <QObject>
#include <QWidget>

class QProgressBar; //TODO remove

namespace apex
{

class  ScreenResult;
class  ExperimentRunDelegate;
struct ExperimentIoPrivate;

namespace gui
{
class ApexMainWindow;
}

namespace stimulus
{
class StimulusOutput;
}

/**
 * @class TimerDescription
 * @author Job Noorman <jobnoorman@gmail.com>
 *
 * @brief Class describing a timer to be controlled by ExperimentIo.
 *
 * This class describes a timer that is started when some signal is emitted
 * and will call some method when finished. Use this class with
 * ExperimentIo::setTimer(TimerDescription).
 *
 * A simple example:
 * @code
 * ExperimentIo* io = ...;
 * TimerDescription timer;
 * timer.msec = 1000;
 * timer.startObject = foo;
 * timer.startSignal = SIGNAL(fooSignal());
 * timer.endObject = bar;
 * timer.endMember = SLOT(barSlot());
 * io->setTimer(timer);
 * @endcode
 * When you run this code, a timer will start after foo's signal fooSignal()
 * has been emitted and will call bar's slot barSlot() after 1 second.
 */
struct TimerDescription
{
    TimerDescription();

    uint msec;               ///The length of the timer.

    QObject*    startObject; ///The object that will signal the start of the timer.
    const char* startSignal; ///The signal that will start the timer.
    QObject*    endObject;   ///The object on which a member will be called on timeout.
    const char* endMember;   ///The member (signal/slot) that will be called on timeout.
};

/**
 * @class ExperimentIo
 * @author Job Noorman <jobnoorman@gmail.com>
 */
class ExperimentIo : public QObject
{
        Q_OBJECT
        Q_PROPERTY(bool startEnabled READ startEnabled WRITE setStartEnabled)
        Q_PROPERTY(bool pauseEnabled READ pauseEnabled WRITE setPauseEnabled)
        Q_PROPERTY(bool stopEnabled READ stopEnabled WRITE setStopEnabled)
        Q_PROPERTY(bool skipEnabled READ skipEnabled WRITE setSkipEnabled)
        Q_PROPERTY(bool repeatEnabled READ repeatEnabled WRITE setRepeatEnabled)
        Q_PROPERTY(bool responseEnabled READ responseEnabled WRITE setResponseEnabled)
        Q_PROPERTY(bool allEnabled READ allEnabled WRITE setAllEnabled)
        Q_PROPERTY(bool paused READ isPaused WRITE setPaused)
        Q_PROPERTY(int  stimulusTimeout READ stimulusTimeout WRITE setStimulusTimeout)
        Q_PROPERTY(int  interTrialTimeout READ interTrialTimeout WRITE setInterTrialTimeout)

    public:

        ExperimentIo();
        ~ExperimentIo();

        void setGui(gui::ApexMainWindow* gui);

        bool startEnabled() const;
        bool pauseEnabled() const;
        bool stopEnabled() const;
        bool skipEnabled() const;
        bool repeatEnabled() const;
        bool responseEnabled() const;
        bool allEnabled() const;
        bool isPaused() const;
        int  stimulusTimeout() const;
        int  interTrialTimeout() const;

        void setStartEnabled(bool enable);
        void setPauseEnabled(bool enable);
        void setStopEnabled(bool enable);
        void setSkipEnabled(bool enable);
        void setRepeatEnabled(bool enable);
        void setResponseEnabled(bool enable);
        void setAllEnabled(bool enable);
        void setPaused(bool paused);
        void setStimulusTimeout(int timeout);
        void setInterTrialTimeout(int timeout);

    public Q_SLOTS:

        void showScreen(const QString& screen);

        void playStimulus(const QString& stimulus, double silenceBefore = 0.0);
        void onStimulusPlayed();

        void showFeedback(rundelegates::ScreenElementRunDelegate::FeedbackMode mode,
                          const QString& screenElement, int msec);
        void endFeedback();
        void setProgress(double percentage);
        void setAnswer(const QString& answer);

        void start();
        void finish();

        void disableStart();
        void enableStartStop();
        void enableResponseStopPause();
        void disableResponseStopPause();
        void enableSelectSoundcard();
        void disableSelectSoundcard();

        /**
         * @brief Sets a timer as described by the parameter.
         *
         * See the documentation of TimerDescription for more information on
         * the options.
         * If TimerDescription::startObject or TimerDescription::startSignal is
         * not specified, the timer will start immediately.
         * If TimerDescription::endObject or TimerDescription::endMember is not
         * specified, timeout() will be emitted when the timer timeouts.
         */
        void setTimer(TimerDescription description);

        /**
         * The same as calling setTimer(TimerDescription) with only the
         * TimerDescription::msec member set.
         */
        void setTimer(uint msec);

        /**
         * Kills all running timers.
         */
        void killTimers();

        /**
         * Finishes all running timers. This means they are restarted with
         * interval 0 so their timeout() signals will be emitted.
         */
        void finishTimers();

    Q_SIGNALS:

        void newStimulus(const QString& stimulus, double silenceBefore);
        void finished();

        void loadRequest();
        void startRequest();
        void pauseRequest();
        void stopRequest();
        void skipRequest();
        void repeatTrialRequest();
        void screenShown();
        void stimulusPlayed();
        void responseGiven(const ScreenResult*); //FIXME this should not be a pointer
        void feedbackDone();
        void timeout();

    private:

        void setTimeout(int timeout, QProgressBar* bar);
        QList<QTimer*> allTimers();

        ExperimentIoPrivate* const d;
};

}//ns apex

#endif
