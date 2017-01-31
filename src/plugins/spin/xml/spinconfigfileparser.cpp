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

#include "spinconfigfileparser.h"
#include "../data/spinconfig.h"

//includes from libtools
#include "xml/apexxmltools.h"

//includes from xerces
#include "xml/xercesinclude.h"

#include <QDebug>

using spin::data::SpinConfig;
using namespace spin::parser;
using namespace apex::ApexXMLTools;
using namespace XERCES_CPP_NAMESPACE;

SpinConfig SpinConfigFileParser::parse(QString file, QString schema)
{
    SpinConfig config;

    DOMElement* root = XMLutils::ParseXMLDocument(file, true, schema);
    Q_ASSERT(root != NULL);

    DOMNode* child;
    for (child = root->getFirstChild(); child != NULL;
            child = child->getNextSibling())
    {
        Q_ASSERT(child->getNodeType() == DOMNode::ELEMENT_NODE);
        QString tag = XMLutils::GetTagName(child);

        if (tag == "uri_prefix")
            parsePrefix(child, &config);
        else if (tag == "speaker_setup")
            parseSpeakers(child, &config);
        else if (tag == "noises")
            parseNoises(child, &config);
        else if (tag == "speechmaterials")
            parseSpeech(child, &config);
        else if (tag == "subject_screen")
            parseSubjectScreen(child, &config);
        else if (tag == "experimenter_screen")
            parseExperimenterScreen(child, &config);
        else if (tag == "custom_screens")
            parseCustomScreens(child, &config);
        else if (tag == "id_setup")
            parseIds(child, &config);
        else if (tag == "general_setup")
            parseGeneralData(child, &config);
        else
            qFatal("SPIN Parser: unknown tag (%s).", qPrintable(tag));
    }

    return config;
}

/*FIXME link against libparser when ready!
This code is a duplicate from prefixparser */
void SpinConfigFileParser::parsePrefix(DOMNode* prefixNode, SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(prefixNode) == "uri_prefix");
    Q_ASSERT(prefixNode->getNodeType() == DOMNode::ELEMENT_NODE);

    QString attrib(XMLutils::GetAttribute(prefixNode, "source"));
    QString value(XMLutils::GetFirstChildText(prefixNode));

    apex::data::FilePrefix prefix;
    prefix.setValue( value.trimmed() );
    if (attrib.isEmpty() || attrib == "inline")
        prefix.setType( apex::data::FilePrefix::PREFIX_INLINE);
    else if (attrib == "apexconfig")
        prefix.setType(apex::data::FilePrefix::PREFIX_MAINCONFIG );
    else
        Q_ASSERT(0 && "invalid attribute value");

    into->setPrefix(prefix);
}

void SpinConfigFileParser::parseSpeakers(DOMNode* speakers, SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(speakers) == "speaker_setup");
    Q_ASSERT(speakers->getNodeType() == DOMNode::ELEMENT_NODE);

    DOMNode* speaker;
    for (speaker = speakers->getFirstChild(); speaker != NULL;
            speaker = speaker->getNextSibling())
    {
        Q_ASSERT(speaker->getNodeType() == DOMNode::ELEMENT_NODE);
        Q_ASSERT(XMLutils::GetTagName(speaker) == "speaker");


        unsigned angle = XMLutils::FindChild(speaker, "angle").toUInt();
        unsigned channel = XMLutils::FindChild(speaker, "channel").toUInt();

        into->addSpeaker(channel, angle);
    }
}

void SpinConfigFileParser::parseNoises(DOMNode* noises, SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(noises) == "noises");
    Q_ASSERT(noises->getNodeType() == DOMNode::ELEMENT_NODE);

    DOMNode* noise;
    for (noise = noises->getFirstChild(); noise != NULL;
            noise = noise->getNextSibling())
    {
        Q_ASSERT(XMLutils::GetTagName(noise) == "noise");
        Q_ASSERT(noise->getNodeType() == DOMNode::ELEMENT_NODE);

        QString id = XMLutils::GetAttribute(noise, "id");
        QString uri = XMLutils::FindChild(noise, "uri");
        double rms = XMLutils::FindChild(noise, "rms").toDouble();
        QString name = XMLutils::FindChild(noise, "name");
        QString description = XMLutils::FindChild(noise, "description");
        //QString speechmat = XMLutils::FindChild( noise, "speechmaterial" );

        DOMNode* speechNode = XMLutils::FindChildNode(noise, "speechmaterial");
        QString speechmat;
        QString speechcat;

        if (speechNode != NULL)
        {
            speechmat = XMLutils::GetFirstChildText(speechNode);
            speechcat = XMLutils::GetAttribute(speechNode, "category");
        }

        into->addNoise(id, uri, rms, name, description, speechmat, speechcat);
    }
}

