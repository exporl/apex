#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULUSCONTROL_H_
#define _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULUSCONTROL_H_

#include <QObject>

namespace appcore
{
class WaitableObject;
}

namespace apex
{
namespace stimulus
{

class StimulusOutput;

class StimulusControl : public QObject
{
    Q_OBJECT

public:
    StimulusControl(StimulusOutput *o);

public Q_SLOTS:

    void playStimulus(const QString &stimulus, double silenceBefore = 0.0);

Q_SIGNALS:

    void stimulusPlayed();

private:
    void signalWhenDone(const appcore::WaitableObject &o, const char *signal);

    StimulusOutput *output;
};

} // ns stimulus
} // ns apex

#endif
