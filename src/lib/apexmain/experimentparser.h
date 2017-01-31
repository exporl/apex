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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_EXPERIMENTPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_EXPERIMENTPARSER_H_

#include "apexdata/calibration/calibrationdata.h"

#include "apexdata/connection/connectiondata.h"

#include "apexdata/datablock/datablocksdata.h"

#include "apexdata/device/devicesdata.h"

#include "apexdata/filter/filtersdata.h"

#include "apexdata/interactive/parameterdialogresults.h"

#include "apexdata/parameters/generalparameters.h"

#include "apexdata/result/resultparameters.h"

#include "apextools/apextypedefs.h"
#include "apextools/version.h"

#include "apextools/xml/xercesinclude.h"

#include "services/errorhandler.h"

#include "configfileparser.h"

#include <QMap>
#include <QString>

#include <vector>

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
class ProcedureData;
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
public:

    // FIXME: this should (together with interactive) probably be moved to a separate class
    ExperimentParser(const QString& configFilename, QMap<QString, QString> expressions = QMap<QString, QString>());
    ~ExperimentParser();

    // if interactive==true, the parse will interact with the user via a GUI
    data::ExperimentData* parse(bool interactive);
    QString getUpgradedConfigFile(); //Needed by the standalone experiment upgrader.

protected:
    virtual bool upgradeFrom(QDomDocument& doc, const QVector<int>& v);

private:
    void upgradeTo3_1_0( QDomDocument& doc );
    void upgradeTo3_1_1( QDomDocument& doc );
    //currently at 3.1.3

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
    bool CheckStimulusDatablocks(data::StimulusDatablocksContainer datablocks, QString trial/*just for error message*/);
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

    void expandTrials(DOMElement* p_base);

    void AddStatusMessage(QString p_message);
    void StatusMessageDone();

private:
    bool            m_interactive;
    QString         m_description;      // experiment description

    //has to be kept since StimulusOutput is not constructed at the time of parsing it
    //would be nicer to have a struct with filters, devices, xtra info
    QString m_sMasterDevice;

    StatusReporter &m_progress;

    //FIXME should all be QScopedPointer
    QScopedPointer<data::ScreensData> screens;
    QScopedPointer<data::ProcedureData> procedureData;
    QScopedPointer<data::ConnectionsData> connectionDatas;
    QScopedPointer<data::CalibrationData> m_CalibrationData;
    QScopedPointer<data::GeneralParameters> m_generalParameters;
    QScopedPointer<data::ResultParameters> m_resultParameters;
    QScopedPointer<data::ParameterDialogResults> m_parameterDialogResults;
    QMap<QString, data::RandomGeneratorParameters*> m_randomgenerators;
    data::DevicesData m_devicesdata;
    data::FiltersData m_filtersdata;
    data::DevicesData m_controldevicesdata;
    data::DatablocksData m_datablocksdata;
    QScopedPointer<data::StimuliData> m_stimuli;
    QScopedPointer<data::ParameterManagerData> parameterManagerData;

    QString upgradedDoc; //Needed for the standalone experiment upgrader.
    QMap<QString, QString> expressions;
};

}

#endif
