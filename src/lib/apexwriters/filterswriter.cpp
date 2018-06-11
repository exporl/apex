/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdata/filter/filterdata.h"
#include "apexdata/filter/filtersdata.h"

#include "apexdata/parameters/parameter.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "filterswriter.h"

using namespace apex;
using apex::writer::FiltersWriter;
using apex::data::FilterData;
using apex::data::FiltersData;
using apex::data::Parameter;

QDomElement FiltersWriter::addElement(QDomDocument *doc,
                                      const FiltersData &data)
{
    QDomElement rootElement = doc->documentElement();
    //<filters>
    QDomElement filters = doc->createElement(QSL("filters"));
    rootElement.appendChild(filters);

    // add all <filter> elements to <filters>
    FiltersData::const_iterator it;
    for (it = data.begin(); it != data.end(); it++) {
        //<filter>
        QDomElement filter = doc->createElement(QSL("filter"));
        filters.appendChild(filter);
        // set the id attribute
        filter.setAttribute(QSL("id"), it.key());
        // and fill the element with the data
        fillFilterElement(*it.value(), &filter);
    }

    return filters;
}

void FiltersWriter::fillFilterElement(const FilterData &data,
                                      QDomElement *theElement)
{
    QDomDocument doc = theElement->ownerDocument();

    // id attribute has already been set, only need to set
    // the type attribute
    theElement->setAttribute(QSL("xsi:type"), data.xsiType());

    //<device>
    theElement->appendChild(
        XmlUtils::createTextElement(&doc, "device", data.device()));

    //<channels>
    theElement->appendChild(
        XmlUtils::createTextElement(&doc, "channels", data.numberOfChannels()));

    //<continuous>
    if (data.hasParameter("continuous")) {
        bool continuous = data.valueByType("continuous").toBool();
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "continuous", continuous ? "true" : "false"));
    }

    //<datablock>
    if (data.hasParameter("datablock")) {
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "datablock", data.valueByType("datablock").toString()));
    }

    //<basegain>
    if (data.hasParameter("basegain")) {
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "basegain", data.valueByType("basegain").toDouble()));
    }

    if (data.hasParameter("invertgain"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "invertgain",
            data.valueByType("invertgain").toBool() ? "true" : "false"));

    //<gain>
    //     if (data.hasParameter("gain"))
    //     {

    // [Tom]: does not work if there is more than one gain
    // Parameter gainAtts = data.parameterByType("gain");

    Q_FOREACH (const Parameter &p, data.parameters()) {
        if (p.type() == "gain") {
            QDomElement gain = XmlUtils::createTextElement(
                &doc, "gain", p.defaultValue().toDouble());
            theElement->appendChild(gain);
            if (p.hasId())
                gain.setAttribute(QSL("id"), p.id());
            if (p.hasChannel())
                gain.setAttribute(QSL("channel"), QString::number(p.channel()));
        }
    }

    // int channel = gainAtts.GetChannel();

    // if ( channel != apex::NO_CHANNEL )
    // gain.setAttribute ( X ( "channel" ), X ( QString ( channel ) ) );

    //     }

    //<frequency>
    if (data.hasParameter("frequency")) {
        QDomElement freq = XmlUtils::createTextElement(
            &doc, "frequency", data.valueByType("frequency").toUInt());
        theElement->appendChild(freq);

        Parameter freqAtts = data.parameterByType("frequency");

        if (freqAtts.hasId())
            freq.setAttribute(QSL("id"), freqAtts.id());
    }

    //<phase>
    if (data.hasParameter("phase"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "phase", data.valueByType("phase").toDouble()));

    //<pulsewidth>
    if (data.hasParameter("pulsewidth"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "pulsewidth", data.valueByType("pulsewidth").toUInt()));

    //<polarity>
    if (data.hasParameter("polarity"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "polarity", data.valueByType("polarity").toString()));

    //<randomjump>
    if (data.hasParameter("randomjump"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "randomjump", data.valueByType("randomjump").toString()));

    //<jump>
    if (data.hasParameter("jump"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "jump", data.valueByType("jump").toString()));

    //<length>
    if (data.hasParameter("length"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "length", data.valueByType("length").toDouble()));

    //<type>
    if (data.hasParameter("type"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "type", data.valueByType("type").toString()));

    //<direction>
    if (data.hasParameter("direction"))
        theElement->appendChild(XmlUtils::createTextElement(
            &doc, "direction", data.valueByType("direction").toString()));
}
