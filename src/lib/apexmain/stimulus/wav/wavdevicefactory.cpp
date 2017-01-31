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

#include "stimulus/filtertypes.h"

#include "datablock/datablocksdata.h"

#include "device/devicedata.h"
#include "device/devicesdata.h"

#include "device/mixer/mixerfactory.h"

#include "device/wavdevicedata.h"

#include "filter/filterdata.h"

#include "services/mainconfigfileparser.h"

#include "soundcard/soundcardfactory.h"

#include "xml/apexxmltools.h"
#include "xml/xmlkeys.h"

#include "qicwavdatablock.h"
#include "wavdatablock.h"
#include "wavdevice.h"
#include "wavdevicefactory.h"
#include "wavfilter.h"
#include "wavparameters.h"

using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;

#include "./soundcarddialog.h"

#include "xml/xercesinclude.h"
using namespace xercesc;

#include <qstringlist.h>

using namespace apex;
using namespace apex::stimulus;


WavDeviceFactory::WavDeviceFactory()
{
}

WavDeviceFactory::~WavDeviceFactory()
{
}

WavDeviceFactory* WavDeviceFactory::sm_pInstance = 0;
appcore::Destroyer<WavDeviceFactory> WavDeviceFactory::sm_Destroyer;

IDeviceFactory* WavDeviceFactory::Instance()
{
  if( sm_pInstance == 0 )
  {
    sm_pInstance = new WavDeviceFactory();
    sm_Destroyer.SetDoomed( sm_pInstance );
  }
  return sm_pInstance;
}

void WavDeviceFactory::DestroyElement( IDeviceFactoryElement* a_pDevice )
{
  m_Rimp.dispose( a_pDevice );
}

void WavDeviceFactory::DestroyAll( )
{
  m_Rimp.disposeAll();
}


DataBlock* WavDeviceFactory::DuplicateDataBlock( const DataBlock& ac_Orig, const QString& ac_sNewID )
{
  return CreateDataBlock( ac_Orig.GetUrl(), ac_sNewID, ac_Orig.GetDescription(), ac_Orig.GetDevice(), ac_Orig.GetBirth(), ac_Orig.GetParameters() );
}



#ifdef REMOVED__
data::FilterData* WavDeviceFactory::CreateFilterParameters( DOMElement* a_pBase, DOMNode* /*a_pFirstChild*/, const data::DevicesData& ac_DevicesData, const data::DatablocksData ac_DataBlocks )
{
  ClearErrors();
  WavFilterParameters* p = 0;

  try
  {
    if( !a_pBase )
      throw( ApexStringException( "DOMEELement is not valid" ) );

    const QString type( XMLutils::GetAttribute( a_pBase, XMLKeys::gc_sType ) );

    if( type == sc_sFilterAmplifierType )
      p = new WavFilterParameters();
    else if( type == sc_sFilterDataLoopType )
      p = new WavGeneratorParameters();
    else if( type == sc_sFilterHrtffiltersumType )
      p = new HrtffiltersumParameters();
    else if( type == sc_sFilterGeneratorType )
    {
      const QString sGenType( XMLutils::FindChild( a_pBase, "type" ) );
      if( sGenType == sc_sFilterNoiseGeneratorType )
        p = new WavGeneratorParameters();
      else if( sGenType == sc_sFilterSinglePulseGenType )
        p = new SinglePulseGeneratorParameters();
      else if( sGenType == sc_sFilterSineGenType )
        p = new SineGeneratorParameters();
      else
        throw( ApexStringException( "WavDeviceFactory::CreateFilterParameters: Unknown filtertype " + sGenType ) );
    }
    else
      throw( ApexStringException( "WavDeviceFactory::CreateFilterParameters: Unknown filtertype " + type ) );


      //set parameters
    p->ExtParse( a_pBase );

    const QString dev( p->GetParameter( "device" ) );
    const QString id( XMLutils::GetAttribute( a_pBase, sc_sID ) );

    p->SetID(id);
    p->m_type=type;
    p->SetDevice(dev);

      //set buffersize/samplerate from device

    /*OutputDevice* pDev = ac_Devices.mf_pGetItem( dev );
    if( !pDev )
    throw( ApexStringException( "couldn't find the device for this filter??" ) );


    DeviceParameters& devParam = pDev->ModParameters();*/
    data::DeviceData* temp;
    try {
        temp=ac_DevicesData.GetDeviceData( dev );
    } catch (ApexStringException &e) {
        throw ApexStringException("Error parsing filter: " + e.what());
    }

    data::WavDeviceData& devParam = dynamic_cast<data::WavDeviceData&> (*temp);         // quick hack, FIXME

    //const QString bs = devParam.GetParameter( "buffersize" ) );
    const QString bs= QString::number( devParam.nBufSize );
    const QString fs( devParam.GetParameter( "samplerate" ) );
    if( !bs.isEmpty() )
        p->SetParameter( "buffersize", bs, "", "" );
    if( !fs.isEmpty() )
        p->SetParameter( "samplerate", fs, "", "" );


  }
  catch( ApexStringException ex )
  {
    error->AddError( "WavDeviceFactory::CreateFilterParameters", ex.what() );
    delete p;
    return 0;
  }
  m_Rimp.keep( p );
  return p;
}
#endif



