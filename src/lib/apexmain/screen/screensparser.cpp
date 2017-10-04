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

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "gui/guidefines.h"

#include "parser/prefixparser.h"
#include "parser/scriptexpander.h"

#include "screenparser.h"
#include "screensparser.h"

#include <QDir>
#include <QFile>
#include <QUrl>

using namespace apex::gui;
using namespace apex::XMLKeys;

namespace apex
{
namespace parser
{

// ScreensParser ===============================================================

ScreensParser::ScreensParser(QWidget *parent) : m_parent(parent)
{
}

data::ScreensData *
ScreensParser::ParseScreens(const QString &fileName, const QDomElement &p_base,
                            const QString &scriptLibraryFile,
                            const QVariantMap &scriptParameters,
                            data::ParameterManagerData *pmd)
{
    QScopedPointer<data::ScreensData> screens(new data::ScreensData());
    QScopedPointer<ScreenParser> m_screenParser(
        new ScreenParser(screens.data(), pmd));

    // start with default font
    screens->setDefaultFont(sc_sDefaultFont);
    screens->setDefaultFontSize(sc_nDefaultFontSize);

#ifndef NOSCRIPTEXPAND
    // find plugin screens and expand them
    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag(currentNode.tagName());
        if (tag == "pluginscreens") {
            ScriptExpander expander(fileName, scriptLibraryFile,
                                    scriptParameters, m_parent);
            expander.ExpandScript(currentNode, "getScreens");
        }
    }
#endif

    // parse
    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag(currentNode.tagName());
        if (tag == "screen") {
            // let it propagate...
            if (!m_screenParser->createScreen(currentNode)) {
                return 0;
            }
        } else if (tag == "reinforcement") {
            for (QDomElement reinf = currentNode.firstChildElement();
                 !reinf.isNull(); reinf = reinf.nextSiblingElement()) {
                const QString tag(reinf.tagName());
                if (tag == "progressbar") {
                    screens->setProgressbarEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == gc_sFeedback) {
                    screens->setFeedbackEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                    screens->setFeedbackLength(
                        reinf.attribute(gc_sLength).toUInt());
                } else if (tag == "feedback_on") {
                    QString value = reinf.text();
                    if (value == "none")
                        screens->setFeedbackOn(data::HIGHLIGHT_NONE);
                    else if (value == "correct")
                        screens->setFeedbackOn(data::HIGHLIGHT_CORRECT);
                    else if (value == "clicked")
                        screens->setFeedbackOn(data::HIGHLIGHT_ANSWER);
                    else
                        throw ApexStringException("Illegal feedback_on value");
                } else if (tag == "feedback_picture_positive") {
                    QString file = reinf.text();
                    QString absPath = ApexTools::addPrefix(
                        file, m_screenParser->GetBasePath());
                    if (!QFile::exists(absPath))
                        throw ApexStringException(
                            "Positive feedback picture not found");
                    screens->setFeedbackPositivePicture(file);
                } else if (tag == "feedback_picture_negative") {
                    QString file = reinf.text();
                    QString absPath = ApexTools::addPrefix(
                        file, m_screenParser->GetBasePath());
                    if (!QFile::exists(absPath))
                        throw ApexStringException(
                            "Positive feedback picture not found");
                    screens->setFeedbackNegativePicture(file);
                } else if (tag == "showcurrent") {
                    screens->setShowCurrentEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == "feedbackplugin") {
                    QString plugin(reinf.attribute(QSL("name")));
                    data::FeedbackPluginParameters params;
                    for (QDomElement pnode = reinf.firstChildElement();
                         !pnode.isNull(); pnode = pnode.nextSiblingElement()) {
                        QString name(pnode.attribute(QSL("name")));
                        QString value(pnode.text());
                        params.push_back(QPair<QString, QString>(name, value));
                    }
                    screens->addFeedbackPlugin(plugin, params);
                } else {
                    qCWarning(APEX_RS, "%s",
                              qPrintable(QSL("%1: %2").arg(
                                  "ExperimentConfigFileParser::ParseScreens",
                                  "Unknown tag: " + tag)));
                }
            }
        } else if (tag == "childmode") {
            for (QDomElement reinf = currentNode.firstChildElement();
                 !reinf.isNull(); reinf = reinf.nextSiblingElement()) {
                const QString tag(reinf.tagName());
                if (tag == "intro") {
                    screens->setIntroScreen(reinf.text());
                    screens->setIntroLength(
                        reinf.attribute(gc_sLength).toUInt());
                } else if (tag == "outro") {
                    screens->setOutroScreen(reinf.text());
                    screens->setOutroLength(
                        reinf.attribute(gc_sLength).toUInt());
                } else if (tag == "panel") {
                    QString file = reinf.text();
                    QString absPath = ApexTools::addPrefix(
                        file, m_screenParser->GetBasePath());
                    // Flash thing wants absolute path:
                    QDir d(absPath);
                    absPath = d.absolutePath();
                    qCDebug(APEX_RS, "%s", qPrintable(file));
                    if (!QFile::exists(absPath)) {
                        qCCritical(APEX_RS, "%s",
                                   qPrintable(QSL("%1: %2").arg(
                                       "ScreenParser",
                                       "Cannot find panel movie: " + file)));
                        return 0;
                    }
                    screens->setPanelMovie(file);
                } else {
                    qCWarning(APEX_RS, "%s",
                              qPrintable(QSL("%1: %2").arg(
                                  "ExperimentConfigFileParser::ParseScreens",
                                  "Unknown tag: " + tag)));
                }
            }
            screens->m_eMode = data::gc_eChild;
        } else if (tag == "general") {
            for (QDomElement reinf = currentNode.firstChildElement();
                 !reinf.isNull(); reinf = reinf.nextSiblingElement()) {
                const QString tag(reinf.tagName());
                if (tag == "stopbutton") {
                    screens->setStopButtonEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == "repeatbutton") {
                    screens->setRepeatButtonEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == "statuspicture") {
                    screens->setStatusPictureEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == "showpanel") {
                    screens->setPanelEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == "showmenu") {
                    screens->setMenuEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == "fullscreen") {
                    screens->setFullScreenEnabled(
                        ApexTools::bQStringToBoolean(reinf.text()));
                } else if (tag == gc_sItiScreen) {
                    screens->setInterTrialScreen(reinf.text());
                    screens->setInterTrialLength(
                        reinf.attribute(gc_sLength).toUInt());
                } else {
                    qCWarning(APEX_RS, "%s",
                              qPrintable(QSL("%1: %2").arg(
                                  "ExperimentConfigFileParser::ParseScreens",
                                  "Unknown tag: " + tag)));
                }
            }
        } else if (tag == "style") {
            screens->setStyle(currentNode.text());
        } else if (tag == "style_apex") {
            screens->setApexStyle(currentNode.text());
        } else if (tag == "prefix") {
            data::FilePrefix prefix = parser::PrefixParser::Parse(currentNode);
            m_screenParser->mp_SetBasePath(prefix);
            screens->setPrefix(prefix);
        } else if (tag == "defaultFont") {
            const QString font = currentNode.text();
            screens->setDefaultFont(font);
        } else if (tag == "defaultFontsize") {
            unsigned fs = currentNode.text().toUInt();
            screens->setDefaultFontSize(fs);
        } else {
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg(
                          "ExperimentConfigFileParser::ParseScreens",
                          "Unknown tag: " + tag)));
        }
    }

    return screens.take();
}
}
}
