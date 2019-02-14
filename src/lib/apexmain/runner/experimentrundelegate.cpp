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

#include "apexdata/corrector/correctordata.h"

#include "apexdata/device/cohdevicedata.h"
#include "apexdata/device/plugincontrollerdata.h"
#include "apexdata/device/wavdevicedata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/filter/filterdata.h"
#include "apexdata/filter/pluginfilterdata.h"
#include "apexdata/filter/wavparameters.h"

#include "apexdata/map/apexmap.h"

#include "apexdata/procedure/multiproceduredata.h"
#include "apexdata/procedure/procedureinterface.h"

#include "apexdata/screen/screensdata.h"

#include "apexdata/stimulus/stimulidata.h"

#include "apextools/apexpluginloader.h"
#include "apextools/apextypedefs.h"

#include "bertha/blockdata.h"
#include "bertha/connectiondata.h"
#include "bertha/devicedata.h"
#include "bertha/experimentdata.h"

#include "calibration/calibrationdatabase.h"
#include "calibration/calibrator.h"

#include "cohstimulus/cohdatablock.h"
#include "cohstimulus/cohdevice.h"

#include "common/exception.h"
#include "common/pluginloader.h"
#include "common/sndfilewrapper.h"

#include "connection/connectionrundelegatecreator.h"

#include "corrector/equalcorrector.h"

#include "device/controllers.h"
#include "device/plugincontroller.h"

#include "dummystimulus/dummydevice.h"

#include "feedback/feedback.h"

#include "filter/dataloopgeneratorfilter.h"
#include "filter/wavgenerator.h"

#include "gui/mainwindow.h"

#include "parameters/parametermanager.h"

#include "randomgenerator/randomgeneratorfactory.h"
#include "randomgenerator/randomgenerators.h"

#include "resultsink/apexresultsink.h"
#include "resultsink/rtresultsink.h"

#include "screen/apexscreen.h"
#include "screen/screenrundelegate.h"

#include "stimulus/filter.h"
#include "stimulus/filtertypes.h"
#include "stimulus/playmatrix.h"
#include "stimulus/stimulusoutput.h"

#include "../study/studymodule.h"

#include "timer/apextimer.h"

#include "trial/trialstarttime.h"

#include "wavstimulus/pluginfilter.h"
#include "wavstimulus/streamgenerator.h"
#include "wavstimulus/wavdatablock.h"
#include "wavstimulus/wavdevice.h"
#include "wavstimulus/wavfader.h"
#include "wavstimulus/wavfilter.h"

#include "apexcontrol.h"
#include "experimentparser.h"
#include "experimentrundelegate.h"
#include "fileprefixconvertor.h"

#include <QMessageBox>
#include <QWidget>

using namespace apex;
using namespace apex::data;
using namespace apex::stimulus;
using namespace cmn;

#ifdef _MSC_VER
#pragma warning(disable : 4065) // switch statement without case labels
#endif

namespace apex
{
typedef QMap<const data::Screen *, gui::ScreenRunDelegate *>
    ScreenToDelegateMap;

class ExperimentRunDelegatePrivate
{
public:
    ExperimentRunDelegatePrivate(QWidget *parent,
                                 const data::MainConfigFileData &p_maindata,
                                 bool det)
        : mainData(p_maindata),
          parent(parent),
          autoAnswer(false),
          deterministic(det),
          blockSize(-1),
          mod_screen(0),
          mod_resultsink(0),
          mod_rtresultsink(0),
          mod_randomgenerators(0),
          mod_controllers(0),
          mod_calibrator(0),
          mod_output(0),
          mod_trialStartTime(0)
    {
    }

    const data::MainConfigFileData &mainData;
    ScreenToDelegateMap screenToDelegateMap;
    tDeviceMap devices;
    stimulus::tDataBlockMap datablocks;
    tStimulusMap stimuli;
    tFilterMap filters;
    tControllerMap controldevices;
    tConnectionsMap connections;
    QWidget *const parent;
    QScopedPointer<ParameterManager> parameterManager;

    bool autoAnswer;
    bool deterministic;
    int blockSize;

    // modules
    QScopedPointer<ApexTimer> mod_timer;
    QScopedPointer<ApexScreen> mod_screen;
    QScopedPointer<ApexResultSink> mod_resultsink;
    QScopedPointer<RTResultSink> mod_rtresultsink;
    QScopedPointer<RandomGenerators> mod_randomgenerators;
    QScopedPointer<device::Controllers> mod_controllers;
    QScopedPointer<Calibrator> mod_calibrator;
    QScopedPointer<stimulus::StimulusOutput> mod_output;
    QScopedPointer<Feedback> mod_feedback;
    QScopedPointer<TrialStartTime> mod_trialStartTime;
    QScopedPointer<StudyModule> studyModule;
    bertha::ExperimentData berthaExperimentData;

