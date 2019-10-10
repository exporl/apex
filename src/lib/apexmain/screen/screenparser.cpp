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

#include "apexdata/parameters/parameter.h"
#include "apexdata/parameters/parametermanagerdata.h"

#include "apexdata/screen/answerlabelelement.h"
#include "apexdata/screen/arclayoutelement.h"
#include "apexdata/screen/buttonelement.h"
#include "apexdata/screen/buttongroup.h"
#include "apexdata/screen/checkboxelement.h"
#include "apexdata/screen/flashplayerelement.h"
#include "apexdata/screen/gridlayoutelement.h"
#include "apexdata/screen/htmlelement.h"
#include "apexdata/screen/labelelement.h"
#include "apexdata/screen/matrixelement.h"
#include "apexdata/screen/numerickeypadelement.h"
#include "apexdata/screen/parameterlabelelement.h"
#include "apexdata/screen/parameterlistelement.h"
#include "apexdata/screen/pictureelement.h"
#include "apexdata/screen/picturelabelelement.h"
#include "apexdata/screen/screen.h"
#include "apexdata/screen/screenelement.h"
#include "apexdata/screen/screenlayoutelement.h"
#include "apexdata/screen/screensdata.h"
#include "apexdata/screen/sliderelement.h"
#include "apexdata/screen/spinboxelement.h"
#include "apexdata/screen/texteditelement.h"

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "apextools/gui/arclayout.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "gui/guidefines.h"

#include "apexcontrol.h"
#include "screenparser.h"

#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QKeySequence>
#include <QUrl>

#include <QtGlobal>

using namespace apex::XMLKeys;
using namespace apex::data;

/**
 * Create a keysequence.
 * Throws ApexStringException in case of error.
 * @param a_pElement the DOMElement
 * @return the keysequence
 */
QKeySequence f_CreateKeySequence(const QDomElement &a_pElement)
{
    const QString sKey(a_pElement.text());
    const QString sMod(a_pElement.attribute(QSL("modifier")));
    const QString sHex(a_pElement.attribute(QSL("hex")));
    bool bHex = false;
    if (!sHex.isEmpty())
        bHex = apex::ApexTools::bQStringToBoolean(sHex);

    // get modifier
    Qt::Modifier eMod = Qt::Modifier(0);
    if (!sMod.isEmpty()) {
        if (sMod == "Alt")
            eMod = Qt::ALT;
        else if (sMod == "Ctrl")
            eMod = Qt::CTRL;
        else if (sMod == "Shift")
            eMod = Qt::SHIFT;
        else
            throw(ApexStringException("f_CreateKeySequence: unknown modifier " +
                                      sMod));
    }

    if (bHex) {
        // create from hexadecimal
        bool bOk = false;
        const int nHex = sKey.toInt(&bOk, 16);
        if (!bOk)
            throw(
                ApexStringException("f_CreateKeySequence: requested hex but " +
                                    sKey + " is not a valid number."));
        if (!sMod.isEmpty())
            return QKeySequence(eMod | nHex);
        return QKeySequence(nHex);
    } else {
        if (sKey.length() == 1) {
            // create from string
            if (!sMod.isEmpty())
                return QKeySequence(sMod + "+" + sKey);
            return QKeySequence(sKey);
        } else {
            // create from enum
            int nKey = 0;
            if (sKey == "UpArrow")
                nKey = Qt::Key_Up;
            else if (sKey == "DownArrow")
                nKey = Qt::Key_Down;
            else if (sKey == "LeftArrow")
                nKey = Qt::Key_Left;
            else if (sKey == "RightArrow")
                nKey = Qt::Key_Right;
            else if (sKey == "Return")
                nKey = Qt::Key_Return;
            else if (sKey == "Space")
                nKey = Qt::Key_Space;
            else if (sKey == "Enter")
                nKey = Qt::Key_Enter;
            else
                throw(ApexStringException("f_CreateKeySequence: unknown key " +
                                          sKey));
            if (!sMod.isEmpty())
                return QKeySequence(eMod | nKey);
            return QKeySequence(nKey);
        }
    }
}

bool sf_bIsLayout(const QString &ac_sTag)
{
    return (ac_sTag == gc_sGridLayout || ac_sTag == gc_sVertLayout ||
            ac_sTag == gc_sHoriLayout || ac_sTag == gc_sArcLayout ||
            ac_sTag == gc_sTwoPartLayout);
}

