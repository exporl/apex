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

#include "apexdata/calibration/calibrationdata.h"

#include "apexdata/connection/connectiondata.h"

#include "apexdata/datablock/datablockdata.h"
#include "apexdata/datablock/datablocksdata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/interactive/parameterdialogresults.h"

#include "apexdata/parameters/apexparameters.h"
#include "apexdata/parameters/generalparameters.h"
#include "apexdata/parameters/parametermanagerdata.h"

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/procedureinterface.h"
#include "apexdata/procedure/trialdata.h"

#include "apexdata/randomgenerator/randomgeneratorparameters.h"

#include "apexdata/result/resultparameters.h"

#include "apexdata/screen/screensdata.h"

#include "apexdata/stimulus/stimulidata.h"
#include "apexdata/stimulus/stimulusparameters.h"

#include "apextools/apexpaths.h"
#include "apextools/apexpluginloader.h"
#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "calibration/calibrationparser.h"

#include "common/exception.h"
#include "common/pluginloader.h"

#include "connection/connection.h"
#include "connection/connectionparser.h"

#include "datablock/datablocksparser.h"

#include "device/devicesparser.h"
#include "device/iapexdevice.h"

#include "filter/filterparser.h"

#include "gui/guidefines.h"
#include "gui/mainwindow.h"

#include "interactive/interactiveparameters.h"
#include "interactive/interactiveparametersdialog.h"

#include "parser/prefixparser.h"
#include "parser/scriptexpander.h"

#include "screen/screensparser.h"

#include "stimulus/datablock.h"
#include "stimulus/filter.h"
#include "stimulus/outputdevice.h"
#include "stimulus/stimuliparser.h"
#include "stimulus/stimulus.h"

#include "experimentparser.h"

#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QFileInfo>
#include <QMessageBox>
#include <QXmlQuery>

#include <limits>

using namespace apex;
using namespace apex::device;
using namespace apex::XMLKeys;
using namespace cmn;

apex::ExperimentParser::ExperimentParser(
    const QString &configFilename, const QMap<QString, QString> &expressions)
    : UpgradableXmlParser(configFilename, ApexPaths::GetExperimentSchemaPath(),
                          QL1S(EXPERIMENT_SCHEMA_URL), QL1S(APEX_NAMESPACE)),
      m_interactive(true),
      connectionDatas(new data::ConnectionsData()),
      m_resultParameters(new data::ResultParameters()),
      expressions(expressions)
{
}

data::ExperimentData *apex::ExperimentParser::parse(bool interactive,
                                                    bool expand)
{
    loadAndUpgradeDom();

    m_interactive = interactive;

    qCDebug(APEX_RS, "ExperimentParser: parsing...");

    if (!(!interactive || ApplyXpathModifications()) || !Parsefile(expand))
        throw ParseException();

    return new data::ExperimentData(
        fileName, screens.take(), procedureData.take(), connectionDatas.take(),
        m_CalibrationData.take(), m_generalParameters.take(),
        m_resultParameters.take(), m_parameterDialogResults.take(),
        new QMap<QString, data::RandomGeneratorParameters *>(
            m_randomgenerators),
        new data::DevicesData(m_devicesdata),
        new data::FiltersData(m_filtersdata),
        new data::DevicesData(m_controldevicesdata),
        new data::DatablocksData(m_datablocksdata), m_stimuli.take(),
        m_description, parameterManagerData.take());
}

/**apex::ExperimentParser::
 * Apply modifications stated in the experiment file based on user input
 * @return
 */
