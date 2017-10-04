#include "apexdata/experimentdata.h"
#include "apexdata/mainconfigfiledata.h"

#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/trialdata.h"

#include "apexdata/screen/screen.h"
#include "apexdata/screen/screenresult.h"

#include "apextools/apexpaths.h"

#include "corrector/corrector.h"
#include "corrector/equalcorrector.h"

#include "gui/mainwindow.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "stimulus/stimulus.h"

#include "experimentparser.h"
#include "procedureapiimplementation.h"

#include <algorithm>

namespace apex
{

ProcedureApiImplementation::ProcedureApiImplementation(
    ExperimentRunDelegate &p_rd, bool deterministic)
    : m_rd(p_rd)
{
    if (deterministic) {
        randomGenerator.setSeed(0);
        randomGeneratorStandardList.setSeed(0);
    }

    connect(this, SIGNAL(showStatusMessage(QString)), &m_rd,
            SIGNAL(showMessage(QString)));
}

const data::StimulusData *
ProcedureApiImplementation::stimulus(const QString &id) const
{
    return m_rd.GetStimulus(id)->data();
}

QStringList ProcedureApiImplementation::stimuli() const
{
    return m_rd.stimuli();
}

const data::Screen *ProcedureApiImplementation::screen(const QString &id) const
{
    return &(m_rd.GetData().screenById(id));
}

const QString
ProcedureApiImplementation::answerElement(const QString &trialid,
                                          const data::ProcedureData *data) const
{
    QString answerElement = data->GetTrial(trialid)->GetAnswerElement();

    if (answerElement.isEmpty()) {
        answerElement = screen(data->GetTrial(trialid)->GetScreen())
                            ->getDefaultAnswerElement();
    }

    Q_ASSERT(!answerElement.isEmpty());
    return answerElement;
}

QStringList
ProcedureApiImplementation::makeTrialList(const data::ProcedureData *d,
                                          const bool doSkip)
{
    QStringList r;

    const data::ProcedureData *p = d;

    if (p->order() == data::ProcedureData::RandomOrder) {
        r = makeOrderedTrialList(d, false);
        randomizeTrialList(r);

        // add some random trials to be skipped
        if (doSkip) {
            QStringList temp = trialListToStringList(d->GetTrials());
            QStringList::iterator i = temp.end();

            for (int count = 0; count < p->skip(); ++count) {
                if (i == temp.end()) {
                    std::random_shuffle(temp.begin(), temp.end(),
                                        randomGenerator);
                    i = temp.begin();
                }

                r.prepend(*i);
                ++i;
            }
        }
    } else if (p->order() == data::ProcedureData::SequentialOrder)
        return makeOrderedTrialList(d, doSkip);
    else
        qFatal("invalid order");

    return r;
}

QStringList
ProcedureApiImplementation::makeOrderedTrialList(const data::ProcedureData *d,
                                                 const bool doSkip)
{
    const data::ProcedureData *p = d;

    // initialize vector size
    int skip = 0;
    if (doSkip)
        skip = p->skip();

    int presentations = p->presentations();

    QStringList temp(trialListToStringList(d->GetTrials()));
    QStringList r; // result list
    QStringList::const_iterator i = temp.begin();

    for (int count = 0; count < skip; ++count) {
        if (temp.end() == i)
            i = temp.begin();
        r.append(*i);
        ++i;
    }

    for (int presentation = 0; presentation < presentations; ++presentation) {
        for (i = temp.begin(); i != temp.end(); ++i)
            r.append(*i);
    }

    return r;
}

void ProcedureApiImplementation::randomizeTrialList(QStringList &t)
{
    std::random_shuffle(t.begin(), t.end(), randomGenerator);
}

QStringList
ProcedureApiImplementation::trialListToStringList(QList<data::TrialData *> tl)
{
    QStringList r;

    for (data::tTrialList::const_iterator it = tl.begin(); it != tl.end(); ++it)
        r.append((*it)->GetID());

    return r;
}

QStringList
ProcedureApiImplementation::makeStandardList(const data::ProcedureData *d,
                                             const QStringList &inputlist) const
{
    QStringList r; // result

    int nChoices = d->choices().nChoices();

    if (inputlist.isEmpty()) {
        QString defaultStandard = d->defaultStandard();

        for (int i = 0; i < nChoices - 1; ++i)
            r.append(defaultStandard);

        return r;
    }

    if (d->uniqueStandard()) {
        if (inputlist.size() + 1 < nChoices) {
            qCDebug(APEX_RS, "Error: can't create unique standardlist"
                             " because the number of standards is to small ");
        } else {
            // randomize standard list
            r = inputlist;
            std::random_shuffle(r.begin(), r.end(),
                                randomGeneratorStandardList);
            r = r.mid(0, nChoices - 1);
            return r;
        }
    }

    for (int i = 0; i < nChoices - 1; ++i)
        r.append(inputlist.at(randomGenerator.nextUInt(inputlist.size())));

    return r;
}

QStringList ProcedureApiImplementation::makeOutputList(
    const data::ProcedureData *d, const QString &stimulus,
    const QStringList &standardList, const int stimulusPosition) const
{
    QStringList ret;

    // select random stimulus
    int nChoices = d->choices().nChoices();

    if (nChoices <= 1)
        return QStringList() << stimulus;

    Q_ASSERT(standardList.size() == nChoices - 1);
    Q_ASSERT(stimulusPosition < nChoices);

    QStringList::const_iterator standardIt = standardList.begin();
    for (int i = 0; i < nChoices; ++i) {
        if (i == stimulusPosition)
            ret << stimulus;
        else {
            Q_ASSERT(standardIt != standardList.end());
            ret << *standardIt;
            ++standardIt;
        }
    }

    Q_ASSERT(ret.size() == nChoices);

    return ret;
}

QStringList ProcedureApiImplementation::makeOutputList(
    const data::ProcedureData *data, const data::TrialData *trialData) const
{
    return makeOutputList(data, trialData->GetRandomStimulus(),
                          makeStandardList(data, trialData->GetStandards()),
                          data->choices().randomPosition());
}

data::tFeedbackOn ProcedureApiImplementation::feedbackOn() const
{
    return m_rd.GetData().screensData()->feedbackOn();
}

QString
ProcedureApiImplementation::highlightedElement(const data::ProcedureData *data,
                                               const QString &answer,
                                               int stimulusPosition) const
{
    if (data->choices().hasMultipleIntervals()) {
        Q_ASSERT(stimulusPosition >= 0);
        Q_ASSERT(stimulusPosition < data->choices().nChoices());

        // return data->parameters()->choices().intervals()[stimulusPosition];
        QString ret = data->choices().element(stimulusPosition);

        if (!ret.isEmpty())
            return ret;
    }
    //     else
    //         return answer;

    return answer;
}

AnswerInfo ProcedureApiImplementation::processAnswer(
    const data::ProcedureData *data, const ScreenResult *screenResult,
    const QString &trialId, int stimulusPosition) const
{
    AnswerInfo ret;

    // find answer element to use
    QString answerElement = this->answerElement(trialId, data);

    if (screenResult->contains(answerElement)) {
        ret.userAnswer = (*screenResult)[answerElement];
        ret.correctAnswer = data->GetTrial(trialId)->GetAnswer();

        if (data->choices().hasMultipleIntervals()) {
            // AFC: we compare ourselves
            Q_ASSERT(stimulusPosition >= 0);
            int userInterval = data->choices().interval(ret.userAnswer);
            // Interval == -1 if no element found in list
            ret.correctness = stimulusPosition == userInterval;
            ret.correctAnswer =
                data->choices().intervals().at(stimulusPosition);
        } else {
            ret.correctness =
                corrector()->compare(ret.userAnswer, ret.correctAnswer);
        }
    } else {
        ret.userAnswer = (*screenResult)[answerElement];
        ret.correctness = false;
    }

    data::tFeedbackOn feedbackOn = this->feedbackOn();

    if (feedbackOn == data::HIGHLIGHT_ANSWER)
        ret.highlightElement = ret.userAnswer;
    else if (feedbackOn == data::HIGHLIGHT_CORRECT)
        ret.highlightElement = ret.correctAnswer;

    return ret;
}

void ProcedureApiImplementation::showMessage(const QString &message) const
{
    Q_EMIT showStatusMessage(message);
}

QString ProcedureApiImplementation::pluginScriptLibrary() const
{
    return m_rd.mainData().pluginScriptLibrary();
}

QVariant ProcedureApiImplementation::parameterValue(const QString &id) const
{
    return m_rd.GetParameterManager()->parameterValue(id);
}

void ProcedureApiImplementation::registerParameter(const QString &name)
{
    data::Parameter paramName("user", name, QVariant(), data::NO_CHANNEL, true);
    paramName.setId(name);

    m_rd.GetParameterManager()->registerParameter(name, paramName);
}

ProcedureInterface *
ProcedureApiImplementation::makeProcedure(const data::ProcedureData *data)
{
    return m_rd.makeProcedure(this, data);
}

void ProcedureApiImplementation::makeCorrector(const data::ProcedureData *)
{
    m_corrector = QSharedPointer<Corrector>(new EqualCorrector());
}

const Corrector *ProcedureApiImplementation::corrector() const
{
    return m_corrector.data();
}

void ProcedureApiImplementation::stopWithError(const QString &source,
                                               const QString &message)
{
    Q_EMIT stoppedWithError(source, message);
}

/*int ProcedureApiImplementation::answerToInterval(const QString& answer,
            const data::ProcedureData* d) const
{
    int interval = d->GetParameters()->choices().interval(answer);
} */
}