namespace apex
{
namespace gui
{

/**
  * Checks whether the file exists. (for flash movies)
  * Appends ac_sRelativePath to ac_sBasePath.
  * @param ac_sBasePath the base path, can be empty
  * @param ac_sRelativePath the file path
  * @return full path if exists, else empty string
  */
QString ScreenParser::f_CheckPath(const QString &ac_sBasePath,
                                  const QString &ac_sRelativePath)
{
    QString s = ac_sBasePath;
    if (!s.isEmpty() && !s.endsWith('/')) {
        s += '/';
    } else if (s.isEmpty()) {
        //[ stijn ] a new feature in qt4: QUrl::path() strips the drive
        // from the path on windows. Completely unusable, so just return the
        // path
        if (!QFile::exists(ac_sRelativePath))
            return QString();
        return ac_sRelativePath;
    }
    const QUrl sPath(s + ac_sRelativePath);
    // must use path, qurl prefixes itself wih unusable "file:"
    QString p = sPath.path();

    QFileInfo fi(p);
    p = fi.absoluteFilePath();

    if (!QFile::exists(p))
        p = "";
    return p;
}

QString ScreenParser::f_CheckPath(data::FilePrefix p,
                                  const QString &ac_sRelativePath)
{
    return f_CheckPath(FilePrefixConvertor::convert(p), ac_sRelativePath);
}

Screen *ScreenParser::createScreen(const QDomElement &a_pElement)
{
    ButtonGroup *buttonGroup = 0;
    ScreenElement *rootElement = 0;
    QString defaultAnswer;
    ScreenElementMap idToElementMap;

    const QString screenID = a_pElement.attribute(gc_sID);

    for (QDomElement it = a_pElement.firstChildElement(); !it.isNull();
         it = it.nextSiblingElement()) {
        const QString tag(it.tagName());

        if (sf_bIsLayout(tag)) {
            try {
                rootElement = createLayout(it, 0, idToElementMap);
            } catch (const std::exception &s) {
                qCCritical(APEX_RS, "Screen Parser: %s", s.what());
                delete rootElement;
                delete buttonGroup;
                return NULL;
            }
        } else if (tag == gc_sButtonGroup) {
            buttonGroup = createButtonGroup(it, idToElementMap);
        } else if (tag == gc_sDefaultAnswer) {
            defaultAnswer = it.text();

            if (idToElementMap.find(defaultAnswer) == idToElementMap.end() &&
                buttonGroup && buttonGroup->getID() != defaultAnswer) {
                throw ApexStringException("Unknown default_answer_element: " +
                                          defaultAnswer);
            }
        } else {
            addUnknownTag("ScreenParser::createScreen", tag);
        }
    }

    if (!rootElement) {
        delete rootElement;
        delete buttonGroup;
        return 0;
    }

    Screen *screen = new Screen(screenID, rootElement, idToElementMap,
                                buttonGroup, defaultAnswer);
    screens->manageScreen(screen);
    return screen;
}

ScreenParser::ScreenParser(ScreensData *s, data::ParameterManagerData *pmd)
    : screens(s), parameterManagerData(pmd)
{
}

Screen *ScreenParser::createTestScreen()
{
    static unsigned nCreated = 0;
    const QString layoutID = "testlayout" + QString::number(nCreated);
    const QString screenID = "testscreen" + QString::number(nCreated++);

    GridLayoutElement *gle = new GridLayoutElement(
        layoutID, 0, 5, 5, data::tStretchList(), data::tStretchList());
    ScreenElementMap idToElementMap;

    ScreenElement *p1 = createTestScreenElement(gle);
    ScreenElement *p2 = createTestScreenElement(gle);

    p1->setX(2);
    p1->setY(3);
    p2->setX(2);
    p2->setY(1);

    idToElementMap[p1->getID()] = p1;
    idToElementMap[p2->getID()] = p2;

    Screen *screen = new Screen(screenID, gle, idToElementMap);
    screens->manageScreen(screen);
    return screen;
}

ScreenElement *ScreenParser::createTestScreenElement(ScreenElement *parent)
{
    static unsigned nCreated = 0;
    const QString id = "testelement" + QString::number(nCreated++);
    ButtonElement *e = new ButtonElement(id, parent);
    e->setText(QString::fromUtf8("TestScreenElement"));
    return e;
}

ButtonGroup *
ScreenParser::createButtonGroup(const QDomElement &a_pElement,
                                const ScreenElementMap &p_idToElementMap)
{
    ButtonGroup *pRet = new ButtonGroup(a_pElement.attribute(gc_sID));

    for (QDomElement currentNode = a_pElement.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        QString id = currentNode.attribute(gc_sID);
        if (p_idToElementMap.find(id) == p_idToElementMap.end()) {
            throw ApexStringException("Unknown screenelement " + id +
                                      " in buttongroup");
        }
        pRet->append(id);
    }

    return pRet;
}

void ScreenParser::addUnknownTag(const QString &ac_sSource,
                                 const QString &ac_sTag)
{
    qCCritical(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(
                                  ac_sSource, "Unknown Tag: " + ac_sTag)));
}

ScreenParser::mt_FeedBackPaths *
ScreenParser::parseFeedBackPaths(const QDomElement &a_pElement,
                                 const QString &ac_sElementID)
{
    mt_FeedBackPaths *pRet = new mt_FeedBackPaths();

    for (QDomElement currentNodee = a_pElement.firstChildElement();
         !currentNodee.isNull();
         currentNodee = currentNodee.nextSiblingElement()) {
        const QString tag(currentNodee.tagName());
        const QString value(currentNodee.text());
        if (tag == "highlight") {
            QString p =
                f_CheckPath(FilePrefixConvertor::convert(m_sPath), value);
            if (p.isEmpty()) {
                delete pRet;
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "ScreenParser::parseFeedBackPaths",
                        ac_sElementID + ": " + value + " doesn't exist")));
                return 0;
            }
            pRet->m_sHighLight = value;
        } else if (tag == "positive") {
            QString p =
                f_CheckPath(FilePrefixConvertor::convert(m_sPath), value);
            if (p.isEmpty()) {
                delete pRet;
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "ScreenParser::parseFeedBackPaths",
                        ac_sElementID + ": " + value + " doesn't exist")));
                return 0;
            }
            pRet->m_sPositive = value;
        } else if (tag == "negative") {
            QString p =
                f_CheckPath(FilePrefixConvertor::convert(m_sPath), value);
            if (p.isEmpty()) {
                delete pRet;
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "ScreenParser::parseFeedBackPaths",
                        ac_sElementID + ": " + value + " doesn't exist")));
                return 0;
            }
            pRet->m_sNegative = value;
        } else if (tag == "disabled") {
            QString p =
                f_CheckPath(FilePrefixConvertor::convert(m_sPath), value);
            if (p.isEmpty()) {
                delete pRet;
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "ScreenParser::parseFeedBackPaths",
                        ac_sElementID + ": " + value + " doesn't exist")));
                return 0;
            }
            pRet->m_sDisabled = value;
        } else {
            addUnknownTag("ScreenParser::parseFeedBackPaths", tag);
        }
    }

    return pRet;
}

