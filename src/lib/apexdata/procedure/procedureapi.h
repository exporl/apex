#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_PROCEDUREAPI_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_PROCEDUREAPI_H_

#include "../screen/screensdata.h"

#include "../stimulus/stimulusdata.h"

namespace apex
{

class ScreenResult;
class ProcedureInterface;
class Corrector;

namespace data
{
class ProcedureData;
class Screen;
class TrialData;
}

struct AnswerInfo
{
    QString  userAnswer;
    QString  correctAnswer;
    QVariant correctness;
    QString  highlightElement;
};

/** API for plugin procedures
 */
class ProcedureApi
{
public:
    /**
     * Return StimulusData from the current experiment for a given ID
     */
    virtual const data::StimulusData* stimulus(const QString& id) const=0;

    /**
      * Return list of all stimuli
      */
    virtual QStringList stimuli() const=0;

    /**
     * Return Screen from the current experiment for a given ID
     */
    virtual const data::Screen* screen(const QString& id) const=0;

    virtual data::tFeedbackOn feedbackOn() const = 0;

    /**
     * Get correct answer element for the given trial
     * Returns default answer element for the current screen if no answer
     * element is defined in the trial
     */
    virtual const QString answerElement(const QString& trialid,
            const data::ProcedureData* data) const=0;

    /**
     * Return (zero based) interval number based on user answer (screen element
     * ID)
     */
    /*virtual int answerToInterval(const QString& answer,
            const data::ProcedureData* d) const=0;*/

    /**
     * Make a list of trials to be presented based on given ProcedureData
     * @param doSkip if false, trials to be skipped will be ignored
     */
    virtual QStringList makeTrialList(const data::ProcedureData* d,
            const bool doSkip)=0;

    /**
     * Make list of standards to be presented, based on a given list
     * The result will contain nchoices-n standards
     * Will randomize if necessary
     * Will use default standard if empty list is given
     */
    virtual QStringList makeStandardList( const data::ProcedureData* d,
            const QStringList& inputlist) const=0;

    /**
     * Make list of stimuli to be sent to the output
     */
    virtual QStringList makeOutputList(const data::ProcedureData* d,
                                       const QString& stimulus,
                                       const QStringList &standardList,
                                       const int stimulusPosition)  const=0;

    virtual QStringList makeOutputList(const data::ProcedureData* data,
                                       const data::TrialData* trialData) const = 0;

    /**
     * Returns the screen element that should be highlighted when the stimulus
     * at stimulusPosition is played. If the number of choices is 1, the given
     * answer is returned. Otherwise the screen element associated with the
     * interval stimulusPosition is returned.
     */
    virtual QString highlightedElement(const data::ProcedureData* data,
                                       const QString& answer,
                                       int stimulusPosition) const = 0;

    virtual AnswerInfo processAnswer(const data::ProcedureData* data,
                                     const ScreenResult* screenResult,
                                     const QString& trialId,
                                     int stimulusPosition = -1) const = 0;

    virtual void showMessage(const QString& message) const = 0;

    virtual QString pluginScriptLibrary() const = 0;

    virtual QVariant parameterValue(const QString& id) const = 0;

    virtual void registerParameter(const QString& name) = 0;

    virtual ProcedureInterface* makeProcedure(const data::ProcedureData* data) = 0;

    virtual void makeCorrector(const data::ProcedureData* data) = 0;

    virtual const Corrector* corrector() const = 0;

    virtual void stopWithError(const QString& source, const QString& message) = 0;
};

}

#endif

