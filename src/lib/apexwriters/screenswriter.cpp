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

#include "screenswriter.h"

//from libtools
#include "xml/apexxmltools.h"
#include "apextools.h"
#include "gui/arclayout.h"

//from lib (apex)
#include "screen/screen.h"
#include "screen/buttongroup.h"
#include "screen/screenelementvisitor.h"
#include "screen/answerlabelelement.h"
#include "screen/arclayoutelement.h"
#include "screen/buttonelement.h"
#include "screen/emptyelement.h"
#include "screen/flashplayerelement.h"
#include "screen/gridlayoutelement.h"
#include "screen/labelelement.h"
#include "screen/parameterlistelement.h"
#include "screen/pictureelement.h"
#include "screen/texteditelement.h"
#include "screen/parameterdata.h"
#include "screen/picturelabelelement.h"
#include "screen/screensdata.h"
#include "screen/parameterlabelelement.h"

#include "fileprefixwriter.h"

//from xerces
#include "xml/xercesinclude.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;
using namespace apex;
using namespace apex::data;

using apex::writer::ScreensWriter;

class ElementToXMLVisitor
	: public ScreenElementVisitor
{
        DOMDocument* domDocument;
        DOMElement* result;

        void visitAnswerLabel(const AnswerLabelElement* e);
        void visitArcLayout(const ArcLayoutElement* e);
        void visitButton(const ButtonElement* e);
        void visitEmpty(const EmptyElement* e);
        void visitFlashPlayer(const FlashPlayerElement* e);
        void visitGridLayout(const GridLayoutElement* e);
        void visitLabel(const LabelElement* e);
        void visitParameterList(const ParameterListElement* e);
        void visitPicture(const PictureElement* e);
        void visitPictureLabel(const PictureLabelElement* e);
        void visitTextEdit(const TextEditElement* e);
        void visitParameterLabel(const ParameterLabelElement* e);
    public:
        ElementToXMLVisitor(DOMDocument* d) : domDocument(d), result(0) { }
        DOMElement* getResult() { return result; }

        DOMElement* elementToXML(const ScreenElement* el);
    private:
        void addXYParams(DOMElement* de, const ScreenElement* e);
        void addIDParam(DOMElement* de, const ScreenElement* e);
        void addFontSizeChild(DOMElement* de, const ScreenElement* se);
        void addLayoutChildren(DOMElement* de, const ScreenLayoutElement* sle);
        void addTextChild(DOMElement* de, const QString& text);
        void addShortcutChild(DOMElement* de, const ScreenElement* e);
        void addFeedbackChild(DOMElement* de, bool overrideFeedback,
                              const QString& highlight,
                              const QString& positive,
                              const QString& negative);
};

void ElementToXMLVisitor::addShortcutChild(DOMElement* de, const ScreenElement* e)
{
    Q_UNUSED(de);
    Q_UNUSED(e);
    //const QKeySequence seq = e->getShortCut();
    DOMElement* shortcutChild = domDocument->createElement(X("shortcut"));
    // TODO: add better shortcut format in xml schema,
    // based on QKeySequence::toString()
    Q_UNUSED(shortcutChild);
}

void ElementToXMLVisitor::addTextChild(DOMElement* de, const QString& text)
{
    DOMElement* textChild = domDocument->createElement(X("text"));
    textChild->appendChild(domDocument->createTextNode(S2X(text)));
    de->appendChild(textChild);
}

void ElementToXMLVisitor::addLayoutChildren(DOMElement* de, const ScreenLayoutElement* sle)
{
    for (int i = 0; i < sle->getNumberOfChildren(); ++i)
    {
        const ScreenElement* child = sle->getChild(i);
        ElementToXMLVisitor v(domDocument);
        DOMElement* childEl = v.elementToXML(child);
        if (childEl)
        {
            de->appendChild(childEl);
        }
    }
}

void ElementToXMLVisitor::addFontSizeChild(DOMElement* de, const ScreenElement* se)
{
    if (se->getFontSize() != -1)
    {
        // font size differs from default...
        DOMElement* child = domDocument->createElement(X("fontsize"));
        child->appendChild(domDocument
                           ->createTextNode(
                                   S2X(QString::number(se->getFontSize()))));
        de->appendChild(child);
    }
}

