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

#include "correctorwriter.h"
#include "xml/apexxmltools.h"
#include "corrector/correctordata.h"

#include "xml/xercesinclude.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::CorrectorWriter;
using apex::data::CorrectorData;

DOMElement* CorrectorWriter::addElement(DOMDocument* doc,
                                        const CorrectorData& data)
{
    DOMElement* rootElement = doc->getDocumentElement();
    DOMElement* corrector = doc->createElement(X("corrector"));
    rootElement->appendChild(corrector);

    QString type;
    switch (data.type())
    {
        case CorrectorData::EQUAL:
        {
            type = "apex:isequal";
            break;
        }
        case CorrectorData::CVC:
        {
            type = "apex:cvc";
            QString lang = data.language() == CorrectorData::DUTCH ?
                           "dutch" : "english";
            corrector->appendChild(XMLutils::CreateTextElement(doc,
                                   "language", lang));
            break;
        }
        case CorrectorData::ALTERNATIVES:
        {
            type = "apex:alternatives";

            if (data.answersDefined())
            {
                DOMElement* answers = doc->createElement(X("answers"));
                corrector->appendChild(answers);

                CorrectorData::AnswerMap answerMap = data.answers();
                CorrectorData::AnswerMap::const_iterator it;

                for (it = answerMap.begin(); it != answerMap.end(); it++)
                {
                    DOMElement* answer = doc->createElement(X("answer"));
                    answer->setAttribute(X("number"),
                                         S2X(QString::number(it.key())));
                    answer->setAttribute(X("value"), S2X(it.value()));
                    answers->appendChild(answer);
                }
            }
            break;
        }
        default:
        {
             qFatal("Invalid corrector type");
        }
    }

    corrector->setAttribute(X("xsi:type"), S2X(type));
    return corrector;
}































