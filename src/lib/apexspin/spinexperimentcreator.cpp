/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdata/device/devicedata.h"
#include "apexdata/device/devicesdata.h"
#include "apexdata/device/wavdevicedata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/filter/filtersdata.h"
#include "apexdata/filter/wavparameters.h"

#include "apexdata/interactive/parameterdialogresults.h"

#include "apexdata/parameters/generalparameters.h"
#include "apexdata/parameters/parametermanagerdata.h"

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/constantproceduredata.h"
#include "apexdata/procedure/proceduredata.h"

#include "apexdata/result/resultparameters.h"

#include "apexdata/screen/screen.h"
#include "apexdata/screen/screensdata.h"

#include "apexdata/stimulus/stimulidata.h"

#include "apextools/apextools.h"

#include "apexwriters/experimentwriter.h"

#include "spinexperimentconstants.h"
#include "spinexperimentcreator.h"

#include <QDebug>

using namespace spin;
using namespace spin::data;

SpinExperimentCreator::SpinExperimentCreator(const SpinConfig& config,
                                             const SpinUserSettings& settings)
        : config(config), settings(settings)
{
}

SpinExperimentCreator::~SpinExperimentCreator()
{
}

void SpinExperimentCreator::createExperimentFile(const QString& fileName) const
{
    apex::writer::ExperimentWriter::write(createExperimentData(), fileName,
                                  QStringList() << screen());
}

apex::data::ExperimentData SpinExperimentCreator::createExperimentData() const
{
    QScopedPointer<apex::data::ScreensData> screens;
    QScopedPointer<apex::data::ProcedureData> procedures;
    QScopedPointer<apex::data::ConnectionsData> connections;
    QScopedPointer<apex::data::CalibrationData> calibration;
    QScopedPointer<apex::data::GeneralParameters> generalParams;
    QScopedPointer<apex::data::ResultParameters> resultParams;
    QScopedPointer<apex::data::ParameterDialogResults> paramDlg;
    QScopedPointer<QMap<QString, apex::data::RandomGeneratorParameters*> > randomGen;
    QScopedPointer<apex::data::DevicesData> devices;
    QScopedPointer<apex::data::FiltersData> filters;
    QScopedPointer<apex::data::DevicesData> controlDevices;
    QScopedPointer<apex::data::DatablocksData> datablocks;
    QScopedPointer<apex::data::StimuliData> stimuli;
    QScopedPointer<apex::data::ParameterManagerData> paramManager;

    datablocks.reset(createDatablocksData());
    stimuli.reset(createStimuliData());
    screens.reset(createScreensData());
    procedures.reset(createProcedureConfig());
    devices.reset(createDevicesData());
    filters.reset(createFiltersData());
    connections.reset(createConnectionsData());
    calibration.reset(createCalibrationData());
    generalParams.reset(createGeneralParameters());
    resultParams.reset(createResultParameters());

    controlDevices.reset(new apex::data::DevicesData());
    paramManager.reset(new apex::data::ParameterManagerData());
    paramDlg.reset(new apex::data::ParameterDialogResults());
    randomGen.reset(new QMap<QString, apex::data::RandomGeneratorParameters*>());

    return apex::data::ExperimentData(QString(),
                                      screens.take(),
                                      procedures.take(),
                                      connections.take(),
                                      calibration.take(),
                                      generalParams.take(),
                                      resultParams.take(),
                                      paramDlg.take(),
                                      randomGen.take(),
                                      devices.take(),
                                      filters.take(),
                                      controlDevices.take(),
                                      datablocks.take(),
                                      stimuli.take(),
                                      QString(),
                                      paramManager.take());
}

apex::data::DatablocksData* SpinExperimentCreator::createDatablocksData() const
{
    apex::data::DatablocksData* data = new apex::data::DatablocksData();
    data->setPrefix(config.prefix());

    if (needSilentDatablocks()) {
        apex::data::DatablockData* silent = new apex::data::DatablockData();
        silent->setId(constants::SILENT_DATABLOCK_ID);
        silent->setFile(constants::SILENT_DATABLOCK_FILE);
        silent->setDevice(constants::DEVICE_ID);

        data->insert(constants::SILENT_DATABLOCK_ID, silent);
    }

    //create a datablock for each speechtoken
    Q_FOREACH(Speechtoken token, list()) {
        apex::data::DatablockData* datablock = new apex::data::DatablockData();
        QString id = datablockId(token.id);
        datablock->setId(id);
        datablock->setFile(token.file);
        datablock->setDevice(constants::DEVICE_ID);

        data->insert(id, datablock);
    }

    //create noise datablock
    apex::data::DatablockData* noise = new apex::data::DatablockData();
    noise->setId(constants::NOISE_DATABLOCK_ID);
    noise->setFile(config.noise(settings.noisematerial()).file);
    noise->setDevice(constants::DEVICE_ID);

    data->insert(constants::NOISE_DATABLOCK_ID, noise);

    return data;
}

