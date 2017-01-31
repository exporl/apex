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
#include "experimentdata.h"

#include "calibration/calibrationdata.h"
#include "connection/connectiondata.h"
#include "corrector/correctordata.h"
#include "datablock/datablockdata.h"
#include "datablock/datablocksdata.h"
#include "device/devicedata.h"
#include "device/devicesdata.h"
#include "filter/filterdata.h"
#include "filter/filtersdata.h"
#include "procedure/apexprocedureconfig.h"
#include "screen/screensdata.h"
#include "randomgenerator/randomgeneratorparameters.h"
#include "parameters/generalparameters.h"
#include "result/resultparameters.h"
#include "interactive/parameterdialogresults.h"

//#include "device/mixer/mixerfactory.h"

//#include "gui/mainwindow.h"
//#include "gui/mainwindowconfig.h"
//#include "screen/screenrundelegate.h"

//#include "parameters/apexmapparameters.h"
#include "parameters/parametermanagerdata.h"



//#include "stimulus/stimulusfactory.h"
#include "stimulus/stimulidata.h"
#include "stimulus/stimulusdata.h"

#include "xml/xmlkeys.h"

//#include "apexcontrol.h"
//#include "apexfactories.h"
//#include "experimentconfigfileparser.h"
#include "experimentdata.h"

#include "apextools.h"

#include <memory>

using namespace apex::XMLKeys;

namespace apex
{
namespace data
{

class ExperimentDataPrivate
{
    public:
        QString fileName;

        // Owned auto_ptrs obtained from the parser
//    std::auto_ptr<MainWindowConfig> mainWindowConfig;
        std::auto_ptr<ScreensData> screens;
        std::auto_ptr<ApexProcedureConfig> procedureConfig;
        std::auto_ptr<CorrectorData> correctorData;
        std::auto_ptr<ConnectionsData> connectionDatas;
        std::auto_ptr<CalibrationData> calibrationData;
        std::auto_ptr<GeneralParameters> generalParameters;
        std::auto_ptr<ResultParameters> resultParameters;
        //std::auto_ptr<ApexMapParameters> fixedParameters;
        std::auto_ptr<ParameterDialogResults> parameterDialogResults;
        std::auto_ptr<ParameterManagerData> parameterManagerData;
        std::auto_ptr<QMap<QString, RandomGeneratorParameters*> > randomgenerators;
        std::auto_ptr<DevicesData> devices;
        std::auto_ptr<DevicesData>  controldevices;
        std::auto_ptr<DatablocksData> datablocksdata;
        std::auto_ptr<FiltersData> filtersdata;
        std::auto_ptr<StimuliData>     stimuli;
        QString experimentDescription;
        //QString masterDevice;
};
}
}

using namespace apex::data;
using namespace apex;

ExperimentData::ExperimentData (const QString& configFile,
                        std::auto_ptr<data::ScreensData>& screens,
                        std::auto_ptr<data::ApexProcedureConfig>& procedures,
                        std::auto_ptr<data::CorrectorData>& correctors,
                        std::auto_ptr<data::ConnectionsData>& connections,
                        std::auto_ptr<data::CalibrationData>& calibration,
                        std::auto_ptr<GeneralParameters>& genParam,
                        std::auto_ptr<ResultParameters>& resParam,
                        std::auto_ptr<ParameterDialogResults>& paramDlg,
                        std::auto_ptr<QMap<QString, RandomGeneratorParameters*> >& rndGen,
                        std::auto_ptr<data::DevicesData>& devices,
                        std::auto_ptr<data::FiltersData>& filters,
                        std::auto_ptr<data::DevicesData>& controlDevices,
                        std::auto_ptr<data::DatablocksData>& datablocks,
                        std::auto_ptr<data::StimuliData>& stimuli,
                        const QString& description,
                        std::auto_ptr<data::ParameterManagerData>& paramMngr)
        : d(new ExperimentDataPrivate())
{
    d->fileName = configFile;
    d->screens  = screens;
    d->procedureConfig  = procedures;
    d->correctorData = correctors;
    d->connectionDatas = connections;
    d->calibrationData = calibration;
    d->generalParameters = genParam;
    d->resultParameters = resParam;
    d->parameterDialogResults = paramDlg;
    d->randomgenerators = rndGen;
    d->devices = devices;
    d->filtersdata = filters;
    d->controldevices = controlDevices;
    d->datablocksdata = datablocks;
    d->stimuli = stimuli;
    d->experimentDescription = description;
    d->parameterManagerData = paramMngr;
}



ExperimentData::~ExperimentData()
{
    //FIXME [job] shouldn't these deletes happen in the destructors of the
    //classes that are iterated

    // remove device data from map
    for (data::DevicesData::const_iterator it = d->devices->begin();
         it!= d->devices->end(); ++it)
    {
        Q_CHECK_PTR( it.value());
        delete it.value();
    }

    // remove filter data from map
    for (data::FiltersData::const_iterator it = d->filtersdata->begin();
         it!= d->filtersdata->end(); ++it)
    {
        Q_CHECK_PTR( it.value());
        delete it.value();
    }

    // remove datablocks from map
    for (data::DatablocksData::const_iterator it = d->datablocksdata->begin();
         it!= d->datablocksdata->end(); ++it)
    {
        Q_CHECK_PTR( it.value());
        delete it.value();
    }

    // No auto ptr to minimize includes in public header file
    delete d;
  }

const QString& ExperimentData::fileName() const
{
    return d->fileName;
}

void ExperimentData::setFileName (const QString& fileName)
{
    d->fileName = fileName;
}

const CorrectorData* ExperimentData::correctorData() const
{
    return d->correctorData.get();
}

ApexProcedureConfig* ExperimentData::procedureConfig() const
{
    return d->procedureConfig.get();
}

