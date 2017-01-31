#include "apexdata/procedure/multiproceduredata.h"

#include "multiprocedure.h"

using namespace apex;

MultiProcedure::MultiProcedure(ProcedureApi *api, const data::ProcedureData *data)
    : ProcedureInterface(api, data),
      indexCurrentProcedure(-1), indexPreviousProcedure(-1),
      numberOfProcedures(dynamic_cast<const data::MultiProcedureData*>(data)->procedures().count()),
      valid(QVector<bool>(numberOfProcedures, true))
{
    initProcedures();

    nextIndex();
}

QString MultiProcedure::firstScreen()
{
    return procedures.at(indexCurrentProcedure)->firstScreen();
}

double MultiProcedure::progress() const
{
    double sum = 0;
    for(int i = 0; i < numberOfProcedures; ++i) {
        sum += procedures.at(i)->progress();
    }

    return sum/numberOfProcedures;
}

ResultHighlight MultiProcedure::processResult(const ScreenResult *screenResult)
{
    return procedures.at(indexPreviousProcedure)->processResult(screenResult);
}

/*!
 * \brief Set the index of the current procedure to the next value respecting
 * the order.

 * When the order is random, the index of the current procedure is set to a
 * random number.
 * When the order is one by one or sequential, the index of the current
 * procedure is set to the next procedure in the vector. This number wraps
 * around if necessary.

 * \remarks This function doesn't check if the current procedure is finished
 * or not.
 */
void MultiProcedure::nextIndex()
{
    if(data->order() == data::ProcedureData::RandomOrder) {
        indexCurrentProcedure = r(numberOfProcedures);
    } else {
        indexCurrentProcedure = (indexCurrentProcedure + 1) % numberOfProcedures;
    }
}

/*!
 * \brief Set the index of the current procedure to the next value respecting
 * order and the validity of the procedure.

 * When the order is one by one and the current procedure can't set up valid
 * trials anymore, then it's time to move on to the next procedure. A variable
 * that stores the fact if a procedure can still set up new trials is needed
 * because you can only call setupNextTrial once for each trial. If the order
 * is one by one and the current procedure can still set up valid trials,
 * nothing happens.
 * If the order is sequential, the next procedure is selected.
 * If the order is random, the next procedure is randomly selected from the
 * procedures that are still valid. If no such procedure exists, nothing happens.
 */
void MultiProcedure::setNextProcedure()
{
    indexPreviousProcedure = indexCurrentProcedure;
    if (data->order() == data::ProcedureData::OneByOneOrder) {
        if (!valid.at(indexCurrentProcedure)) {
            nextIndex();
        }
    } else if (data->order() == data::ProcedureData::SequentialOrder){
        nextIndex();
    } else if(data->order() == data::ProcedureData::RandomOrder) {
        if(valid.contains(true)) {
            do {
                nextIndex();
            } while (!valid.at(indexCurrentProcedure));
        }
    }
}

/*!
 * \brief Sets up the next trial and returns it.
 *
 * Sets up the trial of the current procedure. If the trial is invalid, the next
 * trial will be generated (possibly from an other procedure). Also if the
 * trial is invalid, the current procedure is marked as invalid, so this procedure
 * won't be used anymore.
 */
data::Trial MultiProcedure::setupNextTrial()
{
    data::Trial result = procedures.at(indexCurrentProcedure)->setupNextTrial();

    int startIndex = indexCurrentProcedure;
    while(!result.isValid()) {
        valid.replace(indexCurrentProcedure, false);

        setNextProcedure();
        result = procedures.at(indexCurrentProcedure)->setupNextTrial();

        if(startIndex == indexCurrentProcedure) {
            break;
        }
    }

    setNextProcedure();
    return result;
}

void MultiProcedure::initProcedures()
{
    const data::MultiProcedureData* params
            = dynamic_cast<const data::MultiProcedureData*>(data);

    data::tProcConfigList datas = params->procedures();

    for(int i=0; i<datas.count(); ++i){
          procedures.push_back(api->makeProcedure(datas.at(i)));
    }
}

QString MultiProcedure::resultXml() const
{
    return procedures.at(indexPreviousProcedure)->resultXml();
}

QString MultiProcedure::finalResultXml() const
{
    QString results;

    const data::MultiProcedureData* params
            = dynamic_cast<const data::MultiProcedureData*>(data);

    data::tProcConfigList datas = params->procedures();

    for (int i = 0; i < procedures.count(); ++i) {
        QString id = datas.at(i)->GetID();
        QString idString = "<procedure id=\"" + id + "\">";
        QString search = "<procedure>";

        QString result = procedures.at(i)->finalResultXml();
        result.replace(result.indexOf(search), search.count(), idString);

        results += result;
    }

    return results;
}