apex::data::StimuliData* SpinExperimentCreator::createStimuliData() const
{
    apex::data::StimuliData* data = new apex::data::StimuliData();
    data->setFixedParameters(QStringList() << config.textId());

    bool needSilence = needSilentDatablocks();
    //create a stimulus for each speechtoken
    Q_FOREACH(Speechtoken token, list())
    {
        apex::data::StimulusData stimulus;
        apex::data::StimulusDatablocksContainer datablocks;
        QString stimulusId = this->stimulusId(token.id);
        QString datablockId = this->datablockId(token.id);
        stimulus.setId(stimulusId);

        Q_ASSERT(!data->contains(stimulusId));

        //all datablocks are sequential
        apex::data::StimulusDatablocksContainer seqDatablocks(
                apex::data::StimulusDatablocksContainer::SEQUENTIAL);

        apex::data::StimulusDatablocksContainer datablock(
                apex::data::StimulusDatablocksContainer::DATABLOCK);

        if (needSilence)
        {
            datablock.id = constants::SILENT_DATABLOCK_ID;
            seqDatablocks.append(datablock);
        }

        datablock.id = datablockId;
        seqDatablocks.append(datablock);

        if (needSilence)
        {
            datablock.id = constants::SILENT_DATABLOCK_ID;
            seqDatablocks.append(datablock);
        }

        datablocks.append(seqDatablocks);
        stimulus.setDatablocksContainer(datablocks);

        //create the fixed parameters
        apex::data::StimulusParameters params;
        params.insert(config.textId(), token.text);
        stimulus.setFixedParameters(params);

        data->insert(stimulusId, stimulus);
    }

     //stimulus for calibration
    {
    apex::data::StimulusData stimulus;
    stimulus.setId(constants::CALIBRATION_STIMULUS);
    apex::data::StimulusDatablocksContainer datablocks;
    apex::data::StimulusDatablocksContainer datablock(
            apex::data::StimulusDatablocksContainer::DATABLOCK);
    datablock.id = constants::SILENT_DATABLOCK_ID;
    datablocks.append(datablock);
    stimulus.setDatablocksContainer(datablocks);

    //create the fixed parameters
    apex::data::StimulusParameters params;
    params.insert(config.textId(), constants::CALIBRATION_FIXED_PARAM);
    stimulus.setFixedParameters(params);

    data->insert(constants::CALIBRATION_STIMULUS, stimulus);
    }

    //extra noise stimulus for calibration
    {
    apex::data::StimulusData stimulus;
    stimulus.setId(constants::NOISE_STIMULUS);
    apex::data::StimulusDatablocksContainer datablocks;
    apex::data::StimulusDatablocksContainer datablock(
            apex::data::StimulusDatablocksContainer::DATABLOCK);
    datablock.id = constants::NOISE_DATABLOCK_ID;
    datablocks.append(datablock);
    stimulus.setDatablocksContainer(datablocks);

    //create the fixed parameters
    apex::data::StimulusParameters params;
    params.insert(config.textId(), constants::CALIBRATION_FIXED_PARAM);
    stimulus.setFixedParameters(params);

    data->insert(constants::NOISE_STIMULUS, stimulus);
    }

    return data;
}

