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

#include "experimentrundelegate.h"
#include "apextypedefs.h"

#include "parameters/parametermanager.h"
#include "fileprefixconvertor.h"

#include "experimentparser.h"
#include "gui/mainwindow.h"
//#include "gui/mainwindowconfig.h"
#include "screen/screensdata.h"
#include "screen/screenrundelegate.h"
#include "screen/apexscreen.h"

#include "apexcontrol.h"
#include "connection/connectionrundelegatecreator.h"
#include "stimulus/playmatrix.h"
#include "stimulus/stimulidata.h"

#include "stimulus/wav/wavdatablock.h"
#include "stimulus/wav/streamgenerator.h"
#include "stimulus/stimulusoutput.h"

#include "stimulus/wav/wavdevice.h"
#include "device/wavdevicedata.h"
#include "device/l34devicedata.h"
#include "stimulus/l34/l34device.h"
#include "stimulus/clarion/clariondevice.h"
#include "stimulus/clarion/clariondatablock.h"

#include "stimulus/dummy/dummydevice.h"

#include "filter/filterdata.h"
#include "filter/pluginfilterdata.h"
#include "filter/wavgenerator.h"
#include "filter/dataloopgeneratorfilter.h"
#include "stimulus/wav/pluginfilter.h"
#include "stimulus/wav/wavdevicefactory.h"
#include "stimulus/filter.h"
#include "stimulus/wav/wavfader.h"
#include "stimulus/wav/wavfilter.h"
#include "stimulus/filtertypes.h"

#include "device/mixer/mixerparameters.h"
#include "device/mixer/streamappmixer.h"
#include "device/plugincontroller.h"
#include "device/plugincontrollerdata.h"
#include "device/pa5/pa5device.h"
#include "device/controllers.h"

#include "procedure/apexadaptiveprocedure.h"
#include "procedure/apextrainingprocedure.h"
#include "procedure/apexmultiprocedure.h"
#include "procedure/apexmultiprocedureconfig.h"
#include "procedure/pluginprocedure.h"

#include "corrector/correctordata.h"
#include "corrector/fcacorrector.h"
#include "corrector/equalcomparator.h"
#include "corrector/cvccomparator.h"

#include "randomgenerator/randomgeneratorfactory.h"
#include "randomgenerator/randomgenerators.h"

#include "feedback/feedback.h"

#include "timer/apextimer.h"

#include "resultsink/apexresultsink.h"
#include "resultsink/rtresultsink.h"

#include "calibration/calibrationdata.h"
#include "calibration/calibrationdatabase.h"
#include "calibration/calibrator.h"


//from libdata
#include "device/clariondevicedata.h"
#include "experimentdata.h"

#include <QMessageBox>
#include <QWidget>

using namespace apex;
using namespace apex::data;
using namespace apex::stimulus;

#ifdef _MSC_VER
#pragma warning ( disable : 4065 ) //switch statement without case labels
#endif

namespace apex
{
typedef std::map<const data::Screen*,gui::ScreenRunDelegate*> ScreenToDelegateMapT;

class ExperimentRunDelegatePrivate
{
    public:
        ExperimentRunDelegatePrivate (QWidget* parent, bool d) :
                parent (parent),
                deterministic(d),
                autoAnswer(false),
                blockSize(-1),
                mod_timer(0),
                mod_screen(0),
                mod_procedure(0),
                mod_corrector(0),
                mod_resultsink(0),
                mod_rtresultsink(0),
                mod_randomgenerators(0),
                mod_controllers(0),
                mod_calibrator(0),
                mod_output(0)
        {}
        ScreenToDelegateMapT screenToDelegateMap;
        //std::auto_ptr<ApexProcedure> procedure;
        tDeviceMap devices;
        stimulus::tDataBlockMap datablocks;
        tStimulusMap stimuli;
        tFilterMap filters;
        tControllerMap controldevices;
        tConnectionsMap connections;
        QWidget* const parent;
        bool deterministic;
        std::auto_ptr<ParameterManager> parameterManager;
        //Corrector* corrector;

        bool autoAnswer;
        int blockSize;


        //modules
        std::auto_ptr<ApexTimer>                mod_timer;
        std::auto_ptr<ApexScreen>               mod_screen;
        std::auto_ptr<ApexProcedure>            mod_procedure;
        std::auto_ptr<Corrector>                mod_corrector;
        std::auto_ptr<ApexResultSink>           mod_resultsink;
        std::auto_ptr<RTResultSink>             mod_rtresultsink;
        std::auto_ptr<RandomGenerators>         mod_randomgenerators;
        std::auto_ptr<device::Controllers>      mod_controllers;
        std::auto_ptr<Calibrator>               mod_calibrator;
        std::auto_ptr<stimulus::StimulusOutput> mod_output;
        std::auto_ptr<Feedback>                 mod_feedback;

