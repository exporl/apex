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

#include "configfileparser.h"
#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;
#include "xml/xercesinclude.h"
using namespace xercesc;

#include "services/errorhandler.h"

#include <QFileInfo>
#include <QRegExp>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include <QDomDocument>

namespace apex
{

ConfigFileParser::ConfigFileParser(const QString& configFilename, const QString& configFileSchema, const QString& configfileNamespace):
    m_sConfigFilename(configFilename),
    m_sConfigFileSchema(configFileSchema),
    m_sConfigFileNamespace(configfileNamespace)
{
        m_document=NULL;
        // get absolute path for configfilename
//  m_sConfigFilename = QFileInfo(configFilename).absoluteFilePath();


}

bool ConfigFileParser::CFParse()
{
    try {
        // Check whether we need to upgrade the document
        QDomDocument doc("mydocument");
        QFile file(m_sConfigFilename);
        if (!file.open(QIODevice::ReadOnly))
            throw ApexStringException(
                    tr("Cannot open file %1").arg(m_sConfigFilename));
        if (!doc.setContent(&file)) {
            file.close();
            throw ApexStringException("Could not parse file");
        }
        file.close();

        const QString sVersion = doc.documentElement().attribute("xmlns:apex" );

        bool upgradePerformed = false;
        // Get schema version number from namespace
        QRegExp re("http://med.kuleuven.be/exporl/apex/(\\d+)\\.(\\d+)\\.?(\\d*)/");
        if ( re.lastIndexIn(sVersion) != -1) {
            QVector<int> v(3);
            v[0] = re.capturedTexts()[1].toInt();
            v[1] = re.capturedTexts()[2].toInt();
            if (re.captureCount()==2)
                v[2] = 0;
            else
                v[2] = re.capturedTexts()[3].toInt();

            upgradePerformed = upgradeFrom(doc, v);
        } else {
            throw ApexStringException(
                    tr("Cannot parse XML namespace of experiment %1")
                    .arg(m_sConfigFilename));
        }

        if (!upgradePerformed)
            // read and validate xml document
            m_document = xmlDocumentGetter.GetXMLDocument(m_sConfigFilename,
                                                      m_sConfigFileSchema,
                                                      m_sConfigFileNamespace);
        else {
            ErrorHandler::Get().addWarning(
                    tr("XML parser"),
                    tr("XML file upgraded from previous version"));

            // Write modified DOM tree to memory buffer and parse/validate with xerces
            QByteArray newXml( doc.toByteArray() );
            //qDebug() << newXml;

            MemBufInputSource source(reinterpret_cast<const XMLByte*>(newXml.data()), newXml.length(),
                                      "membufid",
                                      false);

            m_document = xmlDocumentGetter.GetXMLDocument(source,
                                                        m_sConfigFileSchema,
                                                        m_sConfigFileNamespace);
        }
    }
    catch (std::exception& e)
    {
        throw ApexStringException(QString("Couldn't parse configfile. ") +
                            "Error was: " + e.what());
    }
    catch (...)
    {
        throw ApexStringException("Couldn't parse configfile");
    }

    return m_document != 0;
}

bool ConfigFileParser::upgradeFrom(QDomDocument&, const QVector<int>& v)
{
    qDebug("Not upgrading from version %d.%d.%d", v[0], v[1], v[2]);
    return false;
}

void ConfigFileParser::LogErrorsToApex( StatusReporter* a_pError )
{
  xmlDocumentGetter.LogToApex( a_pError );
}


ConfigFileParser::~ConfigFileParser()
{
}


const QString& ConfigFileParser::GetConfigFilename() const {
        return m_sConfigFilename;
}

}

void apex::ConfigFileParser::SetConfigFilename(const QString &name)
{
    m_sConfigFilename=name;
}