apex::data::ProcedureData* SpinExperimentCreator::createProcedureConfig() const
{
    apex::data::ProcedureData* data;

    if (settings.procedureType() == CONSTANT) {
        data = new apex::data::ConstantProcedureData();
    } else { //adaptive procedure
        apex::data::AdaptiveProcedureData* adaptiveData =
                new apex::data::AdaptiveProcedureData();

        // will be overriden later if there is no noise present
        // round SNR to 2 decimals
        double rsnr = settings.hasNoise() ? qRound(settings.snr() * 100) / 100 : 0;

        adaptiveData->setStartValue(rsnr);
        adaptiveData->setNUp(constants::PARAM_N_UP);
        adaptiveData->setNDown(constants::PARAM_N_DOWN);
        adaptiveData->setRepeatFirstUntilCorrect(settings.repeatFirst());

        if (settings.adaptingMaterial() == data::SPEECH) {
            adaptiveData->setLargerIsEasier(true);
            adaptiveData->addAdaptingParameter(amplifierGainId());

            Q_FOREACH (uint channel, config.channelList(settings.speakerType())) {
                data::SpeakerLevels levels;

                if (settings.speakerType() == data::HEADPHONE)
                    levels = settings.speakerLevels(
                            (data::Headphone::Speaker)channel);
                else
                    levels = settings.speakerLevels(
                            config.angleOfChannel(channel));

                if (levels.hasSpeech && !levels.hasNoise)
                    // levels.speech is the target level in dBA. The adaptive
                    // procedure works by changing the gain of the speech, so
                    // the start value is relative to the calibration level.
                    // E.g., if the channel has a calibration level of 65dBA,
                    // this means that the output level will be 65dBA for a
                    // gain of 0.
                    adaptiveData->setStartValue(levels.speech -
                            calibrationLevel(channel));
            }
        } else { //adapt noise; add gain id of each channel with noise
            adaptiveData->setLargerIsEasier(true);   // Needs to be true, because in filters invertgain is set to true for adapting noise
            Q_FOREACH(uint channel, config.channelList(settings.speakerType())) {
                data::SpeakerLevels levels;
                if (settings.speakerType() == data::HEADPHONE)
                    levels = settings.speakerLevels(
                        (data::Headphone::Speaker)channel);
                else
                    levels = settings.speakerLevels(
                        config.angleOfChannel(channel));

                if (levels.hasNoise)
                    adaptiveData->addAdaptingParameter(noiseFilterGainId(channel));
            }
        }

        //add all stepsizes
        QMap<uint, double> stepsizes = settings.stepsizes();
        QMap<uint, double>::const_iterator it;
        for (it = stepsizes.begin(); it != stepsizes.end(); it++) {
            adaptiveData->addUpStepsize(it.key(), it.value());
            adaptiveData->addDownStepsize(it.key(), it.value());
        }

        data = adaptiveData;
    }

    //set data common between constant and adaptive

    data->setTimeBeforeFirstStimulus(settings.timeBeforeFirstStimulus());
    data->setPresentations(constants::PARAM_PRESENTATIONS);
    data->setSkip(constants::PARAM_SKIP);
    data->setNumberOfChoices(constants::PARAM_NB_CHOICES);
    data->setDefaultStandard(constants::PARAM_DEF_STD);
    data->setInputDuringStimulus(constants::PARAM_INPUT_DURING_STIMULUS);
    data->setOrder(settings.trialOrder() == data::ORDER_RANDOM ?
            apex::data::ProcedureData::RandomOrder :
            apex::data::ProcedureData::SequentialOrder);

    data->setCorrectorData(createCorrectorData());

    Q_FOREACH(Speechtoken token, list())
    {
        apex::data::TrialData* trial = new apex::data::TrialData();
        QString answer = (settings.personBeforeScreen() == data::SUBJECT) ?
                                    apex::ApexTools::removeXmlTags(token.text) :
                                    constants::ANSWER_EXPERIMENTER_SCREEN;
        trial->SetAnswer(answer);
        trial->SetID(trialId(token.id));
        trial->SetScreen(screenId());
        trial->AddStimulus(stimulusId(token.id));
        data->AddTrial(trial);
    }

    return data;
}

apex::data::ScreensData* SpinExperimentCreator::createScreensData() const
{
    apex::data::ScreensData* data = new apex::data::ScreensData();
    //if there is some data to be filled in, do it here

    if (settings.personBeforeScreen() == data::EXPERIMENTER)
        data->setRepeatButtonEnabled(true);

    return data;
}