        ModuleList m_modules;   // list of the modules above

};
}

ExperimentRunDelegate::ExperimentRunDelegate (ExperimentData& experiment,
                                              QWidget* parent,
                                              bool deterministic) :
        experiment (experiment),
        priv (new ExperimentRunDelegatePrivate (parent, deterministic))
{
    priv->parameterManager.reset(new ParameterManager(*experiment.parameterManagerData()));
//     priv->procedure.reset (experiment.GetProcedureFactory().MakeProcedure (*this, &experiment.GetProcedureConfig()));

    priv->mod_procedure.reset ( MakeProcedure() );
    MakeOutputDevices();
    MakeControlDevices();
    MakeDatablocks();
//    qDebug("Number of datablocks: %d", priv->datablocks.size());
    MakeStimuli();
//    qDebug("Number of datablocks: %d", priv->datablocks.size());
    MakeFilters();
    MakeCorrector();
    MakeFeedback();

    // Create delegates for connections
    ConnectionRunDelegateCreator connectionMaker (priv->connections,
            priv->devices, priv->filters, priv->datablocks);
    const ConnectionsData* connectionDatas = experiment.connectionsData();
    for (ConnectionsData::const_iterator i = connectionDatas->begin();
            i != connectionDatas->end(); ++i)
        connectionMaker.AddConnection (**i);
    if (connectionDatas->empty())
        connectionMaker.mp_bMakeDefaultConnections();
    connectionMaker.mf_ReportUnconnectedItems();
    ErrorHandler::Get().addItems(connectionMaker.logger().items());
    //priv->connections.SetID( connectionDatas.GetID());

    FixStimuli();

}


ExperimentRunDelegate::~ExperimentRunDelegate()
{
    priv->mod_output.reset();       // StimulusOutput needs the datastructures
                                // that are deleted below in its destructor

    for ( ScreenToDelegateMapT::iterator i = priv->screenToDelegateMap.begin();
            i != priv->screenToDelegateMap.end(); ++i )
        delete i->second;
    // No auto ptr to minimize includes in public header file

    // delete datablocks
    for (stimulus::tDataBlockMap::const_iterator it= priv->datablocks.begin(); it!= priv->datablocks.end(); ++it)
    {
        delete it.value();
    }

    // delete stimuli
    for (stimulus::tStimulusMap::const_iterator it= priv->stimuli.begin(); it!= priv->stimuli.end(); ++it)
    {
        delete it.value();
    }

    // delete output devices
    for (stimulus::tDeviceMap::const_iterator it=  priv->devices.begin(); it!= priv->devices.end(); ++it)
    {
        delete it.value();
    }

    // delete control devices
    for (tControllerMap::const_iterator it=  priv->controldevices.begin(); it!= priv->controldevices.end(); ++it)
    {
        delete it.value();
    }

    // delete filters
    for (tFilterMap::const_iterator it=  priv->filters.begin(); it!= priv->filters.end(); ++it)
    {
        delete it.value();
    }

//    delete priv->corrector;

    delete priv;
}


