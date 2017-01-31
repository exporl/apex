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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SERVICES_MAINCONFIGFILEPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SERVICES_MAINCONFIGFILEPARSER_H_

#include "../configfileparser.h"

#include "apexdata/mainconfigfiledata.h"

#include "apextools/global.h"

#include "apextools/services/paths.h"
#include "apextools/services/servicemanager.h"

#include "apextools/version.h"

#include "apextools/xml/xercesinclude.h"

#include <map>
#include <memory>
#include <string>

namespace XERCES_CPP_NAMESPACE
{
  class DOMElement;
};

namespace apex {

    namespace stimulus {
        class SndDriversMap;
    }

    class APEX_EXPORT MainConfigFileParser: public ConfigFileParser, public Service<MainConfigFileParser> {
    public:
        MainConfigFileParser();
        virtual bool CFParse();

        const char* Name()
        {
            return "Configuration";
        }

        ~MainConfigFileParser();

        const data::MainConfigFileData& data() const;

    protected:
        virtual const QString getConfigfileNamespace()
        {
            return QString ("http://med.kuleuven.be/exporl/apex/"+
                    QString(SCHEMA_VERSION)+"/config");
        };

        virtual bool upgradeFrom(QDomDocument& doc, const QVector<int>& v);

    private:
        static const int m_configfileVersion = 1;
        void ParsePrefixes(XERCES_CPP_NAMESPACE::DOMElement* p_root);
        data::MainConfigFileData m_data;


    };

}
#endif