void ElementToXMLVisitor::addXYParams(DOMElement* de, const ScreenElement* se)
{
    const ScreenElement* parent = se->getParent();
    if (parent)
    {
        de->setAttribute(X("col"), S2X(QString::number(se->getX() + 1)));
        if (parent->elementType() == ScreenElement::GridLayout)
            de->setAttribute(X("row"), S2X(QString::number(se->getY() + 1)));
    }
}

void ElementToXMLVisitor::addIDParam(DOMElement* de, const ScreenElement* se)
{
    if (!se->getID().isEmpty())
        de->setAttribute(X("id"), S2X(se->getID()));
}

void ElementToXMLVisitor::addFeedbackChild(DOMElement* de, bool overrideFeedback,
        const QString& highlight,
        const QString& positive,
        const QString& negative)
{
    if (overrideFeedback)
    {
        DOMElement* feedbackElem = domDocument->createElement(X("feedback"));
        if (!highlight.isEmpty())
        {
            DOMElement* highlightElem = domDocument->createElement(X("highlight"));
            highlightElem->appendChild(
                domDocument->createTextNode(S2X(highlight)));
            feedbackElem->appendChild(highlightElem);
        }
        if (!positive.isEmpty())
        {
            DOMElement* positiveElem = domDocument->createElement(X("positive"));
            positiveElem->appendChild(
                domDocument->createTextNode(S2X(positive)));
            feedbackElem->appendChild(positiveElem);
        }
        if (!negative.isEmpty())
        {
            DOMElement* negativeElem = domDocument->createElement(X("negative"));
            negativeElem->appendChild(
                domDocument->createTextNode(S2X(negative)));
            feedbackElem->appendChild(negativeElem);
        }
        de->appendChild(feedbackElem);
    }
}

void ElementToXMLVisitor::visitAnswerLabel(const AnswerLabelElement* e)
{
    result = domDocument->createElement(X("answerlabel"));
    addXYParams(result, e);
    addIDParam(result, e);
    addFontSizeChild(result, e);
}

void ElementToXMLVisitor::visitArcLayout(const ArcLayoutElement* e)
{
    result = domDocument->createElement(X("arcLayout"));
    addXYParams(result, e);
    addIDParam(result, e);
    addLayoutChildren(result, e);
    result->setAttribute(X("width"), S2X(QString::number(e->getWidth())));
    ArcLayout::ArcType type = (ArcLayout::ArcType)e->getType();
    QString stype;
    switch (type)
    {
        case ArcLayout::ARC_TOP: stype = QLatin1String("upper"); break;
        case ArcLayout::ARC_BOTTOM: stype = QLatin1String("lower"); break;
        case ArcLayout::ARC_LEFT: stype = QLatin1String("left"); break;
        case ArcLayout::ARC_RIGHT: stype = QLatin1String("right"); break;
        case ArcLayout::ARC_FULL: stype = QLatin1String("full"); break;
    }
    result->setAttribute(X("type"), S2X(stype));
}

void ElementToXMLVisitor::visitButton(const ButtonElement* e)
{
    result = domDocument->createElement(X("button"));
    addXYParams(result, e);
    addIDParam(result, e);
    addFontSizeChild(result, e);
    addTextChild(result, e->text());
    addShortcutChild(result, e);
}

void ElementToXMLVisitor::visitEmpty(const EmptyElement*)
{
}

void ElementToXMLVisitor::visitFlashPlayer(const FlashPlayerElement* e)
{
    result = domDocument->createElement(X("flash"));
    addXYParams(result, e);
    addIDParam(result, e);
    DOMElement* pathElem = domDocument->createElement(X("uri"));
    pathElem->appendChild(domDocument->createTextNode(S2X(e->getDefault())));
    result->appendChild(pathElem);
    addFeedbackChild(result, e->getOverrideFeedback(),
                     e->getHighlight(), e->getPositive(), e->getNegative());
}

