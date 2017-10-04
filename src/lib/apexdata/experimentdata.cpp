/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/
#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"

#include "calibration/calibrationdata.h"

#include "connection/connectiondata.h"

#include "datablock/datablockdata.h"
#include "datablock/datablocksdata.h"

#include "device/devicedata.h"
#include "device/devicesdata.h"

#include "filter/filterdata.h"
#include "filter/filtersdata.h"

#include "interactive/parameterdialogresults.h"

#include "parameters/generalparameters.h"
#include "parameters/parametermanagerdata.h"

#include "procedure/proceduredata.h"

#include "randomgenerator/randomgeneratorparameters.h"

#include "result/resultparameters.h"

#include "screen/screensdata.h"

#include "stimulus/stimulidata.h"
#include "stimulus/stimulusdata.h"

#include "experimentdata.h"

using namespace apex::XMLKeys;

namespace apex
{
namespace data
{

class ExperimentDataPrivate
{
public:
    QString fileName;
    QScopedPointer<ScreensData> screens;
    QScopedPointer<ProcedureData> procedureConfig;
    QScopedPointer<ConnectionsData> connectionDatas;
    QScopedPointer<CalibrationData> calibrationData;
    QScopedPointer<GeneralParameters> generalParameters;
    QScopedPointer<ResultParameters> resultParameters;
    QScopedPointer<ParameterDialogResults> parameterDialogResults;
    QScopedPointer<QMap<QString, RandomGeneratorParameters *>> randomgenerators;
    QScopedPointer<DevicesData> devices;
    QScopedPointer<FiltersData> filtersdata;
    QScopedPointer<DevicesData> controldevices;
    QScopedPointer<DatablocksData> datablocksdata;
    QScopedPointer<StimuliData> stimuli;
    QString experimentDescription;
    QScopedPointer<ParameterManagerData> parameterManagerData;
};
}
}

using namespace apex::data;
using namespace apex;

ExperimentData::ExperimentData(
    const QString &configFile, data::ScreensData *screens,
    data::ProcedureData *procedures, data::ConnectionsData *connections,
    data::CalibrationData *calibration, GeneralParameters *genParam,
    ResultParameters *resParam, ParameterDialogResults *paramDlg,
    QMap<QString, RandomGeneratorParameters *> *rndGen,
    data::DevicesData *devices, data::FiltersData *filters,
    data::DevicesData *controlDevices, data::DatablocksData *datablocks,
    data::StimuliData *stimuli, const QString &description,
    data::ParameterManagerData *paramMngr)
    : d(new ExperimentDataPrivate())
{
    d->fileName = configFile;
    d->screens.reset(screens);
    d->procedureConfig.reset(procedures);
    d->connectionDatas.reset(connections);
    d->calibrationData.reset(calibration);
    d->generalParameters.reset(genParam);
    d->resultParameters.reset(resParam);
    d->parameterDialogResults.reset(paramDlg);
    d->randomgenerators.reset(rndGen);
    d->devices.reset(devices);
    d->filtersdata.reset(filters);
    d->controldevices.reset(controlDevices);
    d->datablocksdata.reset(datablocks);
    d->stimuli.reset(stimuli);
    d->experimentDescription = description;
    d->parameterManagerData.reset(paramMngr);
}

ExperimentData::ExperimentData() : d(new ExperimentDataPrivate())
{
}

ExperimentData::~ExperimentData()
{
    // FIXME [job] shouldn't these deletes happen in the destructors of the
    // classes that are iterated

    // remove device data from map
    for (data::DevicesData::const_iterator it = d->devices->begin();
         it != d->devices->end(); ++it) {
        Q_CHECK_PTR(it.value());
        delete it.value();
    }

    // remove filter data from map
    for (data::FiltersData::const_iterator it = d->filtersdata->begin();
         it != d->filtersdata->end(); ++it) {
        Q_CHECK_PTR(it.value());
        delete it.value();
    }

    // remove datablocks from map
    for (data::DatablocksData::const_iterator it = d->datablocksdata->begin();
         it != d->datablocksdata->end(); ++it) {
        Q_CHECK_PTR(it.value());
        delete it.value();
    }

    delete d;
}

const QString &ExperimentData::fileName() const
{
    return d->fileName;
}

void ExperimentData::setFileName(const QString &fileName)
{
    d->fileName = fileName;
}

const ProcedureData *ExperimentData::procedureData() const
{
    return d->procedureConfig.data();
}

ProcedureData *ExperimentData::procedureData()
{
    return d->procedureConfig.data();
}

const TrialData *ExperimentData::trialById(const QString &name) const
{
    return d->procedureConfig->GetTrial(name);
}

TrialData *ExperimentData::trialById(const QString &name)
{
    return d->procedureConfig->GetTrial(name);
}

const Screen &ExperimentData::screenById(const QString &id) const
{
    return d->screens->GetScreen(id);
}

const ConnectionsData *ExperimentData::connectionsData() const
{
    return d->connectionDatas.data();
}

ConnectionsData *ExperimentData::connectionsData()
{
    return d->connectionDatas.data();
}

const ScreensData *ExperimentData::screensData() const
{
    return d->screens.data();
}