const ApexTrial* ExperimentData::trialById (const QString& name) const
{
    return d->procedureConfig->GetTrial (name);
}

ApexTrial* ExperimentData::trialById (const QString& name)
{
    return d->procedureConfig->GetTrial (name);
}

const Screen& ExperimentData::screenById (const QString& id) const
{
    return d->screens->GetScreen (id);
}

const ConnectionsData* ExperimentData::connectionsData() const
{
    return d->connectionDatas.get();
}

ConnectionsData* ExperimentData::connectionsData()
{
    return d->connectionDatas.get();
}

const ScreensData* ExperimentData::screensData() const
{
    return d->screens.get();
}

QMap<QString, RandomGeneratorParameters*> ExperimentData::randomGenerators() const
{
    return *d->randomgenerators;
}

CalibrationData *ExperimentData::calibrationData() const
{
    return d->calibrationData.get();
}

const GeneralParameters* ExperimentData::generalParameters() const
{
    return d->generalParameters.get();
}

GeneralParameters* ExperimentData::generalParameters()
{
    return d->generalParameters.get();
}

ResultParameters* ExperimentData::resultParameters()
{
    return d->resultParameters.get();
}

FixedParameterList ExperimentData::fixedParameters() const
{
    return d->stimuli->GetFixedParameters();
}

const StimulusData* ExperimentData::stimulusById(const QString& p_name) const
{
    if( !d->stimuli->contains( p_name ) )
        throw( ApexStringException(tr("ExperimentData::GetStimulus: error: stimulus %1 not found").arg(p_name)));

    const data::StimulusData& p = d->stimuli->value( p_name );
    return &p;
}

const DeviceData* ExperimentData::deviceById(const QString& p_name) const
{
    data::DevicesData::const_iterator i=d->devices->find(p_name);
    Q_ASSERT (i!=d->devices->end());
    return (*d->devices)[p_name];
}

QString ExperimentData::experimentDescription() const
{
    return d->experimentDescription;
}

const DevicesData* ExperimentData::controlDevices() const
{
    return d->controldevices.get();
}

const ParameterDialogResults* ExperimentData::parameterDialogResults() const
{
    return d->parameterDialogResults.get();
}

bool ExperimentData::isFixedParameter(const QString& p_name) const
{
    return d->stimuli->GetFixedParameters().contains( p_name );
}

const DevicesData* ExperimentData::devicesData() const
{
    return d->devices.get();
}

const StimuliData* ExperimentData::stimuliData() const
{
    return d->stimuli.get();
}

const FiltersData* ExperimentData::filtersData() const
{
    return d->filtersdata.get();
}

const ResultParameters* ExperimentData::resultParameters() const
{
    return d->resultParameters.get();
}

const DatablocksData* ExperimentData::datablocksData( ) const
{
    return d->datablocksdata.get();
}

const ParameterManagerData* ExperimentData::parameterManagerData() const
{
    return d->parameterManagerData.get();
}

void ExperimentData::addFilter(FilterData* filter)
{
    d->filtersdata->insert(filter->id(), filter);
}

#define ARE_EQUAL(a, b) \
    ((a.get() == 0 && b.get() == 0) ? true :\
    ((a.get() == 0 || b.get() == 0) ? false :\
    *a == *b))

bool ExperimentData::operator==(const ExperimentData& other) const
{
    if (!(*d->screens == *other.d->screens))
    {
        qDebug("screensdata not equal");
        return false;
    }
    if (!(*d->procedureConfig == *other.d->procedureConfig))
    {
        qDebug("procedure config not equal");
        return false;
    }
    if (!(*d->correctorData == *other.d->correctorData))
    {
        qDebug("correctordata not equal");
        return false;
    }
    if (!(*d->connectionDatas == *other.d->connectionDatas))
    {
        qDebug("connectiondata not equal");
        return false;
    }
    //if (!(*d->calibrationData == *other.d->calibrationData))
    if (!ARE_EQUAL(d->calibrationData, other.d->calibrationData))
    {
        qDebug("calibrationdata not equal");
        return false;
    }
    if (!(*d->generalParameters == *other.d->generalParameters))
    {
        qDebug("general parameters not equal");
        return false;
    }
    if (!(*d->resultParameters == *other.d->resultParameters))
    {
        qDebug("result parameters not equal");
        return false;
    }
    //if (!(*d->parameterDialogResults == *other.d->parameterDialogResults))
    if (!ARE_EQUAL(d->parameterDialogResults, other.d->parameterDialogResults))
    {
        qDebug("parameter dialog results not equal");
        return false;
    }
    if (!(*d->parameterManagerData == *other.d->parameterManagerData))
    {
        qDebug("parameter manager data not equal");
        return false;
    }
    if (!ApexTools::areEqualPointerMaps(*d->randomgenerators,
                                           *other.d->randomgenerators))
    {
        qDebug("random generators not equal");
        return false;
    }
    if (!(*d->devices == *other.d->devices))
    {
        qDebug("devicesdata not equal");
        return false;
    }
    if (!(*d->controldevices == *other.d->controldevices))
    {
        qDebug("controldevices not equal");
        return false;
    }
    if (!(*d->datablocksdata == *other.d->datablocksdata))
    {
        qDebug("datablocksdata not equal");
        return false;
    }
    if (!(*d->filtersdata == *other.d->filtersdata))
    {
        qDebug("filtersdata not equal");
        return false;
    }
    if (!(*d->stimuli == *other.d->stimuli))
    {
        qDebug("stimulidata not equal");
        return false;
    }
    if (!(d->experimentDescription == other.d->experimentDescription))
    {
        qDebug("description not equal");
        return false;
    }

    return true;
}




