bool apex::ExperimentParser::ApplyXpathModifications()
{
    gui::ApexMainWindow *mwp = 0;
    if (m_interactive)
        mwp = ApexControl::Get().mainWindow();

    InteractiveParameters ip(document);
    if (!expressions.isEmpty()) {
        /* Add the expressions to the interactive results.
         * That way they will be visible in the results file regardless.
         */
        m_parameterDialogResults.reset(new data::ParameterDialogResults);
        m_parameterDialogResults->append(ip.applyExpressions(expressions));
        document = ip.document();
    }

    bool result = false;
    do {
        InteractiveParametersDialog ipd(&ip, mwp);
        if (ip.entries().isEmpty())
            return true;
        result = ipd.exec();

        if (result) {
            if (!ipd.apply()) {
                result = false;
                continue;
            }
            document = ip.document();
        } else {
            result = QMessageBox::warning(
                         mwp, tr("Interactive changes not applied"),
                         tr("Warning: no changes were applied, do you want to "
                            "continue"
                            " with the unmodified document?"),
                         QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;
        }
        if (!m_parameterDialogResults)
            m_parameterDialogResults.reset(new data::ParameterDialogResults);
        m_parameterDialogResults->append(*ip.results());
    } while (!result);

    return result;
}

apex::ExperimentParser::~ExperimentParser()
{
}

bool apex::ExperimentParser::Parsefile(bool expand)
{
    // set current path to experiment file path
    QDir::setCurrent(QFileInfo(fileName).path());

    parameterManagerData.reset(new data::ParameterManagerData());

    bool bSuccess = true;
    QString currentTag = "general";

    // first parse general
    QDomElement baseNode = document.documentElement();
    QDomElement generalNode = baseNode.firstChildElement(QSL("general"));

    if (!generalNode.isNull())
        bSuccess &= ParseGeneral(generalNode);
    else
        m_generalParameters.reset(new data::GeneralParameters());

    for (QDomElement currentNode = baseNode.firstChildElement();
         !currentNode.isNull() && bSuccess;
         currentNode = currentNode.nextSiblingElement()) {
        currentTag = currentNode.tagName();
        qCDebug(APEX_RS, "Parsing %s", qPrintable(currentTag));

        if (currentTag == "description")
            bSuccess = ParseDescription(currentNode);
        else if (currentTag == "filters")
            bSuccess = ParseFilters(currentNode);
        else if (currentTag == "datablocks")
            bSuccess = ParseDatablocks(currentNode, expand);
        else if (currentTag == "procedure")
            bSuccess = ParseProcedure(currentNode, expand);
        else if (currentTag == "stimuli")
            bSuccess = ParseStimuli(currentNode, expand);
        else if (currentTag == "devices")
            bSuccess = ParseDevices(currentNode);
        else if (currentTag == "screens")
            bSuccess = ParseScreens(currentNode);
        else if (currentTag == "connections")
            bSuccess =
                ParseConnections(currentNode); // connections should be parsed
                                               // AFTER datablocks, devices and
                                               // filters because they need to
                                               // lookup their device
        else if (currentTag == "randomgenerators")
            bSuccess = ParseRandomGenerators(currentNode);
        else if (currentTag == "calibration")
            bSuccess = ParseCalibration(currentNode);
        else if (currentTag == "results")
            bSuccess = ParseResults(currentNode);
        else if (currentTag == "general")
            ;
        // skipping, already parsed before
        else if (currentTag == "interactive")
            ;
        // NOP
        else {
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg(
                          "ExperimentParser::Parsefile",
                          tr("Unknown tag \"%1\"").arg(currentTag))));
        }
    }

    if (!bSuccess) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ExperimentParser::Parsefile",
                       tr("Error parsing element \"%1\"").arg(currentTag))));

        return false;
    }

    // register fixed parameters of stimuli
    // FIXME find a better place to do this
    Q_FOREACH (QString fixedParameter, m_stimuli->GetFixedParameters()) {
        data::Parameter parameter;
        parameter.setId(fixedParameter);
        parameter.setOwner("stimuli");
        parameter.setType("fixed");
        parameterManagerData->registerParameter(fixedParameter, parameter);
    }

    if (!DoExtraValidation())
        return false;

    if (!CreateMissing())
        return false;

    return true;
}

bool apex::ExperimentParser::ParseConnections(const QDomElement &p_base)
{
    AddStatusMessage(tr("Processing Connections"));

    parser::ConnectionParser parser;

    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag(currentNode.tagName());

        if (tag == "connection") {
            data::ConnectionData *cd = parser.Parse(currentNode);
            QString dev(GetDeviceForConnection(cd));
            cd->setDevice(dev);

            QString id = cd->fromChannelId();
            if (!id.isEmpty())
                parameterManagerData->registerParameter(
                    id, data::Parameter(dev, "connection-" + id,
                                        cd->fromChannel(), -1, true));
            id = cd->toChannelId();
            if (!id.isEmpty())
                parameterManagerData->registerParameter(
                    id, data::Parameter(dev, "connection-" + id,
                                        cd->toChannel(), -1, true));

            // check if this connection already exists
            for (data::ConnectionsData::const_iterator it =
                     connectionDatas->begin();
                 it != connectionDatas->end(); ++it) {
                if ((**it).duplicateOf(*cd)) {
                    qCWarning(APEX_RS, "%s",
                              qPrintable(QSL("%1: %2").arg(
                                  "Connections",
                                  tr("Duplicate connection from %1 to %2")
                                      .arg(cd->fromId())
                                      .arg(cd->toId()))));
                }
            }

            connectionDatas->push_back(cd);
        } else {
            qFatal("Unknown tag in a connection: %s", qPrintable(tag));
        }
    }

    StatusMessageDone();
    return true;
}

/**
 * iterate over all datablocks, devices and filters to find the device of the
 * connection
 */
QString apex::ExperimentParser::GetDeviceForConnection(data::ConnectionData *cd)
{
    QString targetID = cd->fromId();

    // search devices
    try {
        data::DeviceData *d = m_devicesdata.deviceData(targetID);
        return d->id();
    } catch (...) {
    }

    try {
        const data::FilterData *d = m_filtersdata.filterData(targetID);
        return d->device();
    } catch (...) {
    }

    try {
        const data::DatablockData *d = m_datablocksdata.datablockData(targetID);
        return d->device();
    } catch (...) {
    }

    targetID = cd->toId();

    // search devices
    try {
        data::DeviceData *d = m_devicesdata.deviceData(targetID);
        return d->id();
    } catch (...) {
    }

    try {
        const data::FilterData *d = m_filtersdata.filterData(targetID);
        return d->device();
    } catch (...) {
    }

    try {
        const data::DatablockData *d = m_datablocksdata.datablockData(targetID);
        return d->device();
    } catch (...) {
    }

    throw ApexStringException("Error: device not found for connection");
}

bool apex::ExperimentParser::ParseCalibration(const QDomElement &a_pCalib)
{
    AddStatusMessage(tr("Processing calibration"));

    parser::CalibrationParser parser;
    m_CalibrationData.reset(parser.Parse(a_pCalib));

    StatusMessageDone();
    return true;
}

bool apex::ExperimentParser::ParseFilters(const QDomElement &p_filters)
{
    AddStatusMessage(tr("Processing filters"));

    for (QDomElement currentNode = p_filters.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "filter") {
            if (!ParseFilter(currentNode))
                return false;
        } else {
            qCWarning(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(
                                         "ExperimentParser::ParseFilters",
                                         "Unknown tag: " + tag)));
        }
    }

    StatusMessageDone();
    return true;
}

