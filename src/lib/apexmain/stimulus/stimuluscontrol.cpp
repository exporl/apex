#include "streamapp/appcore/threads/waitableobject.h"

#include "stimuluscontrol.h"
#include "stimulusoutput.h"

#include <QThread>

using namespace apex::stimulus;

// class WaitSignaler **********************************************************

class WaitSignaler : public QThread
{
    Q_OBJECT

public:
    WaitSignaler(QObject *parent, const appcore::WaitableObject &o);
    ~WaitSignaler();

    void run();

Q_SIGNALS:

    void waitDone();

private:
    const appcore::WaitableObject &object;
};

WaitSignaler::WaitSignaler(QObject *parent, const appcore::WaitableObject &o)
    : QThread(parent), object(o)
{
    qCDebug(APEX_THREADS, "Constructing WaitSignaler thread");
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

WaitSignaler::~WaitSignaler()
{
    qCDebug(APEX_THREADS, "Destroying WaitSignaler thread");
    this->quit();
    this->wait();
}
void WaitSignaler::run()
{
    object.mf_eWaitForSignal();
    Q_EMIT waitDone();
}

// class StimulusControl *******************************************************

StimulusControl::StimulusControl(StimulusOutput *o) : output(o)
{
}

void StimulusControl::playStimulus(const QString &stimulus,
                                   double silenceBefore)
{
    output->UnLoadStimulus();

    if (silenceBefore > 0.0)
        output->setSilenceBeforeNextStimulus(silenceBefore);

    try {
        output->LoadStimulus(stimulus, false);
        output->PlayStimulus();
        signalWhenDone(output->GetStimulusEnd(), SIGNAL(stimulusPlayed()));
    } catch (const std::exception &e) {
        // TODO: exceptions in slots need to be caught, but there is no way to
        // signal the error up the stack
        qCCritical(APEX_RS, "LoadStimulus/PlayStimulus failed: %s", e.what());
        Q_EMIT stimulusPlayed();
    }
}

void StimulusControl::signalWhenDone(const appcore::WaitableObject &o,
                                     const char *signal)
{
    if (signal == 0)
        return;

    WaitSignaler *waitSignaler = new WaitSignaler(this, o);
    connect(waitSignaler, SIGNAL(waitDone()), this, signal);
    waitSignaler->start();
}

#include "stimuluscontrol.moc"
