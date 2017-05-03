#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_PROCEDUREINTERFACE_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_PROCEDUREINTERFACE_H_

#include "../screen/screenresult.h"

#include "procedureapi.h"
#include "proceduredata.h"
#include "trial.h"

#include <QtPlugin>

class QDomElement;

namespace apex
{

class ResultHighlight
{
public:
    ResultHighlight():
        overrideCorrectFalse(false),
        showCorrectFalse(true)  { }

    ResultHighlight (bool isCorrect, QString hlElement):
        correct(isCorrect),
        highlightElement(hlElement),
        overrideCorrectFalse(false),
        showCorrectFalse(true) {}

    bool    correct;
    QString highlightElement;
    /**
      * Override showfeedback from experiment file
      **/
    bool    overrideCorrectFalse;
    /**
     * @brief Only used when overrideCorrectFalse==true
     * 0: do not show correct/false feedback
     * 1: do show correct/false feedback
     */
    bool     showCorrectFalse;

};

class ProcedureInterface
{
public:

    /**
     * Create the interface of a Procedure given the ProcedureApi and the
     * ProcedureData. This is done for every procedure, even for the
     * subprocedures of a MultiProcedure. This constructor also creates the
     * corrector of the Procedure from the CorrectorData found in ProcedureData.
     * This corrector will be needed from the ProcedureApi when the answer needs
     * to be processed.
     */
    ProcedureInterface(ProcedureApi* a, const data::ProcedureData* d) :
                                                        api(a), data(d)
    {
        Q_ASSERT(api != 0);
        Q_ASSERT(data != 0);
        makeCorrector();
    }

    virtual ~ProcedureInterface() {}

    /**
     * Return the first screen; this is the screen that should be shown
     * before the user presses start if there is no intro screen.
     */
    virtual QString firstScreen() = 0;

    /**
     * Return a Trial structure with screens and stimuli to be presented
     * when finished, return an empty Trial (with no screens added)
     */
    virtual data::Trial setupNextTrial() = 0;

    /**
     * Return the current progress in %, e.g., trial 5 of 10 would yield 50%
     */
    virtual double progress() const = 0;

    /**
     * Process the result from the last trial
     * return whether the answer was correct or incorrect
     */
    virtual ResultHighlight processResult(const ScreenResult* screenResult) = 0;

    /**
     * Return XML for the results file
     */
    virtual QString resultXml() const = 0;

    /**
     * Return XML for the results file, is called once after the experiment has finished
     */
    virtual QString finalResultXml() const = 0;

    /**
     * Create the corrector from the ProcedureData and store it somewhere
     * accessible for the ProcedureApi.
     */
    virtual void makeCorrector() {
        api->makeCorrector(data);
    }

protected:

    ProcedureApi* api;
    const data::ProcedureData* data;
};


class ProcedureParserInterface
{
public:
    virtual ~ProcedureParserInterface() {}

    virtual data::ProcedureData* parse(const QDomElement &base) = 0;
};

class ProcedureCreatorInterface
{
public:

    virtual ~ProcedureCreatorInterface() {}

    virtual QStringList availablePlugins() const = 0;

    virtual ProcedureInterface* createProcedure(const QString& name,
                                                ProcedureApi* api,
                                                const data::ProcedureData *config) = 0;

    virtual ProcedureParserInterface* createProcedureParser(const QString& name) = 0;
};
}

Q_DECLARE_INTERFACE(apex::ProcedureCreatorInterface, "be.exporl.apex.procedure/1.0")

#endif
