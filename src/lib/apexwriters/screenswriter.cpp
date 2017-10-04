/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdata/screen/screen.h"
#include "apexdata/screen/answerlabelelement.h"
#include "apexdata/screen/arclayoutelement.h"
#include "apexdata/screen/buttonelement.h"
#include "apexdata/screen/buttongroup.h"
#include "apexdata/screen/emptyelement.h"
#include "apexdata/screen/flashplayerelement.h"
#include "apexdata/screen/gridlayoutelement.h"
#include "apexdata/screen/labelelement.h"
#include "apexdata/screen/parameterdata.h"
#include "apexdata/screen/parameterlabelelement.h"
#include "apexdata/screen/parameterlistelement.h"
#include "apexdata/screen/pictureelement.h"
#include "apexdata/screen/picturelabelelement.h"
#include "apexdata/screen/screenelementvisitor.h"
#include "apexdata/screen/screensdata.h"
#include "apexdata/screen/texteditelement.h"

#include "apextools/apextools.h"

#include "apextools/gui/arclayout.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "fileprefixwriter.h"
#include "screenswriter.h"

#include <QtDebug>

using namespace apex;
using namespace apex::data;

using apex::writer::ScreensWriter;

class ElementToXMLVisitor : public ScreenElementVisitor
{
    QDomDocument *domDocument;
    QDomElement result;

    void visitAnswerLabel(const AnswerLabelElement *e);
    void visitArcLayout(const ArcLayoutElement *e);
    void visitButton(const ButtonElement *e);
    void visitEmpty(const EmptyElement *e);
    void visitFlashPlayer(const FlashPlayerElement *e);
    void visitGridLayout(const GridLayoutElement *e);
    void visitLabel(const LabelElement *e);
    void visitParameterList(const ParameterListElement *e);
    void visitPicture(const PictureElement *e);
    void visitPictureLabel(const PictureLabelElement *e);
    void visitTextEdit(const TextEditElement *e);
    void visitParameterLabel(const ParameterLabelElement *e);

public:
    ElementToXMLVisitor(QDomDocument *d) : domDocument(d)
    {
    }
    QDomElement getResult()
    {
        return result;
    }

    QDomElement elementToXML(const ScreenElement *el);

private:
    void addXYParams(QDomElement de, const ScreenElement *e);
    void addIDParam(QDomElement de, const ScreenElement *e);
    void addFontSizeChild(QDomElement de, const ScreenElement *se);
    void addLayoutChildren(QDomElement de, const ScreenLayoutElement *sle);
    void addTextChild(QDomElement de, const QString &text);
    void addShortcutChild(QDomElement de, const ScreenElement *e);
    void addFeedbackChild(QDomElement de, bool overrideFeedback,
                          const QString &highlight, const QString &positive,
                          const QString &negative);
};

void ElementToXMLVisitor::addShortcutChild(QDomElement de,
                                           const ScreenElement *e)
{
    Q_UNUSED(de);
    Q_UNUSED(e);
    // const QKeySequence seq = e->getShortCut();
    QDomElement shortcutChild = domDocument->createElement(QSL("shortcut"));
    // TODO: add better shortcut format in xml schema,
    // based on QKeySequence::toString()
    Q_UNUSED(shortcutChild);
}

void ElementToXMLVisitor::addTextChild(QDomElement de, const QString &text)
{
    QDomElement textChild = domDocument->createElement(QSL("text"));
    textChild.appendChild(domDocument->createTextNode(text));
    de.appendChild(textChild);
}

void ElementToXMLVisitor::addLayoutChildren(QDomElement de,
                                            const ScreenLayoutElement *sle)
{
    for (int i = 0; i < sle->getNumberOfChildren(); ++i) {
        const ScreenElement *child = sle->getChild(i);
        ElementToXMLVisitor v(domDocument);
        QDomElement childEl = v.elementToXML(child);
        if (!childEl.isNull()) {
            de.appendChild(childEl);
        }
    }
}

