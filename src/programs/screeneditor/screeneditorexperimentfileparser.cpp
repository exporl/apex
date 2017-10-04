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

#include "apexmain/screen/screenparser.h"

#include "apextools/version.h"

#include "apextools/xml/xmltools.h"

#include "apexwriters/screenswriter.h"

#include "screeneditorexperimentfileparser.h"

#include <QFile>

using namespace apex::gui;

namespace apex
{
namespace editor
{

ScreenEditorExperimentFileParser::ScreenEditorExperimentFileParser()
{
}

ScreenEditorExperimentFileParser::~ScreenEditorExperimentFileParser()
{
}

ScreenEditorExperimentData *
ScreenEditorExperimentFileParser::parse(const QString &file)
{
    QScopedPointer<ScreenEditorExperimentData> ret(
        new ScreenEditorExperimentData());
    try {
        ret->expDocument = XmlUtils::parseDocument(file);
    } catch (...) {
        return NULL;
    }
    for (QDomElement currentNode =
             ret->expDocument.documentElement().firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "screens") {
            ret->screensElement = currentNode;
            ret->screens = new ScreensData;

            for (QDomElement cn = currentNode.firstChildElement(); !cn.isNull();
                 cn = cn.nextSiblingElement()) {
                const QString ctag = cn.tagName();
                if (ctag == "screen") {
                    ScreenParser screenParser(ret->screens);
                    Screen *s = screenParser.createScreen(cn);
                    ret->screenToElementMap[s] = cn;
                } else if (ctag == "defaultFont") {
                    const QString font = cn.text();
                    ret->screens->setDefaultFont(font);
                } else if (ctag == "defaultFontSize") {
                    unsigned fs = cn.text().toUInt();
                    ret->screens->setDefaultFontSize(fs);
                } else
                    continue;
            }
        } else
            continue;
    }
    return ret.take();
}

void ScreenEditorExperimentFileParser::save(const ScreenEditorExperimentData *d,
                                            const QString &file)
{
    XmlUtils::writeDocument(d->expDocument, file);
}

void ScreenEditorExperimentData::storeScreenChanges(const Screen *s)
{
    screenToElementMapT::const_iterator i = screenToElementMap.find(s);
    Q_ASSERT(i != screenToElementMap.end());
    QDomElement se = i->second;
    Q_ASSERT(se.parentNode() == screensElement);

    writer::ScreensWriter writer;
    QDomDocument doc;
    doc.appendChild(writer.addScreen(&doc, *s));
    QDomElement ne2 =
        expDocument
            .importNode(XmlUtils::parseString(doc.toString()).documentElement(),
                        true)
            .toElement();
    screensElement.replaceChild(ne2, se);
    Q_ASSERT(ne2.parentNode() == screensElement);
    screenToElementMap[s] = ne2;
}

void ScreenEditorExperimentData::addScreen(Screen *s)
{
    screens->manageScreen(s);
    writer::ScreensWriter writer;
    QDomDocument doc;
    doc.appendChild(writer.addScreen(&doc, *s));
    QDomElement se2 =
        expDocument
            .importNode(XmlUtils::parseString(doc.toString()).documentElement(),
                        true)
            .toElement();
    screensElement.appendChild(se2);
    screenToElementMap[s] = se2;
}

ScreenEditorExperimentData::~ScreenEditorExperimentData()
{
    delete screens;
}

QString ScreenEditorExperimentFileParser::schemaLoc;
}
}