bool apex::ExperimentParser::ParseFilter(const QDomElement &p_filter)
{
    const QString type(p_filter.attribute(sc_sType));
    const QString id(p_filter.attribute(sc_sID));
    if (type.isEmpty() || id.isEmpty())
        return false;

    parser::FilterParser parser;

    // create parameters
    data::FilterData *parameters =
        parser.ParseFilter(p_filter, parameterManagerData.data());
    if (!parameters)
        return false;

    m_filtersdata[id] = parameters;
    return true;
}

bool apex::ExperimentParser::ParseDevices(const QDomElement &p_base)
{
    qCDebug(APEX_RS, "Parsing devices");
    parser::DevicesParser parser;
    try {
        parser::tAllDevices all =
            parser.Parse(p_base, parameterManagerData.data());
        m_devicesdata = all.outputdevices;
        m_controldevicesdata = all.controldevices;
    } catch (const std::exception &e) {
        qCCritical(APEX_RS, "ParseDevices: %s", e.what());
        return false;
    }

    return true;
};

bool apex::ExperimentParser::ParseDatablocks(const QDomElement &p_datablocks,
                                             bool expand)
{
    parser::DatablocksParser parser(
        m_interactive ? ApexControl::Get().mainWindow() : NULL);

    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    m_datablocksdata =
        parser.Parse(fileName, p_datablocks, scriptLibrary,
                     m_generalParameters->scriptParameters(), expand);

    return true;
}

bool apex::ExperimentParser::ParseScreens(const QDomElement &p_base)
{
    AddStatusMessage("Parsing Screens");

    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    gui::ApexMainWindow *mwp = 0;
    if (m_interactive)
        mwp = ApexControl::Get().mainWindow();
    parser::ScreensParser parser(mwp);

    screens.reset(parser.ParseScreens(fileName, p_base, scriptLibrary,
                                      m_generalParameters->scriptParameters(),
                                      parameterManagerData.data()));

    if (!screens)
        return false;

    StatusMessageDone();
    return true;
}

void apex::ExperimentParser::expandTrials(const QDomElement &p_base)
{
    // get the main window so we can draw message boxes if necessary
    gui::ApexMainWindow *mwp = NULL;
    if (m_interactive)
        mwp = ApexControl::Get().mainWindow();

    // fetch the script
    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    // find trials block
    QDomElement trialNode = p_base.firstChildElement(QSL("trials"));

    // find plugin trials and expand them
    for (QDomElement currentNode = trialNode.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag(currentNode.tagName());
        if (tag == "plugintrials") {
            qCDebug(APEX_RS, "Script library: %s", qPrintable(scriptLibrary));
            parser::ScriptExpander expander(
                fileName, scriptLibrary,
                m_generalParameters->scriptParameters(), mwp);
            expander.ExpandScript(currentNode, "getTrials");
        }
    }
}

bool apex::ExperimentParser::ParseProcedure(const QDomElement &p_base,
                                            bool expand)
{
    qCDebug(APEX_RS, "Parsing procedure");
    // load plugin for parser

    // Get xsi:type:
    QString type(p_base.attribute(gc_sType));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(type);
    QScopedPointer<ProcedureParserInterface> parser(
        creator->createProcedureParser(type));

    if (expand)
        expandTrials(p_base);

    procedureData.reset(parser->parse(p_base));
    Q_ASSERT(procedureData);

    return true;
}

bool apex::ExperimentParser::ParseStimuli(const QDomElement &p_base,
                                          bool expand)
{
    AddStatusMessage(tr("Processing stimuli"));

    m_stimuli.reset(new data::StimuliData());

    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    gui::ApexMainWindow *mwp = NULL;
    if (m_interactive)
        mwp = ApexControl::Get().mainWindow();
    parser::StimuliParser parser(mwp);

    parser.Parse(fileName, p_base, m_stimuli.data(), scriptLibrary,
                 m_generalParameters->scriptParameters(), expand);

    return true;
}

// Auxilary functions

void apex::ExperimentParser::AddStatusMessage(QString p_message)
{
    qCInfo(APEX_RS, "%s",
           qPrintable(QSL("%1: %2").arg("ExperimentParser", p_message)));
}

void apex::ExperimentParser::StatusMessageDone()
{
    qCInfo(APEX_RS, "ExperimentParser: Done");
}

/**
 * Do checks that can't be done using xml schema
 * @return
 */
bool apex::ExperimentParser::DoExtraValidation()
{
    // check whether every defined fixedparameter appears in every stimulus
    bool result = CheckProcedure() && CheckFixedParameters() &&
                  CheckAnswers() && CheckShowResults() && CheckDatablocks() &&
                  CheckFilters() && CheckTrials() && CheckStimuli() &&
                  CheckDevices() && CheckRandomGenerators();

    return result;
}

/**
 * check whether every defined fixedparameter appears in every stimulus
 * @return
 */
bool apex::ExperimentParser::CheckFixedParameters()
{
    const data::FixedParameterList &fixedParameters =
        m_stimuli->GetFixedParameters();
    for (int i = 0; i < fixedParameters.size(); ++i) {
        for (QMap<QString, data::StimulusData>::const_iterator it =
                 m_stimuli->begin();
             it != m_stimuli->end(); ++it) {
            if (!it.value().GetFixedParameters().contains(
                    fixedParameters.at(i))) {
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "StimuliData",
                        QString("Fixed parameter %1 not defined in stimulus %2")
                            .arg(fixedParameters.at(i))
                            .arg(it.key()))));
                return false;
            }
        }
    }
    return true;
}

