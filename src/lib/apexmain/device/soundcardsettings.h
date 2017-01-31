#ifndef SOUNDCARDSETTINGS_H
#define SOUNDCARDSETTINGS_H

#include "apextools/global.h"
#include <QSettings>

namespace apex{

/**
 * @brief The SoundcardSettings class stores a portaudio device and hostapi name
 * Note that strings are stored because DeviceIDs could change when hardware is changed
 * */
class APEX_EXPORT SoundcardSettings
{
public:
    SoundcardSettings();

    void saveDevice( const QString& hostApi, const QString& device);
    void removeSavedSettings ();

    QString device();
    QString hostApi();

    /**
     * @brief hasData
     * @return true if data has been stored
     */
    bool hasData();

private:
    QSettings * getSettings();
};


} // ns apex

#endif // SOUNDCARDSETTINGS_H
