#include "streamapp/appcore/threads/waitableobject.h"

#include "stimuluscontrol.h"
#include "stimulusoutput.h"

#include <QThread>

using namespace apex::stimulus;

//class WaitSignaler **********************************************************

class WaitSignaler : public QThread
{
        Q_OBJECT

    public:

        WaitSignaler(const appcore::WaitableObject& o);

        void run();

    Q_SIGNALS:

        void waitDone();
    private:

        const appcore::WaitableObject& object;
};

WaitSignaler::WaitSignaler(const appcore::WaitableObject& o) : object(o)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void WaitSignaler::run()
{
    object.mf_eWaitForSignal();
    emit waitDone();
}

//class StimulusControl *******************************************************

StimulusControl::StimulusControl(StimulusOutput* o) : output(o)
{
}

void StimulusControl::playStimulus(const QString& stimulus, double silenceBefore)
{
    output->UnLoadStimulus();

    if (silenceBefore > 0.0)
        output->setSilenceBeforeNextStimulus(silenceBefore);

    output->LoadStimulus(stimulus, false);
    output->PlayStimulus();

    signalWhenDone(output->GetStimulusEnd(), SIGNAL(stimulusPlayed()));
}

void StimulusControl::signalWhenDone(const appcore::WaitableObject& o,
                                     const char* signal)
{
    if (signal == 0)
        return;

    WaitSignaler* s = new WaitSignaler(o);
    connect(s, SIGNAL(waitDone()), this, signal);
    s->start(); //auto deletes
}

#include "stimuluscontrol.moc"