/**
 * Check whether the number of answers (if defined) in Corrector corresponds to
 * the number of alternatives in Procedure
 * @return
 */
bool apex::ExperimentParser::CheckAnswers()
{
    bool result = true;

    // check whether a standard is available for each stimulus if choices>1
    if (procedureData->choices().hasMultipleIntervals()) {
        if (procedureData->defaultStandard().isEmpty()) { // no default standard
            // check each trial
            data::tTrialList trials = procedureData->GetTrials();
            for (data::tTrialList::const_iterator it = trials.begin();
                 it != trials.end(); ++it) {
                if ((*it)->GetRandomStandard().isEmpty()) {
                    qCCritical(
                        APEX_RS, "%s",
                        qPrintable(QSL("%1: %2").arg(
                            "ExperimentParser",
                            tr("Error: trial %1 has no standard and choices>1")
                                .arg((*it)->GetID()))));
                    result = false;
                }
            }
        }
    } else { // warn that there is no answer
        data::tTrialList trials = procedureData->GetTrials();
        for (data::tTrialList::const_iterator it = trials.begin();
             it != trials.end(); ++it) {
            if ((*it)->GetAnswer().isEmpty()) {
                qCWarning(APEX_RS, "%s",
                          qPrintable(QSL("%1: %2").arg(
                              "ExperimentParser",
                              tr("No answer was defined for trial %1. "
                                 "This is probably unwanted.")
                                  .arg((*it)->GetID()))));
            }
        }
    }

    return result;
}

bool apex::ExperimentParser::CheckDatablocks()
{
    data::DatablocksData::const_iterator it;

    QString defaultDevice; // if there's only one device, we use it if
                           // no device is specified in a datablock
    if (m_devicesdata.size() == 1)
        defaultDevice = m_devicesdata.begin().key();

    for (it = m_datablocksdata.begin(); it != m_datablocksdata.end(); it++) {
        if (!defaultDevice.isEmpty() && it.value()->device().isEmpty())
            it.value()->setDevice(defaultDevice);

        // check if all datablocks have a valid device
        try {
            m_devicesdata.deviceData(it.value()->device());
        } catch (const std::exception &e) {
            throw Exception(
                tr("Error in datablock %1: %2").arg(it.key(), e.what()));
        }
    }

    return true;
}

bool apex::ExperimentParser::CheckFilters()
{
    data::FiltersData::const_iterator it;

    for (it = m_filtersdata.begin(); it != m_filtersdata.end(); it++) {
        // check if all filters have a valid device
        try {
            data::DeviceData *temp =
                m_devicesdata.deviceData(it.value()->device());

            if (temp->deviceType() != data::TYPE_WAVDEVICE)
                throw ApexStringException(
                    tr("Filters are only implemented for a WavDevice"));
        } catch (const std::exception &e) {
            throw Exception(
                tr("Error in filter %1: %2").arg(it.key(), e.what()));
        }
    }

    return true;
}

bool apex::ExperimentParser::CheckDevices()
{
    // foreach device
    for (data::DevicesData::const_iterator it = m_devicesdata.begin();
         it != m_devicesdata.end(); ++it) {
        data::DeviceData *d = it.value();

        // foreach gain parameter
        Q_FOREACH (data::Parameter param, d->parameters()) {
            int a = param.channel();

            if (a >= (int)d->numberOfChannels()) {
                throw ApexStringException(tr("Error in device %1: invalid"
                                             " channel number %2 in gain")
                                              .arg(it.key())
                                              .arg(a));
            }
        }
    }

    return true;
}

bool apex::ExperimentParser::CheckRandomGenerators()
{
    Q_FOREACH (data::RandomGeneratorParameters *params, m_randomgenerators) {
        if (params->m_nType == params->TYPE_UNIFORM &&
            params->m_nValueType == params->VALUE_INT &&
            (params->m_dMin != (int)params->m_dMin ||
             params->m_dMax != (int)params->m_dMax))
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg(
                          "ExperimentParser::CheckRandomGenerators",
                          tr("Limits of uniform int random generator are not "
                             "integers, results may be unexpected"))));
    }
    return true;
}

bool apex::ExperimentParser::ParseRandomGenerators(const QDomElement &p_base)
{
    AddStatusMessage(tr("Processing random generators"));

    bool result = true;
    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "randomgenerator") {
            result = result & ParseRandomGenerator(currentNode);
        } else {
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg(
                          "ExperimentParser::ParseRandomGenerators",
                          "Unknown tag: " + tag)));
        }
    }

    StatusMessageDone();
    return result;
}

bool apex::ExperimentParser::ParseRandomGenerator(const QDomElement &p_base)
{
    data::RandomGeneratorParameters *param =
        new data::RandomGeneratorParameters;
    param->Parse(p_base);
    QString id = p_base.attribute(sc_sID);
    m_randomgenerators[id] = param;
    return true;
}

bool apex::ExperimentParser::ParseResults(const QDomElement &p_base)
{
    AddStatusMessage(tr("Processing result parameters"));
    m_resultParameters.reset(new data::ResultParameters());
    bool result = m_resultParameters->Parse(p_base);
    StatusMessageDone();
    return result;
}

bool apex::ExperimentParser::CreateMissing()
{
    if (!m_generalParameters)
        m_generalParameters.reset(new data::GeneralParameters());
    return true;
}

/**
 * Check whether the procedure refers to valid stimuli
 */
