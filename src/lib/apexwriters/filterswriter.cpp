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

#include "filterswriter.h"
#include "xml/apexxmltools.h"
#include "filter/filtersdata.h"
#include "filter/filterdata.h"
#include "parameters/parameter.h"

#include "xml/xercesinclude.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::FiltersWriter;
using apex::data::FilterData;
using apex::data::FiltersData;
using apex::data::Parameter;

DOMElement* FiltersWriter::addElement(DOMDocument* doc,
                                      const FiltersData& data)
{
    DOMElement* rootElement = doc->getDocumentElement();
    //<filters>
    DOMElement* filters = doc->createElement(X("filters"));
    rootElement->appendChild(filters);

    //add all <filter> elements to <filters>
    FiltersData::const_iterator it;
    for (it = data.begin(); it != data.end(); it++)
    {
        //<filter>
        DOMElement* filter = doc->createElement(X("filter"));
        filters->appendChild(filter);
        //set the id attribute
        filter->setAttribute(X("id"), S2X(it.key()));
        //and fill the element with the data
        fillFilterElement(*it.value(), filter);
    }

    return filters;
}

void FiltersWriter::fillFilterElement(const FilterData& data,
                                      DOMElement* theElement)
{
    DOMDocument* doc = theElement->getOwnerDocument();

    //id attribute has already been set, only need to set
    //the type attribute
    theElement->setAttribute(X("xsi:type"), S2X(data.xsiType()));

    //<device>
    theElement->appendChild(XMLutils::CreateTextElement(doc, "device",
                            data.device()));

    //<channels>
    theElement->appendChild(XMLutils::CreateTextElement(doc, "channels",
                            data.numberOfChannels()));

    //<continuous>
    if (data.hasParameter("continuous"))
    {
        bool continuous = data.valueByType("continuous").toBool();
        theElement->appendChild(XMLutils::CreateTextElement(doc, "continuous",
                                continuous ? "true" : "false"));
    }

    //<datablock>
    if (data.hasParameter("datablock"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "datablock",
                                data.valueByType("datablock").toString()));
    }

    //<basegain>
    if (data.hasParameter("basegain"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "basegain",
                                data.valueByType("basegain").toDouble()));
    }

    if (data.hasParameter("invertgain"))
        theElement->appendChild(XMLutils::CreateTextElement(doc, "invertgain",
                    data.valueByType("invertgain").toBool() ? "true" : "false"));

    //<gain>
//     if (data.hasParameter("gain"))
//     {


        // [Tom]: does not work if there is more than one gain
        // Parameter gainAtts = data.parameterByType("gain");

        Q_FOREACH (const Parameter& p,  data.parameters()) {
            if ( p.type() == "gain") {
                DOMElement* gain = XMLutils::CreateTextElement(doc, "gain",
                                p.defaultValue().toDouble());
                theElement->appendChild(gain);
                if (p.hasId())
                    gain->setAttribute(X("id"), S2X(p.id()));
                if (p.hasChannel())
                    gain->setAttribute(X("channel"),
                                S2X(QString::number(p.channel())));

            }
         }

        //int channel = gainAtts.GetChannel();

        //if ( channel != apex::NO_CHANNEL )
        // gain->setAttribute ( X ( "channel" ), X ( QString ( channel ) ) );


//     }

    //<frequency>
    if (data.hasParameter("frequency"))
    {
        DOMElement* freq = XMLutils::CreateTextElement(doc, "frequency",
                           data.valueByType("frequency").toUInt());
        theElement->appendChild(freq);

        Parameter freqAtts = data.parameterByType("frequency");

        if (freqAtts.hasId())
            freq->setAttribute(X("id"), S2X(freqAtts.id()));
    }

    //<phase>
    if (data.hasParameter("phase"))
        theElement->appendChild(XMLutils::CreateTextElement(doc, "phase",
                                data.valueByType("phase").toDouble()));

    //<pulsewidth>
    if (data.hasParameter("pulsewidth"))
        theElement->appendChild(XMLutils::CreateTextElement(doc, "pulsewidth",
                                data.valueByType("pulsewidth").toUInt()));

    //<polarity>
    if (data.hasParameter("polarity"))
        theElement->appendChild(XMLutils::CreateTextElement(doc, "polarity",
                                data.valueByType("polarity").toString()));

    //<randomjump>
    if (data.hasParameter("randomjump"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "randomjump",
                                data.valueByType("randomjump").toString()));
    }

    //<jump>
    if (data.hasParameter("jump"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "jump",
                                data.valueByType("jump").toString()));
    }

    //<length>
    if (data.hasParameter("length"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "length",
                                data.valueByType("length").toDouble()));
    }

    //<type>
    if (data.hasParameter("type"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "type",
                                data.valueByType("type").toString()));
    }

    //<direction>
    if (data.hasParameter("direction"))
    {
        theElement->appendChild(XMLutils::CreateTextElement(doc, "direction",
                                data.valueByType("direction").toString()));
    }
}
