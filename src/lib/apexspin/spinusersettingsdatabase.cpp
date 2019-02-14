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

#include "apextools/settingsfactory.h"

#include "apextools/exceptions.h"

#include "spinusersettingsdatabase.h"

#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QStringList>

namespace spin
{
namespace data
{

class DatabasePrivate
{
public:
    enum Section { NO_SECTION, INFO, MATERIALS, SPEAKERS, PROCEDURE, OPTIONS };

    static QString prefix()
    {
        return "SPIN";
    }

    static QString defaults()
    {
        return prefix() + "/default";
    }

    static QString previous()
    {
        return prefix() + "/previous";
    }

    static QString settings(const QString &name, Section section = NO_SECTION)
    {
        QString path(prefix() + "/" + name);
        QString sectionName;

        switch (section) {
        case NO_SECTION:
            return path;
        case INFO:
            sectionName = "info";
            break;
        case MATERIALS:
            sectionName = "materials";
            break;
        case SPEAKERS:
            sectionName = "speakers";
            break;
        case PROCEDURE:
            sectionName = "procedure";
            break;
        case OPTIONS:
            sectionName = "options";
            break;
        default:
            qFatal("non-existing section requested.");
        }

        return path + "/" + sectionName;
    }

    static QString speakerSection(Headphone::Speaker which)
    {
        Q_ASSERT(which == Headphone::LEFT || which == Headphone::RIGHT);

        return which == Headphone::LEFT ? LEFT_STR : RIGHT_STR;
    }

    static Headphone::Speaker speakerSection(QString which)
    {
        Q_ASSERT(which == LEFT_STR || which == RIGHT_STR);

        return which == LEFT_STR ? Headphone::LEFT : Headphone::RIGHT;
    }

    static QString speakerSection(uint angle)
    {
        return QString::number(angle % 360);
    }

    static QString stepsizesSection()
    {
        return "stepsizes";
    }

    // parameters
    static QString name()
    {
        return "name";
    }

    static QString date()
    {
        return "date";
    }

    static QString speechmaterial()
    {
        return "speechmaterial";
    }

    static QString speechcategory()
    {
        return "speechcategory";
    }

    static QString noisematerial()
    {
        return "noisematerial";
    }

    static QString list()
    {
        return "list";
    }

    static QString type()
    {
        return "type";
    }

    static QString lockSpeech()
    {
        return "lockSpeech";
    }

    static QString lockNoise()
    {
        return "lockNoise";
    }

    static QString speechLevel()
    {
        return "speech";
    }

    static QString noiseLevel()
    {
        return "noise";
    }

    static QString adaptingMaterial()
    {
        return "adaptingMaterial";
    }

    static QString repeatUntilCorrect()
    {
        return "repeatUntilCorrect";
    }

    static QString noiseStopsBetweenTrials()
    {
        return "noiseStopsBetweenTrials";
    }

    static QString personBeforeScreen()
    {
        return "personBeforeScreen";
    }

    static QString customScreen()
    {
        return "customScreen";
    }

    static QString timeBeforeFirstStimulus()
    {
        return "timeBeforeFirstStimulus";
    }

    static QString reinforcement()
    {
        return "reinforcement";
    }

    static QString showResults()
    {
        return "showResults";
    }

    static QString autoSaveResults()
    {
        return "autoSaveResults";
    }

    static QString exitAfter()
    {
        return "exitAfter";
    }

    static QString nbResponsesThatCount()
    {
        return "nbResponsesThatCount";
    }

    static QString trialOrder()
    {
        return "trialOrder";
    }

    static QString lastSavePath()
    {
        return "lastSavePath";
    }

    static QString soundCard()
    {
        return "soundCard";
    }

    // value conversions
    static QString speakerType(SpeakerType type)
    {
        Q_ASSERT(type == HEADPHONE || type == FREE_FIELD);

        return type == HEADPHONE ? HEADPHONE_STR : FREE_FIELD_STR;
    }

    static SpeakerType speakerType(QString type)
    {
        Q_ASSERT(type == HEADPHONE_STR || type == FREE_FIELD_STR);

        return type == HEADPHONE_STR ? HEADPHONE : FREE_FIELD;
    }

