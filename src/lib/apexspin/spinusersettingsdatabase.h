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

#ifndef _APEX_SRC_LIB_APEXSPIN_SPINUSERSETTINGSDATABASE_H_
#define _APEX_SRC_LIB_APEXSPIN_SPINUSERSETTINGSDATABASE_H_

#include "spinusersettings.h"

namespace spin
{
namespace data
{

class SpinUserSettingsDatabase
{
    public:
        SpinUserSettingsDatabase();
        ~SpinUserSettingsDatabase(){}

        /**
         * Saves user settings to disk.
         *
         * @param name  The name to save the settings under.
         * @param settings The settings to save.
         */
        static void save(const QString& name, const SpinUserSettings& settings);

        /**
         * Load settings from disk.
         *
         * @param name The name of the settings to load.
         *
         * @note This method will given an assertion error if the given name
         *   is not the name of saved settings so only give a string
         *   returned by savedSettings() as parameter.
         */
        static SpinUserSettings load(const QString& name);

        /*
         * Remove settings from database
         */
        static bool remove(const QString& name);
        static bool rename(const QString& oldName, const QString& newName);

        /**
         * Sets the path where the user saved a file the last time.
         */
        static void setLastSavePath(const QString& path);

        /**
         * Returns the path where the user saved a file the last time.
         * If this path has not been set yet, the users home directory will
         * be returned.
         */
        static QString lastSavePath();

        /**
         * Returns a list with the names of all saved settings.
         */
        static QStringList savedSettings();

        /**
         * The name under which the default settings will be saved.
         */
        static const QString DEFAULT;

        /**
         * The name under which the previous settings will be saved.
         */
        static const QString PREVIOUS;
};
}//ns data
}//ns spin

#endif






















