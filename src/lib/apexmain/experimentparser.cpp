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

#include "apexdata/calibration/calibrationdata.h"

#include "apexdata/connection/connectiondata.h"

#include "apexdata/datablock/datablockdata.h"
#include "apexdata/datablock/datablocksdata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/interactive/parameterdialogresults.h"

#include "apexdata/mixerdevice/mixerparameters.h"

#include "apexdata/parameters/apexmapparameters.h"
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

#include "apextools/apextools.h"
#include "apextools/apextools.h"
#include "apextools/exceptions.h"
#include "apextools/exceptions.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "calibration/calibrationparser.h"

#include "connection/connection.h"
#include "connection/connectionparser.h"

#include "datablock/datablocksparser.h"
#include "datablock/datablocksparser.h"

#include "device/devicesparser.h"
#include "device/iapexdevice.h"

#include "filter/filterparser.h"

#include "gui/guidefines.h"
#include "gui/mainwindow.h"

#include "interactive/interactiveparameters.h"
#include "interactive/interactiveparametersdialog.h"

#include "mixerdevice/mixerfactory.h"

#include "parser/prefixparser.h"
#include "parser/scriptexpander.h"

#include "screen/screensparser.h"

#include "services/errorhandler.h"
#include "services/mainconfigfileparser.h"
#include "services/pluginloader.h"

#include "stimulus/datablock.h"
#include "stimulus/filter.h"
#include "stimulus/outputdevice.h"
#include "stimulus/stimuliparser.h"
#include "stimulus/stimulus.h"

#include "streamapp/appcore/deleter.h"

#include "experimentparser.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>
#include <QXmlQuery>

#include <limits>

#ifdef CLEBS_DEBUG
#include <iostream>
#endif

using namespace apex;
using namespace apex::ApexXMLTools;
using namespace apex::device;
using namespace apex::XMLKeys;
using namespace xercesc;

apex::ExperimentParser::ExperimentParser(const QString & configFilename, QMap<QString, QString> expressions) :
        ConfigFileParser(configFilename, Paths::Get().GetExperimentSchemaPath(), getConfigfileNamespace()) ,
        m_interactive(true),
        m_progress(ErrorHandler::Get()),
        connectionDatas(new data::ConnectionsData()),
        m_resultParameters(new data::ResultParameters(0)),
        expressions(expressions)
{
    LogErrorsToApex(&m_progress);
}

data::ExperimentData* apex::ExperimentParser::parse(bool interactive)
{
    CFParse();

    m_interactive=interactive;

    if (!m_document)
    {
        m_progress.addError("ExperimentParser", tr("Schema validation error, please check your experiment file agains the apex schema"));
        throw ParseException();
    }

    qCDebug(APEX_RS, "ExperimentParser: parsing...");

    if (!ApplyXpathModifications() || !Parsefile())
        throw ParseException();

    return new data::ExperimentData(GetConfigFilename(),
                                    screens.take(),
                                    procedureData.take(),
                                    connectionDatas.take(),
                                    m_CalibrationData.take(),
                                    m_generalParameters.take(),
                                    m_resultParameters.take(),
                                    m_parameterDialogResults.take(),
                                    new QMap<QString, data::RandomGeneratorParameters*>(m_randomgenerators),
                                    new data::DevicesData(m_devicesdata),
                                    new data::FiltersData(m_filtersdata),
                                    new data::DevicesData(m_controldevicesdata),
                                    new data::DatablocksData(m_datablocksdata),
                                    m_stimuli.take(),
                                    m_description,
                                    parameterManagerData.take());
}

/**apex::ExperimentParser::
 * Apply modifications stated in the experiment file based on user input
 * @return
 */
