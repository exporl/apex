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
 
#include "apexfactories.h"
#include "xml/xmlkeys.h"
using namespace apex::XMLKeys;
#include "stimulus/idevicefactory.h"
#include "stimulus/wav/wavdevicefactory.h"
//#include "stimulus/l34/l34devicefactory.h"
#include "stimulus/dummy/dummydevicefactory.h"
#include <assert.h>

apex::stimulus::IDeviceFactory* apex::stimulus::fChooseFactory( const QString& ac_sModule )
{
  if( ac_sModule == sc_sWavDevice )
    return apex::stimulus::WavDeviceFactory::Instance();
  else if ( ac_sModule == "apex:lauraDeviceType" )
    return 0;//apex::stimulus::LauraDeviceFactory::Instance(); test first
  else if ( ac_sModule == sc_sDummyDevice )
    return apex::stimulus::DummyDeviceFactory::Instance();
  else
  {
          throw(ApexStringException("Invalid device type: does your Apex version support the " + ac_sModule + " type?"));
    return 0;
  }
}