    static QString procedureType(ProcedureType type)
    {
        Q_ASSERT(type == CONSTANT || type == ADAPTIVE || type == ADAPTIVE_BK);
        if (type == CONSTANT)
            return CONSTANT_STR;
        else if (type == ADAPTIVE)
            return ADAPTIVE_STR;
        else if (type == ADAPTIVE_BK)
            return ADAPTIVE_BK_STR;
        else
            return 0;
    }

    static ProcedureType procedureType(QString type)
    {
        Q_ASSERT(type == CONSTANT_STR || type == ADAPTIVE_STR ||
                 type == ADAPTIVE_BK_STR);
        if (type == CONSTANT_STR)
            return CONSTANT;
        else if (type == ADAPTIVE_STR)
            return ADAPTIVE;
        else
            return ADAPTIVE_BK;
    }

    static QString repeatUntilCorrect(bool value)
    {
        return value ? "true" : "false";
    }

    static QString material(Material material)
    {
        Q_ASSERT(material == SPEECH || material == NOISE);

        return material == SPEECH ? SPEECH_STR : NOISE_STR;
    }

    static Material material(QString material)
    {
        Q_ASSERT(material == SPEECH_STR || material == NOISE_STR);

        return material == SPEECH_STR ? SPEECH : NOISE;
    }

    static QString personBeforeScreen(Person person)
    {
        Q_ASSERT(person == EXPERIMENTER || person == SUBJECT);

        return person == EXPERIMENTER ? EXPERIMENTER_STR : SUBJECT_STR;
    }

    static Person personBeforeScreen(QString person)
    {
        Q_ASSERT(person == EXPERIMENTER_STR || person == SUBJECT_STR);

        return person == EXPERIMENTER_STR ? EXPERIMENTER : SUBJECT;
    }

    static QString trialOrder(Order order)
    {
        Q_ASSERT(order == ORDER_RANDOM || order == ORDER_SEQUENTIAL);

        return order == ORDER_RANDOM ? RANDOM_STR : SEQUENTIAL_STR;
    }

    static Order trialOrder(QString order)
    {
        Q_ASSERT(order == RANDOM_STR || order == SEQUENTIAL_STR);

        return order == RANDOM_STR ? ORDER_RANDOM : ORDER_SEQUENTIAL;
    }

    static SoundCard soundCard(const QString &soundCard)
    {
        SoundCard out = SoundCard(0);
        if (soundCard == RMEMULTIFACE_STR)
            out = RmeMultiface;
        else if (soundCard == RMEFIREFACEUC_STR)
            out = RmeFirefaceUc;
        else if (soundCard == LYNXONE_STR)
            out = LynxOne;
        else if (soundCard == DEFAULTSOUNDCARD_STR)
            out = DefaultSoundcard;
        else
            qFatal("Unknown soundcard");
        return out;
    }

