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

#include "apexdata/device/cohdevicedata.h"
#include "apexdata/device/devicedata.h"
#include "apexdata/device/devicesdata.h"
#include "apexdata/device/wavdevicedata.h"

#include "apexdata/map/apexmap.h"
#include "apexdata/map/channelmap.h"

#include "apexdata/parameters/parameter.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "deviceswriter.h"

#include <QDebug>

using namespace apex;
using apex::writer::DevicesWriter;
using apex::data::Parameter;
using apex::data::DeviceData;
using apex::data::WavDeviceData;
using apex::data::CohDeviceData;
using apex::data::DevicesData;
using apex::data::ApexMap;
using apex::data::ChannelMap;

QDomElement DevicesWriter::addElement(QDomDocument *doc,
                                      const DevicesData &data)
{
    QDomElement rootElem = doc->documentElement();

    QDomElement devices = doc->createElement(QSL("devices"));
    rootElem.appendChild(devices);

    QString master = data.masterDevice();
    // if there is a <master> tag
    if (!master.isEmpty()) {
        qCDebug(APEX_RS, "WRITER: master device: %s", qPrintable(master));
        devices.appendChild(XmlUtils::createTextElement(doc, "master", master));
    }

    for (DevicesData::const_iterator it = data.begin(); it != data.end();
         it++) {
        QDomElement device = doc->createElement(QSL("device"));
        devices.appendChild(device);

        DeviceData *devData = it.value();

        // set the attributes common to all types of devices
        device.setAttribute(QSL("id"), devData->id());

        // get data from the specific device type
        WavDeviceData *wavData;
        CohDeviceData *cohData;

        switch (devData->deviceType()) {
        case data::TYPE_WAVDEVICE:
            wavData = dynamic_cast<WavDeviceData *>(devData);
            finishAsWav(&device, *wavData);
            break;
        case data::TYPE_COH:
            cohData = dynamic_cast<CohDeviceData *>(devData);
            finishAsCoh(&device, *cohData);
            break;
        default:
            qCCritical(APEX_RS, "WRITER: unknown device type: %u",
                       devData->deviceType());
            break;
        }
    }

    return devices;
}

void DevicesWriter::finishAsWav(QDomElement *dev, const WavDeviceData &data)
{
    QDomDocument doc = dev->ownerDocument();

    dev->setAttribute(QSL("xsi:type"), QSL("apex:wavDeviceType"));

    dev->appendChild(
        XmlUtils::createTextElement(&doc, "driver", data.driverString()));
    dev->appendChild(
        XmlUtils::createTextElement(&doc, "card", data.cardName()));
    dev->appendChild(
        XmlUtils::createTextElement(&doc, "channels", data.numberOfChannels()));

    // get the gains from SimpleParameters
    Q_FOREACH (Parameter param, data.parameters()) {
        if (param.type() == "gain") {
            QVariant gainValue = param.defaultValue();
            Q_ASSERT(gainValue.canConvert(QVariant::Double));

            QDomElement gain =
                XmlUtils::createTextElement(&doc, "gain", gainValue.toDouble());

            // check if the gain has an id
            if (param.hasId())
                gain.setAttribute(QSL("id"), param.id());

            if (param.hasChannel())
                gain.setAttribute(QSL("channel"),
                                  QString::number(param.channel()));

            dev->appendChild(gain);
        }
    }

    dev->appendChild(
        XmlUtils::createTextElement(&doc, "samplerate", data.sampleRate()));
    if (data.bufferSize() != -1)
        dev->appendChild(
            XmlUtils::createTextElement(&doc, "buffersize", data.bufferSize()));
    if (data.blockSize() != -1)
        dev->appendChild(
            XmlUtils::createTextElement(&doc, "blocksize", data.blockSize()));
    dev->appendChild(XmlUtils::createTextElement(
        &doc, "padzero", data.valueByType("padzero").toString()));
}

void DevicesWriter::finishAsCoh(QDomElement *dev, const CohDeviceData &data)
{
    // get the DOMDocument from dev
    QDomDocument doc = dev->ownerDocument();

    dev->setAttribute(QSL("xsi:type"), QSL("apex:CohDeviceType"));

    dev->appendChild(
        XmlUtils::createTextElement(&doc, "device", data.device()));

    // get the trigger type and set "trigger" accordingly
    QString trigger;

    switch (data.triggerType()) {
    case data::TRIGGER_NONE:
        trigger = "none";
        break;
    case data::TRIGGER_IN:
        trigger = "in";
        break;
    case data::TRIGGER_OUT:
        trigger = "out";
        break;
        // no need for a default label as GetTriggerType() assures
        // to return a valid type or calls qFatal
    }

    dev->appendChild(XmlUtils::createTextElement(&doc, "trigger", trigger));
    dev->appendChild(
        XmlUtils::createTextElement(&doc, "volume", data.volume()));

    // start creation of the <defaultmap> element
    QDomElement defaultmap = doc.createElement(QSL("defaultmap"));
    //<defaultmap> has everything inside an <inline> element
    QDomElement instripe =
        doc.createElement(QSL("inline")); // inline is a keyword...
    // get the map
    ApexMap *map = data.map();
    //<number_electrodes>
    instripe.appendChild(XmlUtils::createTextElement(
        &doc, "number_electrodes", map->numberOfElectrodes()));

    // TODO <mode>
    // TODO <pulsewidth>
    // TODO <pulsegap>
    // TODO <period>
    // TODO don't know where to get these

    // create all the <channel> elements
    // skip first element because it's the powerup stimulus
    for (ApexMap::const_iterator it = map->begin() + 1; it != map->end();
         it++) {
        // NOTE ApexMap inherits from std::map<int,ChannelMap>

        QDomElement channel = doc.createElement(QSL("channel"));
        channel.setAttribute(QSL("number"), QString::number(it.key()));

        // get the channelmap for the other attributes
        ChannelMap chMap = it.value();

        // FIXME if the FIXME in channelmap.h gets fixed, this should be done
        // with the appropriate getters
        channel.setAttribute(QSL("electrode"),
                             QString::number(chMap.stimulationElectrode()));
        channel.setAttribute(QSL("threshold"),
                             QString::number(chMap.thresholdLevel()));
        channel.setAttribute(QSL("comfort"),
                             QString::number(chMap.comfortLevel()));

        // put <channel> into <inline>
        instripe.appendChild(channel);
    }

    //<inline> is finished, put it into <defaultmap>
    defaultmap.appendChild(instripe);
    //<defaultmap> is finished, put it into <device> (dev)
    dev->appendChild(defaultmap);
}
