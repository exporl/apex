/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "spinusersettings.h"

#include "apextools/exceptions.h"

#include <cmath>

using namespace spin::data;

// Options =====================================================================

Options::Options()
    : noiseStops(false),
      personBeforeScreen(INVALID_PERSON),
      timeBeforeFirstStimulus(0),
      reinforcement(true),
      showResults(true),
      autoSaveResults(false),
      nbResponsesThatCount(0),
      trialOrder(ORDER_INVALID),
      soundCard(DefaultSoundcard),
      generatePluginProcedure(true)
{
}

// SpeakerLevels ===============================================================

SpeakerLevels::SpeakerLevels()
    : noise(0), speech(0), hasNoise(false), hasSpeech(false)
{
}

QString SpeakerLevels::toString() const
{
    QString text;

    if (hasSpeech)
        text += QString("speech %1dB, ").arg(speech);
    else
        text += "no speech, ";

    if (hasNoise)
        text += QString("noise %1dB.").arg(noise);
    else
        text += "no noise.";

    return text;
}

bool SpeakerLevels::isInUse() const
{
    return hasNoise || hasSpeech;
}

bool SpeakerLevels::bothEnabled() const
{
    return hasNoise && hasSpeech;
}

// Procedure ===================================================================

Procedure::Procedure()
    : type(INVALID_PROCEDURE), adaptingMaterial(NO_MATERIAL), repeatFirst(true)
{
}

void Procedure::addStepsize(uint trial, double stepsize)
{
    Q_ASSERT(stepsize >= 0);
    Q_ASSERT(type == ADAPTIVE);

    stepsizes.insert(trial, stepsize);
}

// Headphone ===================================================================

uint Headphone::numberOfChannels() const
{
    return leftLevels.isInUse() + rightLevels.isInUse();
}

// FreeField ===================================================================

const SpeakerLevels FreeField::levels(uint degree) const
{
    Q_ASSERT(levelsMap.contains(degree));

    return levelsMap[degree];
}

void FreeField::addLevels(uint degree, SpeakerLevels levels)
{
    levelsMap.insert(degree % 360, levels);
}

QList<uint> FreeField::angles() const
{
    return levelsMap.keys();
}

uint FreeField::numberOfChannelsInUse() const
{
    return levelsMap.size();
}

// SpinUserSettings ============================================================

SpinUserSettings::SpinUserSettings()
{
}

SoundCard SpinUserSettings::soundCard() const
{
    return options.soundCard;
}

void SpinUserSettings::setSoundCard(SoundCard value)
{
    options.soundCard = value;
}

void SpinUserSettings::setGeneratePluginProcedure(bool value)
{
    options.generatePluginProcedure = value;
}

void SpinUserSettings::setSubjectName(const QString &subject)
{
    info.subjectName = subject;
}

void SpinUserSettings::setSpeechmaterial(const QString &speechmaterial)
{
    materials.speechmaterial = speechmaterial;
}

void SpinUserSettings::setSpeechcategory(const QString &speechcategory)
{
    materials.speechcategory = speechcategory;
}

void SpinUserSettings::setNoisematerial(const QString &noisematerial)
{
    materials.noisematerial = noisematerial;
}

void SpinUserSettings::setList(const QString &list)
{
    materials.list = list;
}

void SpinUserSettings::setSpeakerType(SpeakerType type)
{
    Q_ASSERT(type == HEADPHONE || type == FREE_FIELD);

    speakers.type = type;
}

void SpinUserSettings::addLevels(SpeakerLevels levels, uint degree)
{
    Q_ASSERT(speakers.type == FREE_FIELD);

    speakers.freeField.addLevels(degree, levels);
}

void SpinUserSettings::addLevels(SpeakerLevels levels, Headphone::Speaker which)
{
    Q_ASSERT(speakers.type == HEADPHONE);
    Q_ASSERT(which == Headphone::LEFT || which == Headphone::RIGHT);

    if (which == Headphone::LEFT)
        speakers.headphone.leftLevels = levels;
    else
        speakers.headphone.rightLevels = levels;
}

void SpinUserSettings::setProcedureType(ProcedureType type)
{
    Q_ASSERT(type == CONSTANT || type == ADAPTIVE);

    procedure.type = type;
}

void SpinUserSettings::setAdaptingMaterial(Material material)
{
    Q_ASSERT(material == SPEECH || material == NOISE);
    Q_ASSERT(procedure.type == ADAPTIVE);

    procedure.adaptingMaterial = material;
}

