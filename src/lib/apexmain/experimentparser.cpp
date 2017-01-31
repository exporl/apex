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

#include "appcore/deleter.h"

#include "calibration/calibrationdata.h"
#include "calibration/calibrationparser.h"

#include "connection/connection.h"
#include "connection/connectiondata.h"
#include "connection/connectionparser.h"

#include "corrector/correctordata.h"
#include "corrector/correctorparser.h"

#include "datablock/datablockdata.h"
#include "datablock/datablocksdata.h"
#include "datablock/datablocksparser.h"
#include "datablock/datablocksparser.h"

#include "device/devicesparser.h"
#include "device/iapexdevice.h"

#include "device/mixer/mixerfactory.h"
#include "device/mixer/mixerparameters.h"

#include "filter/filterparser.h"

#include "gui/guidefines.h"
#include "gui/mainwindow.h"

#include "interactive/parameterdialog.h"
#include "interactive/parameterdialogresults.h"

#include "parameters/apexmapparameters.h"
#include "parameters/apexmapparameters.h"
#include "parameters/apexparameters.h"
#include "stimulus/stimulusparameters.h"
#include "parameters/generalparameters.h"
#include "parameters/parametermanagerdata.h"

#include "parser/prefixparser.h"

#include "procedure/apexprocedureconfig.h"
#include "procedure/apextrialfactory.h"
#include "procedure/procedureparser.h"

#include "randomgenerator/randomgeneratorparameters.h"

#include "result/resultparameters.h"

#include "screen/screensdata.h"
#include "screen/screensparser.h"

#include "services/errorhandler.h"
#include "services/mainconfigfileparser.h"

#include "stimulus/datablock.h"
#include "stimulus/filter.h"
#include "stimulus/idevicefactory.h"
#include "stimulus/outputdevice.h"
#include "stimulus/stimulidata.h"
#include "stimulus/stimuliparser.h"
#include "stimulus/stimulus.h"

#include "stimulus/wav/wavdevicefactory.h"

#include "xml/apexxmltools.h"
#include "xml/xercesinclude.h"
#include "xml/xmlkeys.h"

#include "apexfactories.h"
#include "apextools.h"
#include "exceptions.h"
#include "experimentdata.h"
#include "experimentparser.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QMessageBox>
#include <QDomDocument>
#include <QXmlQuery>
#include <QDebug>

#include <limits>

using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace xercesc;


#ifdef APEXDEBUG
#include <iostream>
#endif

using namespace apex;
using namespace apex::device;

apex::ExperimentParser::ExperimentParser( const QString & configFilename) :
        ConfigFileParser(configFilename, Paths::Get().GetExperimentSchemaPath(), getConfigfileNamespace()) ,
        m_interactive(true),
        m_progress( ErrorHandler::Get() ),
        //m_fixedParameters( 0 ),
        m_resultParameters(  new data::ResultParameters(0)  ),
        m_generalParameters( 0 ),
        m_parameterDialogResults( 0 ),
        //m_screenParser( 0 ),