bool ExperimentParser::CheckProcedure()
{
    const QString standard = procedureData->defaultStandard();
    if (!standard.isEmpty() && !m_stimuli->contains(standard)) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ScreenChecker",
                       "Invalid default standard stimulus " + standard)));
        return false;
    }

    // only the first adapting parameter of an adaptive procedure is allowed
    // to be a fixed parameter
    const data::ProcedureData *parameters = procedureData.data();
    if (parameters->type() == data::ProcedureData::AdaptiveType) {
        QStringList adaptingParameters =
            static_cast<const data::AdaptiveProcedureData *>(parameters)
                ->adaptingParameters();

        if (!adaptingParameters.isEmpty()) {
            adaptingParameters.takeFirst();

            Q_FOREACH (QString param, adaptingParameters) {
                if (m_stimuli->GetFixedParameters().contains(param)) {
                    qCCritical(APEX_RS, "%s",
                               qPrintable(QSL("%1: %2").arg(
                                   "ProcedureChecker",
                                   "Only the first adaptive parameter can "
                                   "be a fixed parameter.")));

                    return false;
                }
            }
        }
    }

    // Check if there is an interval defined for each choice
    if (parameters->choices().hasMultipleIntervals()) {
        QStringList mIntervals(parameters->choices().intervals());
        for (int i = 0; i < mIntervals.length(); ++i) {
            if (mIntervals[i].isEmpty()) {
                qCWarning(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "IntervalChecker",
                        QString("No screenelement defined for interval %1")
                            .arg(i + 1))));
            }
        }
    }

    return true;
}

bool ExperimentParser::CheckTrials()
{
    data::ScreensData::ScreenMap &lscreens = screens->getScreens();

    bool result = true;
    const data::tTrialList &trials = procedureData->GetTrials();
    for (data::tTrialList::const_iterator it = trials.begin();
         it != trials.end(); ++it) {
        QString screen = (*it)->GetScreen();
        if (lscreens.find(screen) == lscreens.end()) {
            qCCritical(APEX_RS, "%s",
                       qPrintable(QSL("%1: %2").arg("TrialChecker",
                                                    "Invalid screen in trial " +
                                                        (*it)->GetID())));
            result = false;
        }

        const tStimulusList &stimulusList = (*it)->GetStimuli();
        for (int i = 0; i < stimulusList.size(); ++i) {
            if (!m_stimuli->contains(stimulusList.at(i))) {
                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "TrialChecker",
                               "Invalid stimulus in trial " + (*it)->GetID())));
                result = false;
            }
        }
        if (stimulusList.isEmpty()) {
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg(
                          "TrialChecker",
                          tr("No stimulus was found in trial %1, is this "
                             "what you want?")
                              .arg((*it)->GetID()))));
            if (procedureData->type() !=
                data::ProcedureData::Type::PluginType) {
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "TrialChecker", "Unable to have a non-plugin procedure "
                                        "with an empty trial")));
            }
        }

        const tStimulusList &standardList = (*it)->GetStandards();
        for (int i = 0; i < standardList.size(); ++i) {
            if (!m_stimuli->contains(standardList.at(i))) {
                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "TrialChecker",
                               "Invalid standard in trial " + (*it)->GetID())));
                result = false;
            }
        }
    }

    return result;
}

/**
 * Check whether each stimulus refers to existing datablocks
 */
bool apex::ExperimentParser::CheckStimuli()
{
    // if this is an adaptive procedure which adapts a fixed parameter and has a
    // min/max value set for the parameter, than for every fixed parameter there
    // should be at least one stimulus with a value greater/smaller than the
    // min/max value.
    const data::AdaptiveProcedureData *params =
        dynamic_cast<const data::AdaptiveProcedureData *>(procedureData.data());

    if (params != 0) {
        QString fixedParam = params->adaptingParameters().first();

        if (m_stimuli->GetFixedParameters().contains(fixedParam) &&
            (params->hasMinValue() || params->hasMaxValue())) {
            data::adapting_parameter min =
                std::numeric_limits<data::adapting_parameter>::max();
            data::adapting_parameter max =
                std::numeric_limits<data::adapting_parameter>::min();

            Q_FOREACH (data::StimulusData stimulus, m_stimuli->values()) {
                QVariant variant =
                    stimulus.GetFixedParameters().value(fixedParam);
                Q_ASSERT(variant.canConvert<data::adapting_parameter>());
                data::adapting_parameter value =
                    variant.value<data::adapting_parameter>();

                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }

            if (params->hasMinValue() && max < params->minValue()) {
                QString error = tr(
                    "The adaptive procedure adapts a fixed parameter (%1) and "
                    "has "
                    "a minimum parameter value set but no stimulus exists "
                    "with a fixed parameter value greater than this minimum");

                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "StimulusChecker", error.arg(fixedParam))));
                return false;
            }

            if (params->hasMaxValue() && min > params->maxValue()) {
                QString error = tr(
                    "The adaptive procedure adapts a fixed parameter (%1) and "
                    "has "
                    "a maximum parameter value set but no stimulus exists "
                    "with a fixed parameter value smaller than this maximum");

                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "StimulusChecker", error.arg(fixedParam))));
                return false;
            }
        }
    }

    for (data::StimuliData::const_iterator it = m_stimuli->begin();
         it != m_stimuli->end(); ++it) {
        const data::StimulusData &data = it.value();
        const data::StimulusDatablocksContainer datablocks =
            data.GetDatablocksContainer();

        if (!CheckStimulusDatablocks(datablocks, it.key()))
            return false;
    }

    return true;
}

