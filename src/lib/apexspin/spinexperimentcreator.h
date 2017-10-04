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

#ifndef _APEX_SRC_LIB_APEXSPIN_SPINEXPERIMENTCREATOR_H_
#define _APEX_SRC_LIB_APEXSPIN_SPINEXPERIMENTCREATOR_H_

#include "spinconfig.h"
#include "spinusersettings.h"

#include <QList>

namespace apex
{
namespace data
{
class ExperimentData;
class DatablocksData;
class StimuliData;
class ProcedureData;
class ScreensData;
class DevicesData;
class FiltersData;
class FilterData;
class ConnectionsData;
class ConnectionData;
class CorrectorData;
class CalibrationData;
class GeneralParameters;
class ResultParameters;
}
}

class QString;

namespace spin
{

namespace data
{
struct List;
struct SpeakerLevels;
}

class APEXSPIN_EXPORT SpinExperimentCreator
{
    Q_DECLARE_TR_FUNCTIONS(SpinExperimentCreator)
public:
    SpinExperimentCreator(const data::SpinConfig &config,
                          const data::SpinUserSettings &settings);
    ~SpinExperimentCreator();

    void createExperimentFile(const QString &fileName) const;
    void createJavascriptFile(const QString &fileName,
                              const QString &suffixFilename) const;

    /**
     * Returns the name of the device gain parameter for a given channel
     * channels are zero based
     */
    QString deviceChannelParameterName(int channel) const;

private:
    apex::data::ExperimentData
    createExperimentData(const QString &jsFile = QString()) const;
    apex::data::DatablocksData *createDatablocksData() const;
    apex::data::StimuliData *createStimuliData() const;
    apex::data::ProcedureData *createProcedureConfig() const;
    apex::data::ScreensData *createScreensData() const;
    apex::data::DevicesData *createDevicesData() const;
    apex::data::FiltersData *createFiltersData() const;
    apex::data::FilterData *createNoiseFilter(uint channel) const;
    apex::data::FilterData *createNoiseFadeFilter(bool fadein,
                                                  uint channel) const;
    apex::data::FilterData *createSpeechFilter(uint channel) const;
    apex::data::FilterData *createAmplifier() const;
    apex::data::ConnectionsData *createConnectionsData() const;

    /**
     * Creates a connections with the given ids and to channel and from
     * channel set to 0;
     */
    apex::data::ConnectionData *createConnection(const QString &fromId,
                                                 const QString &toId,
                                                 uint toChannel = 0) const;
    apex::data::CorrectorData *createCorrectorData() const;
    apex::data::CalibrationData *createCalibrationData() const;
    apex::data::GeneralParameters *
    createGeneralParameters(const QString &jsFile) const;
    apex::data::ResultParameters *createResultParameters() const;

    /** Returns the number of blocks (8192 frames) padding needed to work
     * around soundcard bugs
     */
    unsigned padZero() const;

    /**
     * Returns the driver string for the audio devices.
     */
    QString driverString() const;

    /**
     * Returns the soundcard buffer size to be used
     */
    int bufferSize() const;

    /**
     * Returns the id of the screen.
     */
    QString screenId() const;

    /**
     * Return the xml code of the screen.
     */
    QString screen() const;

    /**
     * Returns wheter a silent datablock should be inserted before and after
     * each datablock.
     */
    bool needSilentDatablocks() const;

    /**
     * Returns the list that corresponds to the settings and config
     */
    data::List list() const;

    /**
     * Returns the speechmaterial that corresponds to the settings.
     */
    QString speechmaterial() const;

    /**
     * Returns the noisematerial that corresponds to the settings.
     */
    QString noisematerial() const;

    /**
     * Returns the speechcategory that corresponds to the settings.
     */
    QString speechcategory() const;

    /**
     * Return the noise rms that corresponds to the settings.
     */
    double noiseRms() const;

    /**
     * Return the speech rms that corresponds to the settings.
     */
    double speechRms() const;

    /**
     * Return the internal rms that associated with the config.
     */
    double internalRms() const;

    /**
     * Returns the speech level of the given channel.
     */
    double speechLevel(int channel) const;

    /**
     * Returns the noise level of the given channel.
     */
    double noiseLevel(int channel) const;

    /**
     * Returns a list of all used channels.
     */
    QList<uint> usedChannels() const;

    /**
     * Creates an id for a datablock given the id of the speechtoken.
     */
    QString datablockId(QString tokenId) const;

    /**
     * Creates an id for a stimulus given the id of the speechtoken.
     */
    QString stimulusId(QString tokenId) const;

    /**
     * Creates an id for a trial given the id of the speechtoken.
     */
    QString trialId(QString tokenId) const;

    /**
     * Creates an id for the gain of the given channel.
     */
    QString gainId(uint channel) const;

    /**
     * Creates an id for a noise filter on the given channel.
     */
    QString noiseFilterId(uint channel) const;

    /**
     * Creates an id for a speech filter on the given channel.
     */
    QString speechFilterId(uint channel) const;

    /**
     * Creates the id for the amplifier.
     */
    QString amplifierId() const;

    /**
     * Create an id for the gain in an amplifier.
     */
    QString amplifierGainId() const;

    /**
     * Create an id for the gain in a noise filter.
     */
    QString noiseFilterGainId(uint channel) const;

    /**
     * Returns the dBA value of the given value in dBFS
     */
    double fullScaleToAccoustic(double dBFS) const;

    /**
     * Returns the level (dB SPL/A) at which the given channel
     * will be calibrated. If an adaptive procedure is used, this is defined
     * for SNR=0.
     * E.g., if this function returns a value of 63dBA for channel 1, this means
     * that if all filters/amplfiers/dataloops are set at gain=0,
     * noise or speech on this channel will be played at 63dBA.
     */
    double calibrationLevel(uint channel) const;

    /**
     * Returns the needed number of channels
     */
    unsigned numberOfChannels() const;

    const data::SpinConfig config;
    const data::SpinUserSettings settings;

#ifdef CLEBS_DEBUG
public:
    void showResults() const;
#endif
};

} // ns spin

#endif
