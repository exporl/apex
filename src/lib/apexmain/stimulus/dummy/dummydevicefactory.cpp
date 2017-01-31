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

#include "./dummydevicefactory.h"
#include "./dummydevice.h"
#include "stimulus/datablock.h"
#include "stimulus/filter.h"

#include "xml/xercesinclude.h"
using namespace xercesc;

namespace apex{
  namespace stimulus{

DummyDeviceFactory::DummyDeviceFactory()
{
}

DummyDeviceFactory::~DummyDeviceFactory()
{
}

DummyDeviceFactory* DummyDeviceFactory::sm_pInstance = 0;
Destroyer<DummyDeviceFactory> DummyDeviceFactory::sm_Destroyer;

IDeviceFactory* DummyDeviceFactory::Instance()
{
  if( sm_pInstance == 0 )
  {
    sm_pInstance = new DummyDeviceFactory();
    sm_Destroyer.SetDoomed( sm_pInstance );
  }
  return sm_pInstance;
}

void DummyDeviceFactory::DestroyElement( IDeviceFactoryElement* a_pDevice )
{
  m_Rimp.dispose( a_pDevice );
}

void DummyDeviceFactory::DestroyAll( )
{
  m_Rimp.disposeAll();
}

OutputDevice* DummyDeviceFactory::CreateDevice(  const QString& ac_sID, data::DeviceData& ac_Params )
{
  OutputDevice* dev = new DummyDevice( ac_Params );
  m_Rimp.keep( dev );
  return dev;
}

/*DataBlock* DummyDeviceFactory::CreateDataBlock( const QUrl& ac_Url, const QString& ac_sID, const QString& ac_sDesc, const QString& ac_sDevice, const QDateTime& ac_Birth, const data::DatablockData& ac_Params )
{
  DataBlock* p = new DataBlock( ac_Url, ac_sID, ac_sDesc, ac_sDevice, ac_Birth, ac_Params );
  m_Rimp.keep( p );
  return p;
}*/

DataBlock* DummyDeviceFactory::DuplicateDataBlock( const DataBlock& ac_Orig, const QString& ac_sNewID )
{
  return CreateDataBlock( ac_Orig.GetUrl(), ac_sNewID, ac_Orig.GetDescription(), ac_Orig.GetDevice(), ac_Orig.GetBirth(), ac_Orig.GetParameters() );
}

/*Filter* DummyDeviceFactory::CreateFilter( const QString&, const QString&, const QString&, const QString&, FilterParameters* const )
{
  return 0;
}*/

data::DeviceData * apex::stimulus::DummyDeviceFactory::CreateParameters (DOMElement * /*p_base*/)
{
    return new data::DeviceData();
}


  } //end ns stimulus
}//end ns apex