void SpinUserSettings::addStepsize(double stepsize, uint trial)
{
    procedure.addStepsize(trial, stepsize);
}

void SpinUserSettings::setRepeatFirst(bool repeat)
{
    Q_ASSERT(procedure.type == ADAPTIVE);

    procedure.repeatFirst = repeat;
}

void SpinUserSettings::setNoiseStopsBetweenTrials(bool stops)
{
    options.noiseStops = stops;
}

void SpinUserSettings::setPersonBeforeScreen(Person person)
{
    Q_ASSERT(person == EXPERIMENTER || person == SUBJECT);

    options.personBeforeScreen = person;
}

void SpinUserSettings::setCustomScreen(const QString &screen)
{
    options.customScreen = screen;
}

void SpinUserSettings::setTimeBeforeFirstStimulus(double time)
{
    Q_ASSERT(time >= 0);

    options.timeBeforeFirstStimulus = time;
}

void SpinUserSettings::setReinforcement(bool reinforcement)
{
    options.reinforcement = reinforcement;
}

void SpinUserSettings::setShowResults(bool show)
{
    options.showResults = show;
}

void SpinUserSettings::setAutoSaveResults(bool autosave)
{
    options.autoSaveResults = autosave;
}

void SpinUserSettings::setExitAfter(bool exit)
{
    options.exitAfter = exit;
}

void SpinUserSettings::setNbResponsesThatCount(uint nb)
{
    options.nbResponsesThatCount = nb;
}

void SpinUserSettings::setTrialOrder(Order order)
{
    Q_ASSERT(order == ORDER_RANDOM || order == ORDER_SEQUENTIAL);

    options.trialOrder = order;
}

void SpinUserSettings::setNoiseJump(int channel, double jump)
{
    if (speakerType() == FREE_FIELD)
        Q_ASSERT(speakerLevels(channel).hasNoise);
    else
        Q_ASSERT(speakerLevels((Headphone::Speaker)channel).hasNoise);

    Q_ASSERT(jump >= RANDOM);

    options.noiseJumps.insert(channel, jump);
}

bool SpinUserSettings::generatePluginProcedure() const
{
    return options.generatePluginProcedure;
}

void SpinUserSettings::setLockSpeechlevels(bool lock)
{
    speakers.lockSpeech = lock;
}

void SpinUserSettings::setLockNoiselevels(bool lock)
{
    speakers.lockNoise = lock;
}

const QString &SpinUserSettings::subjectName() const
{
    return info.subjectName;
}

const QString &SpinUserSettings::speechmaterial() const
{
    return materials.speechmaterial;
}

const QString &SpinUserSettings::speechcategory() const
{
    return materials.speechcategory;
}

const QString &SpinUserSettings::noisematerial() const
{
    return materials.noisematerial;
}

const QString &SpinUserSettings::list() const
{
    return materials.list;
}

const SpeakerType &SpinUserSettings::speakerType() const
{
    return speakers.type;
}

const SpeakerLevels &
SpinUserSettings::speakerLevels(Headphone::Speaker which) const
{
    Q_ASSERT(speakers.type == HEADPHONE);
    Q_ASSERT(which == Headphone::LEFT || which == Headphone::RIGHT);

    if (which == Headphone::LEFT)
        return speakers.headphone.leftLevels;
    else
        return speakers.headphone.rightLevels;
}

const SpeakerLevels SpinUserSettings::speakerLevels(uint angle) const
{
    Q_ASSERT(speakers.type == FREE_FIELD);

    return speakers.freeField.levels(angle);
}

QList<uint> SpinUserSettings::speakerAngles() const
{
    Q_ASSERT(speakerType() == FREE_FIELD);

    return speakers.freeField.angles();
}

const ProcedureType &SpinUserSettings::procedureType() const
{
    return procedure.type;
}

const Material &SpinUserSettings::adaptingMaterial() const
{
    Q_ASSERT(procedure.type == ADAPTIVE);

    return procedure.adaptingMaterial;
}

const QMap<uint, double> &SpinUserSettings::stepsizes() const
{
    Q_ASSERT(procedure.type == ADAPTIVE);

    return procedure.stepsizes;
}

bool SpinUserSettings::repeatFirst() const
{
    Q_ASSERT(procedure.type == ADAPTIVE);

    return procedure.repeatFirst;
}

bool SpinUserSettings::noiseStopsBetweenTrials() const
{
    return options.noiseStops;
}

