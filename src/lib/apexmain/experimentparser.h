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

#ifndef APEXEXPERIMENTCONFIGFILEPARSER_H
#define APEXEXPERIMENTCONFIGFILEPARSER_H

#include "version.h"
#include "apextypedefs.h"
#include "services/errorhandler.h"
#include "configfileparser.h"

//from libdata
#include "corrector/correctordata.h"
#include "connection/connectiondata.h"  // we have an autoptr to ConnectionsData
#include "device/devicesdata.h"         // we have a member...
#include "datablock/datablocksdata.h"
#include "filter/filtersdata.h"
#include "interactive/parameterdialogresults.h"
#include "result/resultparameters.h"
#include "parameters/generalparameters.h"
#include "calibration/calibrationdata.h"

#include <QString>
#include <QMap>

//FIXME [job refactory] replace with Qt data
#include <vector>

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
}

using namespace apex::gui;
using namespace apex::stimulus;

namespace apex
{

class ApexProcedure;
class ConfigFileParser;
class MainConfigFileParser;
class ApexErrorHandler;

namespace device
{
class IMixer;
class IApexDevice;
}

namespace gui
{
class ScreenParser;
}

namespace data
{
//class ConnectionData;
//class ConnectionsData;
class DevicesData;
class ParameterManagerData;
class StimuliData;
struct StimulusDatablocksContainer;
class ScreensData;
class ApexMapParameters;
class ApexTrial;
class RandomGeneratorParameters;
class ResultParameters;
class GeneralParameters;
class ParameterDialogResults;
class ApexProcedureConfig;
class ExperimentData;
class FilePrefix;
}//ns apex::data

namespace stimulus
{
class OutputDevice;
}

/**
 * Validates and parses an xml experiment file
 *
 * @author Tom Francart,,,
 */
class APEX_EXPORT ExperimentParser: public ConfigFileParser
{
        //Q_OBJECT      // use tr()

    public:

        ExperimentParser(const QString& configFilename);

        /**
         * If interactive==true, the parse will interact with the user via a GUI
         */
        data::ExperimentData* parse(bool interactive);

        ~ExperimentParser();

    private:

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::CorrectorData>& GetCorrectorData()
        {
            return correctorData;
        }

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::ScreensData>& GetScreens()
        {
            return screens;
        }

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::ApexProcedureConfig>& GetProcedureConfig()
        {
            return procedureData;
        }

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::ConnectionsData>& GetConnectionDatas()
        {
            return connectionDatas;
        }

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::CalibrationData>& GetCalibrationData()
        {
            return m_CalibrationData;
        }

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::GeneralParameters>& GetGeneralParameters()
        {
        return m_generalParameters;
        }

        /// Transfers auto_ptr ownership!
        std::auto_ptr<data::ResultParameters>& GetResultParameters()
        {
        return m_resultParameters;
        }

//         std::auto_ptr<data::ApexMapParameters>& GetFixedParameters()
//         {
//         return m_fixedParameters;
//         }

        std::auto_ptr<data::ParameterDialogResults>& GetParameterDialogResults()
        {
        return m_parameterDialogResults;
        }

        std::auto_ptr<data::ParameterManagerData>& GetParameterManagerData()
        {
            return parameterManagerData;
        }

        std::auto_ptr<data::StimuliData>& GetStimuliData()
        {
            return m_stimuli;
        }

        //FIXME all these methods should return auto_ptrs
        data::DevicesData&                 GetDevicesData() { return m_devicesdata; }
        const data::FiltersData&           GetFiltersData() const    { return m_filtersdata; }
        data::DatablocksData&          GetDatablocksData() { return m_datablocksdata;};
        const QString&              GetExperimentDescription() const { return m_description; };
        const device::tControllerMap&       GetControllers() { return m_controldevices; }
        const data::DevicesData   GetControlDevices() { return m_controldevicesdata; }
        QMap<QString, data::RandomGeneratorParameters*>& GetRandomGenerators() {return m_randomgenerators;};
        const data::ParameterDialogResults* GetParameterDialogResults() const;

    protected:
        virtual bool upgradeFrom(QDomDocument& doc, const QVector<int>& v);

    private:
        void upgradeTo302( QDomDocument& doc );


        bool ApplyXpathModifications();