void ElementToXMLVisitor::addFontSizeChild(QDomElement de,
                                           const ScreenElement *se)
{
    if (se->getFontSize() != -1) {
        // font size differs from default...
        QDomElement child = domDocument->createElement(QSL("fontsize"));
        child.appendChild(
            domDocument->createTextNode(QString::number(se->getFontSize())));
        de.appendChild(child);
    }
}

void ElementToXMLVisitor::addXYParams(QDomElement de, const ScreenElement *se)
{
    const ScreenElement *parent = se->getParent();
    if (parent) {
        de.setAttribute(QSL("col"), QString::number(se->getX() + 1));
        if (parent->elementType() == ScreenElement::GridLayout)
            de.setAttribute(QSL("row"), QString::number(se->getY() + 1));
    }
}

void ElementToXMLVisitor::addIDParam(QDomElement de, const ScreenElement *se)
{
    if (!se->getID().isEmpty())
        de.setAttribute(QSL("id"), se->getID());
}

void ElementToXMLVisitor::addFeedbackChild(QDomElement de,
                                           bool overrideFeedback,
                                           const QString &highlight,
                                           const QString &positive,
                                           const QString &negative)
{
    if (overrideFeedback) {
        QDomElement feedbackElem = domDocument->createElement(QSL("feedback"));
        if (!highlight.isEmpty()) {
            QDomElement highlightElem =
                domDocument->createElement(QSL("highlight"));
            highlightElem.appendChild(domDocument->createTextNode(highlight));
            feedbackElem.appendChild(highlightElem);
        }
        if (!positive.isEmpty()) {
            QDomElement positiveElem =
                domDocument->createElement(QSL("positive"));
            positiveElem.appendChild(domDocument->createTextNode(positive));
            feedbackElem.appendChild(positiveElem);
        }
        if (!negative.isEmpty()) {
            QDomElement negativeElem =
                domDocument->createElement(QSL("negative"));
            negativeElem.appendChild(domDocument->createTextNode(negative));
            feedbackElem.appendChild(negativeElem);
        }
        de.appendChild(feedbackElem);
    }
}

void ElementToXMLVisitor::visitAnswerLabel(const AnswerLabelElement *e)
{
    result = domDocument->createElement(QSL("answerlabel"));
    addXYParams(result, e);
    addIDParam(result, e);
    addFontSizeChild(result, e);
}

void ElementToXMLVisitor::visitArcLayout(const ArcLayoutElement *e)
{
    result = domDocument->createElement(QSL("arcLayout"));
    addXYParams(result, e);
    addIDParam(result, e);
    addLayoutChildren(result, e);
    result.setAttribute(QSL("width"), QString::number(e->getWidth()));
    ArcLayout::ArcType type = (ArcLayout::ArcType)e->getType();
    QString stype;
    switch (type) {
    case ArcLayout::ARC_TOP:
        stype = QLatin1String("upper");
        break;
    case ArcLayout::ARC_BOTTOM:
        stype = QLatin1String("lower");
        break;
    case ArcLayout::ARC_LEFT:
        stype = QLatin1String("left");
        break;
    case ArcLayout::ARC_RIGHT:
        stype = QLatin1String("right");
        break;
    case ArcLayout::ARC_FULL:
        stype = QLatin1String("full");
        break;
    }
    result.setAttribute(QSL("type"), stype);
}

void ElementToXMLVisitor::visitButton(const ButtonElement *e)
{
    result = domDocument->createElement(QSL("button"));
    addXYParams(result, e);
    addIDParam(result, e);
    addFontSizeChild(result, e);
    addTextChild(result, e->text());
    addShortcutChild(result, e);
}

void ElementToXMLVisitor::visitEmpty(const EmptyElement *)
{
}

void ElementToXMLVisitor::visitFlashPlayer(const FlashPlayerElement *e)
{
    result = domDocument->createElement(QSL("flash"));
    addXYParams(result, e);
    addIDParam(result, e);
    QDomElement pathElem = domDocument->createElement(QSL("file"));
    pathElem.appendChild(domDocument->createTextNode(e->getDefault()));
    result.appendChild(pathElem);
    addFeedbackChild(result, e->getOverrideFeedback(), e->getHighlight(),
                     e->getPositive(), e->getNegative());
}

