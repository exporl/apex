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

#ifndef _APEX_SRC_LIB_APEXMAIN_EXPERIMENTPARSER_H_
#define _APEX_SRC_LIB_APEXMAIN_EXPERIMENTPARSER_H_

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

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "upgradablexmlparser.h"

#include <QMap>
#include <QString>

namespace apex
{

namespace device
{
class IApexDevice;
}

namespace data
{
class ParameterManagerData;
class StimuliData;
struct StimulusDatablocksContainer;
class ScreensData;
class ApexTrial;
class RandomGeneratorParameters;
class ResultParameters;
class GeneralParameters;
class ParameterDialogResults;
class ProcedureData;
class ExperimentData;
class FilePrefix;
}

/**
 * Validates and parses an xml experiment file
 *
 * @author Tom Francart,,,
 */
class APEX_EXPORT ExperimentParser : public UpgradableXmlParser
{
    Q_OBJECT
public:
    // FIXME: this should (together with interactive) probably be moved to a
    // separate class
    ExperimentParser(
        const QString &configFilename,
        const QMap<QString, QString> &expressions = QMap<QString, QString>());
    ~ExperimentParser();

    // if interactive==true, the parse will interact with the user via a GUI
    data::ExperimentData *parse(bool interactive, bool expand = false);

private Q_SLOTS:
    void upgrade3_0_2();
    void upgrade3_1_0();
    void upgrade3_1_1();
    void upgrade3_1_3();

private:
    bool ApplyXpathModifications();

    bool Parsefile(bool expand);
    bool ParseDescription(const QDomElement &p_base);
    bool ParseFilters(const QDomElement &p_filters);
    bool ParseFilter(const QDomElement &p_filter);
    bool ParseDatablocks(const QDomElement &p_datablocks, bool expand);
    bool ParseDevices(const QDomElement &p_base);
    device::IApexDevice *ParseExtDevice(const QDomElement &a_pDdevice);
    bool ParseScreens(const QDomElement &p_base);
    bool ParseProcedure(const QDomElement &p_base, bool expand);
    bool ParseStimuli(const QDomElement &p_base, bool expand);
    bool ParseConnections(const QDomElement &p_connections);
    QString GetDeviceForConnection(data::ConnectionData *cd);

    bool ParseRandomGenerators(const QDomElement &p_base);
    bool ParseRandomGenerator(const QDomElement &p_base);

    bool ParseResults(const QDomElement &p_base);
    bool ParseCalibration(const QDomElement &p_base);
    bool ParseGeneral(const QDomElement &p_base);

    bool DoExtraValidation();
    bool CheckProcedure();
    bool CheckFixedParameters();
    bool CheckAnswers();
    bool CheckShowResults();
    bool CheckTrials();
    bool CheckStimuli();
    bool CheckStimulusDatablocks(data::StimulusDatablocksContainer datablocks,
                                 QString trial /*just for error message*/);
    bool CheckDatablocks();
    bool CheckFilters();
    bool CreateMissing();
    bool CheckDevices();
    bool CheckRandomGenerators();

    bool FixStimuli();

    void expandTrials(const QDomElement &p_base);

    void AddStatusMessage(QString p_message);
    void StatusMessageDone();

private:
    bool m_interactive;
    QString m_description; // experiment description

    // has to be kept since StimulusOutput is not constructed at the time of
    // parsing it
    // would be nicer to have a struct with filters, devices, xtra info
    QString m_sMasterDevice;

    // FIXME should all be QScopedPointer
    QScopedPointer<data::ScreensData> screens;
    QScopedPointer<data::ProcedureData> procedureData;
    QScopedPointer<data::ConnectionsData> connectionDatas;
    QScopedPointer<data::CalibrationData> m_CalibrationData;
    QScopedPointer<data::GeneralParameters> m_generalParameters;
    QScopedPointer<data::ResultParameters> m_resultParameters;
    QScopedPointer<data::ParameterDialogResults> m_parameterDialogResults;
    QMap<QString, data::RandomGeneratorParameters *> m_randomgenerators;
    data::DevicesData m_devicesdata;
    data::FiltersData m_filtersdata;
    data::DevicesData m_controldevicesdata;
    data::DatablocksData m_datablocksdata;
    QScopedPointer<data::StimuliData> m_stimuli;
    QScopedPointer<data::ParameterManagerData> parameterManagerData;

    QMap<QString, QString> expressions;
};
}

#endif
