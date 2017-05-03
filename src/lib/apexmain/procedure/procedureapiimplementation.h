#ifndef _EXPORL_SRC_LIB_APEXMAIN_PROCEDURE_PROCEDUREAPIIMPLEMENTATION_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PROCEDURE_PROCEDUREAPIIMPLEMENTATION_H_

#include "apexdata/procedure/procedureapi.h"

#include "apextools/random.h"

#include <QSharedPointer>

namespace apex
{

class ProcedureInterface;
class Corrector;

namespace data
{
class TrialData;
class ProcedureData;
class Screen;
}

class ExperimentRunDelegate;

/**
 * Implementation of API for plugin procedures(script procedures)
 */
class APEX_EXPORT ProcedureApiImplementation : public QObject, public ProcedureApi
{
    Q_OBJECT

    public:

        ProcedureApiImplementation(ExperimentRunDelegate& p_rd,
                                   bool deterministic = false);

        virtual const data::StimulusData* stimulus(const QString& id) const;
        virtual QStringList stimuli() const;
        virtual const data::Screen* screen(const QString& id) const;
        virtual data::tFeedbackOn feedbackOn() const;
        virtual const QString answerElement(const QString& trialid,
                const data::ProcedureData* data) const;
        /*virtual int answerToInterval(const QString& answer,
                const data::ProcedureData* d) const;*/
        virtual QStringList makeTrialList(const data::ProcedureData* d,
                const bool doSkip);

        virtual QStringList makeStandardList( const data::ProcedureData* d,
                const QStringList& inputlist) const;

        virtual QStringList makeOutputList(const data::ProcedureData* d,
                                        const QString& stimulus,
                                        const QStringList& standardList,
                                        const int stimulusPosition)  const;

        virtual QStringList makeOutputList(const data::ProcedureData* data,
                                        const data::TrialData* trialData) const;

        virtual QString highlightedElement(const data::ProcedureData* data,
                                        const QString& answer,
                                        int stimulusPosition) const;

        virtual AnswerInfo processAnswer(const data::ProcedureData* data,
                                        const ScreenResult* screenResult,
                                        const QString& trialId,
                                        int stimulusPosition = -1) const;

        virtual void showMessage(const QString& message) const;

        virtual QString pluginScriptLibrary() const;

        virtual QVariant parameterValue(const QString& id) const;

        virtual void registerParameter(const QString& name);

        virtual ProcedureInterface* makeProcedure(const data::ProcedureData* data);

        virtual void makeCorrector(const data::ProcedureData *);

        virtual const Corrector* corrector() const;

        virtual void stopWithError(const QString& source, const QString& message);

    private:

        ExperimentRunDelegate& m_rd;

        QStringList makeOrderedTrialList(
                const data::ProcedureData* d, const bool doSkip);
        void randomizeTrialList(QStringList& t);
        QStringList trialListToStringList(QList<data::TrialData*> tl);

        mutable Random randomGenerator;
        mutable Random randomGeneratorStandardList;

        QSharedPointer<Corrector> m_corrector;

    signals:
        void showMessageBox(const QString title, const QString text) const;
        void showStatusMessage(const QString message) const;
        void stoppedWithError(const QString& source, const QString& message);
};

}

#endif