void ElementToXMLVisitor::visitGridLayout(const GridLayoutElement *e)
{
    result = domDocument->createElement(QSL("gridLayout"));
    addXYParams(result, e);
    addIDParam(result, e);
    result.setAttribute(QSL("width"), QString::number(e->getWidth()));
    result.setAttribute(QSL("height"), QString::number(e->getHeight()));

    // columnstretch
    tStretchList stretch = e->getColumnStretches();
    QStringList stretchStrings = ApexTools::toStringList(stretch);

    if (!stretchStrings.isEmpty())
        result.setAttribute(QSL("columnstretch"), stretchStrings.join(","));

    // rowstretch
    stretch = e->getRowStretches();
    stretchStrings = ApexTools::toStringList(stretch);

    if (!stretchStrings.isEmpty())
        result.setAttribute(QSL("rowstretch"), stretchStrings.join(","));

    addLayoutChildren(result, e);
}

void ElementToXMLVisitor::visitLabel(const LabelElement *e)
{
    result = domDocument->createElement(QSL("label"));
    addXYParams(result, e);
    addIDParam(result, e);
    addFontSizeChild(result, e);
    addTextChild(result, e->getText());
    addShortcutChild(result, e);
}

void ElementToXMLVisitor::visitParameterList(const ParameterListElement *e)
{
    result = domDocument->createElement(QSL("parameterlist"));
    addXYParams(result, e);
    addIDParam(result, e);
    typedef ParameterListElement::ParameterListT ParlistT;
    typedef ParameterData PardataT;
    const ParlistT &parlist = e->getParameterList();
    for (ParlistT::const_iterator i = parlist.begin(); i != parlist.end();
         ++i) {
        const PardataT &pardata = *i;
        QDomElement parElem = domDocument->createElement(QSL("parameter"));
        if (!pardata.name.isEmpty())
            parElem.setAttribute(QSL("name"), pardata.name);
        if (!pardata.expression.isEmpty())
            parElem.setAttribute(QSL("expression"), pardata.expression);
        parElem.appendChild(domDocument->createTextNode(pardata.id));
        result.appendChild(parElem);
    }
}

void ElementToXMLVisitor::visitPicture(const PictureElement *e)
{
    result = domDocument->createElement(QSL("picture"));
    addXYParams(result, e);
    addIDParam(result, e);
    QDomElement pathElem = domDocument->createElement(QSL("file"));
    pathElem.appendChild(domDocument->createTextNode(e->getDefault()));
    result.appendChild(pathElem);
    addFeedbackChild(result, e->getOverrideFeedback(), e->getHighlight(),
                     e->getPositive(), e->getNegative());
}

void ElementToXMLVisitor::visitPictureLabel(const PictureLabelElement *e)
{
    result = domDocument->createElement(QSL("picturelabel"));
    addXYParams(result, e);
    addIDParam(result, e);
    QDomElement pathElem = domDocument->createElement(QSL("file"));
    pathElem.appendChild(domDocument->createTextNode(e->getPicture()));
    result.appendChild(pathElem);
}

void ElementToXMLVisitor::visitTextEdit(const TextEditElement *e)
{
    result = domDocument->createElement(QSL("textEdit"));
    addXYParams(result, e);
    addIDParam(result, e);

    QDomElement textElem = domDocument->createElement(QSL("text"));
    textElem.appendChild(domDocument->createTextNode(e->getText()));
    result.appendChild(textElem);

    QString inputMask = e->getInputMask();
    if (!inputMask.isEmpty()) {
        QDomElement imElem = domDocument->createElement(QSL("inputmask"));
        imElem.appendChild(domDocument->createTextNode(inputMask));
        result.appendChild(imElem);
    }
}