const Person &SpinUserSettings::personBeforeScreen() const
{
    return options.personBeforeScreen;
}

const QString &SpinUserSettings::customScreen() const
{
    return options.customScreen;
}

double SpinUserSettings::timeBeforeFirstStimulus() const
{
    return options.timeBeforeFirstStimulus;
}

bool SpinUserSettings::reinforcement() const
{
    return options.reinforcement;
}

bool SpinUserSettings::showResults() const
{
    return options.showResults;
}

bool SpinUserSettings::autoSaveResults() const
{
    return options.autoSaveResults;
}

bool SpinUserSettings::exitAfter() const
{
    return options.exitAfter;
}

uint SpinUserSettings::nbResponsesThatCount() const
{
    return options.nbResponsesThatCount;
}

Order SpinUserSettings::trialOrder() const
{
    return options.trialOrder;
}

uint SpinUserSettings::numberOfChannelsInUse() const
{
    if (speakers.type == HEADPHONE)
        return speakers.headphone.numberOfChannels();
    if (speakers.type == FREE_FIELD)
        return speakers.freeField.numberOfChannelsInUse();

    throw ApexStringException(tr("unknown speaker type"));

    // make the compiler happy
    return 0;
}

double SpinUserSettings::noiseJump(int channel) const
{
    Q_ASSERT((speakerType() == FREE_FIELD && speakerLevels(channel).hasNoise) ||
             (speakerType() != FREE_FIELD &&
              speakerLevels((Headphone::Speaker)channel).hasNoise));

    return options.noiseJumps[channel];
}

double SpinUserSettings::dBAOfInternalRms() const
{
    // return calibration.dBAOfInternalRms();TODO
    return 60.0;
}

double SpinUserSettings::snr() const
{
    SpeakerLevels levels;

    if (speakerType() == data::HEADPHONE) {
        // look for a speaker with levels
        SpeakerLevels levelsL = speakerLevels(Headphone::LEFT);
        SpeakerLevels levelsR = speakerLevels(Headphone::RIGHT);

        if (levelsL.bothEnabled() && levelsR.bothEnabled()) {
            if (levelsL.noise != levelsR.noise)
                throw ApexStringException(tr("Invalid noise configuration"));

            if (levelsL.speech != levelsR.speech)
                throw ApexStringException(tr("Invalid speech configuration"));
        }

        if (levelsL.bothEnabled())
            return levelsL.speech - levelsL.noise;
        if (levelsR.bothEnabled())
            return levelsR.speech - levelsR.noise;
        if (levelsL.hasSpeech & levelsR.hasNoise & !levelsL.hasNoise)
            return levelsL.speech - levelsR.noise;
        if (levelsR.hasSpeech & levelsL.hasNoise & !levelsR.hasNoise)
            return levelsR.speech - levelsL.noise;

        throw ApexStringException(
            tr("called without a suitable channel with both speech and noise"));
        return 0; // keep compiler happy

    } else { // free field
        double speechLevelLin = 0;
        int speechCount = 0;
        double noiseLevelLin = 0;
        int noiseCount = 0;

        // look for a speaker with levels
        Q_FOREACH (uint angle, speakerAngles()) {
            levels = speakerLevels(angle);

            if (levels.hasSpeech) {
                speechCount++;
                speechLevelLin += pow(10, levels.speech / 10);
            }

            if (levels.hasNoise) {
                noiseCount++;
                noiseLevelLin += pow(10, levels.noise / 10);
            }
        }

        if (procedureType() == ADAPTIVE) {
            if (adaptingMaterial() == data::SPEECH && speechCount != 1)
                throw ApexStringException(tr("SNR not defined"));
            if (adaptingMaterial() == data::NOISE && noiseCount != 1)
                throw ApexStringException(tr("SNR not defined"));
        }

        return 10 * log10(speechLevelLin / noiseLevelLin);
    }
}

double SpinUserSettings::speechLevel() const
{
    SpeakerLevels levels;

    if (speakerType() == data::HEADPHONE) {
        // look for a speaker with levels
        SpeakerLevels levelsL = speakerLevels(Headphone::LEFT);
        SpeakerLevels levelsR = speakerLevels(Headphone::RIGHT);

        if (levelsL.hasSpeech && levelsR.hasSpeech &&
            (levelsL.speech != levelsR.speech)) {
            throw ApexStringException(tr("Invalid speech configuration"));
        }

        if (levelsL.hasSpeech)
            return levelsL.speech;
        if (levelsR.hasSpeech)
            return levelsR.speech;

        throw ApexStringException(
            tr("called without a suitable channel with both speech and noise"));
        return 0; // keep compiler happy

    } else { // free field
        double speechLevelLin = 0;
        int speechCount = 0;

        // look for a speaker with levels
        Q_FOREACH (uint angle, speakerAngles()) {
            levels = speakerLevels(angle);

            if (levels.hasSpeech) {
                speechCount++;
                speechLevelLin += pow(10, levels.speech / 10);
            }
        }

        return 10 * log10(speechLevelLin);
    }
}