void ElementToXMLVisitor::visitGridLayout(const GridLayoutElement* e)
{
    result = domDocument->createElement(X("gridLayout"));
    addXYParams(result, e);
    addIDParam(result, e);
    result->setAttribute(X("width"), S2X(QString::number(e->getWidth())));
    result->setAttribute(X("height"), S2X(QString::number(e->getHeight())));

    //columnstretch
    tStretchList stretch = e->getColumnStretches();
    QStringList stretchStrings = ApexTools::toStringList(stretch);

    if (!stretchStrings.isEmpty())
        result->setAttribute(X("columnstretch"), S2X(stretchStrings.join(",")));

    //rowstretch
    stretch = e->getRowStretches();
    stretchStrings = ApexTools::toStringList(stretch);

    if (!stretchStrings.isEmpty())
        result->setAttribute(X("rowstretch"), S2X(stretchStrings.join(",")));

    addLayoutChildren(result, e);
}

void ElementToXMLVisitor::visitLabel(const LabelElement* e)
{
    result = domDocument->createElement(X("label"));
    addXYParams(result, e);
    addIDParam(result, e);
    addFontSizeChild(result, e);
    addTextChild(result, e->getText());
    addShortcutChild(result, e);
}

void ElementToXMLVisitor::visitParameterList(const ParameterListElement* e)
{
    result = domDocument->createElement(X("parameterlist"));
    addXYParams(result, e);
    addIDParam(result, e);
    typedef ParameterListElement::ParameterListT ParlistT;
    typedef ParameterData PardataT;
    const ParlistT& parlist = e->getParameterList();
    for (ParlistT::const_iterator i = parlist.begin();
            i != parlist.end(); ++i)
    {
        const PardataT& pardata = *i;
        DOMElement* parElem = domDocument->createElement(X("parameter"));
        if (!pardata.name.isEmpty())
            parElem->setAttribute(X("name"), S2X(pardata.name));
        if (!pardata.expression.isEmpty())
            parElem->setAttribute(X("expression"), S2X(pardata.expression));
        parElem->appendChild(
            domDocument->createTextNode(S2X(pardata.id)));
        result->appendChild(parElem);
    }
}

void ElementToXMLVisitor::visitPicture(const PictureElement* e)
{
    result = domDocument->createElement(X("picture"));
    addXYParams(result, e);
    addIDParam(result, e);
    DOMElement* pathElem = domDocument->createElement(X("uri"));
    pathElem->appendChild(
        domDocument->createTextNode(S2X(e->getDefault())));
    result->appendChild(pathElem);
    addFeedbackChild(result, e->getOverrideFeedback(),
                     e->getHighlight(), e->getPositive(), e->getNegative());
}

void ElementToXMLVisitor::visitPictureLabel(const PictureLabelElement* e)
{
    result = domDocument->createElement(X("picturelabel"));
    addXYParams(result, e);
    addIDParam(result, e);
    DOMElement* pathElem = domDocument->createElement(X("uri"));
    pathElem->appendChild(
        domDocument->createTextNode(S2X(e->getPicture())));
    result->appendChild(pathElem);
}

void ElementToXMLVisitor::visitTextEdit(const TextEditElement* e)
{
    result = domDocument->createElement(X("textEdit"));
    addXYParams(result, e);
    addIDParam(result, e);

    DOMElement* textElem = domDocument->createElement(X("text"));
    textElem->appendChild(
        domDocument->createTextNode(S2X(e->getText())));
    result->appendChild(textElem);

    QString inputMask = e->getInputMask();
    if (!inputMask.isEmpty())
    {
        DOMElement* imElem = domDocument->createElement(X("inputmask"));
        imElem->appendChild(
            domDocument->createTextNode(S2X(inputMask)));
        result->appendChild(imElem);
    }
}

void ElementToXMLVisitor::visitParameterLabel(const ParameterLabelElement* e)
{
    result = domDocument->createElement(X("parameterlabel"));
    addXYParams(result, e);
    addIDParam(result, e);

    ParameterData paramData = e->getParameter();
    DOMElement* param = XMLutils::CreateTextElement(domDocument,
                                                    "parameter",
                                                    paramData.id);//argh...
    result->appendChild(param);
    qDebug() << "visited parameter label";
}