void ElementToXMLVisitor::visitParameterLabel(const ParameterLabelElement *e)
{
    result = domDocument->createElement(QSL("parameterlabel"));
    addXYParams(result, e);
    addIDParam(result, e);

    ParameterData paramData = e->getParameter();
    QDomElement param = XmlUtils::createTextElement(domDocument, "parameter",
                                                    paramData.id); // argh...
    result.appendChild(param);
    qCDebug(APEX_RS) << "visited parameter label";
}

QDomElement ElementToXMLVisitor::elementToXML(const ScreenElement *e)
{
    e->visit(this);
    QDomElement ret = result;
    result = QDomElement();
    return ret;
}

QDomElement ScreensWriter::addElement(QDomDocument *doc,
                                      const data::ScreensData &data,
                                      const QStringList &screenList)
{
    QDomElement rootElem = doc->documentElement();
    QDomElement screens = doc->createElement(QSL("screens"));
    rootElem.appendChild(screens);

    // prefix
    if (data.hasPrefix()) {
        FilePrefixWriter fpwriter;
        screens.appendChild(fpwriter.addElement(doc, data.prefix()));
    }

    // general
    if (data.hasGeneralScreenData())
        screens.appendChild(addGeneral(doc, data));

    // reinforcement
    if (data.hasReinforcement())
        screens.appendChild(addReinforcement(doc, data));

    // style_apex
    if (data.hasApexStyle()) {
        screens.appendChild(
            XmlUtils::createTextElement(doc, "apex_style", data.apexStyle()));
    }

    // style
    if (data.hasStyle()) {
        screens.appendChild(
            XmlUtils::createTextElement(doc, "style", data.style()));
    }

    // childmode
    if (data.hasChildmode()) {
        screens.appendChild(addChildmode(doc, data));
    }

    // defaultFont
    if (data.hasDefaultFont()) {
        screens.appendChild(XmlUtils::createTextElement(doc, "defaultFont",
                                                        data.defaultFont()));
    }

    // defaultFontsize
    if (data.hasDefaultFontSize()) {
        screens.appendChild(XmlUtils::createTextElement(
            doc, "defaultFontsize", data.defaultFontSize()));
    }

    // screens
    if (screenList.isEmpty()) {
        std::map<QString, Screen *> map = data.getScreens();
        std::map<QString, Screen *>::const_iterator it;
        for (it = map.begin(); it != map.end(); it++) {
            QDomElement screen = addScreen(doc, *it->second);
            screen.setAttribute(QSL("id"), it->first);
            screens.appendChild(screen);
        }
    } else {
        Q_FOREACH (const QString &screen, screenList) {
            screens.appendChild(doc->importNode(
                XmlUtils::parseString(screen).documentElement(), true));
        }
    }

    return screens;
}

QDomElement ScreensWriter::addScreen(QDomDocument *doc,
                                     const data::Screen &data)
{
    QDomElement e = doc->createElement(QSL("screen"));

    e.setAttribute(QSL("id"), data.getID());

    QDomElement layoutEl = screenElementToXml(doc, *data.getRootElement());
    Q_ASSERT(!layoutEl.isNull());
    e.appendChild(layoutEl);

    const ButtonGroup *buttonGroup = data.getButtonGroup();
    if (buttonGroup) {
        QDomElement buttongroupEl = buttonGroupToXml(doc, *buttonGroup);
        e.appendChild(buttongroupEl);
    }

    QString defaultAnswer = data.getDefaultAnswerElement();
    if (!defaultAnswer.isEmpty()) {
        QDomElement defaultAnswerEl =
            doc->createElement(QSL("default_answer_element"));
        defaultAnswerEl.appendChild(doc->createTextNode(defaultAnswer));
        e.appendChild(defaultAnswerEl);
    }

    return e;
}

QDomElement ScreensWriter::screenElementToXml(QDomDocument *doc,
                                              const ScreenElement &e)
{
    ElementToXMLVisitor v(doc);
    return v.elementToXML(&e);
}

