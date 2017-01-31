#include "soundcardsettings.h"
#include <QSettings>
#include <QDebug>
#include <QScopedPointer>


namespace apex
{

SoundcardSettings::SoundcardSettings()
{
    getSettings();
}

QString SoundcardSettings::device()
{
    return getSettings()->value("soundcardSettings/deviceId").toString();
}

void SoundcardSettings::saveDevice(const QString &hostApi, const QString &device)
{
    getSettings()->setValue("soundcardSettings/hostApi",hostApi);
    getSettings()->setValue("soundcardSettings/deviceId",device);
}

void SoundcardSettings::removeSavedSettings()
{
    getSettings()->remove("soundcardSettings/deviceId");
    getSettings()->remove("soundcardSettings/hostApi");
}

QString SoundcardSettings::hostApi()
{
    return getSettings()->value("soundcardSettings/hostApi").toString();
}

bool SoundcardSettings::hasData()
{
    return getSettings()->contains("soundcardSettings/deviceId");
}

QSettings * SoundcardSettings::getSettings()
{
    static QScopedPointer<QSettings> settings(new QSettings(apex::organizationName,apex::applicationName));
    return settings.data();
}


} // ns apex