DOMElement* ElementToXMLVisitor::elementToXML(const ScreenElement* e)
{
    e->visit(this);
    DOMElement* ret = result;
    result = 0;
    return ret;
}

DOMElement* ScreensWriter::addElement(DOMDocument *doc,
                                      const data::ScreensData &data,
                                      const QStringList& screenList)
{
    DOMElement* rootElem = doc->getDocumentElement();
    DOMElement*  screens = doc->createElement(X("screens"));
    rootElem->appendChild(screens);

    //uri_prefix
    if (data.hasPrefix())
    {
        FilePrefixWriter fpwriter;
        screens->appendChild(fpwriter.addElement(doc, data.prefix()));
    }

    //general
    if (data.hasGeneralScreenData())
        screens->appendChild(addGeneral(doc, data));

    //reinforcement
    if (data.hasReinforcement())
        screens->appendChild(addReinforcement(doc, data));

    //style_apex
    if (data.hasApexStyle())
    {
        screens->appendChild(XMLutils::CreateTextElement(doc,
                             "apex_style", data.apexStyle()));
    }

    //style
    if (data.hasStyle())
    {
        screens->appendChild(XMLutils::CreateTextElement(doc,
                             "style", data.style()));
    }

    //childmode
    if (data.hasChildmode())
        screens->appendChild(addChildmode(doc, data));

    //defaultFont
    if (data.hasDefaultFont())
    {
        screens->appendChild(XMLutils::CreateTextElement(doc,
                             "defaultFont", data.defaultFont()));
    }

    //defaultFontsize
    if (data.hasDefaultFontSize())
    {
        screens->appendChild(XMLutils::CreateTextElement(doc,
                             "defaultFontsize", data.defaultFontSize()));
    }

    //screens
    if (screenList.isEmpty())
    {
        std::map<QString, Screen*> map = data.getScreens();
        std::map<QString, Screen*>::const_iterator it;
        for (it = map.begin(); it != map.end(); it++)
        {
            DOMElement *screen = addScreen(doc, *it->second);
            screen->setAttribute(X("id"), S2X(it->first));
            screens->appendChild(screen);
        }
    }
    else
    {
        Q_FOREACH(QString screen, screenList)
            screens->appendChild(doc->importNode(XMLutils::parseString(screen),
                                 true));
    }

    return screens;
}

DOMElement* ScreensWriter::addScreen(DOMDocument *doc, const data::Screen& data)
{
    DOMElement* e = doc->createElement(X("screen"));

    e->setAttribute(X("id"),S2X(data.getID()));

    DOMElement* layoutEl = screenElementToXml(doc, *data.getRootElement());
    assert(layoutEl);
    e->appendChild(layoutEl);

    const ButtonGroup* buttonGroup = data.getButtonGroup();
    if (buttonGroup)
    {
        DOMElement* buttongroupEl = buttonGroupToXml(doc, *buttonGroup);
        e->appendChild(buttongroupEl);
    }

    QString defaultAnswer = data.getDefaultAnswerElement();
    if (!defaultAnswer.isEmpty())
    {
        DOMElement* defaultAnswerEl = doc->
                                      createElement(X("default_answer_element"));
        defaultAnswerEl->appendChild(
            doc->createTextNode(S2X(defaultAnswer)));
        e->appendChild(defaultAnswerEl);
    }

    return e;
}

DOMElement* ScreensWriter::screenElementToXml(DOMDocument *doc,
        const ScreenElement &e)
{
    ElementToXMLVisitor v(doc);
    return v.elementToXML(&e);
}

DOMElement* ScreensWriter::buttonGroupToXml(DOMDocument *doc, const ButtonGroup &g)
{
    DOMElement* e = doc->createElement(X("buttongroup"));

    e->setAttribute(X("id"), S2X(g.getID()));

    for (ButtonGroup::const_iterator i = g.begin();
            i != g.end(); ++i)
    {
        QString button = *i;
        DOMElement* buttonEl = doc->createElement(X("button"));
        buttonEl->setAttribute(X("id"),S2X( button));
        e->appendChild(buttonEl);
    }
    return e;
}