    static QString soundCard(SoundCard soundCard)
    {
        QString out;
        if (soundCard == RmeMultiface)
            out = RMEMULTIFACE_STR;
        else if (soundCard == RmeFirefaceUc)
            out = RMEFIREFACEUC_STR;
        else if (soundCard == LynxOne)
            out = LYNXONE_STR;
        else if (soundCard == DefaultSoundcard)
            out = DEFAULTSOUNDCARD_STR;
        else
            qFatal("Unknown soundcard");
        return out;
    }

private:
    static const QString HEADPHONE_STR;
    static const QString FREE_FIELD_STR;
    static const QString CONSTANT_STR;
    static const QString ADAPTIVE_STR;
    static const QString ADAPTIVE_BK_STR;
    static const QString SPEECH_STR;
    static const QString NOISE_STR;
    static const QString LEFT_STR;
    static const QString RIGHT_STR;
    static const QString EXPERIMENTER_STR;
    static const QString SUBJECT_STR;
    static const QString RANDOM_STR;
    static const QString SEQUENTIAL_STR;
    static const QString DEFAULTSOUNDCARD_STR;
    static const QString RMEMULTIFACE_STR;
    static const QString RMEFIREFACEUC_STR;
    static const QString LYNXONE_STR;
};

const QString DatabasePrivate::HEADPHONE_STR("headphone");
const QString DatabasePrivate::FREE_FIELD_STR("freeField");
const QString DatabasePrivate::CONSTANT_STR("constant");
const QString DatabasePrivate::ADAPTIVE_STR("adaptive");
const QString DatabasePrivate::ADAPTIVE_BK_STR("adaptive_bk");
const QString DatabasePrivate::SPEECH_STR("speech");
const QString DatabasePrivate::NOISE_STR("noise");
const QString DatabasePrivate::LEFT_STR("left");
const QString DatabasePrivate::RIGHT_STR("right");
const QString DatabasePrivate::EXPERIMENTER_STR("experimenter");
const QString DatabasePrivate::SUBJECT_STR("subject");
const QString DatabasePrivate::RANDOM_STR("random");
const QString DatabasePrivate::SEQUENTIAL_STR("sequential");
const QString DatabasePrivate::DEFAULTSOUNDCARD_STR("defaultsoundcard");
const QString DatabasePrivate::RMEMULTIFACE_STR("rmemultiface");
const QString DatabasePrivate::RMEFIREFACEUC_STR("rmefirefaceuc");
const QString DatabasePrivate::LYNXONE_STR("lynxone");

const QString SpinUserSettingsDatabase::DEFAULT("default");
const QString SpinUserSettingsDatabase::PREVIOUS("previous");

SpinUserSettingsDatabase::SpinUserSettingsDatabase()
{
}

void SpinUserSettingsDatabase::save(const QString &name,
                                    const SpinUserSettings &settings)
{
    QScopedPointer<QSettings> db(apex::SettingsFactory().createSettings());

    // remove previous settings with this name
    db->beginGroup(DatabasePrivate::settings(name));

    Q_FOREACH (const QString &key, db->allKeys())
        db->remove(key);

    db->endGroup();

    // info
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::INFO));
    db->setValue(DatabasePrivate::name(), settings.subjectName());
    db->endGroup();

    // materials
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::MATERIALS));
    db->setValue(DatabasePrivate::speechmaterial(), settings.speechmaterial());
    db->setValue(DatabasePrivate::speechcategory(), settings.speechcategory());
    db->setValue(DatabasePrivate::noisematerial(), settings.noisematerial());
    db->setValue(DatabasePrivate::list(), settings.list());
    db->endGroup();

    // speakers
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::SPEAKERS));

    db->setValue(DatabasePrivate::lockSpeech(), settings.lockSpeechlevels());
    db->setValue(DatabasePrivate::lockNoise(), settings.lockNoiselevels());

    SpeakerLevels levels;
    // TODO save noise jump
    if (settings.speakerType() == HEADPHONE) {
        db->setValue(DatabasePrivate::type(),
                     DatabasePrivate::speakerType(HEADPHONE));

        Headphone::Speaker speaker = Headphone::LEFT;

        for (int i = 0; i < 2; i++) // do twice: each speaker
        {
            db->beginGroup(DatabasePrivate::speakerSection(speaker));
            levels = settings.speakerLevels(speaker);

            if (levels.hasSpeech)
                db->setValue(DatabasePrivate::speechLevel(), levels.speech);
            if (levels.hasNoise)
                db->setValue(DatabasePrivate::noiseLevel(), levels.noise);

            speaker = Headphone::RIGHT;
            db->endGroup();
        }
    } else {
        db->setValue(DatabasePrivate::type(),
                     DatabasePrivate::speakerType(FREE_FIELD));

        QList<uint> angles = settings.speakerAngles();

        QList<uint>::const_iterator it;
        for (it = angles.begin(); it != angles.end(); it++) {
            db->beginGroup(DatabasePrivate::speakerSection(*it));
            levels = settings.speakerLevels(*it);

            if (levels.hasSpeech)
                db->setValue(DatabasePrivate::speechLevel(), levels.speech);
            if (levels.hasNoise)
                db->setValue(DatabasePrivate::noiseLevel(), levels.noise);

            db->endGroup();
        }
    }

    db->endGroup();

    // procedure
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::PROCEDURE));

    if (settings.procedureType() == CONSTANT) {
        db->setValue(DatabasePrivate::type(),
                     DatabasePrivate::procedureType(CONSTANT));
    } else if (settings.procedureType() == ADAPTIVE) {
        db->setValue(DatabasePrivate::type(),
                     DatabasePrivate::procedureType(ADAPTIVE));
        db->setValue(DatabasePrivate::adaptingMaterial(),
                     DatabasePrivate::material(settings.adaptingMaterial()));

        db->beginGroup(DatabasePrivate::stepsizesSection());
        QMap<uint, double> stepsizes = settings.stepsizes();

        QMap<uint, double>::const_iterator it;
        for (it = stepsizes.begin(); it != stepsizes.end(); it++)
            db->setValue(QString::number(it.key()), it.value());

        db->endGroup();
        db->setValue(DatabasePrivate::repeatUntilCorrect(),
                     settings.repeatFirst());
    } else if (settings.procedureType() == ADAPTIVE_BK) {
        db->setValue(DatabasePrivate::type(),
                     DatabasePrivate::procedureType(ADAPTIVE_BK));
        db->setValue(DatabasePrivate::adaptingMaterial(),
                     DatabasePrivate::material(settings.adaptingMaterial()));
        db->setValue(DatabasePrivate::repeatUntilCorrect(),
                     settings.repeatFirst());
    }
    db->endGroup();

    // options
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::OPTIONS));
    db->setValue(DatabasePrivate::noiseStopsBetweenTrials(),
                 settings.noiseStopsBetweenTrials());

    if (settings.customScreen().isEmpty()) {
        db->setValue(
            DatabasePrivate::personBeforeScreen(),
            DatabasePrivate::personBeforeScreen(settings.personBeforeScreen()));

        if (db->contains(DatabasePrivate::customScreen()))
            db->remove(DatabasePrivate::customScreen());
    } else {
        db->setValue(DatabasePrivate::customScreen(), settings.customScreen());

        if (db->contains(DatabasePrivate::personBeforeScreen()))
            db->remove(DatabasePrivate::personBeforeScreen());
    }

    db->setValue(DatabasePrivate::timeBeforeFirstStimulus(),
                 settings.timeBeforeFirstStimulus());
    db->setValue(DatabasePrivate::reinforcement(), settings.reinforcement());
    db->setValue(DatabasePrivate::showResults(), settings.showResults());
    db->setValue(DatabasePrivate::autoSaveResults(),
                 settings.autoSaveResults());
    db->setValue(DatabasePrivate::exitAfter(), settings.exitAfter());
    db->setValue(DatabasePrivate::nbResponsesThatCount(),
                 settings.nbResponsesThatCount());
    db->setValue(DatabasePrivate::trialOrder(),
                 DatabasePrivate::trialOrder(settings.trialOrder()));
    db->setValue(DatabasePrivate::soundCard(),
                 DatabasePrivate::soundCard(settings.soundCard()));
    db->endGroup();
}

