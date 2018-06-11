/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _APEX_SRC_LIB_APEXDATA_EXPERIMENTDATA_H_
#define _APEX_SRC_LIB_APEXDATA_EXPERIMENTDATA_H_

#include "apextools/apextypedefs.h"
#include "apextools/global.h"

#include "stimulus/stimulidata.h"

#include <QCoreApplication>
#include <QMap>
#include <QStringList>

#include <memory>

using apex::device::tControllerMap;
using apex::stimulus::DataBlock;
using apex::stimulus::OutputDevice;
using apex::stimulus::Filter;
using apex::stimulus::Stimulus;
using apex::stimulus::tDataBlockMap;
using apex::stimulus::tDeviceMap;
using apex::stimulus::tFilterMap;

namespace apex
{

class ApexErrorHandler;
class ApexProcedure;
class ExperimentConfigFileParser;
class MainConfigFileParser;

namespace data
{

class FilterData;
class Screen;
class ConnectionsData;
class DevicesData;
class DeviceData;
class DatablocksData;
class FiltersData;
class ParameterManagerData;
class CalibrationData;
class StimulusData;
class ScreensData;
class ProcedureData;
class ApexTrial;
class RandomGeneratorParameters;
class ExperimentDataPrivate;
class GeneralParameters;
class ResultParameters;
class ParameterDialogResults;
class TrialData;

/**
 * Please modify operator== when modifying this data structure.
 */
// FIXME
//[job] I've made all the getters return pointers but there should be thought
// about how we want this data to be modified: either make const and non-const
// versions of all getters and modify through the non-const version or make
// all getters const and provide setters (probably less convenient).

// FIXME [job] what should be the copy behaviour of this class?
class APEXDATA_EXPORT ExperimentData
{
    Q_DECLARE_TR_FUNCTIONS(ExperimentData)
public:
    ExperimentData(const QString &configFile, data::ScreensData *screens,
                   data::ProcedureData *procedures,
                   data::ConnectionsData *connections,
                   data::CalibrationData *calibration,
                   GeneralParameters *genParam, ResultParameters *resParam,
                   ParameterDialogResults *paramDlg,
                   QMap<QString, RandomGeneratorParameters *> *rndGen,
                   data::DevicesData *devices, data::FiltersData *filters,
                   data::DevicesData *controlDevices,
                   data::DatablocksData *datablocks, data::StimuliData *stimuli,
                   const QString &description,
                   data::ParameterManagerData *paramMngr);
    ExperimentData();
    virtual ~ExperimentData();

    const QString &fileName() const;
    void setFileName(const QString &fileName);

    // const getters

    const ScreensData *screensData() const;
    const TrialData *trialById(const QString &id) const;
    const ConnectionsData *connectionsData() const;
    const Screen &screenById(const QString &id) const;
    const ProcedureData *procedureData() const;
    QMap<QString, RandomGeneratorParameters *>
    randomGenerators() const; // FIXME params still modifiable
    CalibrationData *calibrationData() const;
    const GeneralParameters *generalParameters() const;
    const ResultParameters *resultParameters() const;
    FixedParameterList fixedParameters() const;
    const ParameterDialogResults *parameterDialogResults() const;
    const DevicesData *devicesData() const;
    const DeviceData *deviceById(const QString &id) const;
    const DatablocksData *datablocksData() const;
    const FiltersData *filtersData() const;
    const StimuliData *stimuliData() const;
    const DevicesData *controlDevices() const;
    const ParameterManagerData *parameterManagerData() const;
    const StimulusData *stimulusById(const QString &id) const;
    QString experimentDescription() const;
    bool isFixedParameter(const QString &id) const;

    // non-const getters

    TrialData *trialById(const QString &id);
    ResultParameters *resultParameters();
    GeneralParameters *generalParameters();
    ScreensData *screensData();
    ConnectionsData *connectionsData();
    ProcedureData *procedureData();

    void addFilter(FilterData *filter);

    /**
        * Equality operator used for regression test.
        * Please modify if modifying this data structure.
        */
    bool operator==(const ExperimentData &other) const;

private:
    ExperimentDataPrivate *d;
};
}
}
#endif //#ifndef _APEX_SRC_LIB_APEXDATA_EXPERIMENTDATA_H_