// FIXME: remove apexcontrol parameter
void ExperimentRunDelegate::makeModules(ApexControl* apexControl)
{
    RandomGeneratorFactory rgFactory;

    // we pass every module the rundelegate corresponding to the current experiment
//    priv->mod_procedure = GetProcedure();
    priv->mod_screen.reset( new ApexScreen (*this) );
    //priv->mod_corrector.reset( GetCorrector());
    priv->mod_controllers.reset(
                                new device::Controllers (*this));
    priv->mod_output.reset( new StimulusOutput (*this));
    priv->mod_timer.reset( new ApexTimer (*this));
    priv->mod_randomgenerators.reset(rgFactory.GetRandomGenerators
            (*this, experiment.randomGenerators()));

    priv->mod_resultsink.reset( new ApexResultSink (*this));
    if (apexControl && !(apexControl->saveFilename().isEmpty()))
        priv->mod_resultsink->SetFilename (apexControl->saveFilename());

    if (experiment.resultParameters()->showRTResults())
        priv->mod_rtresultsink.reset(
            //new RTResultSink(experiment.resultParameters()));
            new RTResultSink(experiment.resultParameters()->resultPage() ));

    priv->m_modules.push_back (priv->mod_procedure.get());
    priv->m_modules.push_back (priv->mod_screen.get());
    priv->m_modules.push_back (priv->mod_corrector.get());
    priv->m_modules.push_back (priv->mod_controllers.get());
    priv->m_modules.push_back (priv->mod_output.get());
    priv->m_modules.push_back (priv->mod_resultsink.get());
    priv->m_modules.push_back (priv->mod_timer.get());
    priv->m_modules.push_back (priv->mod_randomgenerators.get());

    //!only add calibrator if configuration exists
    //!must be last module added since it requires others
    CalibrationData* calibrationData = experiment.calibrationData();
    if (calibrationData)
    {
        priv->mod_calibrator.reset( new Calibrator (this,
                                         *calibrationData));
        priv->m_modules.push_back (priv->mod_calibrator.get());

        if (apexControl)
            apexControl->mainWindow()->EnableCalibration (true);

        if (!priv->mod_calibrator->calibrate())
        {
            ErrorHandler::Get().addError (tr("Calibration"),
                              tr("Not all parameters are calibrated. Please calibrate them first."));
            return;
        }

        priv->mod_calibrator->updateParameters();

        if (apexControl)
        {
            apexControl->mainWindow()->AddStatusMessage(tr("Hardware profile:\n%1").
                                    arg(CalibrationDatabase().currentHardwareSetup()));
        }
    }

    if (!apexControl)
        return;

    // ATTENTION: if you change the procedure related signals below, it's
    // possible that some signals in apexmultiprocedurefactory must be
    // changed too

    // procedure
    QObject::connect (priv->mod_procedure.get(), SIGNAL(NewScreenElement(QString)),
             apexControl->mainWindow(), SLOT(HighLight(QString)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(NewStimulus(QString)),
             apexControl, SLOT(NewStimulus(QString)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(NewScreen(QString)),
             apexControl, SLOT(SetScreen(QString)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(Finished()),
             apexControl, SLOT(Finished()));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(EnableScreen(bool)),
                      apexControl->mainWindow(), SLOT(EnableScreen(bool)));
    // [Tom] necessary!
    /*connect (mod_procedure, SIGNAL(SetCorrectAnswer(unsigned)),
    mod_corrector, SLOT(SetCorrectAnswer(unsigned)));*/
    QObject::connect (priv->mod_procedure.get(), SIGNAL(SetCorrectAnswer(unsigned)),
             apexControl, SLOT(SetCorrectAnswer(unsigned)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(SetProgress(unsigned)),
                      apexControl->mainWindow(), SLOT(SetProgress(unsigned)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(SetNumTrials(unsigned)),
                      apexControl->mainWindow(), SLOT(SetNumTrials(unsigned)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(NewStimulus(stimulus::Stimulus*)),
             apexControl, SIGNAL(NewStimulus(stimulus::Stimulus*)));
    QObject::connect (priv->mod_procedure.get(), SIGNAL(SendMessage(QString)),
                      apexControl->mainWindow(), SLOT(AddStatusMessage(QString)));

    // screen
    QObject::connect (apexControl->mainWindow(), SIGNAL(Answered(const ScreenResult*)),
             priv->mod_screen.get(), SLOT(iAnswered(const ScreenResult*)));
    QObject::connect (priv->mod_screen.get(), SIGNAL(Answered(const ScreenResult*)),
             apexControl, SLOT(Answered(const ScreenResult*)));
    QObject::connect (apexControl->mainWindow(), SIGNAL(fileSaveAs()),
             priv->mod_resultsink.get(), SLOT(SaveAs()));

    // resultsink
    QObject::connect (priv->mod_resultsink.get(), SIGNAL(Saved()),
             apexControl, SLOT(ExperimentFinished()));

    if ( priv->mod_rtresultsink.get())
        QObject::connect( priv->mod_resultsink.get(), SIGNAL(collected(QString)),
                          priv->mod_rtresultsink.get(), SLOT( newAnswer(QString)));

}


gui::ScreenRunDelegate* ExperimentRunDelegate::GetScreen (const QString& id )
{
    const data::Screen* s = &experiment.screenById (id);
    assert( s );
    ScreenToDelegateMapT::const_iterator i = priv->screenToDelegateMap.find( s );
    if ( i != priv->screenToDelegateMap.end() )
    {
        return i->second;
    }
    else
    {
        const ScreensData* screens = experiment.screensData();
        // create the ScreenRunDelegate..
        ScreenRunDelegate* ret = new ScreenRunDelegate( this,
                s, priv->parent, screens->defaultFont(),
                screens->defaultFontSize() );
        QObject::connect(
            ret, SIGNAL( answered( ScreenElementRunDelegate* ) ),
            ApexControl::Get().GetMainWnd(),
            SLOT( AnswerFromElement( ScreenElementRunDelegate* ) ) );
        QObject::connect(
            &ApexControl::Get(), SIGNAL( NewStimulus( stimulus::Stimulus* ) ),
            ret, SIGNAL( newStimulus( stimulus::Stimulus* ) ) );
        QObject::connect(
            &ApexControl::Get(), SIGNAL( NewAnswer( const QString& ) ),
            ret, SIGNAL( newAnswer( const QString& ) ) );
        priv->screenToDelegateMap[s] = ret;
        return ret;
    }
}


stimulus::tConnectionsMap& ExperimentRunDelegate::GetConnections() const
{
    return priv->connections;
}

//!datablocks cannot be used more then once for playing simultaneously in playmatrix,
//!so they must be duplicated together with their connections
/*void ExperimentRunDelegate::FixStimuli()
{
    tStimuliMap& m_stimuli = experiment.GetStimuli();
    tDataBlockMap& m_datablocks = priv->datablocks; //  experiment.GetDatablocks();

    static QString sError;
    tStimuliMapCIt itB = m_stimuli.begin();
    tStimuliMapCIt itE = m_stimuli.end();
    for ( ; itB != itE ; ++itB )
    {
        stimulus::PlayMatrix* pCur = (*itB).second->ModPlayMatrix();
        if ( pCur )
        {
            stimulus::PlayMatrixCreator::sf_FixDuplicateIDs( pCur, m_datablocks, priv->connections );
            //map is complete now, check whether it's valid
            if ( !stimulus::PlayMatrixCreator::sf_bCheckMatrixValid( (*itB).second->GetPlayMatrix(), m_datablocks, sError ) )
            {
                ErrorHandler::Get().AddError ("Playmatrix", sError);
                return;
            }
            stimulus::StimulusFactory::sf_ConstructDevDBlockMap( (*itB).second, priv->devices, m_datablocks );
        }
    }

    qFatal("fixme");
}*/

ApexProcedure* ExperimentRunDelegate::MakeProcedure( data::ApexProcedureConfig* config )
{
    if ( config == NULL )
        config = experiment.procedureConfig();

    ApexProcedure* proc=0;

    switch ( config->GetParameters()->GetType() )
    {
    case ApexProcedureParameters::TYPE_ADAPTIVE:

        proc = new ApexAdaptiveProcedure ( *this, config );
        break;

    case ApexProcedureParameters::TYPE_CONSTANT:

        proc = new ApexConstantProcedure ( *this, config, priv->deterministic );
        break;

    case ApexProcedureParameters::TYPE_TRAINING:

        proc = new ApexTrainingProcedure ( *this, config, priv->deterministic );
        break;

    case ApexProcedureParameters::TYPE_MULTI:

        return MakeMultiProcedure( config );

    case ApexProcedureParameters::TYPE_PLUGIN:

        proc = new PluginProcedure ( *this, config );
        break;

    default:
        qFatal ( "unknown procedure type: %i", config->GetParameters()->GetType() );
    }

    return proc;
}

ApexMultiProcedure* ExperimentRunDelegate::MakeMultiProcedure( data::ApexProcedureConfig* config )
{
    qDebug ( "Making ApexMultiProcedure" );

    Q_ASSERT( config != NULL );

    ApexMultiProcedure* multiProc = new ApexMultiProcedure ( *this, config );
    ApexMultiProcedureConfig* multiConf = dynamic_cast<ApexMultiProcedureConfig*>( config );
    //qDebug("pointer: %p", multiConf);
    Q_ASSERT( multiConf != NULL );

    const tProcConfigList& pl = multiConf->GetProcedures();
    multiConf->ShowChildren();
    qDebug("%u child procedures", unsigned(pl.size()));
    // add subprocedures

    for ( tProcConfigList::const_iterator it =
                multiConf->GetProcedures().begin();
            it != multiConf->GetProcedures().end(); ++it )
    {
        Q_ASSERT( *it != NULL );
        qDebug( "type: %i", (*it)->GetParameters()->GetType() );

        ApexProcedure* newproc = MakeProcedure ( *it );
        multiProc->AddProcedure ( newproc );

        ApexControl::Get().ConnectProcedures ( newproc, multiProc );
    }

    return multiProc;
}

/**
* Fill device map with devices based on the experimentdata
*/
void ExperimentRunDelegate::MakeOutputDevices()
{
    qDebug("Making output devices");
    const DevicesData* data = experiment.devicesData();
    for (DevicesData::const_iterator it= data->begin(); it!=data->end(); ++it)
    {
        data::DeviceData* devData = it.value();
        Q_CHECK_PTR(devData);
//        qDebug("Creating device with id = %s", qPrintable (devData->GetID()));
        OutputDevice* device = 0;
        switch (devData->deviceType())
        {
        case TYPE_WAVDEVICE:
            device=new WavDevice( dynamic_cast<WavDeviceData*>(devData) );
            priv->blockSize = (dynamic_cast<WavDevice*>(device))->GetBlockSize();
            break;
        case TYPE_L34:
            device=MakeL34Device(  (dynamic_cast<data::L34DeviceData*>(devData) ));
            break;
        case TYPE_CLARION:
            device=MakeClarionDevice( (dynamic_cast<data::ClarionDeviceData*>(devData) ));
            break;
        case TYPE_DUMMY:
            device=MakeDummyDevice( devData);
            break;
        default:
            Q_ASSERT(0 && "Unknown device type");
        }
        Q_CHECK_PTR(device);
        priv->devices[devData->id()]=device;
//        qDebug("ExperimentRunDelegate::MakeDevices: made device widh id=%s", qPrintable (devData->GetID()));
    }
}

void ExperimentRunDelegate::MakeControlDevices()
{
    qDebug("Making control devices");
    const DevicesData* data = experiment.controlDevices();
    for (DevicesData::const_iterator it= data->begin(); it!=data->end(); ++it)
    {
        data::DeviceData* devData = it.value();
//        qDebug("Creating device with id = %s", qPrintable (devData->GetID()));
        device::ControlDevice* device = 0;
        switch (devData->deviceType())
        {
#ifdef SETMIXER
        case TYPE_MIXER:
            device=MakeMixer(  (dynamic_cast<data::MixerParameters*>(devData) ));
            break;
#endif
#ifdef PA5
        case TYPE_PA5:
            device=MakeMixer(  (dynamic_cast<data::MixerParameters*>(devData) ));
            break;
#endif
        case TYPE_PLUGINCONTROLLER:
            device=new device::PluginController((dynamic_cast<PluginControllerData*>(devData) ));
            break;

        default:
            Q_ASSERT(0 && "Unknown device type");
        }

        Q_CHECK_PTR(device);
        priv->controldevices[devData->id()]=device;

//        qDebug("ExperimentRunDelegate::MakeDevices: made control device widh id=%s", qPrintable (devData->GetID()));
    }


}



tDeviceMap& ExperimentRunDelegate::GetDevices() const
{
    return priv->devices;
}

const device::tControllerMap& ExperimentRunDelegate::GetControllers() const
{
    return priv->controldevices;
}

stimulus::OutputDevice* ExperimentRunDelegate::GetDevice(const QString& id) const
{

    return priv->devices[id];
}


stimulus::OutputDevice* ExperimentRunDelegate::MakeDummyDevice( DeviceData* p_params)
{
    Q_UNUSED (p_params);
//    Q_ASSERT(0 && "Not implemented in refactory");
    return new DummyDevice(*p_params);
    return 0;
}


stimulus::L34Device* ExperimentRunDelegate::MakeL34Device( L34DeviceData* p_Params )
{

    L34DeviceData &ac_Params = *p_Params;
    QString ac_sID = p_Params->id();

    std::auto_ptr<L34Device> dev;
    int trig = ((L34DeviceData&) ac_Params).triggerType();
    Q_UNUSED (trig);

retryL34:
    try
    {
        dev.reset( new L34Device( "0.0.0.0", dynamic_cast<L34DeviceData*>( &ac_Params) ) );
    }
    catch (PluginNotFoundException& e)
    {
        QMessageBox::critical(NULL,tr("L34 plugin not found"),
                              "Could not find the L34 plugin\n"
                              "By default this plugin is not included with Apex.\n"
                              "Please contact us if you need it.");
        throw ApexStringException("Error loading L34Device plugin: " + QString(e.what()));

    }
    catch (ApexStringException& /*e*/)
    {
        int result = QMessageBox::warning( 0, "Apex - L34device",
                                           "Could not connect to device " + p_Params->deviceType() + "-" + p_Params->id() +
                                           "\nCheck whether the USB connection is ok and the L34 is switched on\n "
                                           "You can try to turn off and back on the L34\n"
                                           "Apex might have to be completely restarted",
                                           "Retry",
                                           "Cancel", 0, 0, 1 );

        if (result==0)
        {
            // retry
            goto retryL34;
        }
        else
        {
            log().addError("L34DeviceFactory", "Couldn't initialize L34device");
            throw ApexStringException("Error initializing L34Device");
        }
    }


    if ( !dev->HasError() ) //[ stijn ] check or apex will crash later
    {
        if ( !(  *((L34DeviceData*) (&ac_Params)) ).map()->isComplete())
            log().addError("L34DeviceFactory", "Incomplete map");
        dev->SetMap( (  *((L34DeviceData*) (&ac_Params)) ).map());
    }
    else
    {
        throw ApexStringException("Error initializing L34Device");
    }

    return dev.release();
}
stimulus::ClarionDevice* ExperimentRunDelegate::MakeClarionDevice( data::ClarionDeviceData* p_Params )
{
    return new ClarionDevice(p_Params);
}

#if defined(SETMIXER) || defined(PA5)
apex::device::IMixer* ExperimentRunDelegate::MakeMixer( apex::data::MixerParameters* a_pParameters )
{
    QString ac_sID = a_pParameters->id();
    try
    {
        std::auto_ptr<device::IMixer> pRet;
        const MixerType eType = a_pParameters->type();
        if ( eType == DEFAULT_MIXER || eType == RME_MIXER )
            pRet.reset( new device::StreamAppMixer( a_pParameters ));
#ifdef PA5
        else if ( eType == PA5_MIXER )
            pRet.reset( new device::Pa5Device( a_pParameters ) );
#endif
        return pRet.release();
    }
    catch ( ApexStringException& e )
    {
        log().addError( "MixerFactory::mf_pCreateMixer", e.what() );
        throw e;
    }
}
#endif


void apex::ExperimentRunDelegate::MakeDatablocks( )
{
    qDebug("Making datablocks");
    const DatablocksData* data = experiment.datablocksData();
    for (DatablocksData::const_iterator it= data->begin(); it!=data->end(); ++it)
    {
        DatablockData* d = it.value();
        Q_CHECK_PTR(d);
        stimulus::DataBlock* db=0;


        // add path to uri
        QUrl filename = FilePrefixConvertor::addPrefix(d->prefix(), d->uri());

        /*QUrl theUrl = d->uri();
        if (theUrl.isRelative())
        {
            QString modprefix = FilePrefixConvertor::convert( d->m_prefix );
            if (!modprefix.isEmpty() && !modprefix.endsWith('/'))
                modprefix += '/';
            filename = QUrl(modprefix + d->uri());
        }
        else
            filename = QUrl(d->uri());*/

//        qDebug("Making datablock %s", qPrintable (d->GetID()));

        // lookup associated device type
        DeviceType devicetype = experiment.deviceById( d->device() )->deviceType();

        //if ( d->m_type == XMLKeys::sc_sWavDevice)
        if (devicetype == TYPE_WAVDEVICE)
        {
            db = new WavDataBlock(*d, filename, this);

        }
        else if ( devicetype == TYPE_L34)
        {

            db = new L34DataBlock(*d, filename, this);
        }
        else if ( devicetype == TYPE_CLARION)
        {
            db = new ClarionDataBlock(*d, filename, this);
        }
        else if (  devicetype == TYPE_DUMMY)
        {
            db = new DataBlock(*d, filename, this);
        }
        else
        {
            Q_ASSERT(0 && "Invalid device type" );
        }


        if (db && d->doChecksum())
        {
            log().addWarning("MakeDatablocks()", tr("Calculating checksum for datablock ") + d->id());
            QString sum = db->GetMD5Sum();
            if (sum.toLower() != d->checksum().toLower())
            {
                throw ApexStringException("Checksum does not match for datablock "
                                            + d->id() + ". sum obtained was: '"
                                            + sum + "', sum wanted was: '"
                                            + d->checksum() +"'" );
            }
        }

        if (db && ! d->birth().isNull())
        {
            qDebug("exp file birth: %s", qPrintable(d->birth().toString()));
            qDebug("disk file birth: %s", qPrintable(db->GetBirth().toString()));

            if ( db->GetBirth() != d->birth())
            {
                throw ApexStringException(tr("Creation time of file %1 (%2) "
                                              "does not match creation time specified in experiment "
                                              "file (%3).").arg(db->GetUrl()).arg(db->GetBirth().toString()).
                                           arg(d->birth().toString()));
            }
        }

        priv->datablocks[d->id()]=db;
    }


}

void apex::ExperimentRunDelegate::MakeCorrector()
{
    qDebug("Making corrector");

    // This ensures throw safety
    const data::CorrectorData* data = experiment.correctorData();
    data::CorrectorData::Type type = data->type();
    std::auto_ptr<Corrector> corr (
        type == CorrectorData::ALTERNATIVES ?
        new FCACorrector (*this, data) :
        new Corrector (*this, data));

    if (type == CorrectorData::EQUAL || type == CorrectorData::ALTERNATIVES)
        corr->SetComparator (new EqualComparator());
    else if (type == CorrectorData::CVC)
        corr->SetComparator (new CvcComparator (data->language()));
    else
        qFatal ("Unknown comparator type %i", type);

    priv->mod_corrector.reset ( corr.release() );
    Q_ASSERT(priv->mod_corrector.get() != 0);
}

void apex::ExperimentRunDelegate::MakeFeedback()
{
    priv->mod_feedback.reset( new Feedback( experiment.screensData()) );
}


void apex::ExperimentRunDelegate::MakeStimuli( )
{
    const StimuliData* data = experiment.stimuliData();
    for (StimuliData::const_iterator it=data->begin(); it!=data->end(); ++it)
    {
        const data::StimulusData& data = it.value();
        stimulus::Stimulus* newStim = new Stimulus(data);

        stimulus::PlayMatrixCreator::sf_FixDuplicateIDs( newStim->GetPlayMatrix(), priv->datablocks, *experiment.connectionsData() );
        /*QString sError;
           //map is complete now, check whether it's valid
           if ( !stimulus::PlayMatrixCreator::sf_bCheckMatrixValid( newStim->GetPlayMatrix(), priv->datablocks, sError ) )
           {
               ErrorHandler::Get().AddError ("Playmatrix", sError);
               return;
           }

        newStim->ConstructDevDBlockMap( priv->devices,priv->datablocks );*/

        priv->stimuli[it.key()]=newStim;
    }
}

void apex::ExperimentRunDelegate::FixStimuli( )
{

    for (tStimulusMap::const_iterator it=priv->stimuli.begin();
            it!=priv->stimuli.end(); ++it )
    {

        Stimulus* newStim = it.value();


        QString sError;
        //map is complete now, check whether it's valid
        if ( !stimulus::PlayMatrixCreator::sf_bCheckMatrixValid( newStim->GetPlayMatrix(), priv->datablocks, sError ) )
        {
            ErrorHandler::Get().addError ("Playmatrix", sError);
            return;
        }
        newStim->ConstructDevDBlockMap( priv->devices,priv->datablocks );
    }

}

namespace apex
{

void ExperimentRunDelegate::MakeFilters()
{
    qDebug("Making filters");
    const data::FiltersData* data = experiment.filtersData();
    for (FiltersData::const_iterator it= data->begin(); it!=data->end(); ++it)
    {
        data::FilterData* d = it.value();
        Q_CHECK_PTR(d);
//            stimulus::DataBlock* db;

        qDebug("Making filter %s", qPrintable (d->id()));



        QString id=d->id();
        QString type=d->xsiType();
        QString input, output;

        //create filter
        Filter* filter = MakeFilter( id, type, d );
        priv->filters[id]=filter;

    }
}


Filter* ExperimentRunDelegate::MakeFilter( const QString& ac_sID,
        const QString& ac_sType,
        data::FilterData* const ac_pParams )
{
    clearLog();
    Filter* filter = 0;

    DeviceData* devData = experiment.devicesData()->
                          deviceData( ac_pParams->device() );
    WavDeviceData* wavData;


    if ( devData->deviceType() == apex::data::TYPE_WAVDEVICE )
        wavData = dynamic_cast<WavDeviceData*>( devData );
    else
        throw ApexStringException("No filters supported for other device types than wavDevice");

    unsigned sr=wavData->sampleRate();

    Q_ASSERT(priv->blockSize>0);
    unsigned long bs=priv->blockSize;



    if ( ac_sType == sc_sFilterAmplifierType )
        filter = new WavAmplifier( ac_sID, ac_pParams,
                                   sr, bs );
    else if ( ac_sType == sc_sFilterFaderType )
        filter = new WavFader( ac_sID, (WavFaderParameters*) ac_pParams,
                               sr, bs);
    else if ( ac_sType == sc_sFilterGeneratorType )
    {
        Q_ASSERT( wavData != NULL );

        unsigned long sr = wavData->sampleRate();
        filter = new WavGenerator( ac_sID, ac_pParams->valueByType("type").toString() ,
                                   ac_pParams, sr, priv->blockSize, priv->deterministic );
    }
    else if ( ac_sType == sc_sFilterDataLoopType )    //FIXME should be sc_sFilterGeneratorType with subtype DataLoop
    {
        Q_ASSERT( wavData != NULL );

        unsigned long sr = wavData->sampleRate();

        //get datablock if needed
        const QString sDataID( ac_pParams->valueByType( "datablock" ).toString() );
        stimulus::WavDataBlock* pSrc;
        try
        {
            pSrc = (WavDataBlock*) GetDatablock(sDataID);
        }
        catch (ApexStringException& )
        {
            throw( ApexStringException( "dataloop: datablock " + sDataID + " not found" ) );
        }
//            ac_pParams->mp_SetExtraData( pSrc );

        DataLoopGeneratorFilter* gen = new DataLoopGeneratorFilter( ac_sID, sc_sFilterDataLoopType, ac_pParams, sr, priv->blockSize );
        gen->SetSource(pSrc);

        ((DataLoopGenerator*) gen->GetStreamGen() ) ->SetInputStream(pSrc->GetWavStream(bs, sr));

        filter = gen;
    }
    else if (ac_sType == sc_sFilterPluginFilterType)
    {
        filter = new PluginFilter ( ac_sID,
                                    dynamic_cast<data::PluginFilterData*> (ac_pParams),
                                    sr, bs);
    }
    else
    {
        log().addError( "WavDeviceFactory::CreateFilter", "Unknown FilterType" );
        return 0;
    }

    return filter;
}

stimulus::DataBlock* ExperimentRunDelegate::GetDatablock(const QString& p_name) const
{
    return priv->datablocks.value(p_name);
}


void    ExperimentRunDelegate::AddDatablock(const QString& name, stimulus::DataBlock* db)
{
    priv->datablocks.insert(name, db);
}

const stimulus::tDataBlockMap&              ExperimentRunDelegate::GetDatablocks() const
{
    return priv->datablocks;
}

tFilterMap& ExperimentRunDelegate::GetFilters() const
{
    return priv->filters;
}

stimulus::Filter* ExperimentRunDelegate::GetFilter(const QString& p_name) const
{
    tFilterMap::const_iterator i=priv->filters.find(p_name);
    if (i==priv->filters.end())
    {
        //throw ApexStringException(tr(QString("ExperimentRunDelegate::GetFilter: filter with id %1 not found").arg(p_name)));
        return 0;
    }
    return priv->filters[p_name];
}

}


gui::ApexMainWindow* ExperimentRunDelegate::GetMainWindow() const
{
    return ApexControl::Get().GetMainWnd();
}

bool apex::ExperimentRunDelegate::GetAutoAnswer( ) const
{
    return priv->autoAnswer;
}

void apex::ExperimentRunDelegate::SetAutoAnswer( const bool p )
{
    priv->autoAnswer=p;
}

ParameterManager* apex::ExperimentRunDelegate::GetParameterManager() const
{
    return priv->parameterManager.get();
}

stimulus::StimulusOutput*  apex::ExperimentRunDelegate::GetModOutput() const
{
    return modOutput();
}

stimulus::Stimulus* apex::ExperimentRunDelegate::GetStimulus(const QString& id) const
{
    if (! priv->stimuli.contains(id))
        throw ApexStringException(QString("Stimulus not found: %1").arg(id));

    return priv->stimuli.value(id);
}


namespace apex
{

    ApexProcedure* apex::ExperimentRunDelegate::modProcedure() const
    {
        return priv->mod_procedure.get();
    }

    ApexTimer* apex::ExperimentRunDelegate::modTimer() const

    {
        return priv->mod_timer.get();
    }

    ApexScreen* apex::ExperimentRunDelegate::modScreen() const
    {
        return priv->mod_screen.get();
    }

    Corrector* apex::ExperimentRunDelegate::modCorrector() const
    {
        return priv->mod_corrector.get();
    }

    ApexResultSink* apex::ExperimentRunDelegate::modResultSink() const
    {
        return priv->mod_resultsink.get();
    }

    RTResultSink* apex::ExperimentRunDelegate::modRTResultSink() const
    {
        return priv->mod_rtresultsink.get();
    }

    device::Controllers* apex::ExperimentRunDelegate::modControllers() const
    {
        return priv->mod_controllers.get();
    }

    Calibrator*  apex::ExperimentRunDelegate::modCalibrator() const
    {
        return priv->mod_calibrator.get();
    }

    Feedback* apex::ExperimentRunDelegate::modFeedback() const
    {
        return priv->mod_feedback.get();
    }

    stimulus::StimulusOutput* apex::ExperimentRunDelegate::modOutput() const
    {
        return priv->mod_output.get();
    }

    RandomGenerators* apex::ExperimentRunDelegate::modRandomGenerators() const
    {
        return priv->mod_randomgenerators.get();
    }

    const ModuleList *apex::ExperimentRunDelegate::modules() const
    {
        return &priv->m_modules;
    }

    QWidget* apex::ExperimentRunDelegate::parentWidget() const
    {
        return priv->parent;
    }


}