bool apex::ExperimentParser::CheckStimulusDatablocks(
    apex::data::StimulusDatablocksContainer datablocks, QString trial)
{
    using apex::data::StimulusDatablocksContainer;
    StimulusDatablocksContainer::const_iterator it;
    for (it = datablocks.begin(); it != datablocks.end(); it++) {
        if (it->type == StimulusDatablocksContainer::DATABLOCK) {
            QString id = it->id;

            Q_ASSERT(!id.isEmpty());

            if (m_datablocksdata.find(id) == m_datablocksdata.end()) {
                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "StimulusChecker",
                               QString("Invalid datablock %1 in stimulus %2")
                                   .arg(id)
                                   .arg(trial))));
                return false;
            }
        } else {
            Q_ASSERT(it->type == StimulusDatablocksContainer::DATABLOCKS ||
                     it->type == StimulusDatablocksContainer::SEQUENTIAL ||
                     it->type == StimulusDatablocksContainer::SIMULTANEOUS);

            return CheckStimulusDatablocks(*it, trial);
        }
    }

    return true;
}

bool apex::ExperimentParser::ParseGeneral(const QDomElement &p_base)
{
    AddStatusMessage(tr("Processing general parameters"));
    m_generalParameters.reset(new data::GeneralParameters());
    bool result = m_generalParameters->Parse(p_base);
    StatusMessageDone();
    return result;
}

/**
* Check whether an xslt script is available if showresults is true
* @return
*/
bool apex::ExperimentParser::CheckShowResults()
{
    if (m_resultParameters && (m_resultParameters->showResultsAfter()) &&
        m_resultParameters->resultPage().isEmpty()) {
        qCCritical(APEX_RS,
                   "CheckShowResults: No page specified for analyzing results");
        return false;
    }

    return true;
}

bool apex::ExperimentParser::ParseDescription(const QDomElement &p_base)
{
    m_description = p_base.text();
    return true;
}

void apex::ExperimentParser::upgrade3_0_2()
{
    // Change procedure xsi:type
    {
        QString newName;

        QDomNodeList list =
            document.documentElement().elementsByTagName("procedure");
        for (int i = 0; i < list.count(); ++i) {
            QDomElement procedureElement = list.at(i).toElement();
            QString oldName(procedureElement.attribute("xsi:type"));
            if (oldName == "apex:pluginProcedureType")
                newName = "apex:pluginProcedure";
            else if (oldName == "apex:adaptiveProcedureType")
                newName = "apex:adaptiveProcedure";
            else if (oldName == "apex:constantProcedureType")
                newName = "apex:constantProcedure";
            else if (oldName == "apex:trainingProcedureType")
                newName = "apex:trainingProcedure";
            else if (oldName == "apex:multiProcedureType")
                newName = "apex:multiProcedure";
            else
                qCWarning(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "Upgrade",
                        QString("Cannot upgrade procedure type %1")
                            .arg(procedureElement.attribute("xsi:type")))));

            procedureElement.setAttribute("xsi:type", newName);
        }
    }

    // Remove choices & corrector and add intervals
    {
        const QDomNodeList procedureNodes =
            document.documentElement().elementsByTagName("procedure");
        for (int i = 0; i < procedureNodes.count(); ++i) {
            QDomElement procedureElement = procedureNodes.at(i).toElement();
            const QDomElement correctorElement =
                document.documentElement()
                    .elementsByTagName("corrector")
                    .at(0)
                    .toElement();

            QDomElement parametersElements =
                procedureElement.elementsByTagName("parameters")
                    .at(0)
                    .toElement();
            const QDomElement choicesElement =
                parametersElements.elementsByTagName("choices")
                    .at(0)
                    .toElement();

            if (correctorElement.attribute("xsi:type") == "apex:alternatives") {
                // Transform the answers into intervals
                const QDomElement answerselement =
                    correctorElement.elementsByTagName("answers")
                        .at(0)
                        .toElement();
                const QDomNodeList answers =
                    answerselement.elementsByTagName("answer");

                QDomElement intervalsElement =
                    document.createElement("intervals");
                intervalsElement.setAttribute("count", answers.count());

                for (int i = 0; i < answers.count(); ++i) {
                    QDomElement intervalElement =
                        document.createElement("interval");

                    intervalElement.setAttribute(
                        "number",
                        answers.at(i).toElement().attribute("number"));
                    intervalElement.setAttribute(
                        "element",
                        answers.at(i).toElement().attribute("value"));

                    intervalsElement.appendChild(intervalElement);
                }

                // Find a location to insert the intervals
                if (choicesElement.isNull()) {
                    QDomElement afterThis =
                        parametersElements.elementsByTagName("uniquestandard")
                            .at(0)
                            .toElement();
                    if (afterThis.isNull()) {
                        afterThis = parametersElements
                                        .elementsByTagName("defaultstandard")
                                        .at(0)
                                        .toElement();
                        if (afterThis.isNull()) {
                            afterThis =
                                parametersElements.elementsByTagName("order")
                                    .at(0)
                                    .toElement(); // order is required, so we
                                                  // can stop here
                        }
                    }

                    parametersElements.insertAfter(intervalsElement, afterThis);
                } else {
                    QString select = choicesElement.attribute("select", "");
                    if (!select.isEmpty()) {
                        intervalsElement.setAttribute("select", select);
                    }
                    parametersElements.replaceChild(intervalsElement,
                                                    choicesElement);
                }
            } else {
                if (!choicesElement.isNull() &&
                    choicesElement.text() != QL1S("1")) {
                    qCCritical(APEX_RS, "%s",
                               qPrintable(QSL("%1: %2").arg(
                                   tr("XML parser"),
                                   tr("Can't convert a procedure with a "
                                      "choices element and a corrector that "
                                      "isn't an alternatives corrector."))));
                } else {
                    choicesElement.parentNode().removeChild(choicesElement);
                    QDomElement afterThis =
                        parametersElements.elementsByTagName("uniquestandard")
                            .at(0)
                            .toElement();
                    if (afterThis.isNull()) {
                        afterThis = parametersElements
                                        .elementsByTagName("defaultstandard")
                                        .at(0)
                                        .toElement();
                        if (afterThis.isNull()) {
                            afterThis =
                                parametersElements.elementsByTagName("order")
                                    .at(0)
                                    .toElement(); // order is required, so we
                                                  // can stop here
                        }
                    }

                    parametersElements.insertAfter(correctorElement, afterThis);
                }
            }
            // Delete the obsolete corrector
            document.documentElement().removeChild(correctorElement);
        }

        { // Remove birth from datablock
            QDomNodeList list =
                document.documentElement().elementsByTagName("datablock");
            for (int i = 0; i < list.count(); ++i) {
                QDomElement datablock = list.at(i).toElement();
                QDomNodeList births = datablock.elementsByTagName("birth");
                if (!births.isEmpty()) {
                    datablock.removeChild(births.at(0));
                }
            }
        }

        { // Change silence:// to silence:
            QDomNodeList list =
                document.documentElement().elementsByTagName("datablock");
            for (int i = 0; i < list.count(); ++i) {
                QDomElement datablock = list.at(i).toElement();
                QDomNode uri = datablock.elementsByTagName("uri").at(0);

                QDomText text = uri.firstChild().toText();
                QString value = text.nodeValue();
                if (value.contains("silence://")) {
                    value.replace("silence://", "silence:");
                    uri.firstChild().setNodeValue(value);
                }
            }
        }
    }
}