apex::data::DevicesData* SpinExperimentCreator::createDevicesData() const
{
    apex::data::WavDeviceData* wavdevice = new apex::data::WavDeviceData();
    wavdevice->setId(constants::DEVICE_ID);
    //[job setDefault] wavdevice->setDefaultParameterValues();//so called "simple" parameters:)
    wavdevice->setDriverString(driverString());

    if (config.soundcardBlocksize())
        wavdevice->setBlockSize(config.soundcardBlocksize());

    if (config.soundcardBuffersize())
        wavdevice->setBufferSize(config.soundcardBuffersize());


    unsigned padding = padZero();
    if (padding > 0)
        wavdevice->setValueByType("padzero", padding);

    int channels = numberOfChannels();
    wavdevice->setNumberOfChannels(channels);

    for (int i = 0; i < channels; i++)
    {
        apex::data::Parameter name(constants::DEVICE_ID,
               "gain", 0.0,
                i, true, deviceChannelParameterName(i));
        name.setDefaultValue(constants::DEVICE_GAIN);
        wavdevice->addParameter(name);
    }

    apex::data::DevicesData* data = new apex::data::DevicesData();
    (*data)[constants::DEVICE_ID] = wavdevice;
    data->setMasterDevice(constants::DEVICE_ID);

    return data;
}

QString SpinExperimentCreator::deviceChannelParameterName(int channel) const
{
    return  QString("gain%1").arg(channel);
}

apex::data::FiltersData* SpinExperimentCreator::createFiltersData() const
{
    apex::data::FiltersData* data = new apex::data::FiltersData();
    bool hasNoise = false;

    //create filters for each channel
    Q_FOREACH(uint channel, config.channelList(settings.speakerType()))
    {
        data::SpeakerLevels levels;

        if (settings.speakerType() == data::HEADPHONE)
            levels = settings.speakerLevels((data::Headphone::Speaker)channel);
        else
            levels = settings.speakerLevels(config.angleOfChannel(channel));

        if (levels.hasNoise)
        {
            hasNoise = true;
            apex::data::FilterData* filter = createNoiseFilter(channel);
            QString filterId = filter->id();

            // Scale the noise signal to the internal RMS
            // [Tom] FIXME: this is equivalent to (internalRMS - noiseRms())
            double gainCorr = config.defaultCalibration() -
                        fullScaleToAccoustic(noiseRms());
            filter->setValueByType("basegain", gainCorr);
            qCDebug(APEX_RS) << "adding noiselevel" << gainCorr << "to filter" << filterId;

            (*data)[filterId] = filter;

            if (settings.noiseStopsBetweenTrials())
            {
                apex::data::FilterData* fadeInFilter = createNoiseFadeFilter(true, channel);
                QString fadeInFilterId = fadeInFilter->id();
                (*data)[fadeInFilterId] = fadeInFilter;
                apex::data::FilterData* fadeOutFilter = createNoiseFadeFilter(false, channel);
                QString fadeOutFilterId = fadeOutFilter->id();
                (*data)[fadeOutFilterId] = fadeOutFilter;
            }
        }

        if (levels.hasSpeech)
        {
            double clevel = calibrationLevel(channel);

            double slevel = speechLevel(channel);

            // calculate levels for SNR=0, because the procedure will set the SNR
            if ((settings.procedureType() == ADAPTIVE && settings.adaptingMaterial() == data::SPEECH) ||
                    (settings.procedureType() == CONSTANT))
            {
                if (settings.hasNoise())
                    // channel is calibrated to the noise level
                    // calibration is for SNR=0
                    // the speech level includes the start snr, so to get
                    // the value for snr=0 we need to subtract the SNR
                    slevel = slevel - settings.snr();
                else
                    // channel is calibrated to the speech level
                    slevel = 0;
            }

            apex::data::FilterData* filter = createSpeechFilter(channel);
            QString filterId = filter->id();
            // [Tom] FIXME: can be simplified, see above
            double gainCorr = config.defaultCalibration() + slevel - clevel
                - fullScaleToAccoustic(internalRms());
            filter->setValueByType("basegain", gainCorr);
            qCDebug(APEX_RS) << "adding noiselevel" << gainCorr << "to filter" << filterId;

            (*data)[filterId] = filter;
        }
    }

    apex::data::FilterData* amp = createAmplifier();
    double gainCorr = internalRms() - speechRms();
    amp->setValueByType("basegain", gainCorr);

    if (settings.procedureType() == CONSTANT) {
         if (hasNoise) // noise in any channel
            amp->setValueByType("gain", settings.snr());
         else
            amp->setValueByType("gain", settings.speechLevel());
    }
    (*data)[amp->id()] = amp;

    return data;
}