QDomElement ScreensWriter::buttonGroupToXml(QDomDocument *doc,
                                            const ButtonGroup &g)
{
    QDomElement e = doc->createElement(QSL("buttongroup"));

    e.setAttribute(QSL("id"), g.getID());

    for (ButtonGroup::const_iterator i = g.begin(); i != g.end(); ++i) {
        QString button = *i;
        QDomElement buttonEl = doc->createElement(QSL("button"));
        buttonEl.setAttribute(QSL("id"), button);
        e.appendChild(buttonEl);
    }
    return e;
}

QDomElement ScreensWriter::addReinforcement(QDomDocument *doc,
                                            const data::ScreensData &data)
{
    QDomElement reinforcement = doc->createElement(QSL("reinforcement"));

    // progressbar
    reinforcement.appendChild(XmlUtils::createTextElement(
        doc, "progressbar", data.hasProgressbarEnabled()));
    // feedback
    QDomElement feedback = XmlUtils::createTextElement(
        doc, "feedback", ApexTools::boolToString(data.hasFeedbackEnabled()));
    reinforcement.appendChild(feedback);

    if (data.hasFeedbackEnabled() || data.hasFeedbackLengthDefined()) {
        if (data.hasFeedbackLengthDefined()) {
            feedback.setAttribute(QSL("length"),
                                  QString::number(data.feedbackLength()));
        }

        // feedback_on
        reinforcement.appendChild(XmlUtils::createTextElement(
            doc, "feedback_on", data.feedbackOnString()));
        // feedback_picture_positive
        if (!data.feedbackPositivePicture().isNull()) {
            reinforcement.appendChild(
                XmlUtils::createTextElement(doc, "feedback_picture_positive",
                                            data.feedbackPositivePicture()));
        }
        // feedback_picture_negative
        if (!data.feedbackNegativePicture().isNull()) {
            reinforcement.appendChild(
                XmlUtils::createTextElement(doc, "feedback_picture_negative",
                                            data.feedbackNegativePicture()));
        }
    }

    // showcurrent
    if (data.hasShowCurrentEnabled()) {
        reinforcement.appendChild(
            XmlUtils::createTextElement(doc, "showcurrent", "true"));
    }

    return reinforcement;
}

QDomElement ScreensWriter::addGeneral(QDomDocument *doc,
                                      const data::ScreensData &data)
{
    Q_ASSERT(data.hasGeneralScreenData());

    QDomElement general = doc->createElement(QSL("general"));

    // stopbutton
    if (data.hasStopButtonEnabled()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "stopbutton", "true"));
    }

    // repeatbutton
    if (data.hasRepeatButtonEnabled()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "repeatbutton", "true"));
    }

    // showpanel
    if (data.hasPanelEnabled()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "showpanel", "true"));
    }

    // showmenu
    if (data.hasMenuEnabled()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "showmenu", "true"));
    }

    // fullscreen
    if (data.hasFullScreenEnabled()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "fullscreen", "true"));
    }

    // intertrialscreen
    if (data.hasInterTrialScreen()) {
        QDomElement its =
            XmlUtils::createTextElement(doc, "intertrialscreen", "true");
        its.setAttribute(QSL("length"),
                         QString::number(data.interTrialLength()));
        general.appendChild(its);
    }

    return general;
}

QDomElement ScreensWriter::addChildmode(QDomDocument *doc,
                                        const data::ScreensData &data)
{
    Q_ASSERT(data.hasChildmode());

    QDomElement childmode = doc->createElement(QSL("childmode"));

    // intro
    if (data.hasIntroScreen()) {
        QDomElement intro =
            XmlUtils::createTextElement(doc, "intro", data.introScreen());
        intro.setAttribute(QSL("length"), QString::number(data.introLength()));
        childmode.appendChild(intro);
    }

    // outro
    if (data.hasOutroScreen()) {
        QDomElement outro =
            XmlUtils::createTextElement(doc, "outro", data.outroScreen());
        outro.setAttribute(QSL("length"), QString::number(data.outroLength()));
        childmode.appendChild(outro);
    }

    // panel
    if (data.hasPanelMovie()) {
        childmode.appendChild(
            XmlUtils::createTextElement(doc, "panel", data.panelMovie()));
    }

    return childmode;
}