//        mainWndConfig (new MainWindowConfig()),
        screens(),
        connectionDatas (new data::ConnectionsData())
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
    qDebug("ExperimentParser: parsing...");

    if (!ApplyXpathModifications() || !Parsefile())
        throw ParseException();

    std::auto_ptr<QMap<QString, data::RandomGeneratorParameters*> >
            randGen(new QMap<QString, data::RandomGeneratorParameters*>(
                            GetRandomGenerators()));
    std::auto_ptr<data::ParameterDialogResults>
            paramDlg(GetParameterDialogResults());
    std::auto_ptr<data::DevicesData>
            devData(new data::DevicesData(GetDevicesData()));
    std::auto_ptr<data::FiltersData>
            filtData(new data::FiltersData(GetFiltersData()));
    std::auto_ptr<data::DevicesData>
            ctrlDevData(new data::DevicesData(GetControlDevices()));
    std::auto_ptr<data::DatablocksData>
            datablocksData(new data::DatablocksData(GetDatablocksData()));
    return new data::ExperimentData(GetConfigFilename(),
                                    GetScreens(),
                                    GetProcedureConfig(),
                                    GetCorrectorData(),
                                    GetConnectionDatas(),
                                    GetCalibrationData(),
                                    GetGeneralParameters(),
                                    GetResultParameters(),
                                    paramDlg,
                                    randGen,
                                    devData,
                                    filtData,
                                    ctrlDevData,
                                    datablocksData,
                                    GetStimuliData(),
                                    GetExperimentDescription(),
                                    GetParameterManagerData());
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

    ParameterDialog pd(m_document, mwp);

    if (pd.entries().isEmpty())
        return true;
    bool result = pd.exec();

    if (result)
        pd.apply();
    else
    {
        result = QMessageBox::warning(mwp,
                tr("Interactive changes not applied"),
                tr("Warning: no changes were applied, do you want to continue"
                   " with the unmodified document?"),
                QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;
    }

    m_parameterDialogResults.reset(pd.results());

    return result;
}