double SpinExperimentCreator::calibrationLevel(uint channel) const
{
    //Q_ASSERT(channel<settings.numberOfChannelsInUse());

    if (! settings.hasNoise() )
        return 0;

    data::SpeakerLevels levels;
    if (settings.speakerType() == data::HEADPHONE)
        levels = settings.speakerLevels((data::Headphone::Speaker)channel);
    else
        levels = settings.speakerLevels(config.angleOfChannel(channel));

    if (levels.hasNoise) {
        double nlevel = noiseLevel(channel);
        // calculate levels for SNR=0, because the procedure will set the SNR
        if (settings.procedureType() == ADAPTIVE) {
            if (settings.adaptingMaterial() == data::NOISE) {
                // The start value of the adaptive procedure is set in the
                // interface by setting the noise level to a certain value
                // relative to the speech level. This means that to find the
                // noise level for SNR=0, we need to add the SNR to the noise
                // level given in the interface.
                nlevel = nlevel + settings.snr();
            }
        }
        return nlevel;
    }

    // else: no noise
    if (levels.hasSpeech)
    {
        // calculate levels for SNR=0, because the procedure will set the SNR
        double slevel = speechLevel(channel);
        if (settings.procedureType() == ADAPTIVE) {
            if (settings.adaptingMaterial() == data::SPEECH) {
                // The start value of the adaptive procedure is set in the
                // interface by setting the speech level to a certain value
                // relative to the noise level. This means that to find the
                // speech level for SNR=0, we need to subtract the SNR from the
                // noise level given in the interface.
                slevel = slevel - settings.snr();
            }
        }
        return slevel;
    }

    return 0;
}

apex::data::FilterData* SpinExperimentCreator::createNoiseFadeFilter(
        bool fadein, uint channel) const
{
    apex::data::FilterData* filter = new apex::data::WavFaderParameters();
    filter->setId((fadein ? QLatin1String("fadein_") : QLatin1String("fadeout_"))
                + noiseFilterId(channel));
    filter->setXsiType("apex:fader");
    filter->setValueByType("channels", 1);
    filter->setValueByType("device", constants::DEVICE_ID);
    filter->setValueByType("length", 50.0);
    filter->setValueByType("direction", fadein ? "in" : "out");
    filter->setValueByType("type", "cosine");

    return filter;
}

apex::data::FilterData* SpinExperimentCreator::createNoiseFilter(
        uint channel) const
{
    apex::data::FilterData* filter = new apex::data::DataLoopGeneratorParameters();
    filter->setId(noiseFilterId(channel));
    //[job setDefault] filter->setDefaultParameterValues();//so called "simple" parameters:)
    filter->setXsiType("apex:" + constants::FILTER_TYPE_NOISE);
    filter->setValueByType("channels", 1);
    filter->setValueByType("device", constants::DEVICE_ID);
    filter->setValueByType("continuous", !settings.noiseStopsBetweenTrials());
    filter->setValueByType("datablock", constants::NOISE_DATABLOCK_ID);
    if (settings.procedureType() == ADAPTIVE &&
        settings.adaptingMaterial() == data::NOISE)
        filter->setValueByType("invertgain", true);

    //randomjump
    int chanangle;
    if (settings.speakerType() == data::FREE_FIELD)
         chanangle=config.angleOfChannel(channel);
    else
         chanangle=channel;

    if (settings.noiseJump(chanangle) == data::RANDOM)
        filter->setValueByType("randomjump", true);
    else
        filter->setValueByType("jump", settings.noiseJump(channel));

    //give gain parameter an id
    apex::data::Parameter gain = filter->parameterByType("gain");
    filter->removeParameter(gain);//will not replace the gain parametername otherwise
    gain.setId(noiseFilterGainId(channel));
    gain.setDefaultValue(0);
    filter->addParameter(gain);

    return filter;
}

apex::data::FilterData* SpinExperimentCreator::createSpeechFilter(
        uint channel) const
{
    apex::data::FilterData* filter = new apex::data::WavFilterParameters();
    filter->setId(speechFilterId(channel));
    //[job setDefault] filter->setDefaultParameterValues();//so called "simple" parameters:)
    filter->setXsiType("apex:" + constants::FILTER_TYPE_SPEECH);
    filter->setValueByType("channels", 1);
    filter->setValueByType("device", constants::DEVICE_ID);

    return filter;
}