void SpinConfigFileParser::parseSpeech(DOMNode* speechmats, SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(speechmats) == "speechmaterials");
    Q_ASSERT(speechmats->getNodeType() == DOMNode::ELEMENT_NODE);

    DOMNode* speechmat;
    for (speechmat = speechmats->getFirstChild(); speechmat != NULL;
            speechmat = speechmat->getNextSibling())
    {
        Q_ASSERT(XMLutils::GetTagName(speechmat) == "speechmaterial");
        Q_ASSERT(speechmat->getNodeType() == DOMNode::ELEMENT_NODE);

        //get all childs of speechmat. there must be an <rms> child and either
        //<list> or <category> childs.
        spin::data::CategoryMap categoryMap;

        double rms = XMLutils::FindChild(speechmat, "rms").toDouble();
        QString vfm = XMLutils::FindChild(speechmat, "values_for_mean");

        bool hasCategories = false;
        DOMNodeList* catList =
            ((DOMElement*)speechmat)->getElementsByTagName(X("category"));

        for (int i = 0; catList->item(i) != NULL; i++)
        {
            hasCategories = true;
            DOMNode* catElem = catList->item(i);
            QString category = XMLutils::GetAttribute(catElem, "id");

            DOMNodeList* listElems =
                ((DOMElement*)catElem)->getElementsByTagName(X("list"));

            addLists(listElems, &categoryMap, category);

            QString rmsString = XMLutils::FindChild(catElem, "rms");
            if (!rmsString.isEmpty())
                categoryMap.setRms(category, rmsString.toDouble());
        }

        if (!hasCategories)
        {
            DOMNodeList* listList =
                ((DOMElement*)speechmat)->getElementsByTagName(X("list"));
            addLists(listList, &categoryMap);   //no category

            Q_ASSERT(!categoryMap.hasCategories());
        }
        else
            Q_ASSERT(categoryMap.hasCategories());

        QString id = XMLutils::GetAttribute(speechmat, "id");

        if (vfm.isEmpty())
            into->addSpeechmaterial(id, rms, categoryMap);
        else
            into->addSpeechmaterial(id, rms, categoryMap, vfm.toUInt());
    }
}

void SpinConfigFileParser::parseSubjectScreen(DOMNode* screen,
                                              SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(screen) == "subject_screen");
    Q_ASSERT(screen->getNodeType() == DOMNode::ELEMENT_NODE);

    into->setSubjectScreen(XMLutils::elementToString(
                           dynamic_cast<DOMElement*>(screen->getFirstChild())));
}

void SpinConfigFileParser::parseExperimenterScreen(DOMNode* screen,
                                              SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(screen) == "experimenter_screen");
    Q_ASSERT(screen->getNodeType() == DOMNode::ELEMENT_NODE);

    into->setExperimenterScreen(XMLutils::elementToString(
                           dynamic_cast<DOMElement*>(screen->getFirstChild())));
}

void SpinConfigFileParser::parseCustomScreens(XERCES_CPP_NAMESPACE::DOMNode* screens,
                                              SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(screens) == "custom_screens");
    Q_ASSERT(screens->getNodeType() == DOMNode::ELEMENT_NODE);

    DOMNode* screen;
    for (screen = screens->getFirstChild(); screen != 0;
            screen = screen->getNextSibling())
    {
        Q_ASSERT(screen->getNodeType() == DOMNode::ELEMENT_NODE);
        Q_ASSERT(XMLutils::GetTagName(screen) == "screen");

        QString description = XMLutils::GetAttribute(screen, "description");
        QString id = XMLutils::GetAttribute(screen, "id");

        if (description.isEmpty())
            description = id;

        Q_ASSERT(!description.isEmpty());

        into->addCustomScreen(description, id, XMLutils::elementToString(
                                        dynamic_cast<DOMElement*>(screen)));
    }
}

void SpinConfigFileParser::parseIds(DOMNode* ids, SpinConfig* into)
{
    Q_ASSERT(XMLutils::GetTagName(ids) == "id_setup");
    Q_ASSERT(ids->getNodeType() == DOMNode::ELEMENT_NODE);

    into->setGainId(XMLutils::FindChild(ids, "gain"));
    into->setTextId(XMLutils::FindChild(ids, "text"));
}

void SpinConfigFileParser::parseGeneralData(DOMNode* data, SpinConfig* into)
{
    into->setInternalRms(XMLutils::FindChild(data, "internal_rms").toDouble());
    into->setDefaultCalibration(XMLutils::FindChild(data, "default_calibration")
            .toDouble());
}

void SpinConfigFileParser::addLists(DOMNodeList* lists, spin::data::CategoryMap* into,
                                    QString category)
{
    for (int i = 0; lists->item(i) != NULL; i++)
    {
        DOMNode* listNode = lists->item(i);
        Q_ASSERT(XMLutils::GetTagName(listNode) == "list");
        Q_ASSERT(listNode->getNodeType() == DOMNode::ELEMENT_NODE);

        spin::data::List list;
        list.id = XMLutils::GetAttribute(listNode, "id");

        //get the speechtokens
        DOMNode* token;
        for (token = listNode->getFirstChild(); token != NULL;
                token = token->getNextSibling())
        {
            Q_ASSERT(XMLutils::GetTagName(token) == "speechtoken");
            Q_ASSERT(token->getNodeType() == DOMNode::ELEMENT_NODE);

            QString id = XMLutils::GetAttribute(token, "id");
            QString uri = XMLutils::FindChild(token, "uri");
            QString text = XMLutils::NodeToString(
                    XMLutils::FindChildNode(token, "text"));

            list.addToken(id, uri, text);
        }

        into->addList(list, category);
    }
}






