    ModuleList modules; // list of the modules above

    bool useBertha;
};

ExperimentRunDelegate::ExperimentRunDelegate(
    ExperimentData &experiment, const MainConfigFileData &p_maindata,
    QWidget *parent, bool deterministic, bool useBertha)
    : experiment(experiment),
      d(new ExperimentRunDelegatePrivate(parent, p_maindata, deterministic))
{
    d->useBertha = useBertha;
    d->parameterManager.reset(
        new ParameterManager(*experiment.parameterManagerData()));

    MakeOutputDevices();
    MakeControlDevices();
    MakeDatablocks();
    MakeStimuli();
    MakeFilters();
    MakeFeedback();

    if (d->useBertha)
        MakeBerthaExperimentData();

    // Create delegates for connections
    ConnectionRunDelegateCreator connectionMaker(d->connections, d->devices,
                                                 d->filters, d->datablocks);
    const ConnectionsData *connectionsData = experiment.connectionsData();
    for (ConnectionsData::const_iterator i = connectionsData->begin();
         i != connectionsData->end(); ++i)
        connectionMaker.AddConnection(**i);
    if (connectionsData->empty() && !d->useBertha)
        connectionMaker.mp_bMakeDefaultConnections();
    connectionMaker.mf_ReportUnconnectedItems();
    FixStimuli();
}

ExperimentRunDelegate::~ExperimentRunDelegate()
{
    d->mod_output.reset(); // StimulusOutput needs the datastructures
                           // that are deleted below in its destructor

    qDeleteAll(d->screenToDelegateMap);
    // d->screenToDelegateMap.clear();
    qDeleteAll(d->datablocks);
    qDeleteAll(d->stimuli);
    qDeleteAll(d->devices);
    qDeleteAll(d->controldevices);
    qDeleteAll(d->filters);

    delete d;
}

void ExperimentRunDelegate::makeModules()
{
    RandomGeneratorFactory rgFactory;

    // we pass every module the rundelegate corresponding to the current
    // experiment
    d->mod_screen.reset(new ApexScreen(*this));
    d->mod_controllers.reset(new device::Controllers(*this));
    d->mod_output.reset(new StimulusOutput(*this));
    d->mod_timer.reset(new ApexTimer(*this));
    d->mod_trialStartTime.reset(new TrialStartTime(*this));
    d->mod_randomgenerators.reset(
        rgFactory.GetRandomGenerators(*this, experiment.randomGenerators()));
    d->studyModule.reset(new StudyModule(*this));

    d->mod_resultsink.reset(
        new ApexResultSink(*this, ApexControl::Get().GetStartTime()));

    if (!ApexControl::Get().saveFilename().isEmpty())
        d->mod_resultsink->SetFilename(ApexControl::Get().saveFilename());

    if (experiment.resultParameters()->showRTResults()) {
#ifdef Q_OS_ANDROID
        qCCritical(APEX_RS, "Cannot enable real time results on Android");
#else
        d->mod_rtresultsink.reset(
            new RTResultSink(experiment.resultParameters()->resultPage(),
                             experiment.resultParameters()->resultParameters(),
                             experiment.resultParameters()->extraScript()));
#endif
    }

    d->modules << d->mod_screen.data() << d->mod_controllers.data()
               << d->mod_output.data() << d->mod_resultsink.data()
               << d->mod_timer.data() << d->mod_randomgenerators.data()
               << d->mod_trialStartTime.data() << d->studyModule.data();

    // only add calibrator if configuration exists
    // must be last module added since it requires others
    CalibrationData *calibrationData = experiment.calibrationData();
    if (calibrationData != 0) {
        d->mod_calibrator.reset(new Calibrator(this, *calibrationData));
        d->modules << d->mod_calibrator.data();

        ApexControl::Get().mainWindow()->EnableCalibration(true);

        if (!d->mod_calibrator->calibrate()) {
            qCCritical(
                APEX_RS, "%s",
                qPrintable(QSL("%1: %2").arg(
                    tr("Calibration"), tr("Not all parameters are calibrated. "
                                          "Please calibrate them first."))));
            return;
        }

        d->mod_calibrator->updateParameters();

        ApexControl::Get().mainWindow()->AddStatusMessage(
            tr("Hardware profile:\n%1")
                .arg(CalibrationDatabase().currentHardwareSetup()));
    }

    if (d->mod_rtresultsink) {
        QObject::connect(d->mod_resultsink.data(), SIGNAL(collected(QString)),
                         d->mod_rtresultsink.data(), SLOT(newAnswer(QString)));
    }
}

gui::ScreenRunDelegate *ExperimentRunDelegate::GetScreen(const QString &id)
{
    const data::Screen *s = &experiment.screenById(id);
    Q_ASSERT(s != 0);

    if (d->screenToDelegateMap.contains(s)) {
        return d->screenToDelegateMap[s];
    } else {
        const ScreensData *screens = experiment.screensData();

        // create the ScreenRunDelegate..
        ScreenRunDelegate *ret =
            new ScreenRunDelegate(this, s, d->parent, screens->defaultFont(),
                                  screens->defaultFontSize());

        QObject::connect(ret, SIGNAL(answered(ScreenElementRunDelegate *)),
                         ApexControl::Get().mainWindow(),
                         SLOT(AnswerFromElement(ScreenElementRunDelegate *)));

        d->screenToDelegateMap[s] = ret;
        return ret;
    }
}

stimulus::tConnectionsMap &ExperimentRunDelegate::GetConnections() const
{
    return d->connections;
}

/**
* Fill device map with devices based on the experimentdata
*/
void ExperimentRunDelegate::MakeOutputDevices()
{
    qCDebug(APEX_RS, "Making output devices");

    const DevicesData *data = experiment.devicesData();
    Q_FOREACH (data::DeviceData *devData, *data) {
        Q_ASSERT(devData != 0);
        OutputDevice *device = 0;

        switch (devData->deviceType()) {
        case TYPE_WAVDEVICE: {
            WavDeviceData *wavDeviceData =
                static_cast<WavDeviceData *>(devData);
            if (d->useBertha) {
                bertha::DeviceData deviceBlockData(devData->id(),
                                                   QSL("ApexSoundCard"));
                deviceBlockData.setInputPorts(devData->numberOfChannels());
                deviceBlockData.setBlockSize(wavDeviceData->blockSize());
                deviceBlockData.setSampleRate(wavDeviceData->sampleRate());
                d->berthaExperimentData.setDevice(deviceBlockData);
                d->blockSize = deviceBlockData.blockSize();
            } else {
                WavDevice *wavDevice = new WavDevice(
                    wavDeviceData, d->berthaExperimentData, d->useBertha);
                d->blockSize = (wavDevice)->GetBlockSize();
                device = wavDevice;
            }
        } break;
        case TYPE_COH:
            device =
                MakeCohDevice(dynamic_cast<data::CohDeviceData *>(devData));
            break;

        case TYPE_DUMMY:
            device = MakeDummyDevice(devData);
            break;

        default:
            qFatal("Unknown device type");
        }

        if (d->useBertha && devData->deviceType() == TYPE_WAVDEVICE) {
            /* we need the full experimentdata before creating the
             * bertha device*/
            Q_UNUSED(device);
        } else {
            Q_CHECK_PTR(device);
            d->devices[devData->id()] = device;
        }
    }
}

void ExperimentRunDelegate::MakeControlDevices()
{
    qCDebug(APEX_RS, "Making control devices");

    const DevicesData *data = experiment.controlDevices();
    Q_FOREACH (data::DeviceData *devData, *data) {
        device::ControlDevice *device = 0;
        switch (devData->deviceType()) {
        case TYPE_PLUGINCONTROLLER:
            device = new device::PluginController(
                dynamic_cast<PluginControllerData *>(devData));
            break;

        default:
            qFatal("Unknown device type");
        }

        Q_CHECK_PTR(device);

        d->controldevices[devData->id()] = device;
    }
}

stimulus::OutputDevice *
ExperimentRunDelegate::GetDevice(const QString &id) const
{
    RETURN_VAL_IF_FAIL(d->devices.contains(id), NULL);
    return d->devices[id];
}

tDeviceMap &ExperimentRunDelegate::GetDevices() const
{
    return d->devices;
}

device::ControlDevice *
ExperimentRunDelegate::GetController(const QString &id) const
{
    RETURN_VAL_IF_FAIL(d->controldevices.contains(id), NULL);
    return d->controldevices[id];
}

const device::tControllerMap &ExperimentRunDelegate::GetControllers() const
{
    return d->controldevices;
}

stimulus::OutputDevice *
ExperimentRunDelegate::MakeDummyDevice(DeviceData *params)
{
    return new DummyDevice(*params);
}

stimulus::CohDevice *ExperimentRunDelegate::MakeCohDevice(CohDeviceData *params)
{
    QScopedPointer<CohDevice> dev;

    if (!params->map()->isComplete())
        qCCritical(APEX_RS, "CohDeviceFactory: Incomplete map");

    while (!dev) {
        try {
            dev.reset(new CohDevice(params));
        } catch (const std::exception &) {
            QString msg = tr("Could not connect to device %1\n"
                             "Check whether the USB connection is ok and the "
                             "L34 is switched on\n "
                             "You can try to turn off and back on the L34")
                              .arg(params->device());
            if (QMessageBox::warning(NULL, "Apex - CohDevice", msg,
                                     QMessageBox::Retry, QMessageBox::Cancel) !=
                QMessageBox::Retry) {
                qCCritical(APEX_RS,
                           "CohDeviceFactory: Couldn't initialize CohDevice");
                throw;
            }
        }
    }

    return dev.take();
}

void ExperimentRunDelegate::MakeDatablocks()
{
    qCDebug(APEX_RS, "Making datablocks");
    const DatablocksData *data = experiment.datablocksData();
    for (DatablocksData::const_iterator it = data->begin(); it != data->end();
         ++it) {
        DatablockData *dbData = it.value();
        Q_CHECK_PTR(dbData);
        stimulus::DataBlock *db = 0;

        // add path to file
        QString filename =
            FilePrefixConvertor::addPrefix(dbData->prefix(), dbData->file());

        // lookup associated device type
        DeviceType devicetype =
            experiment.deviceById(dbData->device())->deviceType();

        if (devicetype == TYPE_WAVDEVICE) {
            if (d->useBertha)
                MakeBerthaDataBlock(*dbData);
            db = new WavDataBlock(*dbData, filename, this);
        } else if (devicetype == TYPE_COH) {

            db = new CohDataBlock(*dbData, filename, this);
        } else if (devicetype == TYPE_DUMMY) {
            db = new DataBlock(*dbData, filename, this);
        } else {
            qFatal("Invalid device type");
        }

        d->datablocks[dbData->id()] = db;
    }
}

void ExperimentRunDelegate::MakeBerthaExperimentData()
{
    if (experiment.connectionsData()->empty() &&
        !experiment.filtersData()->empty())
        throw ApexStringException(
            tr("Can't make default connections when filter "
               "connections are specified"));

    /* Map to aid in the placement of summation blocks */
    QMap<QString, QVector<bertha::ConnectionData>> connectionMap;
    /* Fill in the datablock connections if they're not present */
    FillDataBlockConnections(connectionMap);

    /* Connectionmaking */
    QMutableListIterator<data::ConnectionData *> connectionsIt(
        *experiment.connectionsData());
    while (connectionsIt.hasNext()) {
        data::ConnectionData tempData(*connectionsIt.next());
        /* If the user has not specified the device of the datablock we assume
         * it's for the wavdevice */
        if (!tempData.device().isEmpty() &&
            tempData.device() != d->berthaExperimentData.device().id())
            continue;

        switch (tempData.matchType()) {
        case data::MATCH_NAME:
            if (tempData.fromId() != "_ALL_") {
                AddBerthaConnection(connectionMap, &tempData);
            } else {
                Q_FOREACH (const QString &id,
                           experiment.datablocksData()->keys()) {
                    if (d->berthaExperimentData.block(id).outputPorts() >=
                        tempData.fromChannel() + 1) {
                        tempData.setFromId(id);
                        AddBerthaConnection(connectionMap, &tempData);
                    }
                }
            }
            break;
        case data::MATCH_WILDCARD:
        case data::MATCH_REGEXP:
            QRegExp re(tempData.fromId());
            if (tempData.matchType() == data::MATCH_WILDCARD)
                re.setPatternSyntax(QRegExp::Wildcard);
            Q_FOREACH (const QString &id, experiment.datablocksData()->keys()) {
                if (re.exactMatch(id)) {
                    tempData.setFromId(id);
                    AddBerthaConnection(connectionMap, &tempData);
                }
            }
            break;
        }
        connectionsIt.remove();
    }

    MakeBerthaSummationBlocks(connectionMap);

    const DevicesData *devicesData = experiment.devicesData();
    Q_FOREACH (data::DeviceData *devData, *devicesData) {
        Q_ASSERT(devData != 0);
        if (devData->deviceType() == TYPE_WAVDEVICE) {
            OutputDevice *device =
                new WavDevice(static_cast<WavDeviceData *>(devData),
                              d->berthaExperimentData, d->useBertha);
            d->devices[devData->id()] = device;
        }
    }
}

void ExperimentRunDelegate::MakeBerthaDataBlock(
    const data::DatablockData &dataBlockData)
{
    QString filename = FilePrefixConvertor::addPrefix(dataBlockData.prefix(),
                                                      dataBlockData.file());
    bertha::BlockData dataBlock(dataBlockData.id(),
                                QSL("ApexCompatibleDataBlock"));
    if (dataBlockData.nbChannels() != 0) {
        dataBlock.setOutputPorts(dataBlockData.nbChannels());
    } else if (filename.startsWith(QSL("silence:"))) {
        dataBlock.setOutputPorts(d->berthaExperimentData.device().inputPorts());
    } else {
        int channels;
        try {
            channels = SndFile::channels(filename);
        } catch (...) {
            qCWarning(APEX_RS, "Couldn't determine channels of datablock %s",
                      qPrintable(filename));
            channels = 1;
        }
        dataBlock.setOutputPorts(channels);
    }
    dataBlock.setParameter(QSL("filename"), filename);
    dataBlock.setParameter(QSL("loops"), dataBlockData.nbLoops());
    d->berthaExperimentData.addBlock(dataBlock);
}

void ExperimentRunDelegate::FillDataBlockConnections(
    QMap<QString, QVector<bertha::ConnectionData>> &connectionMap)
{
    Q_UNUSED(connectionMap);
    /* Generate the connectionsdata if it's not present */
    if (experiment.connectionsData()->empty()) {
        Q_FOREACH (const QString &dataBlock,
                   experiment.datablocksData()->keys()) {
            data::DatablockData *dataBlockData =
                experiment.datablocksData()->datablockData(dataBlock);
            QString fileName = FilePrefixConvertor::addPrefix(
                dataBlockData->prefix(), dataBlockData->file());
            int channels;
            try {
                channels = SndFile::channels(fileName);
            } catch (...) {
                qCWarning(APEX_RS,
                          "Couldn't determine channels of datablock %s",
                          qPrintable(fileName));
                channels = 1;
            }

            for (unsigned i = 0; i < (unsigned)channels; ++i) {
                data::ConnectionData *connectionData = new data::ConnectionData;
                connectionData->setFromId(dataBlock);
                connectionData->setToId(d->berthaExperimentData.device().id());
                connectionData->setFromChannel(i);
                connectionData->setToChannel(i);
                connectionData->setDevice(
                    d->berthaExperimentData.device().id());
                experiment.connectionsData()->append(connectionData);
            }
        }
    }
}

void ExperimentRunDelegate::AddBerthaConnection(
    QMap<QString, QVector<bertha::ConnectionData>> &connectionMap,
    const data::ConnectionData *connectionData)
{
    QString sourceBlock = connectionData->fromId();
    QString targetBlock = connectionData->toId();
    QString targetPort =
        connectionData->toId() == d->berthaExperimentData.device().id()
            ? QString::fromLatin1("playback-%1")
                  .arg(connectionData->toChannel() + 1)
            : QString::fromLatin1("input-%1")
                  .arg(connectionData->toChannel() + 1);
    QString sourcePort =
        connectionData->fromId() == d->berthaExperimentData.device().id()
            ? QString::fromLatin1("capture-%1")
                  .arg(connectionData->fromChannel() + 1)
            : QString::fromLatin1("output-%1")
                  .arg(connectionData->fromChannel() + 1);

    if (!connectionData->fromChannelId().isEmpty()) {
        unsigned sourceChannels =
            d->berthaExperimentData.device().id() == connectionData->fromId()
                ? d->berthaExperimentData.device().inputPorts()
                : d->berthaExperimentData.block(connectionData->fromId())
                      .inputPorts();

        bertha::BlockData switchBlock;
        switchBlock.setPlugin(QString::fromLatin1("Mixer"));
        switchBlock.setId(connectionData->fromChannelId());
        switchBlock.setInputPorts(sourceChannels);
        switchBlock.setOutputPorts(1);
        switchBlock.setParameter(QString::fromLatin1("logarithmicGain"), false);
        d->berthaExperimentData.addBlock(switchBlock);
        /* sourceblock -> switchblock */
        for (unsigned i = 0; i < sourceChannels; ++i) {
            QString switchSourcePort =
                connectionData->fromId() ==
                        d->berthaExperimentData.device().id()
                    ? QString::fromLatin1("capture-%1").arg(i + 1)
                    : QString::fromLatin1("output-%1").arg(i + 1);
            bertha::ConnectionData switchConnection(
                connectionData->fromId(), switchSourcePort, switchBlock.id(),
                QString::fromLatin1("input-%1").arg(i + 1));
            QString mapId =
                switchConnection.targetBlock() +
                QString(switchConnection.targetPort()).remove(QChar('-'));
            connectionMap[mapId].append(switchConnection);
        }
        /* switchblock -> targetblock */
        sourceBlock = switchBlock.id();
        sourcePort = QString::fromLatin1("output-1");
    }

    if (!connectionData->toChannelId().isEmpty()) {
        unsigned targetChannels =
            d->berthaExperimentData.device().id() == connectionData->toId()
                ? d->berthaExperimentData.device().inputPorts()
                : d->berthaExperimentData.block(connectionData->toId())
                      .inputPorts();

        bertha::BlockData switchBlock;
        switchBlock.setPlugin(QString::fromLatin1("Mixer"));
        switchBlock.setId(connectionData->toChannelId());
        switchBlock.setInputPorts(1);
        switchBlock.setOutputPorts(targetChannels);
        switchBlock.setParameter(QString::fromLatin1("logarithmicGain"), false);
        d->berthaExperimentData.addBlock(switchBlock);
        /* switchblock -> targetblock */
        for (unsigned i = 0; i < targetChannels; ++i) {
            QString switchTargetPort =
                connectionData->toId() == d->berthaExperimentData.device().id()
                    ? QString::fromLatin1("playback-%1").arg(i + 1)
                    : QString::fromLatin1("input-%1").arg(i + 1);
            bertha::ConnectionData switchConnection(
                switchBlock.id(), QString::fromLatin1("output-%1").arg(i + 1),
                connectionData->toId(), switchTargetPort);
            QString mapId =
                switchConnection.targetBlock() +
                QString(switchConnection.targetPort()).remove(QChar('-'));
            connectionMap[mapId].append(switchConnection);
        }
        /* sourceblock -> switchblock */
        targetBlock = switchBlock.id();
        targetPort = QString::fromLatin1("input-1");
    }

    bertha::ConnectionData berthaConnection(sourceBlock, sourcePort,
                                            targetBlock, targetPort);
    QString mapId = berthaConnection.targetBlock() +
                    QString(berthaConnection.targetPort()).remove(QChar('-'));
    connectionMap[mapId].append(berthaConnection);
}

void ExperimentRunDelegate::MakeBerthaSummationBlocks(
    QMap<QString, QVector<bertha::ConnectionData>> &connectionMap)
{
    Q_FOREACH (QString id, connectionMap.keys()) {
        QVector<bertha::ConnectionData> connectionList = connectionMap[id];
        if (connectionList.size() == 1)
            d->berthaExperimentData.addConnection(connectionList[0]);
        else {
            bertha::BlockData summationBlock;
            summationBlock.setPlugin(QString::fromLatin1("Summation"));
            summationBlock.setId(id);
            summationBlock.setOutputPorts(1);
            summationBlock.setInputPorts(connectionList.size());
            d->berthaExperimentData.addBlock(summationBlock);

            d->berthaExperimentData.addConnection(bertha::ConnectionData(
                id, QString::fromLatin1("output-%1").arg(1),
                connectionList[0].targetBlock(),
                connectionList[0].targetPort()));

            for (int i = 0; i < connectionList.size(); ++i) {
                connectionList[i].setTargetBlock(id);
                connectionList[i].setTargetPort(
                    QString::fromLatin1("input-%1").arg(i + 1));
                d->berthaExperimentData.addConnection(connectionList[i]);
            }
        }
    }
}

void ExperimentRunDelegate::MakeFeedback()
{
    d->mod_feedback.reset(new Feedback(experiment.screensData()));
}

void ExperimentRunDelegate::MakeStimuli()
{
    const StimuliData *data = experiment.stimuliData();
    for (StimuliData::const_iterator it = data->begin(); it != data->end();
         ++it) {
        const data::StimulusData &data = it.value();
        stimulus::Stimulus *newStim = new Stimulus(data);
        stimulus::PlayMatrixCreator::sf_FixDuplicateIDs(
            newStim->GetPlayMatrix(), d->datablocks,
            *experiment.connectionsData());
        d->stimuli[it.key()] = newStim;
    }

    /* FixDuplicateIDs might have added more datablocks. */
    if (d->useBertha) {
        Q_FOREACH (const QString &dataBlockId, d->datablocks.keys()) {
            if (!d->berthaExperimentData.blockExists(dataBlockId)) {
                MakeBerthaDataBlock(
                    d->datablocks[dataBlockId]->GetParameters());
            }
        }
    }
}

ProcedureInterface *
ExperimentRunDelegate::makeProcedure(ProcedureApi *api,
                                     const ProcedureData *data)
{
    QString procedureType = data->name();
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(procedureType);
    return creator->createProcedure(procedureType, api, data);
}

void ExperimentRunDelegate::FixStimuli()
{

    for (tStimulusMap::const_iterator it = d->stimuli.begin();
         it != d->stimuli.end(); ++it) {

        Stimulus *newStim = it.value();

        QString sError;
        // map is complete now, check whether it's valid
        if (!stimulus::PlayMatrixCreator::sf_bCheckMatrixValid(
                newStim->GetPlayMatrix(), d->datablocks, sError)) {
            qCCritical(APEX_RS, "%s",
                       qPrintable(QSL("%1: %2").arg("Playmatrix", sError)));
            return;
        }
        newStim->ConstructDevDBlockMap(d->devices, d->datablocks);
    }
}

void ExperimentRunDelegate::MakeFilters()
{
    qCDebug(APEX_RS, "Making filters");
    const data::FiltersData *data = experiment.filtersData();
    for (FiltersData::const_iterator it = data->begin(); it != data->end();
         ++it) {
        data::FilterData *data = it.value();
        Q_CHECK_PTR(data);

        if (d->useBertha) {
            bertha::BlockData filterData(data->id(), QSL("ApexAdapter"));
            filterData.setInputPorts(data->numberOfChannels());
            filterData.setOutputPorts(data->numberOfChannels());

            QString filterType;
            QVariantList parameters;
            if (data->xsiType() == sc_sFilterPluginFilterType) {
                filterType = data->valueByType(QSL("plugin")).toString();
            } else if (data->xsiType() == sc_sFilterGeneratorType) {
                if (data->type() == sc_sFilterSineGenType) {
                    filterType = QSL("sinegenerator");
                    parameters.append(QVariant(QVariantList() << QSL("id") << -1
                                                              << data->id()));
                } else if (data->type() == sc_sFilterNoiseGeneratorType) {
                    filterType = QSL("noisegenerator");
                    parameters.append(QVariant(QVariantList()
                                               << QL1S("deterministic") << -1
                                               << d->deterministic));

                } else if (data->type() == sc_sFilterSinglePulseGenType) {
                    filterType = QSL("singlepulsegenerator");
                }
            } else {
                filterType = data->xsiType().split(':').last();
                if (data->xsiType() == sc_sFilterDataLoopType) {
                    bertha::BlockData dataBlock = d->berthaExperimentData.block(
                        data->valueByType("datablock").toString());
                    parameters.append(
                        QVariant(QVariantList()
                                 << QSL("fileName") << -1
                                 << dataBlock.parameter(QSL("fileName"))));
                    parameters.append(QVariant(QVariantList() << QSL("id") << -1
                                                              << data->id()));
                    parameters.append(QVariant(QVariantList()
                                               << QL1S("deterministic") << -1
                                               << d->deterministic));
                }
            }
            filterData.setParameter(QSL("apexFilter"), filterType);

            Q_FOREACH (const Parameter &param, data->parameters())
                parameters.append(QVariant(QVariantList()
                                           << param.type() << param.channel()
                                           << param.defaultValue()));
            filterData.setParameter(QSL("parameters"), parameters);

            d->berthaExperimentData.addBlock(filterData);

            data::WavGeneratorParameters *wavGenData =
                dynamic_cast<data::WavGeneratorParameters *>(data);

            if (wavGenData != nullptr) {
                if (data->xsiType() == sc_sFilterGeneratorType) {
                    d->filters[data->id()] = new WavGenerator(
                        data->id(), data->valueByType("type").toString(), data,
                        d->berthaExperimentData.device().sampleRate(),
                        d->blockSize);
                } else if (data->xsiType() == sc_sFilterDataLoopType) {
                    d->filters[data->id()] = new DataLoopGeneratorFilter(
                        data->id(), sc_sFilterDataLoopType, data,
                        d->berthaExperimentData.device().sampleRate(),
                        d->blockSize);
                }
            } else {
                /* fader is the only plugin that needs to know the length
                   of longest row */
                d->filters[data->id()] = new WavFilter(
                    data->id(), data,
                    d->berthaExperimentData.device().sampleRate(), d->blockSize,
                    filterType == QL1S("fader") ? true : false);
            }
        } else {
            QString id = data->id();
            QString type = data->xsiType();
            Filter *filter = MakeFilter(id, type, data);
            d->filters[id] = filter;
        }
    }
}

Filter *ExperimentRunDelegate::MakeFilter(const QString &ac_sID,
                                          const QString &ac_sType,
                                          data::FilterData *const ac_pParams)
{
    Filter *filter = 0;
    DeviceData *devData =
        experiment.devicesData()->deviceData(ac_pParams->device());
    WavDeviceData *wavData;

    if (devData->deviceType() == data::TYPE_WAVDEVICE)
        wavData = dynamic_cast<WavDeviceData *>(devData);
    else
        throw ApexStringException(
            "No filters supported for other device types than wavDevice");

    unsigned sr = wavData->sampleRate();

    Q_ASSERT(d->blockSize > 0);
    unsigned long bs = d->blockSize;

    if (ac_sType == sc_sFilterAmplifierType)
        filter = new WavAmplifier(ac_sID, ac_pParams, sr, bs);
    else if (ac_sType == sc_sFilterFaderType)
        filter = new WavFader(ac_sID, (WavFaderParameters *)ac_pParams, sr, bs);
    else if (ac_sType == sc_sFilterGeneratorType) {
        Q_ASSERT(wavData != NULL);

        unsigned long sr = wavData->sampleRate();
        filter =
            new WavGenerator(ac_sID, ac_pParams->valueByType("type").toString(),
                             ac_pParams, sr, d->blockSize, d->deterministic);
    } else if (ac_sType == sc_sFilterDataLoopType) // FIXME should be
                                                   // sc_sFilterGeneratorType
                                                   // with subtype DataLoop
    {
        Q_ASSERT(wavData != NULL);

        unsigned long sr = wavData->sampleRate();

        // get datablock if needed
        const QString sDataID(ac_pParams->valueByType("datablock").toString());
        stimulus::WavDataBlock *pSrc;
        try {
            pSrc = (WavDataBlock *)GetDatablock(sDataID);
        } catch (const std::exception &e) {
            throw Exception(tr("dataloop: datablock %1 not found: %2")
                                .arg(sDataID, e.what()));
        }
        //            ac_pParams->mp_SetExtraData( pSrc );

        DataLoopGeneratorFilter *gen = new DataLoopGeneratorFilter(
            ac_sID, sc_sFilterDataLoopType, ac_pParams, sr, d->blockSize);
        gen->SetSource(pSrc);

        ((DataLoopGenerator *)gen->GetStreamGen())
            ->SetInputStream(pSrc->GetWavStream(bs, sr));

        filter = gen;
    } else if (ac_sType == sc_sFilterPluginFilterType) {
        filter = new PluginFilter(
            ac_sID, dynamic_cast<data::PluginFilterData *>(ac_pParams), sr, bs);
    } else {
        qCCritical(APEX_RS,
                   "WavDeviceFactory::CreateFilter: Unknown FilterType");
        return 0;
    }

    return filter;
}

stimulus::DataBlock *
ExperimentRunDelegate::GetDatablock(const QString &p_name) const
{
    return d->datablocks.value(p_name);
}

void ExperimentRunDelegate::AddDatablock(const QString &name,
                                         stimulus::DataBlock *db)
{
    d->datablocks.insert(name, db);
}

const ExperimentData &ExperimentRunDelegate::GetData() const
{
    return experiment;
}

const bertha::ExperimentData &
ExperimentRunDelegate::getBerthaExperimentData() const
{
    return d->berthaExperimentData;
}

const stimulus::tDataBlockMap &ExperimentRunDelegate::GetDatablocks() const
{
    return d->datablocks;
}

tFilterMap &ExperimentRunDelegate::GetFilters() const
{
    return d->filters;
}

stimulus::Filter *ExperimentRunDelegate::GetFilter(const QString &p_name) const
{
    tFilterMap::const_iterator i = d->filters.find(p_name);
    if (i == d->filters.end()) {
        // throw
        // ApexStringException(tr(QString("ExperimentRunDelegate::GetFilter:
        // filter with id %1 not found").arg(p_name)));
        return 0;
    }
    return d->filters[p_name];
}

bool ExperimentRunDelegate::GetAutoAnswer() const
{
    return d->autoAnswer;
}

void ExperimentRunDelegate::enableAutoAnswer()
{
    d->autoAnswer = true;
}

ParameterManager *ExperimentRunDelegate::GetParameterManager() const
{
    return d->parameterManager.data();
}

const data::MainConfigFileData &ExperimentRunDelegate::mainData() const
{
    return d->mainData;
}

stimulus::StimulusOutput *ExperimentRunDelegate::GetModOutput() const
{
    return modOutput();
}

stimulus::Stimulus *ExperimentRunDelegate::GetStimulus(const QString &id) const
{
    if (!d->stimuli.contains(id))
        throw ApexStringException(QString("Stimulus not found: %1").arg(id));

    return d->stimuli.value(id);
}

QStringList ExperimentRunDelegate::stimuli() const
{
    return d->stimuli.keys();
}

TrialStartTime *ExperimentRunDelegate::trialStartTime() const
{
    return d->mod_trialStartTime.data();
}

ApexTimer *ExperimentRunDelegate::modTimer() const

{
    return d->mod_timer.data();
}

ApexScreen *ExperimentRunDelegate::modScreen() const
{
    return d->mod_screen.data();
}

ApexResultSink *ExperimentRunDelegate::modResultSink() const
{
    return d->mod_resultsink.data();
}

RTResultSink *ExperimentRunDelegate::modRTResultSink() const
{
    return d->mod_rtresultsink.data();
}

device::Controllers *ExperimentRunDelegate::modControllers() const
{
    return d->mod_controllers.data();
}

Calibrator *ExperimentRunDelegate::modCalibrator() const
{
    return d->mod_calibrator.data();
}

Feedback *ExperimentRunDelegate::modFeedback() const
{
    return d->mod_feedback.data();
}

stimulus::StimulusOutput *ExperimentRunDelegate::modOutput() const
{
    return d->mod_output.data();
}

StudyModule *ExperimentRunDelegate::modStudy() const
{
    return d->studyModule.data();
}

RandomGenerators *ExperimentRunDelegate::modRandomGenerators() const
{
    return d->mod_randomgenerators.data();
}

const ModuleList *ExperimentRunDelegate::modules() const
{
    return &d->modules;
}

QWidget *ExperimentRunDelegate::parentWidget() const
{
    return d->parent;
}

bool ExperimentRunDelegate::usingBertha() const
{
    return !d->berthaExperimentData.device().id().isNull() && d->useBertha;
}
}
