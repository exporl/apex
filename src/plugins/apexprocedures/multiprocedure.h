#ifndef _EXPORL_SRC_PLUGINS_APEXPROCEDURES_MULTIPROCEDURE_H_
#define _EXPORL_SRC_PLUGINS_APEXPROCEDURES_MULTIPROCEDURE_H_

#include "apexdata/procedure/procedureinterface.h"

#include "apextools/random.h"

namespace apex
{
class ProcedureApi;

namespace data
{
class ProcedureData;
class MultiProcedureData;
}

/**
*/
class MultiProcedure : public ProcedureInterface
{
public:
    MultiProcedure(ProcedureApi* api, const data::ProcedureData* data);

    QString firstScreen();
    data::Trial setupNextTrial();
    double progress() const;
    ResultHighlight processResult(const ScreenResult *screenResult);
    QString resultXml() const;
    QString finalResultXml() const;

private:
    int indexCurrentProcedure;
    int indexPreviousProcedure;
    int numberOfProcedures;
    Random r;

public:
    void setNextProcedure();
    void nextIndex();
private:

    //A vector that indicates if the procedure with a certain index can still
    // set up valid trials. The order should correspond with the field procedures.
    QVector<bool> valid;
    QVector<ProcedureInterface*> procedures;
    void initProcedures();
};
}

#endif // _EXPORL_SRC_PLUGINS_APEXPROCEDURES_MULTIPROCEDURE_H_