ScreenLayoutElement *ScreenParser::createLayout(const QDomElement &element,
                                                ScreenElement *parent,
                                                ScreenElementMap &elements)
{
    ScreenLayoutElement *ret = 0;

    const QString tag = element.tagName();
    QDomElement it = element;

    // it should be a layout?
    Q_ASSERT(sf_bIsLayout(tag));

    // get common stuff
    QString width = it.attribute(gc_sWidth);
    QString height = it.attribute(gc_sHeight);
    QString id = it.attribute(gc_sID);
    QString x = it.attribute(gc_sX);
    QString y = it.attribute(gc_sY);
    QString row = it.attribute(gc_sRow);
    QString col = it.attribute(gc_sCol);

    if (!x.isEmpty() && !col.isEmpty()) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ScreenParser::createLayout",
                       id + tr(": x and col cannot both be defined"))));
        throw(0);
    }
    if (!y.isEmpty() && !row.isEmpty()) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ScreenParser::createLayout",
                       id + tr(": x and col cannot both be defined"))));
        throw(0);
    }

    unsigned nX;
    if (!col.isEmpty())
        nX = col.toUInt();
    else
        nX = x.toUInt();

    unsigned nY;
    if (!row.isEmpty())
        nY = row.toUInt();
    else
        nY = y.toUInt();

    if (x.isEmpty() && col.isEmpty())
        nX = 1;
    if (y.isEmpty() && row.isEmpty())
        nY = 1;

    if (tag == gc_sVertLayout || tag == gc_sHoriLayout ||
        tag == gc_sGridLayout) {
        unsigned nW = width.toUInt();
        unsigned nH = height.toUInt();

        // columnstretch and rowstretch
        QString sRowstretch = it.attribute(QSL("rowstretch"));
        QString sColstretch = it.attribute(QSL("columnstretch"));

        data::tStretchList rowstretch;
        if (!sRowstretch.isEmpty()) {
            rowstretch = parseStretchList(sRowstretch);
            //            qCDebug(APEX_RS, "SRowstretch=%s, rowstretch.size=%d",
            //            qPrintable(sRowstretch), rowstretch.size());
            if (rowstretch.size() != (int)nH)
                throw ApexStringException(
                    tr("The number of row stretch factors for layout %1 is not "
                       "equal to its height")
                        .arg(id));
        }

        data::tStretchList colstretch;
        if (!sColstretch.isEmpty()) {
            colstretch = parseStretchList(sColstretch);
            if (colstretch.size() != (int)nW)
                throw ApexStringException(
                    tr("The number of column stretch factors for layout %1 is "
                       "not equal to its width")
                        .arg(id));
        }

        if (tag == gc_sVertLayout)
            nW = 1;
        if (tag == gc_sHoriLayout)
            nH = 1;
        if (nH == 0 || nW == 0 || nW > sc_nMaxElementsInRow ||
            nH > sc_nMaxElementsInCol) {
            qCCritical(APEX_RS, "%s",
                       qPrintable(QSL("%1: %2").arg(
                           "ScreenParser::createLayout",
                           id + ": Width or Height not in range")));
            throw(0);
        }

        ret = new GridLayoutElement(id, parent, nW, nH, rowstretch, colstretch);
    } else if (tag == gc_sArcLayout) {
        const unsigned nW = width.toUInt();
        if (nW == 0) {
            qCCritical(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(
                                          "ScreenParser::createLayout",
                                          id + ": Width must be > 0")));
            throw(0);
        }
        const QString sType(it.attribute(QSL("type")));

        ::ArcLayout::ArcType eType = ::ArcLayout::ARC_TOP;
        if (sType == "lower")
            eType = ::ArcLayout::ARC_BOTTOM;
        else if (sType == "left")
            eType = ::ArcLayout::ARC_LEFT;
        else if (sType == "right")
            eType = ::ArcLayout::ARC_RIGHT;
        else if (sType == "full")
            eType = ::ArcLayout::ARC_FULL;

        ArcLayoutElement *p = new ArcLayoutElement(id, parent, nW, eType);

        ret = p;
    } else if (tag == gc_sTwoPartLayout) {

        throw ApexStringException(tr("TwoPartlayout is obsolete. Please use "
                                     "gridlayout with stretchfactors instead"));

    } else {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg("ScreenParser::createLayout",
                                                id + ": unknown layout type")));
        throw(0);
    }

    elements[id] = ret;

    ret->setX(nX - 1);
    ret->setY(nY - 1);

    // go deeper, return value is unused since created elements
    // automatically become children of ret
    for (QDomElement itt = element.firstChildElement(); !itt.isNull();
         itt = itt.nextSiblingElement()) {
        ScreenElement *childel = createElement(itt, ret, elements);
        if (childel) {
            ret->checkChild(childel);
        } else {
            throw ApexStringException("Error parsing child element of layout");
        }
    }

    return ret;
}