        bool Parsefile ();
        bool ParseDescription( XERCES_CPP_NAMESPACE::DOMElement* p_base );
        bool ParseFilters( XERCES_CPP_NAMESPACE::DOMElement* p_filters );
        bool ParseFilter( XERCES_CPP_NAMESPACE::DOMElement* p_filter );
        bool ParseDatablocks(XERCES_CPP_NAMESPACE::DOMElement* p_datablocks);
        bool ParseDevices(XERCES_CPP_NAMESPACE::DOMElement* p_base);
        device::IApexDevice* ParseExtDevice( XERCES_CPP_NAMESPACE::DOMElement* a_pDdevice );
        bool ParseScreens(XERCES_CPP_NAMESPACE::DOMElement* p_base );
        bool ParseProcedure(XERCES_CPP_NAMESPACE::DOMElement* p_base);
        bool ParseStimuli(XERCES_CPP_NAMESPACE::DOMElement* p_base);
        bool ParseCorrector(XERCES_CPP_NAMESPACE::DOMElement* p_base);
        bool ParseConnections(XERCES_CPP_NAMESPACE::DOMElement* p_connections);
        QString GetDeviceForConnection(data::ConnectionData* cd);

        bool ParseRandomGenerators(XERCES_CPP_NAMESPACE::DOMElement* p_base );
        bool ParseRandomGenerator(XERCES_CPP_NAMESPACE::DOMElement* p_base);

        bool ParseResults(XERCES_CPP_NAMESPACE::DOMElement* p_base );
        bool ParseCalibration( XERCES_CPP_NAMESPACE::DOMElement* p_base );
        bool ParseGeneral(XERCES_CPP_NAMESPACE::DOMElement* p_base );

        //QString AddPrefix(const QString& p_base, const QString& prefix);


        bool DoExtraValidation();
        bool CheckProcedure();
        bool CheckFixedParameters();
        bool CheckAnswers();
        bool CheckShowResults();
        bool CheckTrials();
        bool CheckStimuli();
        bool CheckStimulusDatablocks(data::StimulusDatablocksContainer datablocks,
                                    QString trial/*just for error message*/);
        bool CheckDatablocks();
        bool CheckFilters();
        bool CreateMissing();
        bool CheckDevices();
        bool CheckRandomGenerators();

        bool FixStimuli();

        virtual const QString getConfigfileNamespace()
        {
            return EXPERIMENT_NAMESPACE;
        }

        void AddStatusMessage(QString p_message);
        void StatusMessageDone();

    // data
    private:

        // a hash_map as described in stroustrup would be better
            //FIXME should all be auto_ptrs
        bool            m_interactive;
        QString         m_description;      // experiment description
        tDeviceMap      m_devices;
        data::DevicesData    m_devicesdata;
        data::DevicesData   m_controldevicesdata;
        //tFilterMap      m_filters;
        device::tControllerMap  m_controldevices;
        //tDataBlockMap   m_datablocks;
        data::DatablocksData m_datablocksdata;
        data::FiltersData m_filtersdata;
        //tStimuliMap     m_stimuli;
        std::auto_ptr<data::StimuliData> m_stimuli;
        QMap<QString, data::RandomGeneratorParameters*> m_randomgenerators;

            //has to be kept since StimulusOutput is not constructed at the time of parsing it
            //would be nicer to have a struct with filters, devices, xtra info
        QString                             m_sMasterDevice;
        //bool                                m_bAllConnected;

        StatusReporter&            m_progress;
        //std::auto_ptr<data::ApexMapParameters>    m_fixedParameters;  // list of given fixed parameters in the <stimulus> block
        std::auto_ptr<data::CalibrationData> m_CalibrationData;
        std::auto_ptr<data::ResultParameters>     m_resultParameters;
        std::auto_ptr<data::GeneralParameters>    m_generalParameters;
        std::auto_ptr<data::ParameterDialogResults> m_parameterDialogResults;
        //    ScreenParser*         m_screenParser;

        // The following fields manage the auto_ptrs until the corresponding
        // method is called, which transfer the ownership to the caller
        //std::auto_ptr<MainWindowConfig> mainWndConfig;
        std::auto_ptr<data::ScreensData> screens;
        //     std::auto_ptr<ApexProcedureFactory> procedureFactory;
        std::auto_ptr<data::ApexProcedureConfig> procedureData;
        std::auto_ptr<data::CorrectorData> correctorData;
        std::auto_ptr<data::ConnectionsData> connectionDatas;
        std::auto_ptr<data::ParameterManagerData> parameterManagerData;
};

}

#endif