DOMElement* ScreensWriter::addReinforcement(DOMDocument *doc,
                                            const data::ScreensData &data)
{
    DOMElement* reinforcement = doc->createElement(X("reinforcement"));

    //progressbar
    reinforcement->appendChild(XMLutils::CreateTextElement(doc, "progressbar",
                               data.hasProgressbarEnabled()));
    //feedback
    DOMElement *feedback = XMLutils::CreateTextElement(doc, "feedback",
            ApexTools::boolToString(data.hasFeedbackEnabled()));
    reinforcement->appendChild(feedback);

    if (data.hasFeedbackEnabled() || data.hasFeedbackLengthDefined())
    {
        if (data.hasFeedbackLengthDefined())
        {
            feedback->setAttribute(X("length"),
                                S2X(QString::number(data.feedbackLength())));
        }

        //feedback_on
        reinforcement->appendChild(XMLutils::CreateTextElement(doc, "feedback_on",
                                   data.feedbackOnString()));
        //feedback_picture_positive
        if (!data.feedbackPositivePicture().isNull())
        {
            reinforcement->appendChild(XMLutils::CreateTextElement(doc,
                                    "feedback_picture_positive",
                                    data.feedbackPositivePicture()));
        }
        //feedback_picture_negative
        if (!data.feedbackNegativePicture().isNull())
        {
            reinforcement->appendChild(XMLutils::CreateTextElement(doc,
                                    "feedback_picture_negative",
                                    data.feedbackNegativePicture()));
        }
    }

    //showcurrent
    if (data.hasShowCurrentEnabled())
    {
        reinforcement->appendChild(XMLutils::CreateTextElement(doc,
                                   "showcurrent", "true"));
    }

    return reinforcement;
}

DOMElement* ScreensWriter::addGeneral(DOMDocument *doc,
                                     const data::ScreensData &data)
{
    Q_ASSERT(data.hasGeneralScreenData());

    DOMElement* general = doc->createElement(X("general"));

     //stopbutton
    if (data.hasStopButtonEnabled())
    {
        general->appendChild(XMLutils::CreateTextElement(doc,
                             "stopbutton", "true"));
    }

    //repeatbutton
    if (data.hasRepeatButtonEnabled())
    {
        general->appendChild(XMLutils::CreateTextElement(doc,
                             "repeatbutton", "true"));
    }


    //showpanel
    if (data.hasPanelEnabled())
    {
        general->appendChild(XMLutils::CreateTextElement(doc,
                             "showpanel", "true"));
    }

     //showmenu
    if (data.hasMenuEnabled())
    {
        general->appendChild(XMLutils::CreateTextElement(doc,
                             "showmenu", "true"));
    }

    //fullscreen
    if (data.hasFullScreenEnabled())
    {
        general->appendChild(XMLutils::CreateTextElement(doc,
                             "fullscreen", "true"));
    }


    //intertrialscreen
    if (data.hasInterTrialScreen())
    {
        DOMElement *its = XMLutils::CreateTextElement(doc, "intertrialscreen",
                "true");
        its->setAttribute(X("length"), S2X(QString::number(data.interTrialLength())));
        general->appendChild(its);
    }

    return general;
}

DOMElement* ScreensWriter::addChildmode(DOMDocument *doc,
                                      const data::ScreensData &data)
{
    Q_ASSERT(data.hasChildmode());

    DOMElement* childmode = doc->createElement(X("childmode"));

    //intro
    if (data.hasIntroScreen())
    {
        DOMElement *intro = XMLutils::CreateTextElement(doc, "intro",
                data.introScreen());
        intro->setAttribute(X("length"), S2X(QString::number(data.introLength())));
        childmode->appendChild(intro);
    }

    //outro
    if (data.hasOutroScreen())
    {
        DOMElement *outro = XMLutils::CreateTextElement(doc, "outro",
                data.outroScreen());
        outro->setAttribute(X("length"), S2X(QString::number(data.outroLength())));
        childmode->appendChild(outro);
    }

    //panel
    if (data.hasPanelMovie())
    {
        childmode->appendChild(XMLutils::CreateTextElement(doc, "panel",
                               data.panelMovie()));
    }

    return childmode;
}








