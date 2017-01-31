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

#include "devicestester.h"
#include "tools.h"
#include "exceptions.h"

#include "device/devicesparser.h"
//#include "datablock/datablockswriter.h"
#include "deviceswriter.h"

#include "xml/xercesinclude.h"

using namespace xercesc;

using namespace apex;
using namespace apex::data;

#include <QStringList>



void DevicesTester::ParseData( DOMElement* root )
{
        parser::DevicesParser parser;
        parser::tAllDevices devices;
        // use the parser to parse data into the datastructure
        try
        {
                devices = parser.Parse ( root, 0 );
                outputDev = devices.outputdevices;
                controlDev = devices.controldevices;
        }
        catch ( ApexStringException e )
        {
                qCDebug(APEX_RS, "Exception: %s",  e.what() );
        }
}

DOMElement* DevicesTester::WriteData ( DOMDocument* doc )
{
        Q_ASSERT( &outputDev != NULL );
        Q_ASSERT( &controlDev != NULL );

        // use the apex writer to convert the data structure into a DOM tree
        writer::DevicesWriter writer;

        //put control- and outputdevices data together in a DevicesData object
        DevicesData devicesdata;

        //pair to insert into DevicesData object
        typedef std::pair<QString,DeviceData*> DataPair;

        DevicesData::const_iterator from;

        qCDebug(APEX_RS, "Inserting outputDev into devicesdata" );
        for ( from = outputDev.begin(); from != outputDev.end(); from++ )
        {
                qCDebug(APEX_RS, "Label: %s", qPrintable( from->first ) );
                DataPair data( from->first, from->second );
                devicesdata.insert ( data );
        }
        qCDebug(APEX_RS, "Inserting controlDev into devicesdata" );
        for ( from = controlDev.begin(); from != controlDev.end(); from++ )
        {
                qCDebug(APEX_RS, "Label: %s", qPrintable( from->first ) );
                DataPair data( from->first, from->second );
                devicesdata.insert ( data );
        }
        qCDebug(APEX_RS, "Printing devicesdata" );
        for ( from = devicesdata.begin(); from != devicesdata.end(); from++ )
        {
                qCDebug(APEX_RS, "Label: %s", qPrintable( from->first ) );
        }

        return writer.addElement ( doc, devicesdata );
}

























