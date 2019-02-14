#include "soundcardsettings.h"
#include "apextools/settingsfactory.h"
#include <QDebug>
#include <QScopedPointer>
#include <QSettings>

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

void SoundcardSettings::saveDevice(const QString &hostApi,
                                   const QString &device)
{
    getSettings()->setValue("soundcardSettings/hostApi", hostApi);
    getSettings()->setValue("soundcardSettings/deviceId", device);
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

QSettings *SoundcardSettings::getSettings()
{
    static QScopedPointer<QSettings> settings(
        SettingsFactory().createSettings());
    return settings.data();
}

} // ns apex
