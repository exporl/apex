/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apexdata/mainconfigfiledata.h"

#include "apextools/apexpaths.h"
#include "apextools/apextools.h"
#include "apextools/version.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "wavstimulus/soundcarddrivers.h"

#include "mainconfigfileparser.h"

#include <QDomDocument>
#include <QUrl>

#include <QtGlobal>

using namespace apex::XMLKeys;

Q_GLOBAL_STATIC(apex::MainConfigFileParser, instance)

namespace apex
{

using namespace stimulus;

MainConfigFileParser::MainConfigFileParser()
    : UpgradableXmlParser(ApexPaths::GetApexConfigFilePath(),
                          ApexPaths::GetApexConfigSchemaPath(),
                          QL1S(CONFIG_SCHEMA_URL), QL1S(APEX_NAMESPACE))
{
}

MainConfigFileParser::MainConfigFileParser(const QString &fileName)
    : UpgradableXmlParser(fileName, ApexPaths::GetApexConfigSchemaPath(),
                          QL1S(CONFIG_SCHEMA_URL), QL1S(APEX_NAMESPACE))
{
}

MainConfigFileParser::~MainConfigFileParser()
{
}

MainConfigFileParser &MainConfigFileParser::Get()
{
    return *instance;
}

bool MainConfigFileParser::parse()
{
    loadAndUpgradeDom();

    QDomElement root = document.documentElement();

    QDomElement scriptnode = root.firstChildElement(QSL("paths"))
                                 .firstChildElement(QSL("pluginscriptlibrary"));
    if (!scriptnode.isNull()) {
        m_data.setPluginScriptLibrary(ApexPaths::GetNonBinaryPluginPath() +
                                      '/' + scriptnode.text());
    }

    for (QDomElement j = root.firstChildElement(QSL("soundcards"))
                             .firstChildElement(QSL("card"));
         !j.isNull(); j = j.nextSiblingElement(QSL("card"))) {
        for (QDomElement i = j.firstChildElement(QSL("drivername"));
             !i.isNull(); i = i.nextSiblingElement(QSL("drivername"))) {
            m_data.addSoundDriver(j.attribute(QSL("id")),
                                  i.attribute(QSL("driver")), i.text());
        }
    }

    for (QDomElement i = root.firstChildElement(QSL("cohdrivers"))
                             .firstChildElement(QSL("cohdriver"));
         !i.isNull(); i = i.nextSiblingElement(QSL("cohdriver"))) {
        m_data.addCohDriverName(i.attribute(QSL("id")), i.text());
    }

    for (QDomElement i = root.firstChildElement(QSL("prefixes"))
                             .firstChildElement(QSL("prefix"));
         !i.isNull(); i = i.nextSiblingElement(QSL("prefix"))) {
        m_data.addPrefix(i.attribute(QSL("id")), i.text());
    }

    QDomElement berthaNode = root.firstChildElement(QSL("bertha"));
    m_data.setBertha(berthaNode.isNull() || berthaNode.text() == QL1S("true"));

    QDomElement studyShareUrlNode =
        root.firstChildElement(QSL("studyshareurl"));
    m_data.setStudyShareUrl(studyShareUrlNode.text().trimmed());

    for (QDomElement constraintNode = root.firstChildElement(QSL("interactive"))
                                          .firstChildElement(QSL("constraints"))
                                          .firstChildElement(QSL("constraint"));
         !constraintNode.isNull();
         constraintNode =
             constraintNode.nextSiblingElement(QSL("constraint"))) {
        m_data.addInteractiveConstraint(
            constraintNode.attribute(QSL("expression")), constraintNode.text());
    }

    if (root.firstChildElement("useInstallationWideSettings").text() == "true")
        m_data.enableInstallationWideSettings();

    return true;
}

const data::MainConfigFileData &MainConfigFileParser::data() const
{
    return m_data;
}

static void fixUri(const QDomDocument &document, const QString &from,
                   const QString &to)
{
    QDomNodeList list = document.elementsByTagName(from);
    // walk backwards as the list is updated dynamically
    for (int i = list.count() - 1; i >= 0; --i) {
        QDomElement element = list.at(i).toElement();
        QString text = element.text();
        if (text.startsWith(QSL("file:")) || text.contains(QSL("%20")))
            text = QUrl(text).path();
        element.setTagName(to);
        XmlUtils::setText(&element, text);
    }
}

void MainConfigFileParser::upgrade3_1_3()
{
    QDomElement root = document.documentElement();
    QDomElement pathsnode = root.firstChildElement(QSL("paths"));

    // remove perl
    pathsnode.removeChild(pathsnode.firstChildElement(QSL("perl")));

    // fix js extension of library javascript files
    QDomElement scriptnode =
        pathsnode.firstChildElement(QSL("pluginscriptlibrary"));
    if (!scriptnode.isNull() && !scriptnode.text().endsWith(QSL(".js")))
        XmlUtils::setText(&scriptnode, scriptnode.text() + QSL(".js"));

    // nic -> coh
    QDomElement nicdrivers = root.firstChildElement(QSL("nicdrivers"));
    nicdrivers.setTagName(QSL("cohdrivers"));
    for (QDomElement i = nicdrivers.firstChildElement(QSL("nicdriver"));
         !i.isNull(); i = i.nextSiblingElement(QSL("nicdriver"))) {
        i.setTagName(QSL("cohdriver"));
    }

    // uri -> file
    fixUri(document, QSL("prefix"), QSL("prefix"));
}
}