ScreenElement *ScreenParser::createElement(const QDomElement &element,
                                           ScreenElement *parent,
                                           ScreenElementMap &elements)
{
    const QString tag = element.tagName();

    if (sf_bIsLayout(tag))
        return createLayout(element, parent, elements);
    else
        return createNonLayoutElement(element, parent, elements);
}

ScreenElement *ScreenParser::createNonLayoutElement(const QDomElement &element,
                                                    ScreenElement *parent,
                                                    ScreenElementMap &elements)
{
    ScreenElement *ret = 0;
    const QString tag = element.tagName();
    const QString id = element.attribute(gc_sID);
    QString x = element.attribute(gc_sX);
    QString y = element.attribute(gc_sY);
    QString row = element.attribute(gc_sRow);
    QString col = element.attribute(gc_sCol);

    if (!x.isEmpty() && !col.isEmpty()) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ScreenParser::createLayout",
                       id + tr(": x and col cannot both be defined"))));
        throw(0);
    }
    if (!y.isEmpty() && !row.isEmpty()) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ScreenParser::createLayout",
                       id + tr(": x and col cannot both be defined"))));
        throw(0);
    }

    unsigned nX;
    if (!col.isEmpty())
        nX = col.toUInt();
    else
        nX = x.toUInt();

    unsigned nY;
    if (!row.isEmpty())
        nY = row.toUInt();
    else
        nY = y.toUInt();

    if (x.isEmpty() && col.isEmpty())
        nX = 1;
    if (y.isEmpty() && row.isEmpty())
        nY = 1;

    Q_ASSERT(!sf_bIsLayout(tag));
    Q_ASSERT(parent);

    if (tag == gc_sButton) {
        ButtonElement *temp = new ButtonElement(id, parent);
        temp->setPrefix(m_sPath);

        QDomElement te =
            element.elementsByTagName(gc_sText).item(0).toElement();
        if (!te.isNull())
            temp->setText(te.text());

        ret = temp;
    } else if (tag == gc_sSpinBox) {
        data::SpinBoxElement *temp = new data::SpinBoxElement(id, parent);
        temp->setPrefix(m_sPath);

        qCDebug(APEX_RS, "Creating spinbox %s", qPrintable(id));

        QDomElement te =
            element.elementsByTagName(gc_sValue).item(0).toElement();
        if (!te.isNull())
            temp->SetValue(te.text().toFloat());

        te = element.elementsByTagName(gc_sMin).item(0).toElement();
        if (!te.isNull())
            temp->SetMin(te.text().toFloat());

        te = element.elementsByTagName(gc_sMax).item(0).toElement();
        if (!te.isNull())
            temp->SetMax(te.text().toFloat());

        te = element.elementsByTagName(gc_sStep).item(0).toElement();
        if (!te.isNull())
            temp->SetStep(te.text().toFloat());

        te = element.elementsByTagName(gc_sParameter).item(0).toElement();
        if (!te.isNull())
            temp->SetParameter(te.text());

        te = element.elementsByTagName(gc_sReset).item(0).toElement();
        if (!te.isNull())
            temp->SetReset(te.text() == "true");

        ret = temp;
    } else if (tag == gc_sMatrix) {
        data::MatrixElement *temp = new data::MatrixElement(id, parent);
        temp->setPrefix(m_sPath);

        for (QDomElement currentNode = element.firstChildElement();
             !currentNode.isNull();
             currentNode = currentNode.nextSiblingElement()) {
            QString tag(currentNode.tagName());
            if (tag == "autocontinue") {
                QString value(currentNode.text());
                temp->setAutoContinue((value == "true") ? true : false);
            } else if (tag == "element") {
                int row = currentNode.attribute(QSL("row")).toInt();
                int col = currentNode.attribute(QSL("col")).toInt();
                QString name;
                QString text;

                for (QDomElement c = currentNode.firstChildElement();
                     !c.isNull(); c = c.nextSiblingElement()) {
                    QString tag(c.tagName());
                    QString value(c.text());
                    if (tag == "name")
                        name = value;
                    else if (tag == "text")
                        text = value;
                    else
                        Q_ASSERT("Invalid tag");
                }
                if (text.isEmpty())
                    text = name;

                data::MatrixElement::MatrixButton newButton(name, text);
                temp->setButton(row, col, newButton);
            }
        }

        ret = temp;
    } else if (tag == gc_sTextEdit) {
        TextEditElement *temp = new TextEditElement(id, parent);
        temp->setPrefix(m_sPath);

        QDomElement te =
            element.elementsByTagName(gc_sText).item(0).toElement();
        if (!te.isNull())
            temp->setText(te.text());

        te = element.elementsByTagName(gc_sInputmask).item(0).toElement();
        if (!te.isNull()) {
            temp->setInputMask(te.text());
        }

        ret = temp;
    } else if (tag == gc_sPicture) {
        PictureElement *temp = new PictureElement(id, parent);
        temp->setPrefix(m_sPath);

        for (QDomElement currentNode = element.firstChildElement();
             !currentNode.isNull();
             currentNode = currentNode.nextSiblingElement()) {
            QString tag(currentNode.tagName());
            if (tag == QSL("file")) {
                QString filename = currentNode.text();
                QString p = f_CheckPath(FilePrefixConvertor::convert(m_sPath),
                                        filename);
                if (p.isEmpty()) {
                    delete temp;
                    qCCritical(
                        APEX_RS, "%s",
                        qPrintable(QSL("%1: %2").arg(
                            "ScreenParser::createNonLayoutElement",
                            id + ": " + FilePrefixConvertor::convert(m_sPath) +
                                "/" + filename + " doesn't exist")));
                    throw ApexStringException("Error parsing screen");
                }
                // check whether it is a valid picture
                QImage testimage(p);
                if (testimage.isNull()) {
                    qCCritical(APEX_RS, "%s",
                               qPrintable(QSL("%1: %2").arg(
                                   "ScreenParser::createNonLayoutElement",
                                   id + ": " + p +
                                       " invalid picture.\nCheck whether the "
                                       "file is not corrupt and you have the "
                                       "necessary image plugins.")));
                    return 0;
                }
                temp->setDefault(filename);
                temp->setFileId(currentNode.attribute(QSL("id")));
                if (!temp->getFileId().isEmpty() && parameterManagerData) {
                    parameterManagerData->registerParameter(
                        temp->getFileId(),
                        data::Parameter("Picture", "string", temp->getDefault(),
                                        0, true, temp->getFileId()));
                }
            } else if (tag == gc_sFeedback) {
                mt_FeedBackPaths *p = parseFeedBackPaths(currentNode, id);
                if (p) {
                    temp->setHighlight(p->m_sHighLight);
                    temp->setPositive(p->m_sPositive);
                    temp->setNegative(p->m_sNegative);
                    temp->setDisabled(p->m_sDisabled);
                    delete p;
                } else {
                    delete temp;
                    return 0;
                }
                temp->setOverrideFeedback(true);
            } else if (tag == gc_sShortcut) {
                try {
                    QKeySequence seq = f_CreateKeySequence(currentNode);
                    temp->setShortCut(seq.toString());
                } catch (const std::exception &e) {
                    qCCritical(APEX_RS,
                               "ScreenParser::createNonLayoutElement: %s",
                               e.what());
                }
            } else {
                addUnknownTag("ScreenParser::createNonLayoutElement", tag);
            }
        }

        ret = temp;
    } else if (tag == gc_sLabel) {
        LabelElement *temp = new LabelElement(id, parent);
        temp->setPrefix(m_sPath);

        QDomElement te =
            element.elementsByTagName(gc_sText).item(0).toElement();
        if (!te.isNull())
            temp->setText(te.text());

        ret = temp;
    } else if (tag == gc_sPictureLabel) {
        PictureLabelElement *temp = new PictureLabelElement(id, parent);
        temp->setPrefix(m_sPath);

        QDomElement te =
            element.elementsByTagName(QSL("file")).item(0).toElement();
        if (!te.isNull()) {
            QString filename = te.text();
            QString p =
                f_CheckPath(FilePrefixConvertor::convert(m_sPath), filename);
            if (p.isEmpty()) {
                delete temp;
                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "ScreenParser::createNonLayoutElement",
                               id + ": " + p + " doesn't exist")));
                return 0;
            }

            // check whether it is a valid picture
            QImage testimage(p);
            if (testimage.isNull()) {
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "ScreenParser::createNonLayoutElement",
                        id + ": " + p + " invalid picture.\nCheck whether the "
                                        "file is not corrupt and you have the "
                                        "necessary image plugins.")));
                return 0;
            }
            temp->setPicture(filename);
        } else {
            Q_ASSERT(0);
        }

        te = element.elementsByTagName("disabledfile").item(0).toElement();
        if (!te.isNull()) {
            QString filename = te.text();
            QString p =
                f_CheckPath(FilePrefixConvertor::convert(m_sPath), filename);
            if (p.isEmpty()) {
                delete temp;
                qCCritical(APEX_RS, "%s",
                           qPrintable(QSL("%1: %2").arg(
                               "ScreenParser::createNonLayoutElement",
                               id + ": " + p + " doesn't exist")));
                return 0;
            }

            // check whether it is a valid picture
            QImage testimage(p);
            if (testimage.isNull()) {
                qCCritical(
                    APEX_RS, "%s",
                    qPrintable(QSL("%1: %2").arg(
                        "ScreenParser::createNonLayoutElement",
                        id + ": " + p + " invalid picture.\nCheck whether the "
                                        "file is not corrupt and you have the "
                                        "necessary image plugins.")));
                return 0;
            }
            temp->setPictureDisabled(filename);
        }

        ret = temp;
    } else if (tag == gc_sFlash) {
#ifndef FLASH
// throw ApexStringException("this apex version does not support flash");
#endif

        FlashPlayerElement *temp = new FlashPlayerElement(id, parent);
        temp->setPrefix(m_sPath);
        for (QDomElement currentNode = element.firstChildElement();
             !currentNode.isNull();
             currentNode = currentNode.nextSiblingElement()) {
            QString tag(currentNode.tagName());
            if (tag == QSL("file")) {
                QString filename = currentNode.text();
                QString p = f_CheckPath(m_sPath, filename);
                if (p.isEmpty()) {
                    delete temp;
                    throw(ApexStringException(
                        id + ": cannot find movie file " +
                        FilePrefixConvertor::convert(m_sPath) + filename));
                    //          return 0;
                } else {
                    qCDebug(APEX_RS, "File %s found", qPrintable(p));
                }
                temp->setDefault(filename);
            } else if (tag == gc_sFeedback) {
                mt_FeedBackPaths *p = parseFeedBackPaths(currentNode, id);
                if (p) {
                    temp->setHighlight(p->m_sHighLight);
                    temp->setPositive(p->m_sPositive);
                    temp->setNegative(p->m_sNegative);
                    delete p;
                } else {
                    delete temp;
                    return 0;
                }
                temp->setOverrideFeedback(true);
            } else if (tag == gc_sShortcut) {
                try {
                    QKeySequence seq = f_CreateKeySequence(currentNode);
                    temp->setShortCut(seq.toString());
                } catch (const std::exception &e) {
                    qCCritical(APEX_RS,
                               "ScreenParser::createNonLayoutElement: %s",
                               e.what());
                }
            } else {
                addUnknownTag("ScreenParser::createNonLayoutElement", tag);
            }
        }

        ret = temp;
    } else if (tag == gc_sAnswerLabel) {
        AnswerLabelElement *temp = new AnswerLabelElement(id, parent);
        temp->setPrefix(m_sPath);
        ret = temp;
    } else if (tag == gc_sParameterList) {
        ParameterListElement *temp = new ParameterListElement(id, parent);
        temp->setPrefix(m_sPath);

        for (QDomElement currentNode = element.firstChildElement();
             !currentNode.isNull();
             currentNode = currentNode.nextSiblingElement()) {
            QString tag = currentNode.tagName();
            QString value = currentNode.text();
            QString name = currentNode.attribute(QSL("name"));
            QString expression = currentNode.attribute(QSL("expression"));

            if (tag == "parameter") {
                bool result = temp->addParameter(value, name, expression);

                if (!result) {
                    qCCritical(APEX_RS, "%s",
                               qPrintable(QSL("%1: %2").arg(
                                   "ScreenParser::createNonLayoutElement" + id,
                                   "invalid expression:" + expression)));
                }
            }
        }

        //     m_pControl.connect(&m_pControl,
        //     SIGNAL(NewStimulus(stimulus::Stimulus* )), temp,
        //     SLOT(NewStimulus(stimulus::Stimulus*)));

        //     temp->mp_SetGeometry( ScreenElement::mt_Geometry( 0.9, 0.9 ) );

        ret = temp;
    } else if (tag == "parameterlabel") {
        ParameterLabelElement *temp = new ParameterLabelElement(id, parent);
        temp->setPrefix(m_sPath);

        QDomElement te =
            element.elementsByTagName("parameter").item(0).toElement();
        if (te.isNull())
            qFatal("Parameter element not found");

        QString value = te.text();
        QString name = te.attribute(QSL("name"));
        QString expression = te.attribute(QSL("expression"));

        temp->setParameter(ParameterData(value, name, expression));

        ret = temp;

    } else if (tag == "slider") {
        SliderElement *temp = new SliderElement(id, parent);
        temp->setPrefix(m_sPath);

        for (QDomElement currentNode = element.firstChildElement();
             !currentNode.isNull();
             currentNode = currentNode.nextSiblingElement()) {
            QString tag = currentNode.tagName();
            QString value = currentNode.text();

            if (tag == "orientation") {
                if (value == "horizontal")
                    temp->SetOrient(Qt::Horizontal);
                else if (value == "vertical")
                    temp->SetOrient(Qt::Vertical);
                else
                    qFatal("Invalid orientation");
            } else if (tag == "min") {
                temp->SetMin(value.toFloat());
            } else if (tag == "max") {
                temp->SetMax(value.toFloat());
            } else if (tag == "value") {
                temp->SetValue(value.toFloat());
            } else if (tag == "tickinterval") {
                temp->SetTickInt(value.toInt());
            } else if (tag == "stepsize") {
                temp->SetStepSize(value.toInt());
            } else if (tag == "pagesize") {
                temp->SetPageSize(value.toInt());
            } else {
                qFatal("Invalid tag");
            }
        }
        ret = temp;
    } else if (tag == gc_sHtml) {
        HtmlElement *temp = new HtmlElement(id, parent);
        temp->setPrefix(m_sPath);

        QDomElement te = element.elementsByTagName("page").item(0).toElement();
        if (te.isNull())
            qFatal("Parameter page not found");

        QString value = te.text();

        temp->setPage(value);

        ret = temp;
    } else if (tag == gc_sCheckBox) {
        CheckBoxElement *temp = new CheckBoxElement(id, parent);
        temp->setPrefix(m_sPath);
        QDomElement te =
            element.elementsByTagName(gc_sText).item(0).toElement();
        if (!te.isNull())
            temp->setText(te.text());
        ret = temp;
    } else if (tag == gc_sNumericKeypad) {
        ret = new NumericKeypadElement(id, parent);
    } else
        return 0;

    ret->setX(nX - 1);
    ret->setY(nY - 1);

    // generic attributes
    for (QDomElement currentNode = element.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        QString tag = currentNode.tagName();
        QString value = currentNode.text();

        if (tag == gc_sFontsize) {
            ret->setFontSize(value.toInt());
        } else if (tag == "font") {
            ret->setFont(value);
        } else if (tag == gc_sFGColor) {
            if (!checkColor(value, ret->getID())) {
                delete ret;
                return 0;
            }
            ret->setFGColor(value);
        } else if (tag == gc_sBGColor) {
            if (!checkColor(value, ret->getID())) {
                delete ret;
                return 0;
            }
            ret->setBGColor(value);
        } else if (tag == "style") {
            ret->setStyle(value);
        } else if (tag == gc_sDisabled) {
            ret->setDisabled(apex::ApexTools::bQStringToBoolean(value));
        } else if (tag == gc_sShortcut) {
            try {
                QKeySequence seq = f_CreateKeySequence(currentNode);
                QString action(currentNode.attribute(QSL("action")));
                if (action.isEmpty())
                    action = QLatin1String("click");
                ret->setShortCut(seq.toString(), action);
            } catch (const std::exception &e) {
                qCCritical(APEX_RS, "ScreenParser::createNonLayoutElement: %s",
                           e.what());
            }
        }
    }

    elements[ret->getID()] = ret;
    return ret;
}

bool ScreenParser::checkColor(const QString &value,
                              const QString &element) const
{
    QColor tc(value);
    if (!tc.isValid()) {
        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ScreenParser::createNonLayoutElement",
                       "Invalid foreground color string \"" + value +
                           "\" for element " + element)));
        return false;
    }
    return true;
}
}
}

apex::data::tStretchList
apex::gui::ScreenParser::parseStretchList(const QString &d)
{
    QStringList temp(d.split(","));
    data::tStretchList result;
    for (QStringList::const_iterator it = temp.begin(); it != temp.end();
         ++it) {
        result.append(it->toInt());
    }
    return result;
}
