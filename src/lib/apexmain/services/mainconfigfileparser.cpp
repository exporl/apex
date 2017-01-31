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


#include "xml/xercesinclude.h"
using namespace xercesc;

#include "stimulus/wav/soundcarddrivers.h"
#include "services/paths.h"
#include "apextools.h"
#include "mainconfigfileparser.h"
#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;

#include <string>
#include <iostream>

#include <QtGlobal>
#include <QDomDocument>

namespace apex {

    using namespace stimulus;

    MainConfigFileParser::MainConfigFileParser() :
            ConfigFileParser(
                    Paths::Get().GetApexConfigFilePath(),
                    Paths::Get().GetApexConfigSchemaPath(),
                    getConfigfileNamespace()),
            m_xslt_path_online("https://gilbert.med.kuleuven.be/apex/xslt"),
            m_SndDrivers(new SndDriversMap())
    {
        m_plugin_script_library = "pluginscriptlibrary.js";
    }

    bool MainConfigFileParser::CFParse() {
        bool result=ConfigFileParser::CFParse();
        if (!result)
            return false;

        Q_CHECK_PTR(m_document);
        DOMElement* root = (DOMElement*) m_document->getFirstChild();
        Q_ASSERT(root);

        const QString sVersion = XMLutils::GetAttribute( root, "version" );
        if ( sVersion.toInt() != m_configfileVersion) {
            std::cout << "Error: invalid config file version";
            throw(0);
        }

        // analyse certain tags and put their values in private members
        //const QString sPrefix = "prefix";
        DOMElement* tnode;

        DOMElement* pathnode = XMLutils::GetElementsByTagName( root, "paths" );

        if ( (tnode = XMLutils::GetElementsByTagName( pathnode, "perl" )) ) // this is correct
            m_perl_path = XMLutils::GetFirstChildText( tnode );

        if ( (tnode = XMLutils::GetElementsByTagName( pathnode, "xslt" )) ) // this is correct
            m_xslt_path_online = XMLutils::GetFirstChildText( tnode );


        if ( (tnode = XMLutils::GetElementsByTagName( pathnode, "pluginscriptlibrary" )) ){
            m_plugin_script_library = XMLutils::GetFirstChildText( tnode );
//            std::string scriptlibraryST = m_plugin_script_library.toUtf8().constData();
//            std::cout<<"m_plugin_script_library in the XXXXCFParse function is: ******#######"<<scriptlibraryST<<endl;
        }


        tnode = XMLutils::GetElementsByTagName( root, "soundcards" );
        Q_ASSERT(tnode);
        for( DOMNode* curCard=tnode->getFirstChild(); curCard!=0; curCard=curCard->getNextSibling() )
        {
            Q_ASSERT(curCard);
            DOMElement* el = (DOMElement*) curCard; //card

            const QString id     = XMLutils::GetAttribute( el, "id" );
            t_SndDrivers* names = new t_SndDrivers();
            for( DOMNode* curDrvr=curCard->getFirstChild(); curDrvr!=0; curDrvr=curDrvr->getNextSibling() )
            {
                DOMElement* elD = (DOMElement*) curDrvr;
                const QString driver = XMLutils::GetAttribute( elD, "driver" );
                gt_eDeviceType m_eDevType = fSndStringToEnum( driver );
                if( m_eDevType == NODEV )
                {
                    std::cout << "Error: wrong drivername";
                    throw(0);
                }
                const QString m_sDriverName  = XMLutils::GetFirstChildText( elD );
                names->mp_AddItem( new t_SndDriverInfo( m_eDevType, m_sDriverName ) );
            }

            m_SndDrivers->insert(std::pair<QString,t_SndDrivers*>(id,names));
        }

        ParsePrefixes(root);
        m_schemas_path=ApexTools::MakeDirEnd(m_schemas_path);
        m_experiment_schema=m_schemas_path+m_experiment_schema;

        return true;
    }


    MainConfigFileParser::~MainConfigFileParser() {}


    void apex::MainConfigFileParser::ParsePrefixes( DOMElement * p_root ) {
        DOMNode* tnode = XMLutils::GetElementsByTagName( p_root, "prefixes" );
        if (!tnode)
            return;

        for( DOMNode* cur=tnode->getFirstChild(); cur!=0; cur=cur->getNextSibling() )  {
            QString prefix = XMLutils::GetFirstChildText( cur );
            QString tag = XMLutils::GetTagName(cur);
            QString id = XMLutils::GetAttribute(cur, "id");

            Q_ASSERT(tag=="prefix");

            if (!id.isEmpty()) {
                m_prefixes[id]=prefix;
            }
        }

    }

    const stimulus::SndDriversMap* MainConfigFileParser::GetSoundCardDrivers() const
    {
        return m_SndDrivers.get();
    }

    bool MainConfigFileParser::upgradeFrom(QDomDocument& doc,
                                           const QVector<int>& v)
    {
        // Currently there are no changes between versions, we only update
        // the schema namespace etc

        QString versionString;
        versionString.sprintf("%d.%d.%d", v[0], v[1], v[2]);
        if (SCHEMA_VERSION != versionString) {
            doc.documentElement().setAttribute("xmlns:apex",
                                               getConfigfileNamespace());
            doc.documentElement().setAttribute("xsi:schemaLocation",
                    getConfigfileNamespace() + " " +
                    QString::fromUtf8(QUrl::fromLocalFile(Paths::Get().GetExperimentSchemaPath()).toEncoded()));
            return true;
        }
        else
            return false;
    }

}

/**
 * Return prefix according to given id, if no prefix is found, an empty string is returned
 * @param p_id
 * @return
 */
const QString  apex::MainConfigFileParser::GetPrefix( const QString & p_id )
{

    std::map<QString, QString>::const_iterator p = m_prefixes.find(p_id);
    if ( p != m_prefixes.end() )
        return (*p).second;
    else {
        qDebug("Prefix with id %s not found", qPrintable (p_id));
        throw ApexStringException( QString(tr("Prefix with id %1 not found")).arg(p_id));
    }

}

const QString apex::MainConfigFileParser::GetPluginScriptLibrary()
{
    QString filename( Paths::Get().GetNonBinaryPluginPath() +
                      m_plugin_script_library);
//    std::cout<<"Filename returned by getplugin script library is: "<<filename.toStdString()<<std::endl;
//    std::cout<<"Value of m_plugin_script_library is: "<<m_plugin_script_library.toStdString()<<std::endl;
    return filename;
}