QMap<QString, RandomGeneratorParameters *>
ExperimentData::randomGenerators() const
{
    return *d->randomgenerators;
}

CalibrationData *ExperimentData::calibrationData() const
{
    return d->calibrationData.data();
}

const GeneralParameters *ExperimentData::generalParameters() const
{
    return d->generalParameters.data();
}

GeneralParameters *ExperimentData::generalParameters()
{
    return d->generalParameters.data();
}

ResultParameters *ExperimentData::resultParameters()
{
    return d->resultParameters.data();
}

ScreensData *ExperimentData::screensData()
{
    return d->screens.data();
}

FixedParameterList ExperimentData::fixedParameters() const
{
    return d->stimuli->GetFixedParameters();
}

const StimulusData *ExperimentData::stimulusById(const QString &p_name) const
{
    if (!d->stimuli->contains(p_name))
        throw ApexStringException(
            tr("ExperimentData::GetStimulus: error: stimulus %1 not found")
                .arg(p_name));

    return &(*d->stimuli)[p_name];
}

const DeviceData *ExperimentData::deviceById(const QString &p_name) const
{
    data::DevicesData::const_iterator i = d->devices->find(p_name);
    Q_ASSERT(i != d->devices->end());
    return i.value();
}

QString ExperimentData::experimentDescription() const
{
    return d->experimentDescription;
}

const DevicesData *ExperimentData::controlDevices() const
{
    return d->controldevices.data();
}

const ParameterDialogResults *ExperimentData::parameterDialogResults() const
{
    return d->parameterDialogResults.data();
}

bool ExperimentData::isFixedParameter(const QString &p_name) const
{
    return d->stimuli->GetFixedParameters().contains(p_name);
}

const DevicesData *ExperimentData::devicesData() const
{
    return d->devices.data();
}

const StimuliData *ExperimentData::stimuliData() const
{
    return d->stimuli.data();
}

const FiltersData *ExperimentData::filtersData() const
{
    return d->filtersdata.data();
}

const ResultParameters *ExperimentData::resultParameters() const
{
    return d->resultParameters.data();
}

const DatablocksData *ExperimentData::datablocksData() const
{
    return d->datablocksdata.data();
}

const ParameterManagerData *ExperimentData::parameterManagerData() const
{
    return d->parameterManagerData.data();
}

void ExperimentData::addFilter(FilterData *filter)
{
    d->filtersdata->insert(filter->id(), filter);
}

#define ARE_EQUAL(a, b)                                                        \
    ((a.data() == 0 && b.data() == 0)                                          \
         ? true                                                                \
         : ((a.data() == 0 || b.data() == 0) ? false : *a == *b))

bool ExperimentData::operator==(const ExperimentData &other) const
{
    if (!(*d->screens == *other.d->screens)) {
        qCDebug(APEX_RS, "screensdata not equal");
        return false;
    }
    if (!(*d->procedureConfig == *other.d->procedureConfig)) {
        qCDebug(APEX_RS, "procedure config not equal");
        return false;
    }
    if (!(*d->connectionDatas == *other.d->connectionDatas)) {
        qCDebug(APEX_RS, "connectiondata not equal");
        return false;
    }
    if (!ARE_EQUAL(d->calibrationData, other.d->calibrationData)) {
        qCDebug(APEX_RS, "calibrationdata not equal");
        return false;
    }
    if (!(*d->generalParameters == *other.d->generalParameters)) {
        qCDebug(APEX_RS, "general parameters not equal");
        return false;
    }
    if (!(*d->resultParameters == *other.d->resultParameters)) {
        qCDebug(APEX_RS, "result parameters not equal");
        return false;
    }
    if (!ARE_EQUAL(d->parameterDialogResults,
                   other.d->parameterDialogResults)) {
        qCDebug(APEX_RS, "parameter dialog results not equal");
        return false;
    }
    if (!(*d->parameterManagerData == *other.d->parameterManagerData)) {
        qCDebug(APEX_RS, "parameter manager data not equal");
        return false;
    }
    if (!ApexTools::areEqualPointerMaps(*d->randomgenerators,
                                        *other.d->randomgenerators)) {
        qCDebug(APEX_RS, "random generators not equal");
        return false;
    }
    if (!(*d->devices == *other.d->devices)) {
        qCDebug(APEX_RS, "devicesdata not equal");
        return false;
    }
    if (!(*d->controldevices == *other.d->controldevices)) {
        qCDebug(APEX_RS, "controldevices not equal");
        return false;
    }
    if (!(*d->datablocksdata == *other.d->datablocksdata)) {
        qCDebug(APEX_RS, "datablocksdata not equal");
        return false;
    }
    if (!(*d->filtersdata == *other.d->filtersdata)) {
        qCDebug(APEX_RS, "filtersdata not equal");
        return false;
    }
    if (!(*d->stimuli == *other.d->stimuli)) {
        qCDebug(APEX_RS, "stimulidata not equal");
        return false;
    }
    if (!(d->experimentDescription == other.d->experimentDescription)) {
        qCDebug(APEX_RS, "description not equal");
        return false;
    }

    return true;
}
