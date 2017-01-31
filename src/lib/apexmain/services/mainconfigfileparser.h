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

#ifndef MAINCONFIGFILEPARSERSERVICE_H
#define MAINCONFIGFILEPARSERSERVICE_H

#include "version.h"

#include "services/servicemanager.h"
#include "services/paths.h"
#include "configfileparser.h"

#include <memory>
#include <string>
#include <map>

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
  class DOMElement;
};

namespace apex {

    namespace stimulus {
        class SndDriversMap;
    }

    class MainConfigFileParser: public ConfigFileParser, public Service<MainConfigFileParser> {
    public:
        MainConfigFileParser();
        virtual bool CFParse();

        const char* Name()
        {
            return "Configuration";
        }

        const QString& GetPerlPath() { return m_perl_path; };
        const QString& GetXsltOnlinePath() { return m_xslt_path_online; };
        const QString GetPluginScriptLibrary();
        const stimulus::SndDriversMap* GetSoundCardDrivers() const;

        const QString GetPrefix(const QString& p_id);

        ~MainConfigFileParser();

    protected:
        virtual const QString getConfigfileNamespace()
        {
            return QString ("http://med.kuleuven.be/exporl/apex/"+
                    QString(SCHEMA_VERSION)+"/config");
        };

        virtual bool upgradeFrom(QDomDocument& doc, const QVector<int>& v);

    private:
        void ParsePrefixes(XERCES_CPP_NAMESPACE::DOMElement* p_root);

        static const int m_configfileVersion = 1;
        QString m_schemas_path;
        QString m_experiment_schema;
        QString m_perl_path;
        QString m_scripts_path;
        QString m_xslt_path_online;
        QString m_plugins_path;
        QString m_plugin_script_library;
        std::auto_ptr<stimulus::SndDriversMap> m_SndDrivers;
        std::map<QString,QString> m_prefixes;

    };

}
#endif
