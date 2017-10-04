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

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "trialparser.h"

#include <QString>

using namespace apex::XMLKeys;

namespace apex
{

TrialParser::TrialParser()
{
}

TrialParser::~TrialParser()
{
}

data::TrialData *TrialParser::GetTrial(const QDomElement &p_base)
{
    data::TrialData *newTrial = MakeTrial();
    Q_ASSERT(newTrial);

    // get id
    QString id = p_base.attribute(gc_sID);
    Q_ASSERT(!id.isEmpty());
    newTrial->SetID(id);

    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        const QString subid = currentNode.attribute(gc_sID);
        const QString nodeText = currentNode.text();

        if (tag == "answer") {
            QString text = GetAnswer(currentNode);
            newTrial->SetAnswer(text);
            // Q_ASSERT(nodeText.isEmpty()==0);
            // newTrial->SetAnswer(nodeText);
        } else if (tag == "answer_element") {
            Q_ASSERT(nodeText.isEmpty() == 0);
            newTrial->SetAnswerElement(nodeText);
        } else if (tag == "screen") {
            Q_ASSERT(subid.isEmpty() == 0);
            newTrial->SetScreen(subid);
        } else if (tag == "stimulus") {
            Q_ASSERT(subid.isEmpty() == 0);
            newTrial->AddStimulus(subid);
        } else if (tag == "standard") {
            Q_ASSERT(subid.isEmpty() == 0);
            newTrial->AddStandard(subid);
        } else {
            qCDebug(APEX_RS, "Unknown tag in parsedatablocks: %s",
                    qPrintable(tag));
        }
    }

    return newTrial;
}

QString TrialParser::GetAnswer(const QDomElement &p_base)
{
    return XmlUtils::richText(p_base);
}

data::TrialData *apex::TrialParser::MakeTrial()
{
    return new data::TrialData();
}
}