bool apex::ExperimentParser::ApplyXpathModifications()
{
    gui::ApexMainWindow* mwp = 0;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();

    InteractiveParameters* ip = new InteractiveParameters(m_document, xmlDocumentGetter);
    if(!expressions.isEmpty()) {
        ip->applyExpressions(expressions);
        m_document = ip->document();
    }


    InteractiveParametersDialog ipd(ip, mwp);

    if (ip->entries().isEmpty())
        return true;
    bool result = ipd.exec();

    if (result) {
        ipd.apply();
        m_document = ip->document();
    } else {
        result = QMessageBox::warning(mwp,
                tr("Interactive changes not applied"),
                tr("Warning: no changes were applied, do you want to continue"
                   " with the unmodified document?"),
                QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;
    }

    m_parameterDialogResults.reset(ip->results());

    return result;
}

apex::ExperimentParser::~ExperimentParser()
{
}

bool apex::ExperimentParser::Parsefile ()
{
    // set current path to experiment file path
    QUrl tUrl = QUrl(m_sConfigFilename);
    QDir::setCurrent( QFileInfo( m_sConfigFilename ).path() );

    parameterManagerData.reset(new data::ParameterManagerData());

    bool bSuccess = true;
    QString currentTag = "general";

    // first parse general
    DOMNode* baseNode = m_document->getDocumentElement();
    DOMNode* generalNode = XMLutils::FindChildNode(baseNode, "general");

    if (generalNode)
        bSuccess &= ParseGeneral((DOMElement*)generalNode);
    else
        m_generalParameters.reset(new data::GeneralParameters());

    for (DOMNode* currentNode = m_document->getDocumentElement()->getFirstChild();
         currentNode != 0 && bSuccess; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        currentTag = XMLutils::GetTagName( currentNode );
        qCDebug(APEX_RS, "Parsing %s", qPrintable(currentTag));

        if (currentTag == "description")
            bSuccess = ParseDescription((DOMElement*)currentNode);
        else if (currentTag == "filters")
            bSuccess = ParseFilters((DOMElement*)currentNode);
        else if (currentTag == "datablocks")
            bSuccess = ParseDatablocks((DOMElement*)currentNode);
        else if (currentTag == "procedure")
            bSuccess = ParseProcedure((DOMElement*)currentNode);
        else if (currentTag == "stimuli")
            bSuccess = ParseStimuli((DOMElement*)currentNode);
        else if (currentTag == "devices")
            bSuccess = ParseDevices((DOMElement*)currentNode);
        else if (currentTag == "screens")
            bSuccess = ParseScreens((DOMElement*)currentNode);
        else if (currentTag == "connections")
            bSuccess = ParseConnections((DOMElement*)currentNode);         // connections should be parsed AFTER datablocks, devices and filters because they need to lookup their device
        else if (currentTag == "randomgenerators")
            bSuccess = ParseRandomGenerators((DOMElement*)currentNode);
        else if (currentTag == "calibration")
            bSuccess = ParseCalibration((DOMElement*)currentNode);
        else if (currentTag == "results")
            bSuccess = ParseResults((DOMElement*)currentNode);
        else if (currentTag == "general");
        // skipping, already parsed before
        else if (currentTag == "interactive");
        // NOP
        else
        {
            m_progress.addWarning("ExperimentParser::Parsefile",
                                  tr("Unknown tag \"%1\"").arg(currentTag));
        }
    }

    if (!bSuccess)
    {
        m_progress.addError("ExperimentParser::Parsefile",
                            tr("Error parsing element \"%1\"").arg(currentTag));

        return false;
    }

    //register fixed parameters of stimuli
    //FIXME find a better place to do this
    Q_FOREACH (QString fixedParameter, m_stimuli->GetFixedParameters())
    {
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

bool apex::ExperimentParser::ParseConnections( DOMElement* p_base )
{
    AddStatusMessage (tr ("Processing Connections"));

    parser::ConnectionParser parser;

    for (DOMNode* currentNode = p_base->getFirstChild(); currentNode != NULL;
            currentNode = currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag (XMLutils::GetTagName (currentNode));

        if (tag == "connection")
        {
            data::ConnectionData* cd = parser.Parse (currentNode);
            QString dev( GetDeviceForConnection(cd));
//            qAPEXDEBUG(qPrintable("Device id " + dev));
            cd->setDevice( dev );

            QString id = cd->fromChannelId();
            if ( ! id.isEmpty() )
                parameterManagerData->registerParameter(id, data::Parameter(dev, "connection-" + id, cd->fromChannel(), -1, true));
            id = cd->toChannelId();
            if ( ! id.isEmpty() )
                parameterManagerData->registerParameter(id, data::Parameter(dev, "connection-" + id, cd->toChannel(), -1, true));


            // check if this connection already exists
            for (data::ConnectionsData::const_iterator it=connectionDatas->begin(); it!=connectionDatas->end(); ++it)
            {
                if ((**it).duplicateOf(*cd))
                {
                    m_progress.addWarning("Connections",
                                          tr("Duplicate connection from %1 to %2")
                                             .arg(cd->fromId()).arg(cd->toId()));
                }
            }


            connectionDatas->push_back (cd);
        }
        else
            qFatal ("Unknown tag in a connection: %s", qPrintable (tag));
    }

    StatusMessageDone();
    return true;
}

/**
 * iterate over all datablocks, devices and filters to find the device of the connection
 */
QString apex::ExperimentParser::GetDeviceForConnection(data::ConnectionData* cd)
{
    QString targetID = cd->fromId();

    // search devices
    try
    {
        data::DeviceData* d = m_devicesdata.deviceData(targetID);
        return d->id();
    }
    catch (...) {}

    try
    {
        const data::FilterData* d =  m_filtersdata.filterData(targetID);
        return d->device();
    }
    catch (...) {}


    try
    {
        const data::DatablockData* d = m_datablocksdata.datablockData(targetID);
        return d->device();
    }
    catch (...) {}

    targetID = cd->toId();

    // search devices
    try
    {
        data::DeviceData* d = m_devicesdata.deviceData(targetID);
        return d->id();
    }
    catch (...) {}

    try
    {
        const data::FilterData* d =  m_filtersdata.filterData(targetID);
        return d->device();
    }
    catch (...) {}


    try
    {
        const data::DatablockData* d = m_datablocksdata.datablockData(targetID);
        return d->device();
    }
    catch (...) {}

    throw ApexStringException("Error: device not found for connection");


}

/*QString ConnectionParser::GenerateType(QString parameterid) {
    return QString("connection-")  + parameterid;
}*/


bool apex::ExperimentParser::ParseCalibration( DOMElement* a_pCalib )
{
    AddStatusMessage( tr( "Processing calibration" ) );

    parser::CalibrationParser parser;
    m_CalibrationData.reset(parser.Parse(a_pCalib));

    StatusMessageDone();
    return true;
}

bool apex::ExperimentParser::ParseFilters(DOMElement* p_filters)
{
    AddStatusMessage(tr("Processing filters"));

    for (DOMNode* currentNode=p_filters->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName( currentNode );
        if (tag == "filter")
        {
            bool f = ParseFilter( (DOMElement*)currentNode );
            if ( !f )
                return false;

        }
        else
            m_progress.addWarning( "ExperimentParser::ParseFilters",
                                   "Unknown tag: " + tag );
    }

    StatusMessageDone();
    return true;
}

bool apex::ExperimentParser::ParseFilter( DOMElement* p_filter )
{
    //error checks
    if ( !p_filter )
        return 0;

    const QString type( XMLutils::GetAttribute( p_filter, sc_sType ) );
    const QString id( XMLutils::GetAttribute( p_filter, sc_sID ) );
    if ( type.isEmpty() || id.isEmpty() )
        return 0;

    //parse
    QString input, output, sModule;

    //find the device id
    for (DOMNode* currentNode=p_filter->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        const QString tag( XMLutils::GetTagName( currentNode ) );
        if ( tag == "device" )
        {
            output = XMLutils::GetFirstChildText( currentNode );
            sModule = XMLutils::FindAttribute( m_document->getDocumentElement()->getFirstChild(), "devices", "device", output, sc_sType );
            break;
        }
    }


    parser::FilterParser parser;

    //create parameters
    data::FilterData* parameters = parser.ParseFilter( p_filter, /*m_devicesdata, m_datablocksdata,*/ parameterManagerData.data());
    m_progress.addItems(parser.logger().items());
    if ( !parameters )
        return 0;

    m_filtersdata[id]=parameters;

    return true;
}

bool apex::ExperimentParser::ParseDevices(DOMElement* p_base)
{
    qCDebug(APEX_RS, "Parsing devices");
    parser::DevicesParser parser;
    try
    {
        parser::tAllDevices all = parser.Parse(p_base, parameterManagerData.data());
        m_devicesdata = all.outputdevices;
        m_controldevicesdata = all.controldevices;
        //m_sMasterDevice= all.outputdevices.GetMasterDevice();
    }
    catch (ApexStringException &e)
    {
        qCDebug(APEX_RS, "Number of errors: %u", parser.logger().numberOfErrors());
        m_progress.addItems(parser.logger().items());
        m_progress.addError("ParseDevices", e.what());
        return false;
    }

    m_progress.addItems(parser.logger().items());
    return true;



};


bool apex::ExperimentParser::ParseDatablocks(DOMElement* p_datablocks)
{
    gui::ApexMainWindow* mwp = NULL;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();

    parser::DatablocksParser parser(mwp);

    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    m_datablocksdata = parser.Parse(p_datablocks,
                                    scriptLibrary,
                                    m_generalParameters->scriptParameters()  );

    return true;
}



/**
 * Add the given prefix to p_base if p_base is a relative path
 * @param p_base
 * @param prefix
 * @return
 */
// QString apex::ExperimentParser::AddPrefix(const QString& p_base, const QString& prefix)
// {
//     QString result;
//
//     QUrl theUrl( p_base );
//     if ( theUrl.isRelative() )
//     {
//         result = prefix+p_base;
//     }
//     else
//     {
//         result = p_base;
//     }
//
//
//     return result;
// }






bool apex::ExperimentParser::ParseScreens( DOMElement* p_base )
{
    AddStatusMessage( "Parsing Screens" );

    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    gui::ApexMainWindow* mwp = 0;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();
    parser::ScreensParser parser(mwp);

    screens.reset(parser.ParseScreens(p_base, scriptLibrary,
                                      m_generalParameters->scriptParameters(),
                                      parameterManagerData.data() ));

    if (!screens)
        return false;
    /*
        //start with default font
        screens->setDefaultFont( sc_sDefaultFont );
        screens->setDefaultFontSize( sc_nDefaultFontSize );

        //parse
        for ( DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
        {
            const QString tag( XMLutils::GetTagName( currentNode ) );
            if (tag == "screen")
            {
                // let it propagate...

                if (!m_screenParser->createScreen ((DOMElement*) currentNode)) {
                    m_progress.AddItem( m_screenParser->GetError() );
                    return false;
                }

            }
            else if (tag== "reinforcement")
            {
                for ( DOMNode* reinf = currentNode->getFirstChild() ; reinf != 0 ; reinf = reinf->getNextSibling() )
                {
                    const QString tag( XMLutils::GetTagName( reinf ) );
                    if ( tag == "progressbar" )
                        mainWndConfig->m_PanelConfig.m_bShowProgress = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else if ( tag == gc_sFeedback )
                    {
                        mainWndConfig->m_bShowFeedback = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                        mainWndConfig->SetFeedbackLength( XMLutils::GetAttribute( reinf, gc_sLength ).toUInt());
                    }
                    else if ( tag == "feedback_on" )
                    {
                        QString value = XMLutils::GetFirstChildText( reinf );
                        if (value=="none")
                            mainWndConfig->m_feedbackOn=HIGHLIGHT_NONE;

                        else if (value=="correct")
                            mainWndConfig->m_feedbackOn=HIGHLIGHT_CORRECT;

                        else if (value=="clicked")
                            mainWndConfig->m_feedbackOn=HIGHLIGHT_ANSWER;
                        else
                            throw ApexStringException("Illegal feedback_on value");
                    }
                    else if ( tag == "feedback_picture_positive" )
                    {
                        QString file = AddPrefix( XMLutils::GetFirstChildText( reinf ),
                                                  m_screenParser->GetBasePath());
                        if ( ! QFile::exists(file))
                            throw ApexStringException(
                                "Positive feedback picture not found");

                        mainWndConfig->m_sFeedbackPositive = file;
                    }
                    else if ( tag == "feedback_picture_negative" )
                    {
                        QString file = AddPrefix( XMLutils::GetFirstChildText( reinf ),
                                                  m_screenParser->GetBasePath());
                        if ( ! QFile::exists(file))
                            throw ApexStringException(
                                "Positive feedback picture not found");

                        mainWndConfig->m_sFeedbackNegative = file;
                    }
                    else if ( tag == "showcurrent" )
                        mainWndConfig->m_bShowCurrent = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else
                        m_progress.AddWarning( "ExperimentParser::ParseScreens", "Unknown tag: " + tag );
                }
            }
            else if ( tag == "childmode" )
            {
                for ( DOMNode* reinf = currentNode->getFirstChild() ; reinf != 0 ; reinf = reinf->getNextSibling() )
                {
                    const QString tag( XMLutils::GetTagName( reinf ) );
                    if ( tag == "intro" )           // name of intro screen
                    {
                        mainWndConfig->m_sIntroScreen = XMLutils::GetFirstChildText( reinf );
                        mainWndConfig->m_nIntroLength = XMLutils::GetAttribute( reinf, gc_sLength ).toUInt();
                    }
                    else if ( tag == "outro" )
                    {
                        mainWndConfig->m_sOutroScreen = XMLutils::GetFirstChildText( reinf );
                        mainWndConfig->m_nOutroLength = XMLutils::GetAttribute( reinf, gc_sLength ).toUInt();
                    }
                    else if ( tag == "panel" ) {
                        QString file = AddPrefix( XMLutils::GetFirstChildText( reinf ),  m_screenParser->GetBasePath());
                        // Flash thing wants absolute path:
                        QDir d(file);
                        file = d.absolutePath();
                        qAPEXDEBUG("%s", qPrintable (file));
                        if ( !QFile::exists(file)) {
                            m_progress.AddError("ScreenParser", "Cannot find panel movie: " + file);
                            return false;
                        }
                        mainWndConfig->m_PanelConfig.m_sPanelMovie = file;
                    } else
                        m_progress.AddWarning( "ExperimentParser::ParseScreens", "Unknown tag: " + tag );
                }
                mainWndConfig->m_eMode = gc_eChild;
            }
            else if ( tag== "general" )
            {
                for ( DOMNode* reinf = currentNode->getFirstChild() ; reinf != 0 ; reinf = reinf->getNextSibling() )
                {
                    const QString tag( XMLutils::GetTagName( reinf ) );
                    if ( tag == "stopbutton" )
                        mainWndConfig->m_PanelConfig.m_bShowStopButton = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else if ( tag == "repeatbutton" )
                        mainWndConfig->m_PanelConfig.m_bShowRepeatButton = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else if ( tag == "showpanel" )
                        mainWndConfig->m_PanelConfig.m_bShowPanel = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else if ( tag == "showmenu" )
                        mainWndConfig->m_showMenu = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else if ( tag == "fullscreen" )
                        mainWndConfig->m_fullScreen = ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) );
                    else if ( tag == gc_sItiScreen )
                    {
                        mainWndConfig->m_sItiScreen = XMLutils::GetFirstChildText( reinf );
                        mainWndConfig->m_nItiLength = XMLutils::GetAttribute( reinf, gc_sLength ).toUInt();
                    }
                    else
                        m_progress.AddWarning( "ExperimentParser::ParseScreens", "Unknown tag: " + tag );
                }
            } else if (tag=="style") {
                mainWndConfig->setStyle(XMLutils::GetFirstChildText( currentNode ) );

            } else if (tag=="style_apex") {
                mainWndConfig->setApexStyle(XMLutils::GetFirstChildText( currentNode ));
            } else if ( tag== "uri_prefix" )
            {
                m_screenParser->mp_SetBasePath( parser::PrefixParser::Parse( (DOMElement*) currentNode ) );
            }
            else if ( tag == "defaultFont" )
            {
                const QString font = XMLutils::GetFirstChildText( currentNode );
                screens->setDefaultFont( font );
            }
            else if ( tag == "defaultFontsize" )
            {
                unsigned fs = XMLutils::GetFirstChildText( currentNode ).toUInt();
                screens->setDefaultFontSize( fs );
            }
            else
            {
                m_progress.AddWarning( "ExperimentParser::ParseScreens", "Unknown tag: " + tag );
            }
        }

        mainWndConfig->m_PanelConfig.m_eMode = mainWndConfig->m_eMode;
        mainWndConfig->m_PanelConfig.m_bShowFeedback = mainWndConfig->m_bShowFeedback;
    */
    StatusMessageDone();
    return true;
}

void apex::ExperimentParser::expandTrials(DOMElement* p_base)
{
    // get the main window so we can draw message boxes if necessary
    gui::ApexMainWindow* mwp = NULL;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();

    // fetch the script
    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    // find trials block
    DOMNode* trialNode = XMLutils::GetElementsByTagName (p_base, "trials");
    Q_ASSERT (trialNode);
    Q_ASSERT (trialNode->getNodeType() == DOMNode::ELEMENT_NODE);

    // find plugin trials and expand them
    for (DOMNode* currentNode = trialNode->getFirstChild(); currentNode != NULL;
            currentNode = currentNode->getNextSibling()) {
        const QString tag(XMLutils::GetTagName(currentNode));
        if (tag == "plugintrials") {
            qCDebug(APEX_RS, "Script library: %s", qPrintable(scriptLibrary));
            parser::ScriptExpander expander(scriptLibrary,
                                            m_generalParameters->scriptParameters(),
                                            mwp);
            expander.ExpandScript(currentNode, "getTrials");
        }
    }
}

bool apex::ExperimentParser::ParseProcedure(DOMElement* p_base)
{
    qCDebug(APEX_RS, "Parsing procedure");
    // load plugin for parser

    // Get xsi:type:
    QString type( XMLutils::GetAttribute(p_base, gc_sType));
    QList<ProcedureCreatorInterface*> creators =
        PluginLoader::Get().availablePlugins<ProcedureCreatorInterface>();

    ProcedureCreatorInterface* creator = 0;
    Q_FOREACH (ProcedureCreatorInterface* module, creators)
    {
        if (module->availableProcedurePlugins().contains(type))
        {
            creator = module;
            break;
        }
    }

    if (creator == NULL)
        return false;

    QScopedPointer<ProcedureParserInterface> parser(creator->createProcedureParser(type));

#ifndef NOSCRIPTEXPAND
    expandTrials(p_base);
#endif //NOSCRIPTEXPAND

    procedureData.reset(parser->parse(p_base));
    Q_ASSERT(procedureData);

    return true;

    /*
    //for testing constant procedure
    parser::ProcedureParser parser;
    gui::ApexMainWindow* mwp = NULL;
    if (m_interactive)
    mwp = ApexControl::Get().GetMainWnd();

    //for testing constant procedure
    parser::ProcedureParser parser(mwp);

    //use the parser to parse data into the datastructure

    try
    {
    procedureData.reset (parser.Parse ( p_base, scriptLibrary,
                                            m_generalParameters->scriptParameters() ));
    }
    catch ( ApexStringException e )
    {
        qCDebug(APEX_RS, "Exception: %s",  e.what()  );
        return false;
    }

    //qAPEXDEBUG("pointer: %p", procedureData.get());
    return procedureData->GetParameters()->CheckParameters();
        */
}


bool apex::ExperimentParser::ParseStimuli( DOMElement* p_base )
{
    AddStatusMessage(tr("Processing stimuli"));

    m_stimuli.reset(new data::StimuliData());

    QString scriptLibrary;
    if (m_generalParameters)
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    gui::ApexMainWindow* mwp = NULL;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();
    parser::StimuliParser parser(mwp);

    parser.Parse(p_base, m_stimuli.data(), scriptLibrary,
                 m_generalParameters->scriptParameters() );

    return true;

}

// Auxilary functions

void apex::ExperimentParser::AddStatusMessage( QString p_message )
{
    m_progress.addMessage( "ExperimentParser", p_message);
}

void apex::ExperimentParser::StatusMessageDone( )
{
    m_progress.addMessage( "ExperimentParser", "Done" );
}

/**
 * Do checks that can't be done using xml schema
 * @return
 */
bool apex::ExperimentParser::DoExtraValidation()
{
    // check whether every defined fixedparameter appears in every stimulus
    bool result =
        CheckProcedure() &&
        CheckFixedParameters() &&
        CheckAnswers() &&
        CheckShowResults() &&
        CheckDatablocks() &&
        CheckFilters() &&
        CheckTrials() &&
        CheckStimuli() &&
        CheckDevices() &&
        CheckRandomGenerators();

    // turn of feedback for training procedure
    //FIXME
//     if ( procedureFactory->usesFeedback() == false)
//     {
//         if (mainWndConfig->m_bShowFeedback)
//         {
//             mainWndConfig->m_bShowFeedback=false;
//             m_progress.AddWarning( "DoExtraValidation", tr("Turning off feedback for training procedure") );
//         }
//     }

    return result;
}


/**
 * check whether every defined fixedparameter appears in every stimulus
 * @return
 */
bool apex::ExperimentParser::CheckFixedParameters()
{
    return m_stimuli->CheckFixedParameters(&m_progress);
}

/**
 * Check whether the number of answers (if defined) in Corrector corresponds to the number of alternatives in Procedure
 * @return
 */
bool apex::ExperimentParser::CheckAnswers( )
{

    bool result=true;

    // check whether a standard is available for each stimulus if choices>1
    if (procedureData->choices().hasMultipleIntervals())
    {
        if (procedureData->defaultStandard().isEmpty())    // no default standard
        {
            // check each trial
            data::tTrialList trials = procedureData->GetTrials();
            for ( data::tTrialList::const_iterator it = trials.begin();  it!=trials.end(); ++it)
            {
                if ( (*it)->GetRandomStandard().isEmpty())
                {
                    m_progress.addError("ExperimentParser",
                                        tr( "Error: trial %1 has no standard and choices>1")
                                        .arg((*it)->GetID()));
                    result=false;
                }
            }
        }
    }
    else          // warn that there is no answer
    {
        data::tTrialList trials = procedureData->GetTrials();
        for ( data::tTrialList::const_iterator it = trials.begin();  it!=trials.end(); ++it)
        {
            if ( (*it)->GetAnswer().isEmpty())
            {
                m_progress.addWarning("ExperimentParser",
                                      tr( "No answer was defined for trial %1. "
                                          "This is probably unwanted.")
                                      .arg((*it)->GetID()));
            }
        }

    }

    return result;

}

bool apex::ExperimentParser::CheckDatablocks()
{
    data::DatablocksData::const_iterator it;

    QString defaultDevice;          // if there's only one device, we use it if
                                    // no device is specified in a datablock
    if ( m_devicesdata.size() ==1)
        defaultDevice = m_devicesdata.begin().key();

    for ( it = m_datablocksdata.begin(); it != m_datablocksdata.end(); it++ )
    {
        if (!defaultDevice.isEmpty() && it.value()->device().isEmpty())
            it.value()->setDevice(defaultDevice);

        //check if all datablocks have a valid device
        try
        {
            m_devicesdata.deviceData( it.value()->device() );
        }
        catch ( ApexStringException& e )
        {
            throw ApexStringException( "Error in datablock " + it.key()
                    + ": " + e.what() );
        }
    }

    return true;
}

bool apex::ExperimentParser::CheckFilters()
{
    data::FiltersData::const_iterator it;

    for ( it = m_filtersdata.begin(); it != m_filtersdata.end(); it++ )
    {
        //check if all filters have a valid device
        try
        {
            data::DeviceData* temp =
                    m_devicesdata.deviceData( it.value()->device() );

            if( temp->deviceType() != data::TYPE_WAVDEVICE)
                throw ApexStringException( tr(
                    "Filters are only implemented for a WavDevice" ) );
        }
        catch ( ApexStringException& e )
        {
            throw ApexStringException( tr( "Error in filter " ) + it.key()
                    + ": " + e.what() );
        }
    }

    return true;
}

bool apex::ExperimentParser::CheckDevices()
{
    // foreach device
    for (data::DevicesData::const_iterator it=m_devicesdata.begin();
         it!=m_devicesdata.end(); ++it)
    {
        data::DeviceData* d = it.value();

        // foreach gain parameter
        Q_FOREACH (data::Parameter param, d->parameters())
        {
            int a = param.channel();

            if (a >= (int)d->numberOfChannels())
            {
                throw ApexStringException(tr("Error in device %1: invalid"
                                             " channel number %2 in gain")
                                             .arg(it.key()).arg( a));
            }

        }
    }

    return true;
}


bool apex::ExperimentParser::CheckRandomGenerators()
{

    foreach (data::RandomGeneratorParameters* params, m_randomgenerators) {
        if (params->m_nType == params->TYPE_UNIFORM && params->m_nValueType==params->VALUE_INT &&
                ( params->m_dMin != (int)params->m_dMin || params->m_dMax != (int)  params->m_dMax ))
            m_progress.addWarning("ExperimentParser::CheckRandomGenerators",
                                  tr("Limits of uniform int random generator are not integers, results may be unexpected"));
    }
    return true;

}

bool apex::ExperimentParser::ParseRandomGenerators(DOMElement* p_base )
{
    AddStatusMessage(tr("Processing random generators"));

    bool result=true;

    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName( currentNode );
        if (tag == "randomgenerator")
        {
            result = result & ParseRandomGenerator((DOMElement*)currentNode);
        }
        else
            m_progress.addWarning( "ExperimentParser::ParseRandomGenerators",
                                   "Unknown tag: " + tag );
    }

    StatusMessageDone();
    return result;
}


bool apex::ExperimentParser::ParseRandomGenerator(DOMElement* p_base)
{
    data::RandomGeneratorParameters* param = new data::RandomGeneratorParameters;
    param->Parse(p_base);

    Q_CHECK_PTR(param);

    QString id=XMLutils::GetAttribute(p_base, sc_sID);

    m_randomgenerators[id]=param;

    return true;
}


bool apex::ExperimentParser::ParseResults(DOMElement* p_base )
{
    AddStatusMessage(tr("Processing result parameters"));
    m_resultParameters.reset(new data::ResultParameters(p_base));
    Q_CHECK_PTR(m_resultParameters.data());

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
    if (!standard.isEmpty() && !m_stimuli->contains(standard))
    {
        m_progress.addError("ScreenChecker",
                "Invalid default standard stimulus " + standard);
        return false;
    }

    //only the first adapting parameter of an adaptive procedure is allowed
    //to be a fixed parameter
    const data::ProcedureData* parameters = procedureData.data();
    if (parameters->type() == data::ProcedureData::AdaptiveType)
    {
        QStringList adaptingParameters =
            static_cast<const data::AdaptiveProcedureData*>(parameters)
                                        ->adaptingParameters();

        if (!adaptingParameters.isEmpty())
        {
            adaptingParameters.takeFirst();

            Q_FOREACH (QString param, adaptingParameters)
            {
                if (m_stimuli->GetFixedParameters().contains(param))
                {
                    m_progress.addError("ProcedureChecker",
                                        "Only the first adaptive parameter can "
                                        "be a fixed parameter.");

                    return false;
                }
            }
        }
    }

    // Check if there is an interval defined for each choice
    if (parameters->choices().hasMultipleIntervals()) {
        QStringList mIntervals( parameters->choices().intervals() );
        for (int i=0; i<mIntervals.length() ; ++i) {
            if (mIntervals[i].isEmpty()) {
                m_progress.addWarning("IntervalChecker",
                                      QString("No screenelement defined for interval %1").arg(i+1));
            }
        }
    }

    return true;
}

bool ExperimentParser::CheckTrials()
{
    data::ScreensData::ScreenMap& lscreens = screens->getScreens();

    bool result=true;
    const data::tTrialList& trials = procedureData->GetTrials();
    for (data::tTrialList::const_iterator it=trials.begin();
            it!=trials.end(); ++it)
    {
        QString screen = (*it)->GetScreen();
       // qCDebug(APEX_RS) << lscreens.find(screen);
        if (lscreens.find(screen)==lscreens.end() ) {
            m_progress.addError("TrialChecker", "Invalid screen in trial " + (*it)->GetID());
            result = false;
        }

        const tStimulusList& stimulusList = (*it)->GetStimuli();
        for (int i=0; i<stimulusList.size(); ++i) {
            if (!m_stimuli->contains( stimulusList.at(i) )) {
                m_progress.addError("TrialChecker",
                                    "Invalid stimulus in trial " + (*it)->GetID());
                result = false;
            }
        }
        if (stimulusList.isEmpty())
            m_progress.addWarning("TrialChecker",
                    tr("No stimulus was found in trial %1, is this "
                            "what you want?").arg((*it)->GetID()));

        const tStimulusList& standardList = (*it)->GetStandards();
        for (int i=0; i<standardList.size(); ++i) {
            if (!m_stimuli->contains( standardList.at(i) )) {
                m_progress.addError("TrialChecker", "Invalid standard in trial " + (*it)->GetID());
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
    //if this is an adaptive procedure which adapts a fixed parameter and has a
    //min/max value set for the parameter, than for every fixed parameter there
    //should be at least one stimulus with a value greater/smaller than the
    //min/max value.
    const data::AdaptiveProcedureData* params =
        dynamic_cast<const data::AdaptiveProcedureData*>(procedureData.data());

    if (params != 0)
    {
        QString fixedParam = params->adaptingParameters().first();

        if (m_stimuli->GetFixedParameters().contains(fixedParam) &&
            (params->hasMinValue() || params->hasMaxValue()))
        {
            data::adapting_parameter min =
                        std::numeric_limits<data::adapting_parameter>::max();
            data::adapting_parameter max =
                        std::numeric_limits<data::adapting_parameter>::min();

            Q_FOREACH (data::StimulusData stimulus, m_stimuli->values())
            {
                QVariant variant = stimulus.GetFixedParameters().value(fixedParam);
                Q_ASSERT(variant.canConvert<data::adapting_parameter>());
                data::adapting_parameter value =
                                variant.value<data::adapting_parameter>();

                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }

            if (params->hasMinValue() && max < params->minValue())
            {
                QString error =
                    tr("The adaptive procedure adapts a fixed parameter (%1) and has "
                        "a minimum parameter value set but no stimulus exists "
                        "with a fixed parameter value greater than this minimum");

                m_progress.addError("StimulusChecker", error.arg(fixedParam));
                return false;
            }

            if (params->hasMaxValue() && min > params->maxValue())
            {
                QString error =
                    tr("The adaptive procedure adapts a fixed parameter (%1) and has "
                        "a maximum parameter value set but no stimulus exists "
                        "with a fixed parameter value smaller than this maximum");

                m_progress.addError("StimulusChecker", error.arg(fixedParam));
                return false;
            }
        }
    }

    for ( data::StimuliData::const_iterator it= m_stimuli->begin();
          it!=m_stimuli->end(); ++it)
    {
        const data::StimulusData& data = it.value();
        const data::StimulusDatablocksContainer datablocks =
                data.GetDatablocksContainer();

        if (!CheckStimulusDatablocks(datablocks, it.key()))
            return false;

//         const PlayMatrix& ac_Mat = *data.GetPlayMatrix();
//
//         const unsigned nS = ac_Mat.mf_nGetBufferSize();
//         const unsigned nP = ac_Mat.mf_nGetChannelCount();
// //        Q_ASSERT( nS && nP );
//
//
//         for( unsigned i = 0 ; i < nS ; ++i )
//         {
//             for( unsigned j = 0 ; j < nP ; ++j )
//             {
//                 const QString& sCur = ac_Mat( j, i );
//                 if( !sCur.isEmpty() )
//                 {
//                     if ( m_datablocksdata.find(sCur)==m_datablocksdata.end()) {
//                         m_progress.AddError("StimulusChecker",
//                                             QString("Invalid datablock %1 in trial %2").arg(sCur).arg(it.key()));
//                         success=false;
//                     }
//                 }
//             }
//         }

    }

    return true;
}

bool apex::ExperimentParser::CheckStimulusDatablocks(
    apex::data::StimulusDatablocksContainer datablocks, QString trial)
{
    using apex::data::StimulusDatablocksContainer;
    StimulusDatablocksContainer::const_iterator it;
    for (it = datablocks.begin(); it != datablocks.end(); it++)
    {
        if (it->type == StimulusDatablocksContainer::DATABLOCK)
        {
            QString id = it->id;

            Q_ASSERT(!id.isEmpty());

            if (m_datablocksdata.find(id) == m_datablocksdata.end())
            {
                m_progress.addError("StimulusChecker",
                              QString("Invalid datablock %1 in stimulus %2")
                                      .arg(id)
                                      .arg(trial));
                return false;
            }
        }
        else
        {
            Q_ASSERT(it->type == StimulusDatablocksContainer::DATABLOCKS ||
                    it->type == StimulusDatablocksContainer::SEQUENTIAL ||
                    it->type == StimulusDatablocksContainer::SIMULTANEOUS);

            return CheckStimulusDatablocks(*it, trial);
        }
    }

    return true;
}

bool apex::ExperimentParser::ParseGeneral(DOMElement* p_base )
{
    AddStatusMessage(tr("Processing general parameters"));
    m_generalParameters.reset(new data::GeneralParameters(p_base));
    Q_CHECK_PTR(m_generalParameters.data());

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
    if (m_resultParameters &&
        (m_resultParameters->showResultsAfter() || m_resultParameters->saveResults()) &&
        m_resultParameters->resultPage().isEmpty()) {
        m_progress.addError("CheckShowResults", "No page specified for analyzing results");
        return false;
    }

    return true;
}


bool apex::ExperimentParser::ParseDescription( DOMElement* p_base )
{
    m_description = XMLutils::GetFirstChildText( p_base );
    return true;
}

bool apex::ExperimentParser::upgradeFrom(QDomDocument& doc,
                                                 const QVector<int>& v)
{
    QString versionString;
    versionString.sprintf("%d.%d.%d", v[0], v[1], v[2]);

    bool upgraded = false;
    if ((v[1] < 1) && (v[2] < 2))  {
        //upgradeTo3_0_2(doc);
        // Only additions, no changes
        upgraded = true;
    }
    if (v[1] < 1) {
        upgradeTo3_1_0(doc);
        upgradeTo3_1_1(doc);

        upgraded = true;
    }
    else if( v[2] < 1) {
        upgradeTo3_1_1(doc);
        upgraded = true;
    }
    else if( v[2] < 3) {
        upgraded = true;
    }

    if (upgraded) {
        doc.documentElement().setAttribute("xmlns:apex",
                                           getConfigfileNamespace());
        doc.documentElement().setAttribute("xsi:schemaLocation",
                getConfigfileNamespace()
                    + QString(" https://exporl.med.kuleuven.be/apex/schemas/%1/experiment.xsd").arg(SCHEMA_VERSION) );

                //QString::fromUtf8(QUrl::fromLocalFile(Paths::Get().GetExperimentSchemaPath()).toEncoded()));

        upgradedDoc = doc.toString();
        return true;
    }
    else
        return false;
}

void apex::ExperimentParser::upgradeTo3_1_0( QDomDocument& doc )
{
    // Change procedure xsi:type
    {
        QString newName;

        QDomNodeList list = doc.documentElement().elementsByTagName("procedure");
        for(int i = 0; i < list.count(); ++i) {
            QDomElement procedureElement = list.at(i).toElement();
            QString oldName( procedureElement.attribute("xsi:type") );
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
                m_progress.addWarning("Upgrade", QString("Cannot upgrade procedure type %1").arg(procedureElement.attribute("xsi:type")));

            procedureElement.setAttribute("xsi:type", newName);
        }
    }

    // Remove choices & corrector and add intervals
    {
        const QDomNodeList procedureNodes = doc.documentElement().elementsByTagName("procedure");
        for (int i = 0; i < procedureNodes.count(); ++i) {
            QDomElement procedureElement = procedureNodes.at(i).toElement();
            const QDomElement correctorElement = doc.documentElement().elementsByTagName("corrector").at(0).toElement();

            QDomElement parametersElements = procedureElement.elementsByTagName("parameters").at(0).toElement();
            const QDomElement choicesElement = parametersElements.elementsByTagName("choices").at(0).toElement();

            if (correctorElement.attribute("xsi:type") == "apex:alternatives") {
                //Transform the answers into intervals
                const QDomElement answerselement = correctorElement.elementsByTagName("answers").at(0).toElement();
                const QDomNodeList answers = answerselement.elementsByTagName("answer");

                QDomElement intervalsElement = doc.createElement("intervals");
                intervalsElement.setAttribute("count", answers.count());

                for (int i = 0; i < answers.count(); ++i) {
                    QDomElement intervalElement = doc.createElement("interval");

                    intervalElement.setAttribute("number", answers.at(i).toElement().attribute("number"));
                    intervalElement.setAttribute("element", answers.at(i).toElement().attribute("value"));

                    intervalsElement.appendChild(intervalElement);
                }

                //Find a location to insert the intervals
                if (choicesElement.isNull()) {
                    QDomElement afterThis = parametersElements.elementsByTagName("uniquestandard").at(0).toElement();
                    if (afterThis.isNull()) {
                        afterThis = parametersElements.elementsByTagName("defaultstandard").at(0).toElement();
                        if (afterThis.isNull()) {
                            afterThis = parametersElements.elementsByTagName("order").at(0).toElement(); //order is required, so we can stop here
                        }
                    }

                    parametersElements.insertAfter(intervalsElement, afterThis);
                } else {
                    QString select = choicesElement.attribute("select", "");
                    if (!select.isEmpty()) {
                        intervalsElement.setAttribute("select", select);
                    }
                    parametersElements.replaceChild(intervalsElement, choicesElement);
                }
            } else {
                if (!choicesElement.isNull()) {
                    ErrorHandler::Get().addError(
                            tr("XML parser"),
                            tr("Can't convert a procedure with a choices element and a corrector that isn't an alternatives corrector."));
                } else {
                    QDomElement afterThis = parametersElements.elementsByTagName("uniquestandard").at(0).toElement();
                    if (afterThis.isNull()) {
                        afterThis = parametersElements.elementsByTagName("defaultstandard").at(0).toElement();
                        if (afterThis.isNull()) {
                            afterThis = parametersElements.elementsByTagName("order").at(0).toElement(); //order is required, so we can stop here
                        }
                    }

                    parametersElements.insertAfter(correctorElement, afterThis);
                }
            }
            //Delete the obsolete corrector
            doc.documentElement().removeChild(correctorElement);
        }

        {//Remove birth from datablock
            QDomNodeList list = doc.documentElement().elementsByTagName("datablock");
            for(int i=0; i<list.count(); ++i) {
                QDomElement datablock = list.at(i).toElement();
                QDomNodeList births = datablock.elementsByTagName("birth");
                if(!births.isEmpty()) {
                    datablock.removeChild(births.at(0));
                }
            }
        }

        { //Change silence:// to silence:
            QDomNodeList list = doc.documentElement().elementsByTagName("datablock");
            for(int i=0; i<list.count(); ++i) {
                QDomElement datablock = list.at(i).toElement();
                QDomNode uri = datablock.elementsByTagName("uri").at(0);

                QDomText text = uri.firstChild().toText();
                QString value = text.nodeValue();
                if(value.contains("silence://")) {
                    value.replace("silence://", "silence:");
                    uri.firstChild().setNodeValue(value);
                }
            }
        }
    }
}


void apex::ExperimentParser::upgradeTo3_1_1( QDomDocument& doc )
{
    //Get the contents of the results section
    QDomNodeList resultsList = doc.documentElement().elementsByTagName("results");

    QDomNode results = resultsList.item(0);
    //Get all the child nodes from the resultsType section:
    QDomNodeList resultNodes = results.childNodes();
    QDomNode currentNode, pageNode, realtimeNode, showNode, subjectNode, scriptNode;
    QString pageName;
    QDomNodeList parameterList;

    for(int i=0; i<resultNodes.length(); i++)
    {
        currentNode = resultNodes.item(i);
        if(currentNode.nodeName() == "xsltscript")
        {
            //Take the name of the script as the name of the html page.
            QDomNode scriptTextNode = currentNode.firstChild();
            /*pageName = scriptTextNode.nodeValue();
            pageName.truncate(pageName.lastIndexOf("."));
            pageName.append(".html");*/
            results.removeChild(currentNode);
            //go to the next node, the list is updated live
            currentNode = resultNodes.item(i);
        }
        if(currentNode.nodeName() == "xsltscriptparameters")
        {
            parameterList = currentNode.childNodes();
            //the children will be appended to the new node.
            results.removeChild(currentNode);
            currentNode = resultNodes.item(i);
        }
        if(currentNode.nodeName() == "showresults")
        {
            showNode = currentNode.cloneNode();

            results.removeChild(currentNode);
            //go to the next node, the list is updated live
            currentNode = resultNodes.item(i);
        }
        if(currentNode.nodeName() == "javascript")
        {
            //Get the childNodes.
            QDomNodeList javascriptList = currentNode.childNodes();
            QDomNode currentJavascriptNode;
            for(int j = 0; j < javascriptList.length(); j++)
            {
                currentJavascriptNode = javascriptList.item(j);
                if (currentJavascriptNode.nodeName() == "page")
                {
                    pageNode = currentJavascriptNode.cloneNode();
                }
                else if (currentJavascriptNode.nodeName() == "realtime")
                {
                    realtimeNode = currentJavascriptNode.cloneNode();
                }
                else if (currentJavascriptNode.nodeName() == "showafterexperiment")
                {
                    showNode = currentJavascriptNode.cloneNode();
                }
            }

            //Now remove the javascript node
            results.removeChild(currentNode);
            currentNode = resultNodes.item(i);

        }
        if(currentNode.nodeName() == "matlabscript")
        {
            scriptNode = currentNode.cloneNode();
        }
        if(currentNode.nodeName() == "subject")
        {
            subjectNode = currentNode.cloneNode();
        }
        if(currentNode.nodeName() == "page")
        {
            pageNode = currentNode;
        }

    }

    //create the parameter node and default element:
    QDomElement resultParameters= doc.createElement("resultparameters");
    //Append the parameters to the new node
    if(!parameterList.isEmpty())
    {
        for(int i = 0; i<parameterList.count(); i++)
        {
            resultParameters.appendChild(parameterList.item(i));
        }
    }

    //check if the old nodes are present, if so, add them
    //at the right place, if not, create defaults and add them.
    QDomElement showAfterElement;
    showAfterElement = showNode.toElement();
    if(showAfterElement.isNull())
    {
        showAfterElement = doc.createElement("showafterexperiment");
        showAfterElement.appendChild(doc.createTextNode("false"));
    }
    else
    {
        showAfterElement.setTagName("showafterexperiment");
    }

    QDomElement realtimeElement;
    if(realtimeNode.isNull())
    {
        realtimeElement = doc.createElement("showduringexperiment");
        realtimeElement.appendChild(doc.createTextNode("false"));
    }
    else
    {
        realtimeElement.setTagName("showduringexperiment");
    }

    QDomElement pageElement;
    if(pageNode.isNull())
    {
        if(pageName.isEmpty()) {
            pageName = "apex:resultsviewer.html";
        }
        pageElement = doc.createElement("page");
        QDomText pageText;
        pageText = doc.createTextNode(pageName);
        pageElement.appendChild(pageText);
    }
    else
    {
        pageElement = pageNode.toElement();
    }

    results.insertBefore(showAfterElement, resultNodes.item(0));
    results.insertBefore(realtimeElement, resultNodes.item(0));
    results.insertBefore(resultParameters, resultNodes.item(0));
    results.insertBefore(pageElement, resultNodes.item(0));

    //insert the new results in the dom document.
    //First get the right position to set the results:
    QDomNode previousChild = results.previousSibling();
    doc.removeChild(results);
    //doc.insertAfter(results, previousChild);
    //doc.clear();
}

QString apex::ExperimentParser::getUpgradedConfigFile()
{
    return upgradedDoc;
}