void apex::ExperimentParser::upgrade3_1_0()
{
    // Get the contents of the results section
    QDomNodeList resultsList =
        document.documentElement().elementsByTagName("results");

    QDomNode results = resultsList.item(0);
    // Get all the child nodes from the resultsType section:
    QDomNodeList resultNodes = results.childNodes();
    QDomNode currentNode, pageNode, realtimeNode, showNode, subjectNode,
        scriptNode;
    QString pageName;
    QDomNodeList parameterList;

    for (int i = 0; i < resultNodes.length(); i++) {
        currentNode = resultNodes.item(i);
        if (currentNode.nodeName() == "xsltscript") {
            // Take the name of the script as the name of the html page.
            QDomNode scriptTextNode = currentNode.firstChild();
            /*pageName = scriptTextNode.nodeValue();
            pageName.truncate(pageName.lastIndexOf("."));
            pageName.append(".html");*/
            results.removeChild(currentNode);
            // go to the next node, the list is updated live
            currentNode = resultNodes.item(i);
        }
        if (currentNode.nodeName() == "xsltscriptparameters") {
            parameterList = currentNode.childNodes();
            // the children will be appended to the new node.
            results.removeChild(currentNode);
            currentNode = resultNodes.item(i);
        }
        if (currentNode.nodeName() == "showresults") {
            showNode = currentNode.cloneNode();

            results.removeChild(currentNode);
            // go to the next node, the list is updated live
            currentNode = resultNodes.item(i);
        }
        if (currentNode.nodeName() == "javascript") {
            // Get the childNodes.
            QDomNodeList javascriptList = currentNode.childNodes();
            QDomNode currentJavascriptNode;
            for (int j = 0; j < javascriptList.length(); j++) {
                currentJavascriptNode = javascriptList.item(j);
                if (currentJavascriptNode.nodeName() == "page") {
                    pageNode = currentJavascriptNode.cloneNode();
                } else if (currentJavascriptNode.nodeName() == "realtime") {
                    realtimeNode = currentJavascriptNode.cloneNode();
                } else if (currentJavascriptNode.nodeName() ==
                           "showafterexperiment") {
                    showNode = currentJavascriptNode.cloneNode();
                }
            }

            // Now remove the javascript node
            results.removeChild(currentNode);
            currentNode = resultNodes.item(i);
        }
        if (currentNode.nodeName() == "matlabscript") {
            scriptNode = currentNode.cloneNode();
        }
        if (currentNode.nodeName() == "subject") {
            subjectNode = currentNode.cloneNode();
        }
        if (currentNode.nodeName() == "page") {
            pageNode = currentNode;
        }
    }

    // create the parameter node and default element:
    QDomElement resultParameters = document.createElement("resultparameters");
    // Append the parameters to the new node
    if (!parameterList.isEmpty()) {
        for (int i = 0; i < parameterList.count(); i++) {
            resultParameters.appendChild(parameterList.item(i));
        }
    }

    // check if the old nodes are present, if so, add them
    // at the right place, if not, create defaults and add them.
    QDomElement showAfterElement;
    showAfterElement = showNode.toElement();
    if (showAfterElement.isNull()) {
        showAfterElement = document.createElement("showafterexperiment");
        showAfterElement.appendChild(document.createTextNode("false"));
    } else {
        showAfterElement.setTagName("showafterexperiment");
    }

    QDomElement realtimeElement;
    if (realtimeNode.isNull()) {
        realtimeElement = document.createElement("showduringexperiment");
        realtimeElement.appendChild(document.createTextNode("false"));
    } else {
        realtimeElement.setTagName("showduringexperiment");
    }

    QDomElement pageElement;
    if (pageNode.isNull()) {
        if (pageName.isEmpty()) {
            pageName = "apex:resultsviewer.html";
        }
        pageElement = document.createElement("page");
        QDomText pageText;
        pageText = document.createTextNode(pageName);
        pageElement.appendChild(pageText);
    } else {
        pageElement = pageNode.toElement();
    }

    results.insertBefore(showAfterElement, resultNodes.item(0));
    results.insertBefore(realtimeElement, resultNodes.item(0));
    results.insertBefore(resultParameters, resultNodes.item(0));
    results.insertBefore(pageElement, resultNodes.item(0));

    // insert the new results in the dom document.
    // First get the right position to set the results:
    QDomNode previousChild = results.previousSibling();
    document.removeChild(results);
}

