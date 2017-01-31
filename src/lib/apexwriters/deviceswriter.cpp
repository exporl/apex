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

/**
 *@author Job Noorman
 */

#include "deviceswriter.h"
#include "xml/apexxmltools.h"
#include "device/devicesdata.h"
#include "device/devicedata.h"
#include "device/wavdevicedata.h"
#include "device/l34devicedata.h"
#include "parameters/parameter.h"
#include "map/apexmap.h"
#include "map/channelmap.h"
#include "xml/xercesinclude.h"

#include <QDebug>

using namespace apex::ApexXMLTools;
using namespace XERCES_CPP_NAMESPACE;
using apex::writer::DevicesWriter;
using apex::data::Parameter;
using apex::data::DeviceData;
using apex::data::WavDeviceData;
using apex::data::L34DeviceData;
using apex::data::DevicesData;
using apex::data::ApexMap;
using apex::data::ChannelMap;
using apex::ApexXMLTools::XMLutils;

DOMElement* DevicesWriter::addElement(DOMDocument* doc, const DevicesData& data)
{
    DOMElement* rootElem = doc->getDocumentElement();

    DOMElement*  devices = doc->createElement(X("devices"));
    rootElem->appendChild(devices);

    QString master = data.masterDevice();
    //if there is a <master> tag
    if (!master.isEmpty())
    {
        qDebug("WRITER: master device: %s", qPrintable(master));
        devices->appendChild(XMLutils::CreateTextElement(doc, "master", master));
    }

    for (DevicesData::const_iterator it = data.begin(); it != data.end(); it++)
    {
        DOMElement* device = doc->createElement(X("device"));
        devices->appendChild(device);

        DeviceData* devData = it.value();

        //set the attributes common to all types of devices
        device->setAttribute(X("id"), S2X(devData->id()));

        //get data from the specific device type
        WavDeviceData* wavData;
        L34DeviceData* l34Data;

        switch (devData->deviceType())
        {
            case data::TYPE_WAVDEVICE:

                wavData = dynamic_cast<WavDeviceData*>(devData);
                finishAsWav(device, *wavData);
                break;

            case data::TYPE_L34:

                //qFatal("WRITER: l34 won't get handled");
                l34Data = dynamic_cast<L34DeviceData*>(devData);
                finishAsL34(device, *l34Data);
                break;

            default:
                qFatal("WRITER: unknown device type: %u", devData->deviceType());
        }
    }

    return devices;
}

void DevicesWriter::finishAsWav(DOMElement* dev, const WavDeviceData& data)
{
    //get the DOMDocument from dev
    DOMDocument* doc = dev->getOwnerDocument();

    Q_ASSERT(doc != NULL);

    dev->setAttribute(X("xsi:type"), X("apex:wavDeviceType"));

    dev->appendChild(XMLutils::CreateTextElement(doc, "driver", data.driverString()));
    dev->appendChild(XMLutils::CreateTextElement(doc, "card", data.cardName()));
    dev->appendChild(XMLutils::CreateTextElement(doc, "channels", data.numberOfChannels()));

    //get the gains from SimpleParameters
    Q_FOREACH (Parameter param, data.parameters())
    {
        if (param.type() == "gain")
        {
            QVariant gainValue = param.defaultValue();
            Q_ASSERT(gainValue.canConvert(QVariant::Double));

            DOMElement* gain = XMLutils::CreateTextElement(doc, "gain",
                    gainValue.toDouble());

            //check if the gain has an id
            if (param.hasId())
                gain->setAttribute(X("id"), S2X(param.id()));

            if (param.hasChannel())
            {
                gain->setAttribute(X("channel"),
                                   S2X(QString::number(param.channel())));
            }


            dev->appendChild(gain);
        }
    }

    dev->appendChild(XMLutils::CreateTextElement(doc, "samplerate", data.sampleRate()));
    if ( data.bufferSize() != -1)
        dev->appendChild(XMLutils::CreateTextElement(doc, "buffersize", data.bufferSize()));
    dev->appendChild(XMLutils::CreateTextElement(doc, "padzero", data.valueByType("padzero").toString()));
}

void DevicesWriter::finishAsL34(DOMElement* dev, const L34DeviceData& data)
{
    //get the DOMDocument from dev
    DOMDocument* doc = dev->getOwnerDocument();

    Q_ASSERT(doc != NULL);

    dev->setAttribute(X("xsi:type"), X("apex:L34DeviceType"));

    dev->appendChild(XMLutils::CreateTextElement(doc, "device_id", data.deviceId()));
    dev->appendChild(XMLutils::CreateTextElement(doc, "implant", data.implantType()));

    //get the trigger type and set "trigger" accordingly
    QString trigger;

    switch (data.triggerType())
    {
        case data::TRIGGER_NONE:

            trigger = "none";
            break;

        case data::TRIGGER_IN:

            trigger = "in";
            break;

        case data::TRIGGER_OUT:

            trigger = "out";
            break;

            //no need for a default label as GetTriggerType() assures
            //to return a valid type or calls qFatal
    }

    dev->appendChild(XMLutils::CreateTextElement(doc, "trigger", trigger));
    dev->appendChild(XMLutils::CreateTextElement(doc, "volume", data.volume()));

    //start creation of the <defaultmap> element
    DOMElement* defaultmap = doc->createElement(X("defaultmap"));
    //<defaultmap> has everything inside an <inline> element
    DOMElement* instripe = doc->createElement(X("inline"));       //inline is a keyword...
    //get the map
    ApexMap* map = data.map();
    //<number_electrodes>
    instripe->appendChild(XMLutils::CreateTextElement(doc, "number_electrodes",
                                                      map->numberOfElectrodes()));

    //TODO <mode>
    //TODO <pulsewidth>
    //TODO <pulsegap>
    //TODO <period>
    //TODO don't know where to get these

    //create all the <channel> elements
    for (ApexMap::const_iterator it = map->begin(); it != map->end(); it++)
    {
        //NOTE ApexMap inherits from std::map<int,ChannelMap>

        DOMElement* channel = doc->createElement(X("channel"));
        channel->setAttribute(X("number"), S2X(QString::number(it.key())));

        //get the channelmap for the other attributes
        ChannelMap chMap = it.value();

        //FIXME if the FIXME in channelmap.h gets fixed, this should be done
        //with the appropriate getters
        channel->setAttribute(X("electrode"),
                              S2X(QString::number(chMap.stimulationElectrode())));
        channel->setAttribute(X("threshold"),
                              S2X(QString::number(chMap.thresholdLevel())));
        channel->setAttribute(X("comfort"),
                              S2X(QString::number(chMap.comfortLevel())));

        //put <channel> into <inline>
        instripe->appendChild(channel);
    }

    //<inline> is finished, put it into <defaultmap>
    defaultmap->appendChild(instripe);
    //<defaultmap> is finished, put it into <device> (dev)
    dev->appendChild(defaultmap);
}

































