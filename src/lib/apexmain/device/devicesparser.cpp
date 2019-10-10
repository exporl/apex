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
#include "apexdata/device/dummydevicedata.h"
#include "apexdata/device/plugincontrollerdata.h"
#include "apexdata/device/wavdevicedata.h"

#include "apexdata/parameters/parametermanagerdata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "device/cohdeviceparser.h"
#include "device/wavdeviceparser.h"

#include "devicesparser.h"

#include <QObject>
#include <QScopedPointer>

using namespace apex::XMLKeys;

namespace apex
{

namespace parser
{

DevicesParser::DevicesParser()
{
}

DevicesParser::~DevicesParser()
{
}

/**
 * Parse the <devices> tag and return a map of DeviceData objects of the
 * correct class
 */
tAllDevices DevicesParser::Parse(const QDomElement &p_base,
                                 data::ParameterManagerData *pm)
{
    data::DevicesData devMap;
    data::DevicesData controlMap;

    qCInfo(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(
                              tr("DevicesParser"), tr("Processing devices"))));

    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();

        if (tag == "device") {
            QScopedPointer<data::DeviceData> tempData(
                ParseDevice(currentNode, pm));
            QString id = tempData->id();
            if (tempData->isControlDevice())
                controlMap[id] = tempData.take();
            else
                devMap[id] = tempData.take();
        } else if (tag == "master") {
            devMap.setMasterDevice(currentNode.text());
        } else {
            qCWarning(APEX_RS, "%s",
                      qPrintable(QSL("%1: %2").arg("EDevicesParser::Parse",
                                                   "Unknown tag: " + tag)));
            throw ApexStringException("Parsing devices failed");
        }
    }

    if (devMap.masterDevice().isEmpty() && devMap.size())
        devMap.setMasterDevice(devMap.begin().key());

    tAllDevices all;
    all.outputdevices = devMap;
    all.controldevices = controlMap;
    return all;
}

data::DeviceData *DevicesParser::ParseDevice(const QDomElement &p_base,
                                             data::ParameterManagerData *pm)
{
    const QString sModule = p_base.attribute(sc_sType);
    const QString id = p_base.attribute(sc_sID);
    const QString sMode = p_base.attribute(QSL("mode"));

    if (id.isEmpty())
        throw ApexStringException(tr("No device id found"));

    QScopedPointer<data::DeviceData> result;

    if (sModule == sc_sWavDevice) {
        WavDeviceParser parser;
        parser.SetParameterManagerData(pm);
        data::WavDeviceData *d = new data::WavDeviceData();
        parser.Parse(p_base, d);
        result.reset(d);
    } else if (sModule == sc_sDummyDevice) {
        result.reset(new data::DummyDeviceData());
    } else if (sModule == sc_sPluginController) {
        parser::SimpleParametersParser parser;
        parser.SetParameterManagerData(pm);
        data::PluginControllerData *pfd = new data::PluginControllerData;
        parser.Parse(p_base, pfd);
        result.reset(pfd);
    } else if (sModule == sc_sCohDevice) {
#ifdef ENABLE_COH
        parser::CohDeviceParser parser;
        parser.SetParameterManagerData(pm);
        data::CohDeviceData *d = new data::CohDeviceData();
        parser.Parse(p_base, d);
        result.reset(d);
#else
        throw ApexStringException(tr("No support for %1").arg(sModule));
#endif
    } else {
        throw ApexStringException(tr("Unknown device type: %1. Does your apex "
                                     "version include this feature?")
                                      .arg(sModule));
    }

    result->setId(id);

    return result.take();
}
}
}