bool SpinUserSettingsDatabase::remove(const QString &name)
{
    if (!savedSettings().contains(name))
        return false;

    QScopedPointer<QSettings> db(apex::SettingsFactory().createSettings());
    db->beginGroup(DatabasePrivate::prefix());

    db->remove(name);
    return true;
}

bool SpinUserSettingsDatabase::rename(const QString &oldName,
                                      const QString &newName)
{
    if (!savedSettings().contains(oldName))
        return false;

    save(newName, load(oldName));
    remove(oldName);
    return true;
}

SpinUserSettings SpinUserSettingsDatabase::load(const QString &name)
{
    if (!savedSettings().contains(name))
        throw(ApexStringException("No saved setting in SPIN with the name " +
                                  name));

    SpinUserSettings settings;
    QScopedPointer<QSettings> db(apex::SettingsFactory().createSettings());

    // info
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::INFO));
    QString subjectName = db->value(DatabasePrivate::name()).toString();
    settings.setSubjectName(subjectName);
    db->endGroup();

// materials
// we don't want to write toString() all the time:)
#define value(str) value(str).toString()
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::MATERIALS));
    settings.setSpeechmaterial(db->value(DatabasePrivate::speechmaterial()));
    settings.setSpeechcategory(db->value(DatabasePrivate::speechcategory()));
    settings.setNoisematerial(db->value(DatabasePrivate::noisematerial()));
    settings.setList(db->value(DatabasePrivate::list()));
    db->endGroup();