apex::data::FilterData* SpinExperimentCreator::createAmplifier() const
{
    apex::data::FilterData* filter = new apex::data::WavFilterParameters();
    filter->setId(constants::FILTER_TYPE_SPEECH);
    //[job setDefault] filter->setDefaultParameterValues();//so called "simple" parameters:)
    filter->setXsiType("apex:" + constants::FILTER_TYPE_SPEECH);
    filter->setValueByType("channels", 1);
    filter->setValueByType("device", constants::DEVICE_ID);

    //give gain parameter an id
    apex::data::Parameter gain = filter->parameterByType("gain");
    filter->removeParameter(gain);//will not replace the gain parametername otherwise
    gain.setId(amplifierGainId());
    gain.setDefaultValue(0);
    filter->addParameter(gain);

    return filter;
}

apex::data::ConnectionsData* SpinExperimentCreator::createConnectionsData() const
{
    apex::data::ConnectionsData* data = new apex::data::ConnectionsData();

    //connection from all to amplifier
    data->push_back(createConnection(constants::CONN_FROM_ALL_ID, amplifierId()));
    //connections from amplifier to all speech filters (amplifiers) and from
    //those filters to the device
    //we find those filters by looking for channels with speech
    //also create connections from noise filters (dataloops) to the device
    //in the same way
    Q_FOREACH(uint channel, config.channelList(settings.speakerType()))
    {
        data::SpeakerLevels levels;

        if (settings.speakerType() == data::HEADPHONE)
            levels = settings.speakerLevels((data::Headphone::Speaker)channel);
        else
            levels = settings.speakerLevels(config.angleOfChannel(channel));

        if (levels.hasSpeech)
        {
            data->push_back(createConnection(amplifierId(),
                            speechFilterId(channel)));
            data->push_back(createConnection(speechFilterId(channel),
                            constants::DEVICE_ID, channel));
        }
        if (levels.hasNoise)
        {
            if (settings.noiseStopsBetweenTrials()) {
                data->push_back(createConnection(noiseFilterId(channel),
                            "fadein_" + noiseFilterId(channel), 0));
                data->push_back(createConnection("fadein_" + noiseFilterId(channel),
                            "fadeout_" + noiseFilterId(channel), 0));
                data->push_back(createConnection("fadeout_" + noiseFilterId(channel),
                            constants::DEVICE_ID, channel));
            } else {
                data->push_back(createConnection(noiseFilterId(channel),
                            constants::DEVICE_ID, channel));
            }
        }
    }

    return data;
}

apex::data::ConnectionData* SpinExperimentCreator::createConnection(
            const QString& fromId, const QString& toId, uint toChannel) const
{
    apex::data::ConnectionData* connection = new apex::data::ConnectionData();

    connection->setFromId(fromId);
    connection->setFromChannel(0);
    connection->setToId(toId);
    connection->setToChannel(toChannel);

    return connection;
}

apex::data::CorrectorData* SpinExperimentCreator::createCorrectorData() const
{
    apex::data::CorrectorData* data = new apex::data::CorrectorData();
    data->setType(apex::data::CorrectorData::EQUAL);
    return data;
}

apex::data::CalibrationData* SpinExperimentCreator::createCalibrationData() const
{
    apex::data::CalibrationData* data = new apex::data::CalibrationData();

    QString profileName( constants::CALIBRATION_PROFILE +
            "-" + settings.noisematerial() );

    data->setCalibrationProfile(profileName);
    data->addAvailableStimulus(constants::CALIBRATION_STIMULUS);
    data->addAvailableStimulus(constants::NOISE_STIMULUS);

    apex::data::CalibrationParameterData param(constants::CALIBRATION_MIN,
            constants::CALIBRATION_MAX,
            constants::CALIBRATION_DEFAULT,
            constants::CALIBRATION_MUTE,
            80, 0);

    // calibrate one parameter per channel
    Q_FOREACH(uint channel, config.channelList(settings.speakerType()))
    {
        data::SpeakerLevels levels;

        if (settings.speakerType() == data::HEADPHONE)
            levels = settings.speakerLevels((data::Headphone::Speaker)channel);
        else
            levels = settings.speakerLevels(config.angleOfChannel(channel));

        if (levels.hasSpeech || levels.hasNoise) {
            param.setFinalTargetAmplitude(calibrationLevel(channel));
            data->addParameter(deviceChannelParameterName(channel), param);
        }
    }



    return data;
}

apex::data::GeneralParameters* SpinExperimentCreator::createGeneralParameters() const
{
    apex::data::GeneralParameters* data = new apex::data::GeneralParameters();
    data->setExitAfter(settings.exitAfter());
    data->setAutoSave(settings.autoSaveResults());
    return data;
}

