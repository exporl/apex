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

#include "apexdata/screen/screensdata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "gui/guidefines.h"

#include "parser/prefixparser.h"
#include "parser/scriptexpander.h"

#include "services/errorhandler.h"

#include "screenparser.h"
#include "screensparser.h"

#include <QDir>
#include <QFile>
#include <QUrl>

using namespace XERCES_CPP_NAMESPACE;

using namespace apex::gui;
using namespace apex::ApexXMLTools;
using namespace apex::XMLKeys;

#include <memory>

namespace apex
{
namespace parser
{

    ScreensParser::ScreensParser(QWidget* parent):
            m_parent(parent)
{

}

data::ScreensData* ScreensParser::ParseScreens(
            XERCES_CPP_NAMESPACE::DOMElement* p_base, /*MainWindowConfig* mainWndConfig,*/
            QString scriptLibraryFile, QVariantMap scriptParameters,
            data::ParameterManagerData* pmd)
    {
    data::ScreensData* screens=new data::ScreensData();
    QScopedPointer<ScreenParser> m_screenParser(new ScreenParser( screens, pmd ));


//start with default font
    screens->setDefaultFont( sc_sDefaultFont );
    screens->setDefaultFontSize( sc_nDefaultFontSize );

#ifndef NOSCRIPTEXPAND
    // find plugin screens and expand them
    for ( DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        const QString tag( XMLutils::GetTagName( currentNode ) );
        if (tag == "pluginscreens") {
            ScriptExpander expander(scriptLibraryFile, scriptParameters, m_parent);
            expander.ExpandScript(currentNode, "getScreens");
        }
    }
#endif


    //parse
    for ( DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        const QString tag( XMLutils::GetTagName( currentNode ) );
        if (tag == "screen")
        {
            // let it propagate...
            /*try
            {*/
            if (!m_screenParser->createScreen ((DOMElement*) currentNode))
            {
                ErrorHandler::Get() << m_screenParser->logger().items();
                return 0;
            }
            /*}
            catch( ... )
            {
            m_progress.AddItem( m_screenParser->GetError() );
            return false;
            }*/
        }
        else if (tag== "reinforcement")
        {
            for ( DOMNode* reinf = currentNode->getFirstChild() ; reinf != 0 ; reinf = reinf->getNextSibling() )
            {
                const QString tag( XMLutils::GetTagName( reinf ) );
                if ( tag == "progressbar" )
                    screens->setProgressbarEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ) );
                else if ( tag == gc_sFeedback )
                {
                    screens->setFeedbackEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                    screens->setFeedbackLength( XMLutils::GetAttribute( reinf, gc_sLength ).toUInt());
                }
                else if ( tag == "feedback_on" )
                {
                    QString value = XMLutils::GetFirstChildText( reinf );
                    if (value=="none")
                        screens->setFeedbackOn(data::HIGHLIGHT_NONE);

                    else if (value=="correct")
                        screens->setFeedbackOn(data::HIGHLIGHT_CORRECT);

                    else if (value=="clicked")
                        screens->setFeedbackOn(data::HIGHLIGHT_ANSWER);
                    else
                        throw ApexStringException("Illegal feedback_on value");
                }
                else if ( tag == "feedback_picture_positive" )
                {
                    QString file = XMLutils::GetFirstChildText( reinf );
                    QString absPath = ApexTools::addPrefix( file,
                                              m_screenParser->GetBasePath());
                    if ( ! QFile::exists(absPath))
                        throw ApexStringException(
                            "Positive feedback picture not found");

                    screens->setFeedbackPositivePicture(file);
                }
                else if ( tag == "feedback_picture_negative" )
                {
                    QString file = XMLutils::GetFirstChildText( reinf );
                    QString absPath = ApexTools::addPrefix(file,
                                              m_screenParser->GetBasePath());
                    if ( ! QFile::exists(absPath))
                        throw ApexStringException(
                            "Positive feedback picture not found");

                    screens->setFeedbackNegativePicture(file);
                }
                else if ( tag == "showcurrent" )
                {
                    screens->setShowCurrentEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                }
                else if ( tag == "feedbackplugin" )
                {
                    QString plugin( XMLutils::GetAttribute( reinf, "name" ) );
                    data::FeedbackPluginParameters params;

                    for ( DOMNode* pnode = reinf->getFirstChild() ; pnode != 0 ;
                        pnode = pnode->getNextSibling() ) {
                        QString name( XMLutils::GetAttribute( pnode, "name" ));
                        QString value( XMLutils::GetFirstChildText( pnode ) );

                        params.push_back(QPair<QString,QString>(name,value));
                    }

                    screens->addFeedbackPlugin( plugin, params);
                }
                else
                    ErrorHandler::Get().addItem( StatusItem(StatusItem::Warning,
                                      "ExperimentConfigFileParser::ParseScreens",
                                      "Unknown tag: " + tag ));
            }
        }
        else if ( tag == "childmode" )
        {
            for ( DOMNode* reinf = currentNode->getFirstChild() ; reinf != 0 ; reinf = reinf->getNextSibling() )
            {
                const QString tag( XMLutils::GetTagName( reinf ) );
                if ( tag == "intro" )           // name of intro screen
                {
                    screens->setIntroScreen(XMLutils::GetFirstChildText( reinf ));
                    screens->setIntroLength(XMLutils::GetAttribute( reinf, gc_sLength ).toUInt());
                }
                else if ( tag == "outro" )
                {
                    screens->setOutroScreen(XMLutils::GetFirstChildText( reinf ));
                    screens->setOutroLength(XMLutils::GetAttribute( reinf, gc_sLength ).toUInt());
                }
                else if ( tag == "panel" )
                {
                    QString file = XMLutils::GetFirstChildText( reinf );
                    QString absPath = ApexTools::addPrefix( file,  m_screenParser->GetBasePath());
                    // Flash thing wants absolute path:
                    QDir d(absPath);
                    absPath = d.absolutePath();
                    qCDebug(APEX_RS, "%s", qPrintable (file));
                    if ( !QFile::exists(absPath))
                    {
                        ErrorHandler::Get().addItem( StatusItem(StatusItem::Error,"ScreenParser",
                                          "Cannot find panel movie: " + file));
                        return 0;
                    }
                    screens->setPanelMovie(file);
                }
                else
                    ErrorHandler::Get().addItem( StatusItem(StatusItem::Warning,"ExperimentConfigFileParser::ParseScreens",
                                      "Unknown tag: " + tag ));
            }
            screens->m_eMode = data::gc_eChild;
        }
        else if ( tag== "general" )
        {
            for ( DOMNode* reinf = currentNode->getFirstChild() ; reinf != 0 ; reinf = reinf->getNextSibling() )
            {
                const QString tag( XMLutils::GetTagName( reinf ) );
                if ( tag == "stopbutton" )
                    screens->setStopButtonEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                else if ( tag == "repeatbutton" )
                    screens->setRepeatButtonEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                else if ( tag == "statuspicture" )
                    screens->setStatusPictureEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                else if ( tag == "showpanel" )
                    screens->setPanelEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                else if ( tag == "showmenu" )
                    screens->setMenuEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                else if ( tag == "fullscreen" )
                    screens->setFullScreenEnabled(ApexTools::bQStringToBoolean( XMLutils::GetFirstChildText( reinf ) ));
                else if ( tag == gc_sItiScreen )
                {
                    screens->setInterTrialScreen(XMLutils::GetFirstChildText( reinf ));
                    screens->setInterTrialLength(XMLutils::GetAttribute( reinf, gc_sLength ).toUInt());
                }
                else
                    ErrorHandler::Get().addItem( StatusItem(StatusItem::Warning,
                                      "ExperimentConfigFileParser::ParseScreens",
                                      "Unknown tag: " + tag ));
            }
        }
        else if (tag=="style")
        {
            screens->setStyle(XMLutils::GetFirstChildText( currentNode ) );

        }
        else if (tag=="style_apex")
        {
            screens->setApexStyle(XMLutils::GetFirstChildText( currentNode ));
        }
        else if (tag == "uri_prefix")
        {
            data::FilePrefix prefix = parser::PrefixParser::Parse
                    ((DOMElement*) currentNode);
            m_screenParser->mp_SetBasePath(prefix);
            screens->setPrefix(prefix);
        }
        else if ( tag == "defaultFont" )
        {
            const QString font = XMLutils::GetFirstChildText( currentNode );
            screens->setDefaultFont( font );
        }
        else if ( tag == "defaultFontsize" )
        {
            unsigned fs = XMLutils::GetFirstChildText( currentNode ).toUInt();
            screens->setDefaultFontSize( fs );
        }
        else
        {
            ErrorHandler::Get().addItem( StatusItem(StatusItem::Warning,"ExperimentConfigFileParser::ParseScreens", "Unknown tag: " + tag ));
        }
    }

//    mainWndConfig->m_PanelConfig.m_eMode = mainWndConfig->m_eMode;
//    mainWndConfig->m_PanelConfig.m_bShowFeedback = mainWndConfig->m_bShowFeedback;

    return screens;

}

}
}

