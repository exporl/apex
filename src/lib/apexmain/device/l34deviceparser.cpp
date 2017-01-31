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
 
#include "l34deviceparser.h"
#include "device/l34devicedata.h"
#include "map/apexmap.h"
#include "map/mapfactory.h"

#include "services/errorhandler.h"

namespace apex
{

namespace parser
{




L34DeviceParser::L34DeviceParser() : SimpleParametersParser()
{
}


L34DeviceParser::~L34DeviceParser()
{
}


void L34DeviceParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* base, data::L34DeviceData* p)
{

// stuff from DeviceFactory->CreateParameters( p_base);  FIXME

    Q_ASSERT(base );

    SimpleParametersParser::Parse( base, p );
}






void L34DeviceParser::AddParameter(data::SimpleParameters* p, XERCES_CPP_NAMESPACE::DOMElement* e, const QString& owner, const QString& type, const QString& id, const QVariant& value, const int channel) {
    SimpleParametersParser::AddParameter(p, e, owner, type, id, value, channel);

        data::L34DeviceData* q = dynamic_cast<data::L34DeviceData*>(p);
        Q_ASSERT(q);

        if( type == "defaultmap" )  {
            MapFactory mapfac;
            data::ApexMap* pmap = mapfac.GetMap(e);
            if( !pmap )
                throw ApexStringException("Could not parse map for " + p->id());
            q->setMap(pmap);

            ErrorHandler::Get().addItems(mapfac.logger().items());
        } else if (type=="trigger") {

/*            if (p_value=="in")
                m_iTriggerType=TRIGGER_IN;
            else if (p_value=="out")
                m_iTriggerType=TRIGGER_OUT;
            else if (p_value=="none")
                m_iTriggerType=TRIGGER_NONE;
            else {
                qDebug("Invalid trigger type");
                return false;
            }*/
        } else if (type=="device_id") {
//            m_nDeviceID=p_value.toInt();
        } else if (type=="volume") {
//            m_volume=p_value.toFloat();
        } else if (type=="implant") {
//            m_implantType=p_value;
        } else {
            throw ApexStringException( "Unknown tag " +  type + " in L34DeviceData::SetParameter");
        }


}

}
}