bool SpinUserSettings::snrDefined() const
{
    SpeakerLevels levels;

    if (speakerType() == data::HEADPHONE) {
        // look for a speaker with levels
        SpeakerLevels levelsL = speakerLevels(Headphone::LEFT);
        SpeakerLevels levelsR = speakerLevels(Headphone::RIGHT);

        if (levelsR.hasSpeech && levelsR.hasNoise && levelsL.hasSpeech &&
            levelsL.hasNoise) {
            if ((levelsL.speech - levelsL.noise) !=
                (levelsR.speech - levelsR.noise))
                return false;
            else
                return true;
        }

        if ((levelsR.hasSpeech && levelsR.hasNoise) ||
            (levelsL.hasSpeech && levelsL.hasNoise))
            return true;

        return false;

    } else { // free field
        bool foundLevels = false;
        double currentSNR = -120;

        // look for a speaker with levels
        Q_FOREACH (uint angle, speakerAngles()) {
            levels = speakerLevels(angle);

            if (levels.hasSpeech && levels.hasNoise) {
                double newSNR = levels.speech - levels.noise;
                if (!foundLevels) {
                    foundLevels = true;
                    currentSNR = newSNR;
                } else {
                    if (newSNR != currentSNR)
                        return false;
                }
            }
        }

        if (foundLevels)
            return true;

        return false;
    }
}

bool SpinUserSettings::hasNoise() const
{
    if (speakerType() == data::HEADPHONE) {
        // look for a speaker with levels
        SpeakerLevels levelsL = speakerLevels(Headphone::LEFT);
        SpeakerLevels levelsR = speakerLevels(Headphone::RIGHT);

        if (levelsL.hasNoise || levelsR.hasNoise)
            return true;
    } else { // free field
        // look for a speaker with levels
        Q_FOREACH (uint angle, speakerAngles()) {
            SpeakerLevels levels = speakerLevels(angle);

            if (levels.hasNoise)
                return true;
        }
    }
    return false;
}

bool SpinUserSettings::lockSpeechlevels() const
{
    return speakers.lockSpeech;
}

bool SpinUserSettings::lockNoiselevels() const
{
    return speakers.lockNoise;
}

// extra
void SpinUserSettings::print() const
{
    QString text("settings:\n");
    text += QString("subject: %1\n").arg(subjectName());
    text += QString("speechmaterial: %1\n").arg(speechmaterial());
    text += QString("speechcategory: %1\n").arg(speechcategory());
    text += QString("noisematerial: %1\n").arg(noisematerial());
    text += QString("list: %1\n").arg(list());
    text += QString("speaker type: %1\n")
                .arg(speakerType() == HEADPHONE ? "headphone" : "free field");

    if (speakerType() == HEADPHONE) {
        text += QString("\tleft: %1\n")
                    .arg(speakerLevels(Headphone::LEFT).toString());
        text += QString("\tright: %1\n")
                    .arg(speakerLevels(Headphone::RIGHT).toString());
    } else {
        QList<uint> angles = speakers.freeField.angles();

        QList<uint>::const_iterator it;
        for (it = angles.begin(); it != angles.end(); it++) {
            text += QString("\t%1: %2\n")
                        .arg(*it)
                        .arg(speakers.freeField.levels(*it).toString());
        }
    }

    text += QString("procedure type: %1\n")
                .arg(procedureType() == CONSTANT ? "constant" : "adaptive");

    if (procedureType() == ADAPTIVE) {
        text += QString("adapting material: %1\n")
                    .arg(adaptingMaterial() == SPEECH ? "speech" : "noise");

        QMap<uint, double> steps = stepsizes();
        text += "stepsizes:\n";

        QMap<uint, double>::const_iterator it;
        for (it = steps.begin(); it != steps.end(); it++) {
            text += QString("\ttrial %1: %2dB\n").arg(it.key()).arg(it.value());
        }
    }

    qCDebug(APEX_RS, "%s", qPrintable(text));
}
