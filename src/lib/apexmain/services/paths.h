/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef __PATHSSERVICE_H__
#define __PATHSSERVICE_H__

#include <QString>
#include <QDir>
#include <QCoreApplication>

#include "services/servicemanager.h"
#include "global.h"

namespace apex
{

    /**
      * Paths
      *   contains all the paths for use at runtime.
      *   Must be configured as soon as possible when the application starts.


          TODO parse everything except config from mainconfig and set it here.

      *********************************************************************** */
    class APEX_EXPORT Paths : public Service<Paths>
    {
        Q_DECLARE_TR_FUNCTIONS ( Paths )

        public:
        Paths();

        const char* Name()
        {
            return "Paths";
        }

      /**
        * Check if we have at least the schema and config files.
        * @return true if ok
        */
    bool mf_bIsValid() const;

      /**
        * Get the path to directory containing the executable.
        * @return text string reference
        */
    const QString GetExecutablePath() const;


      /**
        * Get the base path containing /config, /tests etc.
       * Linux: /usr or /usr/local
       * Windows: c:\program files\apex or ...
        * @return text string reference
        */
    const QString& GetBasePath() const;


      /**
        * Get the path to the mru.ini
        * @return text string reference
        */
    const QString GetMruIniPath() const;

      /**
        * Get the path to the mainconfig.xml
        * @return text string reference
        */
    /*const QString mf_sMainXmlPath() const
    { return mf_sBasePath() + sm_sConfigDir + sm_sConfigXml; }*/

      /**
        * Get the path to the mainconfig.xsd
        * @return text string reference
        */
    /*const QString mf_sMainXsdPath() const
    { return mf_sBasePath() + sm_sConfigDir + sm_sConfigXsd; }*/

      /**
        * Get the path to the tests directory
        * @return text string reference
        */
    /*const QString mf_sTestsPath() const
    { return mf_sBasePath() + sm_sTestDir; }*/

      /**
        * Get the path to the calibration directory
        * @return text string reference
        */
    /*const QString mf_sCalibrationPath() const
    { return mf_sBasePath() + sm_sConfigDir + sm_sCalibDir; }*/

      /**
        * Get the path to the data directory
        * @return text string reference
        */
    const QString GetDataPath() const;


      /**
        * Get the path to the LabExpORL logo
        * @return text string reference
        */
    const QString GetMainLogoPath() const;


    /** Find a file with a certain number of possible prefixes and extensions in
     * different directories. If no existing readable file is found, an empty
     * string is returned. The current directory as well as the empty prefix and
     * extension are always included.
     *
     * @param baseName base file name that you want to look for
     * @param directories directories to search
     * @param extensions possible extensions of the file, the dot before the
     * extension has to be included, e.g., ".js"
     * @param prefixes possible prefixes to try
     */
    static QString findReadableFile (const QString &baseName,
            const QStringList &directories, const QStringList &extensions = QStringList(),
            const QStringList &prefixes = QStringList());

    /** Find all files with a certain number of possible extensions in different
     * directories. If no existing readable file is found, an empty list is
     * returned. The current directory is always included. The files *must* have
     * one of the given extensions.
     *
     * @param directories directories to search
     * @param extensions possible extensions of the file, the dot before the
     * extension has to be included, e.g., ".js"
     */
    static QStringList findReadableFiles (const QStringList &directories,
            const QStringList &extensions);


    /**
     * Get full path of schema for apex experiment files
     */
    const QString GetExperimentSchemaPath() const;

    /**
     * Get full path of plugin directory for non-binary plugins, e.g.
     * pluginprocedure or xml plugins
     */
    const QString GetNonBinaryPluginPath() const;

    /**
     * Get full path of binary plugin directory
     * on windows this is the apex executable path, on linux
     * lib/apex/
     */
    const QString GetBinaryPluginPath() const;

    /**
     * Get full path of examples directory
     */
    const QString GetExamplesPath() const;

        /**
     * Get full path of scripts directory
         */
    const QString GetScriptsPath() const;


        /**
     * Get full path of xslt scripts directory
         */
    const QString GetXsltScriptsPath() const;


    /**
     * Get full path of apexconfig.xml
     */
    const QString GetApexConfigFilePath() const;

    /**
     * Get full path of apexconfig.xsd
     */
    const QString GetApexConfigSchemaPath() const;


    static const QString cApexConfig;
    static const QString cApexConfigSchema;
    static const QString cMruIni;
    static const QString cMainIcon;
    static const QString cPluginDir;
    static const QString cDataDir;
    static const QString cScriptsDir;
    static const QString cXsltScriptsDir;
    static const QString cExamplesDir;
    static const QString cExperimentSchema;


  private:
	  /**
        * Check whether the given path is a valid apex path
        */
	bool CheckApexDir(QDir d);

    /**
     * Return /usr/share/apex or equivalent
     */
    const QString GetSharePath() const;

      /**
     * Return configuration file directory of the current user
        * Linux: ~/.config/LabExpORL/
     * Windows: ...\local settings\LabExpORL\
       */
    static const QString GetUserConfigFilePath();


    QString m_sExec;
    QString m_sBase;
  };

}

#endif //#ifndef __PATHSSERVICE_H__