apex::data::ResultParameters* SpinExperimentCreator::createResultParameters() const
{
    apex::data::ResultParameters* data = new apex::data::ResultParameters();
    data->setSubject(settings.subjectName());
    data->setShowResultsAfter(settings.showResults());
    data->setResultPage(constants::HTML_PAGE);
    if (settings.procedureType() == ADAPTIVE) {
         data->setResultParameter("reversals for mean",
                 QString::number(settings.nbResponsesThatCount()));
    }
    if (settings.snrDefined())
        data->setResultParameter("snr", QString::number(settings.snr()));
    return data;
}

QString SpinExperimentCreator::driverString() const
{
    if (! config.soundcardDriver().isEmpty())
        return config.soundcardDriver();

    switch (settings.soundCard()) {
        case RmeMultiface:
        case RmeFirefaceUc:
            return QLatin1String("asio");
        case LynxOne:
            return QLatin1String("portaudio");
        case DefaultSoundcard:
#ifdef Q_OS_WIN32
        return QLatin1String("asio");       // make sure 24bit precision is used
#else
        return QLatin1String("portaudio");
#endif
    default:
        qFatal("Invalid sound card");
    }

    return QString();       // avoid compiler warning
}

unsigned SpinExperimentCreator::padZero() const
{
    switch (settings.soundCard()) {
    case RmeFirefaceUc:
        // It seems like the RME Fireface device has an internal buffer that is
        // 8 times as big as the hardware buffer size (8x max. 2048 samples =
        // max 16364 samples). When stopped, the Fireface will continue to
        // present the contents of this buffer although the wav data in it is
        // old, i.e. the end of the stimulus will be repeated. To work around
        // this, this adds 2 buffers (*apex* buffers, default 8192 samples each)
        // with silence to the end of the stimulus.
        return 2;
    case LynxOne:
        // Seems like the LynxOne portaudio driver drops the end of the
        // stimulus, pad with some zeros to prevent that
        return 1;
    case RmeMultiface:
        return 0;
    default:
        // Just in case, for all soundcards
        return 1;
    }
}

int SpinExperimentCreator::bufferSize() const
{
    /*if (driverString() == "asio")
        return 4096;
    else
        return 8192;*/
    return -1;                  // system default
}

QString SpinExperimentCreator::screen() const
{
    if (settings.customScreen().isEmpty())
    /*{
        return (settings.personBeforeScreen() == SUBJECT) ?
                config.subjectScreen() : config.experimenterScreen();
    }*/

    {
        if (settings.personBeforeScreen() == SUBJECT)
                return config.subjectScreen();
        else {
            if (settings.hasNoise()){
                return config.experimenterScreenNoise();}
            else
                return config.experimenterScreenQuiet();
        }
    }

    return config.customScreen(settings.customScreen());
}

QString SpinExperimentCreator::screenId() const
{
    if (settings.customScreen().isEmpty())
    {
        if (settings.personBeforeScreen() == SUBJECT)
                return constants::SCREEN_SUBJECT;
        else {
            if (settings.hasNoise())
                return constants::SCREEN_EXPERIMENTER_NOISE;
            else
                return constants::SCREEN_EXPERIMENTER_QUIET;
        }
    }

    return config.customScreenId(settings.customScreen());
}

bool SpinExperimentCreator::needSilentDatablocks() const
{
    // necessary for e.g. fader
    return true;
}

data::List SpinExperimentCreator::list() const
{
    return config.list(settings.list(), speechmaterial(), speechcategory());
}

QString SpinExperimentCreator::speechmaterial() const
{
    return settings.speechmaterial();
}

QString SpinExperimentCreator::speechcategory() const
{
    return settings.speechcategory();
}

QString SpinExperimentCreator::noisematerial() const
{
    return settings.noisematerial();
}

double SpinExperimentCreator::noiseRms() const
{
    return config.noiseRms(noisematerial());
}

double SpinExperimentCreator::speechRms() const
{
    return config.speechRms(speechmaterial(), speechcategory());
}

double SpinExperimentCreator::internalRms() const
{
    return config.internalRms();
}

