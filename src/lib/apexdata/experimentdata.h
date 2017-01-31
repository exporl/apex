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

#ifndef __EXPERIMENTDATA_H__
#define __EXPERIMENTDATA_H__


#include "stimulus/stimulidata.h"       // tFixedParameterList typedef

#include "corrector/correctordata.h"

#include <QStringList>
#include <QCoreApplication>         // tr
#include <QMap>
#include <memory>

#include "apextypedefs.h"
#include "global.h"

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
class ApexMapParameters;
class ApexProcedure;
class ConfigFileParser;
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
class CorrectorData;
class ApexProcedureConfig;
class ApexTrial;
class RandomGeneratorParameters;
class ExperimentDataPrivate;
class GeneralParameters;
class ResultParameters;
class ParameterDialogResults;

/**
 * Please modify operator== when modifying this data structure.
 */
//FIXME
//[job] I've made all the getters return pointers but there should be thought
//about how we want this data to be modified: either make const and non-const
//versions of all getters and modify through the non-const version or make
//all getters const and provide setters (probably less convenient).

//FIXME [job] what should be the copy behaviour of this class?
class APEXDATA_EXPORT ExperimentData
{
        Q_DECLARE_TR_FUNCTIONS(ExperimentData)

    public:

        /**
         * Create a new experiment that gets its data from the
         * given parser..
         */
        ExperimentData (const QString& configFile,
                        std::auto_ptr<ScreensData>& screens,
                        std::auto_ptr<ApexProcedureConfig>& procedures,
                        std::auto_ptr<CorrectorData>& correctors,
                        std::auto_ptr<ConnectionsData>& connections,
                        std::auto_ptr<CalibrationData>& calibration,
                        std::auto_ptr<GeneralParameters>& genParam,
                        std::auto_ptr<ResultParameters>& resParam,
                        std::auto_ptr<ParameterDialogResults>& paramDlg,
                        std::auto_ptr<QMap<QString, RandomGeneratorParameters*> >& rndGen,
                        std::auto_ptr<DevicesData>& devices,
                        std::auto_ptr<FiltersData>& filters,
                        std::auto_ptr<DevicesData>& controlDevices,
                        std::auto_ptr<DatablocksData>& datablocks,
                        std::auto_ptr<StimuliData>& stimuli,
                        const QString& description,
                        std::auto_ptr<ParameterManagerData>& paramMngr);
        virtual ~ExperimentData ();

        const QString& fileName() const;
        void setFileName (const QString& fileName);

        //const getters

        const ScreensData* screensData() const;
        const ApexTrial* trialById (const QString& id) const;
        const CorrectorData* correctorData() const;
        const ConnectionsData* connectionsData() const;
        const Screen& screenById (const QString& id) const;
        ApexProcedureConfig* procedureConfig() const;
        QMap<QString, RandomGeneratorParameters*> randomGenerators() const; //FIXME params still modifiable
        CalibrationData* calibrationData() const;
        const GeneralParameters* generalParameters() const;
        const ResultParameters* resultParameters() const;
        FixedParameterList fixedParameters() const;
        const ParameterDialogResults* parameterDialogResults() const;
        const DevicesData* devicesData() const;
        const DeviceData* deviceById (const QString& id) const;
        const DatablocksData* datablocksData() const;
        const FiltersData* filtersData() const;
        const StimuliData* stimuliData() const;
        const DevicesData* controlDevices() const;
        const ParameterManagerData* parameterManagerData() const;
        const StimulusData* stimulusById (const QString& id) const;
        QString experimentDescription() const;
        bool isFixedParameter (const QString& id) const;

        //non-const getter

        ApexTrial* trialById (const QString& id);
        ConnectionsData* connectionsData();
        ResultParameters* resultParameters();
        GeneralParameters* generalParameters();

        void addFilter(FilterData* filter);

        /**
         * Equality operator used for regression test.
         * Please modify if modifying this data structure.
         */
        bool operator== (const ExperimentData& other) const;

    private:

        ExperimentDataPrivate* d;
};
}
}
#endif //#ifndef __EXPERIMENT_H__