#undef value

    // speakers
    // TODO load noise jump
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::SPEAKERS));

    settings.setLockSpeechlevels(
        db->value(DatabasePrivate::lockSpeech()).toBool());
    settings.setLockNoiselevels(
        db->value(DatabasePrivate::lockNoise()).toBool());

    settings.setSpeakerType(DatabasePrivate::speakerType(
        db->value(DatabasePrivate::type()).toString()));

    Q_FOREACH (QString speaker, db->childGroups()) {
        db->beginGroup(speaker);
        SpeakerLevels levels;

        Q_FOREACH (QString key, db->childKeys()) {
            double level = db->value(key).toDouble();

            if (key == DatabasePrivate::noiseLevel()) {
                levels.noise = level;
                levels.hasNoise = true;
            } else {
                levels.speech = level;
                levels.hasSpeech = true;
            }
        }

        if (settings.speakerType() == HEADPHONE)
            settings.addLevels(levels,
                               DatabasePrivate::speakerSection(speaker));
        else {
            bool ok;
            settings.addLevels(levels, speaker.toUInt(&ok));
            Q_ASSERT(ok);
        }

        db->endGroup();
    }

    db->endGroup();

    // procedure
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::PROCEDURE));

    settings.setProcedureType(DatabasePrivate::procedureType(
        db->value(DatabasePrivate::type()).toString()));

    if (settings.procedureType() == ADAPTIVE ||
        settings.procedureType() == ADAPTIVE_BK) {
        settings.setAdaptingMaterial(DatabasePrivate::material(
            db->value(DatabasePrivate::adaptingMaterial()).toString()));
        settings.setRepeatFirst(
            db->value(DatabasePrivate::repeatUntilCorrect()).toBool());
    }
    if (settings.procedureType() == ADAPTIVE) {
        db->beginGroup(DatabasePrivate::stepsizesSection());

        Q_FOREACH (QString trial, db->childKeys()) {
            bool iOk, dOk;
            settings.addStepsize(db->value(trial).toDouble(&dOk),
                                 trial.toUInt(&iOk));

            Q_ASSERT(iOk && dOk);
        }

        db->endGroup();
    }

    db->endGroup();
    db->beginGroup(DatabasePrivate::settings(name, DatabasePrivate::OPTIONS));
    settings.setNoiseStopsBetweenTrials(
        db->value(DatabasePrivate::noiseStopsBetweenTrials()).toBool());

    if (db->contains(DatabasePrivate::personBeforeScreen())) {
        settings.setPersonBeforeScreen(DatabasePrivate::personBeforeScreen(
            db->value(DatabasePrivate::personBeforeScreen()).toString()));
    } else {
        settings.setCustomScreen(
            db->value(DatabasePrivate::customScreen()).toString());
    }

    settings.setTimeBeforeFirstStimulus(
        db->value(DatabasePrivate::timeBeforeFirstStimulus()).toDouble());
    settings.setReinforcement(
        db->value(DatabasePrivate::reinforcement()).toBool());
    settings.setShowResults(db->value(DatabasePrivate::showResults()).toBool());
    settings.setAutoSaveResults(
        db->value(DatabasePrivate::autoSaveResults()).toBool());
    settings.setExitAfter(db->value(DatabasePrivate::exitAfter()).toBool());
    settings.setNbResponsesThatCount(
        db->value(DatabasePrivate::nbResponsesThatCount()).toUInt());
    settings.setTrialOrder(DatabasePrivate::trialOrder(
        db->value(DatabasePrivate::trialOrder()).toString()));
    settings.setSoundCard(DatabasePrivate::soundCard(
        db->value(DatabasePrivate::soundCard()).toString()));
    db->endGroup();

    return settings;
}

void SpinUserSettingsDatabase::setLastSavePath(const QString &path)
{
    QScopedPointer<QSettings> db(apex::SettingsFactory().createSettings());
    db->beginGroup(DatabasePrivate::prefix());
    db->setValue(DatabasePrivate::lastSavePath(), path);
    db->endGroup();
}

QString SpinUserSettingsDatabase::lastSavePath()
{
    QScopedPointer<QSettings> db(apex::SettingsFactory().createSettings());
    db->beginGroup(DatabasePrivate::prefix());
    QString dir = db->value(DatabasePrivate::lastSavePath()).toString();
    db->endGroup();

    if (dir.isEmpty())
        return QDir::homePath();

    return dir;
}

QStringList SpinUserSettingsDatabase::savedSettings()
{
    QScopedPointer<QSettings> db(apex::SettingsFactory().createSettings());
    db->beginGroup(DatabasePrivate::prefix());
    return db->childGroups();
}

} // ns data
} // ns spin
