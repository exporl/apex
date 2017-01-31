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

#ifndef _EXPORL_SRC_LIB_APEXSPIN_SPINUSERSETTINGS_H_
#define _EXPORL_SRC_LIB_APEXSPIN_SPINUSERSETTINGS_H_

#include "apextools/global.h"

#include "spindataenums.h"

#include <QDate>
#include <QMap>
#include <QString>

namespace spin
{
namespace data
{

struct APEXSPIN_EXPORT Info
{
    QString subjectName;
};

struct APEXSPIN_EXPORT Materials
{
    QString speechmaterial;
    QString speechcategory;
    QString noisematerial;
    QString list;
};

struct APEXSPIN_EXPORT SpeakerLevels
{
    SpeakerLevels();

    // FIXME: make methods and on noise() or speech() check whether hasNoise and
    // hasSpeech are satisfied
    double noise;
    double speech;
    bool hasNoise;
    bool hasSpeech;

    QString toString() const;
    bool bothEnabled() const;
    bool isInUse() const;
};

struct APEXSPIN_EXPORT Headphone
{
    enum Speaker { LEFT , RIGHT };

    Q_DECLARE_FLAGS(Speakers, Speaker)

    SpeakerLevels leftLevels;
    SpeakerLevels rightLevels;

    uint numberOfChannels() const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Headphone::Speakers)

struct APEXSPIN_EXPORT FreeField
{
    /**
     * Returns the speaker levels of the speaker at the given degree.
     * Will assert if there is no such speaker.
     */
    const SpeakerLevels levels(uint degree) const;

    /**
     * Adds the given levels for the given degree. If there existed
     * levels for the given degree, they will be overwritten.
     * If the given degree is greater than 360, the degree will be
     * set to degree mod 360.
     */
    void addLevels(uint degree, SpeakerLevels levels);

    QList<uint> angles() const;
    uint numberOfChannelsInUse() const;

private:
    QMap<uint, SpeakerLevels> levelsMap;
};

struct APEXSPIN_EXPORT Speakers
{
    Speakers() :
        type(NO_TYPE),
        lockSpeech(false),
        lockNoise(false)
    {
    }

    SpeakerType type;
    Headphone headphone;
    FreeField freeField;

    bool lockSpeech;
    bool lockNoise;
};

struct APEXSPIN_EXPORT Procedure
{
    Procedure();

    ProcedureType type;
    Material adaptingMaterial;
    QMap<uint, double> stepsizes; //maps from trial to stepsize
    bool repeatFirst;

    /**
     * Adds a stepsize for the given trial. If there existed a stepsize
     * for the given trial, it will be overwritten.
     * Will assert if stepsize holds a negative value or when type is
     * CONSTANT.
     *
     * @note It is possible to use insert() directly on stepsizes but
     *   it is recommended to use this method because it ensure
     *   correct insertion.
     */
    void addStepsize(uint trial, double stepsize);
};

struct APEXSPIN_EXPORT Calibration
{
    double dBAOfInternalRms;
};

struct APEXSPIN_EXPORT Options
{
    Options();

    bool noiseStops;
    Person personBeforeScreen;
    QString customScreen;
    double timeBeforeFirstStimulus;
    bool reinforcement;
    bool showResults;
    bool autoSaveResults;
    bool exitAfter;
    uint nbResponsesThatCount;
    Order trialOrder;
    QMap<int, double> noiseJumps;
    SoundCard soundCard;
};

class APEXSPIN_EXPORT SpinUserSettings
{
public:
    SpinUserSettings();

    //setters
    void setSubjectName(const QString &subject);
    void setSpeechmaterial(const QString &speechmaterial);
    void setSpeechcategory(const QString &speechcategory);
    void setNoisematerial(const QString &noisematerial);
    void setList(const QString &list);
    void setSpeakerType(SpeakerType type);

    /**
        * Adds levels to a free field speaker setup. Will assert if the speaker
        * type has not been set to FREE_FIELD. If there already exist
        * levels for the given degree, they will be overwritten.
        */
    void addLevels(SpeakerLevels levels, uint degree);

    /**
        * Adds levels to a headphone speaker setup. Will assert if the speaker
        * type has not been set to HEADPHONE. If the there already exist
        * levels for the given speaker (which is LEFT or RIGHT), they will
        * be overwritten.
        */
    void addLevels(SpeakerLevels levels, Headphone::Speaker which);

    void setProcedureType(ProcedureType type);

    /**
        * Sets the adapting material for an adaptive procedure.
        * Will assert of the procedure type has not been set
        * to ADAPTIVE.
        */
    void setAdaptingMaterial(Material material);

    /**
        * Adds a stepsize to a trial of a procedure.
        *
        * @see  Procedure::addStepsize()
        */
    void addStepsize(double stepsize, uint trial = 0);
    void setRepeatFirst(bool repeat);
    void setNoiseStopsBetweenTrials(bool stops);
    void setPersonBeforeScreen(Person person);
    void setCustomScreen(const QString& screen);
    void setTimeBeforeFirstStimulus(double time);
    void setReinforcement(bool reinforcement);
    void setShowResults(bool show);
    void setAutoSaveResults(bool autosave);
    void setExitAfter(bool exit);
    void setNbResponsesThatCount(uint nb);
    void setTrialOrder(Order order);
    void setNoiseJump(int channel, double jump);
    void setLockSpeechlevels(bool lock);
    void setLockNoiselevels(bool lock);
    void setSoundCard(SoundCard value);

    //getters
    const QString& subjectName() const;
    const QString& speechmaterial() const;
    const QString& speechcategory() const;
    const QString& noisematerial() const;
    const QString& list() const;
    const SpeakerType& speakerType() const;
    const SpeakerLevels& speakerLevels(Headphone::Speaker which) const;
    const SpeakerLevels speakerLevels(uint angle) const;
    QList<uint> speakerAngles() const;
    const ProcedureType& procedureType() const;
    const Material& adaptingMaterial() const;
    const QMap<uint, double>& stepsizes() const;
    bool repeatFirst() const;
    bool noiseStopsBetweenTrials() const;
    const Person& personBeforeScreen() const;
    const QString& customScreen() const;
    double timeBeforeFirstStimulus() const;
    bool reinforcement() const;
    bool showResults() const;
    bool autoSaveResults() const;
    bool exitAfter() const;
    uint nbResponsesThatCount() const;
    Order trialOrder() const;
    uint numberOfChannelsInUse() const;
    double noiseJump(int channel) const;

    // [Tom] FIXME: this corresponds to defaultCalibration in spinconfig
    double dBAOfInternalRms() const;
    bool lockSpeechlevels() const;
    bool lockNoiselevels() const;
    SoundCard soundCard() const;

    //FIXME only applicable when levels are locked
    double snr() const;

    /*
        * Returns true if the SNR is defined. The snr is not defined if there
        * is no noise, or if there is a different SNR for each channel
        */
    bool snrDefined() const;

    /*
        * Returns true if there is noise on any channel
        */
    bool hasNoise() const;

    /**
      * Return level of the speech signal in dB.
      * Not defined if different speech levels are presented at each ear with headphones
      * Total level from all speakers for free field
      **/
    double speechLevel() const;

    //extra
    void print() const;

private:
    Info info;
    Materials materials;
    Speakers speakers;
    Procedure procedure;
    Options options;
    Calibration calibration;
};

} //ns data
} //ns spin

#endif
