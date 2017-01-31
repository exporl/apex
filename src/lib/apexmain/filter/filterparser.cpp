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

#include "apexdata/datablock/datablockdata.h"
#include "apexdata/datablock/datablocksdata.h"

#include "apexdata/device/devicesdata.h"
#include "apexdata/device/wavdevicedata.h"

#include "apexdata/filter/filterdata.h"
#include "apexdata/filter/pluginfilterdata.h"
#include "apexdata/filter/wavparameters.h"

#include "apexdata/parameters/parametermanagerdata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "parser/plugindataparser.h"

#include "stimulus/filtertypes.h"

#include "wavstimulus/wavdeviceio.h"

#include "filterparser.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace apex::stimulus;
using namespace apex::ApexXMLTools;

#include <memory>

#include <QDebug>

namespace apex {

namespace parser {

FilterParser::FilterParser()
    : Parser()
{
}


FilterParser::~FilterParser()
{
}


data::FilterData* FilterParser::ParseFilter( DOMElement* a_pBase, data::ParameterManagerData* parameterManagerData )
{
    clearLog();

    if( !a_pBase )
        throw( ApexStringException( "DOMELement is not valid" ) );

    const QString type( ApexXMLTools::XMLutils::GetAttribute( a_pBase, XMLKeys::gc_sType ) );



    // new structure
    data::FilterData* data;
    QScopedPointer<parser::SimpleParametersParser> parser;

    if (type== sc_sFilterPluginFilterType) {
        data = new data::PluginFilterData();
    } else if( type == sc_sFilterAmplifierType )
        data = new data::WavFilterParameters();
    else if( type == sc_sFilterFaderType )
        data = new data::WavFaderParameters();
    else if( type == sc_sFilterDataLoopType )
        data = new data::DataLoopGeneratorParameters();
    else if( type == sc_sFilterGeneratorType )
    {
        const QString sGenType( ApexXMLTools::XMLutils::FindChild( a_pBase, "type" ) );
        if( sGenType == sc_sFilterNoiseGeneratorType )
            data = new data::WavGeneratorParameters();
        else if( sGenType == sc_sFilterSinglePulseGenType )
            data = new data::SinglePulseGeneratorParameters();
        else if( sGenType == sc_sFilterSineGenType )
            data = new data::SineGeneratorParameters();
        else
            throw( ApexStringException( "WavDeviceFactory::CreateFilterParameters: Unknown filtertype" + sGenType ) );
    }
    else
        throw( ApexStringException( "WavDeviceFactory::CreateFilterParameters: Unknown filtertype" + type ) );


    if (!parser)
        parser.reset(new parser::SimpleParametersParser());


    parser->SetParameterManagerData(parameterManagerData);
    parser->Parse(a_pBase, data);

        const QString dev( data->device() );
        const QString nchannels( data->numberOfChannels());
        const QString id( data->id() );


       // look for device to get sample rate and buffer size

        //data::DeviceData* temp;
                //TODO remove old code when commit 2165 proves to be ok
        //try {
                //temp=ac_DevicesData.GetDeviceData( dev );
        //} catch (ApexStringException &e) {
                //throw ApexStringException("Error parsing filter: " + e.what());
        //}

        //if( temp->GetDeviceType() != data::TYPE_WAVDEVICE) {
                //throw ApexStringException(QString(tr("Filters of type %1 are only implemented for a WavDevice")).arg(type));
        //}

        //data::WavDeviceData& devParam = dynamic_cast<data::WavDeviceData&> (*temp);

        //data->SetSampleRate (devParam.GetSampleRate());
        //data->SetBlockSize (devParam.GetBlockSize());

        return data;


}



}

}