double SpinExperimentCreator::speechLevel(int channel) const
{
    data::SpeakerLevels levels;

    if (settings.speakerType() == data::HEADPHONE)
        levels = settings.speakerLevels((data::Headphone::Speaker)channel);
    else
        levels = settings.speakerLevels(config.angleOfChannel(channel));

    Q_ASSERT(levels.hasSpeech);

    return levels.speech;
}

double SpinExperimentCreator::noiseLevel(int channel) const
{
    data::SpeakerLevels levels;

    if (settings.speakerType() == data::HEADPHONE)
        levels = settings.speakerLevels((data::Headphone::Speaker)channel);
    else
        levels = settings.speakerLevels(config.angleOfChannel(channel));

    Q_ASSERT(levels.hasNoise);

    return levels.noise;
}

QList<uint> SpinExperimentCreator::usedChannels() const
{
    QList<uint> channels;

    Q_FOREACH(uint channel, config.channelList(settings.speakerType()))
    {
        data::SpeakerLevels levels;

        if (settings.speakerType() == data::HEADPHONE)
            levels = settings.speakerLevels((data::Headphone::Speaker)channel);
        else
            levels = settings.speakerLevels(channel);

        if (levels.isInUse())
            channels += channel;
    }

    return channels;
}

QString SpinExperimentCreator::datablockId(QString tokenId) const
{
    if (tokenId.isEmpty())
        qFatal("Cannot create id with an empty string");

    QString id = constants::PREFIX_DATABLOCK + "_" + speechmaterial();

    if (!speechcategory().isEmpty())
        id += "_" + speechcategory();

    return id + "_" + tokenId;
}

QString SpinExperimentCreator::stimulusId(QString tokenId) const
{
    if (tokenId.isEmpty())
        qFatal("Cannot create id with an empty string");

    QString id = constants::PREFIX_STIMULUS + "_" + speechmaterial();

    if (!speechcategory().isEmpty())
        id += "_" + speechcategory();

    return id + "_" + tokenId;
}

QString SpinExperimentCreator::trialId(QString tokenId) const
{
    if (tokenId.isEmpty())
        qFatal("Cannot create id with an empty string");

    QString id = constants::PREFIX_TRIAL + "_" + speechmaterial();

    if (!speechcategory().isEmpty())
        id += "_" + speechcategory();

    return id + "_" + tokenId;
}

QString SpinExperimentCreator::gainId(uint channel) const
{
    return constants::PREFIX_GAIN + "_channel" + QString::number(channel);
}

QString SpinExperimentCreator::noiseFilterId(uint channel) const
{
    return constants::FILTER_TYPE_NOISE + "_channel" + QString::number(channel);
}

QString SpinExperimentCreator::speechFilterId(uint channel) const
{
    return constants::FILTER_TYPE_SPEECH + "_channel" + QString::number(channel);
}

QString SpinExperimentCreator::amplifierId() const
{
    return constants::FILTER_TYPE_SPEECH;
}

QString SpinExperimentCreator::amplifierGainId() const
{
    // in the constant or adaptive speech case, this is the gain that needs to
    // be displayed, give it the right id so that the screen can find it
    if ((settings.procedureType() == CONSTANT) ||
        (settings.procedureType() == ADAPTIVE && settings.adaptingMaterial() == data::SPEECH))
        return config.gainId();
    return constants::FILTER_GAIN_SPEECH;
}

QString SpinExperimentCreator::noiseFilterGainId(uint channel) const
{
    // in the adaptive noise case on channel 0, this is the gain that needs to
    // be displayed, give it the right id so that the screen can find it
    if ((settings.procedureType() == ADAPTIVE && settings.adaptingMaterial() == data::NOISE && channel == 0))
        return config.gainId();
    return constants::FILTER_GAIN_NOISE + "_channel" + QString::number(channel);
}

double SpinExperimentCreator::fullScaleToAccoustic(double dBFS) const
{
    return dBFS + (settings.dBAOfInternalRms() - internalRms());
}

unsigned SpinExperimentCreator::numberOfChannels() const
{
// calculate necessary number of channels
    unsigned int channels=0;
    if (settings.speakerType() == data::HEADPHONE) {
        channels=2;
    } else if (settings.speakerType() == data::FREE_FIELD) {
        channels=0;

        QList<uint> angles = settings.speakerAngles();
        Q_FOREACH(uint angle, angles) {
            uint channel =  config.channelOfAngle(angle);
            if (channel>=channels)
                channels=channel;
        }
        ++channels;
    } else {
        qFatal("Invalid type");
    }

    return channels;
}