apex::ExperimentParser::~ExperimentParser()
{
//    delete m_screenParser;
//  m_filters.clear();
//    m_datablocks.clear();
    m_devices.clear();
//    m_stimuli.clear();
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
        m_generalParameters.reset( new data::GeneralParameters() );

    for (DOMNode* currentNode=m_document->getDocumentElement()->getFirstChild();
         currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
        const QString tag = XMLutils::GetTagName( currentNode );
        qDebug("Parsing %s", qPrintable(tag));
        if (tag=="description")
            bSuccess &= ParseDescription((DOMElement*)currentNode);
        else if      (tag == "filters")
            bSuccess &= ParseFilters((DOMElement*)currentNode);
        else if (tag == "datablocks")
            bSuccess &= ParseDatablocks((DOMElement*)currentNode);
        else if (tag == "procedure")
            bSuccess &= ParseProcedure((DOMElement*)currentNode);
        else if (tag == "stimuli")
            bSuccess &= ParseStimuli((DOMElement*)currentNode);
        else if (tag == "devices")
            bSuccess &= ParseDevices((DOMElement*)currentNode);
        else if (tag == "screens")
            bSuccess &= ParseScreens((DOMElement*)currentNode);
        else if (tag == "corrector")
            bSuccess &= ParseCorrector((DOMElement*)currentNode);
        else if (tag == "connections")
            bSuccess &= ParseConnections((DOMElement*)currentNode);         // connections should be parsed AFTER datablocks, devices and filters because they need to lookup their device
        else if (tag == "randomgenerators")
            bSuccess &= ParseRandomGenerators((DOMElement*)currentNode);
        else if (tag == "calibration")
            bSuccess &= ParseCalibration((DOMElement*)currentNode);
        else if (tag == "results")
            bSuccess &= ParseResults((DOMElement*)currentNode);
        else if (tag == "general");
        // skipping, already parsed before
        else if (tag == "interactive");
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
            for (data::ConnectionsData::const_iterator it=connectionDatas->begin();                 it!=connectionDatas->end(); ++it)
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
    m_CalibrationData.reset (parser.Parse (a_pCalib));

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
    data::FilterData* parameters = parser.ParseFilter( p_filter, /*m_devicesdata, m_datablocksdata,*/ parameterManagerData.get());
    m_progress.addItems(parser.logger().items());
    if ( !parameters )
        return 0;

    m_filtersdata[id]=parameters;

    return true;
}

bool apex::ExperimentParser::ParseDevices(DOMElement* p_base)
{
    qDebug("Parsing devices");
    parser::DevicesParser parser;
    try
    {
        parser::tAllDevices all = parser.Parse(p_base, parameterManagerData.get());
        m_devicesdata = all.outputdevices;
        m_controldevicesdata = all.controldevices;
        //m_sMasterDevice= all.outputdevices.GetMasterDevice();
    }
    catch (ApexStringException &e)
    {
        qDebug("Number of errors: %u", parser.logger().numberOfErrors());
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
    if (m_generalParameters.get())
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
    if (m_generalParameters.get())
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    gui::ApexMainWindow* mwp = 0;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();
    parser::ScreensParser parser(mwp);

    screens.reset(parser.ParseScreens(p_base, scriptLibrary,
                                      m_generalParameters->scriptParameters(),
                                      parameterManagerData.get() ));

    if (!screens.get())
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

bool apex::ExperimentParser::ParseProcedure(DOMElement* p_base)
{
//     AddStatusMessage(tr("Processing procedure"));
//
//     // get type of procedure
//     const QString myType( XMLutils::GetAttribute( p_base, sc_sType ) );
//
//     procedureFactory.reset (ApexProcedureFactory::GetProcedureFactory (myType));
//     procedureData.reset (procedureFactory->MakeProcedureConfig (p_base));
//
//     StatusMessageDone();
//
    // return procedureData->GetParameters()->CheckParameters();

    QString scriptLibrary;
    if (m_generalParameters.get())
        scriptLibrary = m_generalParameters->GetScriptLibrary();

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
        qDebug ( "Exception: %s",  e.what()  );
        return false;
    }

    //qAPEXDEBUG("pointer: %p", procedureData.get());
    return procedureData->GetParameters()->CheckParameters();
}


bool apex::ExperimentParser::ParseStimuli( DOMElement* p_base )
{
    AddStatusMessage(tr("Processing stimuli"));

    m_stimuli.reset(new data::StimuliData());

    QString scriptLibrary;
    if (m_generalParameters.get())
        scriptLibrary = m_generalParameters->GetScriptLibrary();

    gui::ApexMainWindow* mwp = NULL;
    if (m_interactive)
        mwp = ApexControl::Get().GetMainWnd();
    parser::StimuliParser parser(mwp);

    parser.Parse(p_base, m_stimuli.get(), scriptLibrary,
                 m_generalParameters->scriptParameters() );

    return true;

    /*

    StimulusFactory* factory = StimulusFactory::sf_pInstance();

    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName( currentNode );
        if (tag == "stimulus")
        {
            Stimulus* s = factory->CreateStimulus( (DOMElement*)currentNode);
            m_stimuli[s->GetID()] = s;
        }
        else if (tag == "fixed_parameters")
        {
            m_fixedParameters.reset( new ApexMapParameters((DOMElement*)currentNode) );
        }
        else
        {
            m_progress.AddWarning( "ExperimentParser::ParseStimuli",
                                   "Unknown tag: " + tag );
        }

    }

    StatusMessageDone();
    return true;*/
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
/*    std::map<QString,QString>::const_iterator i = m_fixedParameters->begin();
    for (; i!=m_fixedParameters->end(); ++i)
    {
        //qAPEXDEBUG("Checking parameter %s", (*i).first.ascii() );

        std::map<QString, Stimulus*>::const_iterator j = m_stimuli.begin();
        bool tempresult=true;
        for (; j!=m_stimuli.end(); ++j)
        {
            if ( ! (*j).second->GetFixParameters()->HasParameter((*i).first)  )
            {
                m_progress.AddError( "CheckFixedParameters", QString("Error: fixed parameter %1 not found in stimulus %2").arg( (*i).first ).arg((*j).second->GetID()) );
                tempresult=false;
            }
        }
        if (tempresult==false)
        {
            return false;
        }
    }

    return true;*/
    return m_stimuli->CheckFixedParameters(&m_progress);
}


bool apex::ExperimentParser::ParseCorrector(DOMElement* p_base)
{
    correctorData.reset (CorrectorParser().Parse (p_base));
    return true;
}


/**
 * Check whether the number of answers (if defined) in Corrector corresponds to the number of alternatives in Procedure
 * @return
 */
bool apex::ExperimentParser::CheckAnswers( )
{

    bool result=true;

    // check whether a standard is available for each stimulus if choices>1
    if (procedureData->GetParameters()->GetChoices().nChoices>1)
    {
        if (procedureData->GetParameters()->GetDefaultStandard().isEmpty())    // no default standard
        {
            // check each trial
            std::vector<data::ApexTrial*> trials = procedureData->GetTrials();
            for ( std::vector<data::ApexTrial*>::const_iterator it = trials.begin();  it!=trials.end(); ++it)
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
        std::vector<data::ApexTrial*> trials = procedureData->GetTrials();
        for ( std::vector<data::ApexTrial*>::const_iterator it = trials.begin();  it!=trials.end(); ++it)
        {
            if ( (*it)->GetAnswer().string().isEmpty())
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
    m_resultParameters.reset( new data::ResultParameters(p_base) );
    Q_CHECK_PTR(m_resultParameters.get());

    bool result = m_resultParameters->Parse(p_base);

    StatusMessageDone();
    return result;
}

bool apex::ExperimentParser::CreateMissing()
{
    if (!m_generalParameters.get())
        m_generalParameters.reset( new data::GeneralParameters() );

    return true;
}


/**
 * Check whether the procedure refers to valid stimuli
 */
bool ExperimentParser::CheckProcedure()
{
    bool result = true;

    const QString standard = procedureData->GetParameters()->GetDefaultStandard();
    if (!standard.isEmpty() && !m_stimuli->contains(standard)) {
        m_progress.addError("ScreenChecker",
                "Invalid default standard stimulus " + standard);
        result = false;
    }

    return result;
}

bool ExperimentParser::CheckTrials()
{
    data::ScreensData::ScreenMap& lscreens = screens->getScreens();

    bool result=true;
    const std::vector<data::ApexTrial*>& trials = procedureData->GetTrials();
    for (std::vector<data::ApexTrial*>::const_iterator it=trials.begin();
            it!=trials.end(); ++it)
    {
        QString screen = (*it)->GetScreen();
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
bool apex::ExperimentParser::CheckStimuli() {
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
// //        assert( nS && nP );
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
    m_generalParameters.reset( new data::GeneralParameters(p_base) );
    Q_CHECK_PTR(m_generalParameters.get());

    bool result = m_generalParameters->Parse(p_base);

    StatusMessageDone();
    return result;
}


/**
* Check whether an xslt script is available if showresults is true
* @return
*/
bool apex::ExperimentParser::CheckShowResults( )
{
    if (m_resultParameters.get() &&
        ( m_resultParameters->showXsltResultsAfter()  ||
        m_resultParameters->GetSaveResults()))
    {
        if ( m_resultParameters->GetXsltScript().isEmpty())
        {
            m_progress.addError("CheckShowResults", "No script specified for analyzing results");
            return false;
        }
    }

    return true;
}


bool apex::ExperimentParser::ParseDescription( DOMElement* p_base )
{
    m_description = XMLutils::GetFirstChildText( p_base );
    return true;
}

const data::ParameterDialogResults* apex::ExperimentParser::GetParameterDialogResults() const
{
    return m_parameterDialogResults.get();
}


bool apex::ExperimentParser::upgradeFrom(QDomDocument& doc,
                                                 const QVector<int>& v)
{
    QString versionString;
    versionString.sprintf("%d.%d.%d", v[0], v[1], v[2]);

    bool upgraded = false;
    if (v[2] < 2) {
        //upgradeTo302(doc);
        // Only additions, no changes
        upgraded = true;
    }

    // if( v[2] < 3) ...

    if (upgraded) {
        doc.documentElement().setAttribute("xmlns:apex",
                                           getConfigfileNamespace());
        doc.documentElement().setAttribute("xsi:schemaLocation",
                getConfigfileNamespace() + " " +
                QString::fromUtf8(QUrl::fromLocalFile(Paths::Get().GetExperimentSchemaPath()).toEncoded()));
        return true;
    }
    else
        return false;
}

/*void apex::ExperimentParser::upgradeTo302( QDomDocument& doc )
{
    return;
}*/
