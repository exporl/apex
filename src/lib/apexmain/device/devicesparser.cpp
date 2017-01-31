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

#include "devicesparser.h"
#include "xml/apexxmltools.h"
#include "xml/xmlkeys.h"

//from libdata
#include "device/devicedata.h"
#include "device/devicesdata.h"
#include "device/l34devicedata.h"
#include "device/clariondevicedata.h"
#include "device/wavdevicedata.h"
#include "device/dummydevicedata.h"
#include "device/mixer/mixerparameters.h"
#include "device/plugincontrollerdata.h"
#include "parameters/parametermanagerdata.h"

using namespace apex::XMLKeys;

#include "xml/xercesinclude.h"

using namespace XERCES_CPP_NAMESPACE;

#include "device/wavdeviceparser.h"
#include "device/clariondeviceparser.h"
#include "device/l34deviceparser.h"

#include "parser/plugindataparser.h"


#include <QObject>

#include <memory>


namespace apex {

namespace parser {

DevicesParser::DevicesParser() {}


DevicesParser::~DevicesParser() {}


/**
 * Parse the <devices> tag and return a map of DeviceData objects of the
 * correct class
 */
//data::DevicesData DevicesParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base) {
tAllDevices DevicesParser::Parse( XERCES_CPP_NAMESPACE::DOMElement* p_base, data::ParameterManagerData* pm ) {
    data::DevicesData devMap;
    data::DevicesData controlMap;

    AddStatusMessage( tr( "Processing devices" ) );

    for ( DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling() ) {
        Q_ASSERT( currentNode );
#ifndef WRITERTEST
        Q_ASSERT( currentNode->getNodeType() == DOMNode::ELEMENT_NODE );
#else
	if( currentNode->getNodeType() != DOMNode::ELEMENT_NODE ) {
		qDebug("skipping non-text node");
		continue;
	}
#endif
        // Does not use dynamic_cast, no rtti for xercesc
	DOMElement * const currentElement = static_cast<DOMElement*> (currentNode);

        const QString tag = ApexXMLTools::XMLutils::GetTagName( currentNode );

        if ( tag == "device" ) {
	    std::auto_ptr<data::DeviceData> tempData (ParseDevice (currentElement, pm));

            QString id=tempData->id();

            if ( tempData->isControlDevice() )
                controlMap[id]=tempData.release();
            else
                devMap[id]=tempData.release();

            //data::DeviceData* bla = tempData.release();

            //devMap.insert( data::tDeviceDataPair(id, bla) );
        } else if ( tag == "master" ) {
            devMap.setMasterDevice(
                    ApexXMLTools::XMLutils::GetFirstChildText(
                    ( DOMElement* )currentNode ) );
        } else {
            log().addWarning( "EDevicesParser::Parse", "Unknown tag: " + tag );
            throw ApexStringException( "Parsing devices failed" );
        }
    }

    if ( devMap.masterDevice().isEmpty() && devMap.size() )
        devMap.setMasterDevice(devMap.begin().key());

    tAllDevices all;

    all.outputdevices=devMap;

    all.controldevices=controlMap;

    //return devMap;
    return all;
}


data::DeviceData* DevicesParser::ParseDevice( DOMElement* p_base, data::ParameterManagerData* pm ) {
    Q_CHECK_PTR( p_base );

    const QString sModule = ApexXMLTools::XMLutils::GetAttribute( ( DOMElement* )p_base, sc_sType );
    const QString id      = ApexXMLTools::XMLutils::GetAttribute( p_base, sc_sID );
    const QString sMode   = ApexXMLTools::XMLutils::GetAttribute( p_base, "mode" );

    if ( id.isEmpty() )
        throw ApexStringException( tr( "No device id found" ) );

    std::auto_ptr<data::DeviceData> result;

    if ( sModule==sc_sWavDevice ) {
        WavDeviceParser parser;
        parser.SetParameterManagerData( pm );
        data::WavDeviceData* d = new data::WavDeviceData();
        parser.Parse( p_base, d );
        result.reset( d );
    } else if ( sModule==sc_sDummyDevice ) {
        result.reset (new data::DummyDeviceData());
    } else if ( sModule==sc_sPluginController ) {
        parser::PluginDataParser parser;
        parser.SetParameterManagerData(pm);
        data::PluginControllerData* pfd = new data::PluginControllerData;
        parser.Parse( p_base, pfd );
        result.reset ( pfd );
    } else if ( sModule==sc_sL34Device ) {
        parser::L34DeviceParser parser;
        parser.SetParameterManagerData(pm);
        data::L34DeviceData* d = new data::L34DeviceData();
        parser.Parse( p_base, d);
        result.reset( d );
    } else if ( sModule==sc_sClarionDevice ) {
        ClarionDeviceParser parser;
        parser.SetParameterManagerData(pm);
        data::ClarionDeviceData* d = new data::ClarionDeviceData();
        parser.Parse( p_base, d);
        result.reset( d );

    }
#ifdef SETMIXER
        else if ( sModule==sc_sSoundcardMixer ) {
            parser::SimpleParametersParser parser;
            parser.SetParameterManagerData(pm);
            data::MixerParameters* d = new data::MixerParameters();
            parser.Parse(p_base,d);
            result.reset( d );
        }

#endif
#ifdef PA5
        else if ( sModule==sc_sPA5 ) {
            parser::SimpleParametersParser parser;
            parser.SetParameterManagerData(pm);
            data::MixerParameters* d = new data::MixerParameters();
            parser.Parse(p_base,d);
            result.reset( d );
        }

#endif

        else {
            throw ApexStringException( tr( "Unknown device type: " ) + sModule + tr( "does your apex version include this feature?" ) );
        }

        result->setId( id );

        return result.release();
    }
}

}

