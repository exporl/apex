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

#include "apexmain/parser/simpleparametersparser.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "stimulus/filtertypes.h"

#include "filterparser.h"

using namespace apex::stimulus;

namespace apex
{

namespace parser
{

FilterParser::FilterParser()
{
}


FilterParser::~FilterParser()
{
}


data::FilterData* FilterParser::ParseFilter(const QDomElement &a_pBase,
        data::ParameterManagerData* parameterManagerData )
{
    const QString type(a_pBase.attribute(XMLKeys::gc_sType));

    // new structure
    data::FilterData* data;
    QScopedPointer<parser::SimpleParametersParser> parser;

    if (type == sc_sFilterPluginFilterType) {
        data = new data::PluginFilterData();
    } else if (type == sc_sFilterAmplifierType) {
        data = new data::WavFilterParameters();
    } else if (type == sc_sFilterFaderType) {
        data = new data::WavFaderParameters();
    } else if (type == sc_sFilterDataLoopType) {
        data = new data::DataLoopGeneratorParameters();
    } else if (type == sc_sFilterGeneratorType) {
        const QString sGenType(a_pBase.firstChildElement(QSL("type")).text());
        if (sGenType == sc_sFilterNoiseGeneratorType)
            data = new data::WavGeneratorParameters();
        else if (sGenType == sc_sFilterSinglePulseGenType)
            data = new data::SinglePulseGeneratorParameters();
        else if (sGenType == sc_sFilterSineGenType)
            data = new data::SineGeneratorParameters();
        else
            throw ApexStringException("WavDeviceFactory::CreateFilterParameters: Unknown filtertype" + sGenType);
    } else {
        throw ApexStringException("WavDeviceFactory::CreateFilterParameters: Unknown filtertype" + type);
    }

    if (!parser)
        parser.reset(new parser::SimpleParametersParser());

    parser->SetParameterManagerData(parameterManagerData);
    parser->Parse(a_pBase, data);

    return data;
}

}
}
