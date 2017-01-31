#include "trial.h"

#include <QList>
#include <QString>
#include <QVariant>
#include <QSharedData>
#include <QDebug>

namespace apex
{
namespace data
{

/**
 * Contains a stimulus and associated information
 */
class StimulusInfo
{
    public:
        StimulusInfo(const QString &id, const QMap<QString, QVariant> &params,
                     const QString &hlElement, double wa):
                stimulus(id),
                parameters(params),
                highlightedElement(hlElement),
                waitAfter(wa)
        {
        }

        QString stimulus;
        QMap<QString, QVariant> parameters;
        QString highlightedElement;
        double waitAfter;
};



/**
 * Contains a screen and associated stimuli
 */
class TrialPart
{
    public:
        TrialPart(const QString &scr, bool ar, int to):
                screen(scr),
                acceptResponse(ar),
                timeout(to)
        {
        }

        QString screen;
        bool acceptResponse;
        double timeout;
        QList<StimulusInfo> stimuli;
};

struct TrialPrivate : public QSharedData
{
    QList<TrialPart> parts;
    QString id;
    QString answer;
};


///////////////////// Trial
Trial::Trial(QObject* parent) : QObject(parent), d(new TrialPrivate)
{
}

Trial::Trial(const Trial& other) :
        QObject(other.parent()),
        d(other.d)
{
}

Trial::~Trial()
{
}

Trial& Trial::operator=(const apex::data::Trial& other)
{
    d = other.d;
    return *this;
}

void Trial::reset()
{
    d = new TrialPrivate();
}

void Trial::addScreen(const QString& id, bool acceptResponse, double timeout)
{
    Q_ASSERT(timeout <= 0 || !acceptResponse);

    d->parts.append(TrialPart(id, acceptResponse, timeout));
}

void Trial::addStimulus(const QString &id, const QMap<QString, QVariant> &parameters,
                        const QString &highlightedElement, double waitAfter)
{
    Q_ASSERT(d->parts.size());   // there must be a screen
    d->parts.last().stimuli.append(StimulusInfo(id, parameters, highlightedElement, waitAfter));
}


bool Trial::isValid() const
{
    return screenCount() > 0;
}


int Trial::screenCount() const
{
    return d->parts.size();
}

QString Trial::screen(int screenIndex) const
{
    return d->parts.at(screenIndex).screen;
}

bool Trial::acceptResponse(int screenIndex) const
{
    return d->parts.at(screenIndex).acceptResponse;
}

double Trial::timeout(int screenIndex) const
{
    return d->parts.at(screenIndex).timeout;
}


int Trial::stimulusCount(int screenIndex) const
{
    return d->parts.at(screenIndex).stimuli.size();
}

QString Trial::stimulus(int screenIndex, int stimulusIndex) const
{
    Q_ASSERT(screenIndex < d->parts.size());
    Q_ASSERT(stimulusIndex < d->parts[screenIndex].stimuli.size());
    return d->parts.at(screenIndex).stimuli.at(stimulusIndex).stimulus;
}

QMap<QString, QVariant> Trial::parameters(int screenIndex, int stimulusIndex) const
{
    return d->parts.at(screenIndex).stimuli.at(stimulusIndex).parameters;
}

QString Trial::highlightedElement(int screenIndex, int stimulusIndex) const
{
    return d->parts.at(screenIndex).stimuli.at(stimulusIndex).highlightedElement;
}

bool Trial::doWaitAfter(int screenIndex, int stimulusIndex) const
{
    return d->parts.at(screenIndex).stimuli.at(stimulusIndex).waitAfter > 0;
}

double Trial::waitAfter(int screenIndex, int stimulusIndex)
{
    return d->parts.at(screenIndex).stimuli.at(stimulusIndex).waitAfter;
}


void Trial::setId(const QString &id)
{
    d->id = id;
}


QString Trial::id() const
{
    return d->id;
}

void Trial::setAnswer(const QString& answer)
{
    d->answer = answer;
}

QString Trial::answer() const
{
    return d->answer;
}

}
}