void apex::ExperimentParser::upgrade3_1_1()
{
    QDomNodeList list = document.elementsByTagName("default_answer_element");
    for (int i = 0; i < list.count(); ++i) {
        QDomElement answerElement = list.at(i).toElement();
        answerElement.replaceChild(
            document.createTextNode(answerElement.text().trimmed()),
            answerElement.firstChild());
    }

    list = document.elementsByTagName("device");
    for (int i = 0; i < list.count(); ++i) {
        QDomElement deviceElement = list.at(i).toElement();
        if (deviceElement.attribute("xsi:type") == QL1S("apex:L34DeviceType")) {
            QString device_type(QL1S("L34"));
            QString implant;
            QString device_id;
            QDomNodeList deviceNodes = deviceElement.childNodes();
            for (int j = deviceNodes.length(); j >= 0; j--) {
                QDomNode currentNode = deviceNodes.item(j);
                QString text = currentNode.toElement().text();
                if (currentNode.nodeName() == QL1S("device_type")) {
                    device_type = text;
                    deviceElement.removeChild(currentNode);
                } else if (currentNode.nodeName() == QL1S("implant")) {
                    implant = text;
                    deviceElement.removeChild(currentNode);
                } else if (currentNode.nodeName() == QL1S("device_id")) {
                    device_id = text;
                    deviceElement.removeChild(currentNode);
                }
            }
            QDomElement device = document.createElement("device");
            device.appendChild(document.createTextNode(
                QString::fromLatin1("%1-%2-%3")
                    .arg(device_type, implant, device_id)));
            deviceElement.insertBefore(device, QDomNode());
        }
    }
}

static void fixJsSources(const QDomNodeList &list)
{
    for (int i = 0; i < list.count(); ++i) {
        QDomElement element = list.at(i).firstChildElement(QSL("script"));
        if (element.attribute(QSL("source")) == QL1S("file") &&
            !element.text().endsWith(QSL(".js"))) {
            XmlUtils::setText(&element, element.text() + QSL(".js"));
        }
    }
}

static void fixUri(const QDomDocument &document, const QString &from,
                   const QString &to)
{
    QDomNodeList list = document.elementsByTagName(from);
    // walk backwards as the list is updated dynamically
    for (int i = list.count() - 1; i >= 0; --i) {
        QDomElement element = list.at(i).toElement();
        QString text = element.text();
        if (text.startsWith(QSL("file:")) || text.contains(QSL("%20")))
            text = QUrl(text).path();
        element.setTagName(to);
        XmlUtils::setText(&element, text);
    }
}

void apex::ExperimentParser::upgrade3_1_3()
{
    QDomElement root = document.documentElement();

    // L34DeviceType -> CohDeviceType
    QDomNodeList list = document.elementsByTagName(QSL("device"));
    for (int i = 0; i < list.count(); ++i) {
        QDomElement element = list.at(i).toElement();
        if (element.attribute(QSL("xsi:type")) == QL1S("apex:L34DeviceType"))
            element.setAttribute(QSL("xsi:type"), QL1S("apex:CohDeviceType"));
    }

    // fix js extension of xml plugin javascript files
    fixJsSources(document.elementsByTagName(QSL("pluginscreens")));
    fixJsSources(document.elementsByTagName(QSL("plugintrials")));
    fixJsSources(document.elementsByTagName(QSL("pluginstimuli")));
    fixJsSources(document.elementsByTagName(QSL("plugindatablocks")));

    // fix js extension of library javascript files
    QDomElement libnode = root.firstChildElement(QSL("general"))
                              .firstChildElement(QSL("scriptlibrary"));
    if (!libnode.isNull() && !libnode.text().endsWith(QSL(".js")))
        XmlUtils::setText(&libnode, libnode.text() + QSL(".js"));

    // fix js extension of procedure javascript files
    QDomElement scriptnode = root.firstChildElement(QSL("procedure"))
                                 .firstChildElement(QSL("parameters"))
                                 .firstChildElement(QSL("script"));
    if (!scriptnode.isNull() && !scriptnode.text().endsWith(QSL(".js")))
        XmlUtils::setText(&scriptnode, scriptnode.text() + QSL(".js"));

    // uri -> file
    fixUri(document, QSL("uri_prefix"), QSL("prefix"));
    fixUri(document, QSL("uri"), QSL("file"));
    fixUri(document, QSL("uriDisabled"), QSL("disabledfile"));

    list = document.elementsByTagName(QSL("parameter"));
    for (int i = 0; i < list.count(); ++i) {
        QDomElement element = list.at(i).toElement();
        if (element.attribute(QSL("name")) == QSL("uri"))
            element.setAttribute(QSL("name"), QL1S("file"));
    }

    // remove datablock > checksum
    list = document.elementsByTagName(QSL("datablock"));
    for (int i = 0; i < list.count(); ++i) {
        QDomElement element = list.at(i).toElement();
        element.removeChild(element.firstChildElement("checksum"));
    }
}
