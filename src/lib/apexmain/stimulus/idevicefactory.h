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

#ifndef __IApexDeviceFactory_H__
#define __IApexDeviceFactory_H__

#include "idevicefactoryelement.h"
#include "parameters/apexparameters.h"
//#include "parameters/apexmapparameters.h"
#include "datablock/datablockdata.h"
#include "device/devicedata.h"

//from libtools
#include "status/errorlogger.h"

#include <appcore/destroyer.h>
#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
  class DOMElement;
  class DOMNode;
};

using namespace appcore;

#include <QUrl>
#include <QString>
#include <QDateTime>


/**
  * DeviceFactory interface
  *
  * DeviceFactory is a singleton responsible for creating
  * and destroying Devices, Datablocks and Filters of the same family,
  * eg wav/nic/l34/...
  * Clients should NEVER explicitly delete the created resources
  ********************************************************************/

namespace apex{

  class MainConfigFileParser; //FIXME this doesn't belong here, we should have a singleton with the config, and
                              //a parser to parse it, instead of keeping the config in the parser
    namespace data {
        class DeviceData;
    }

  namespace stimulus{

    class OutputDevice;
    class Filter;
    class DataBlock;
    class ApexFilterParameters;

    class IDeviceFactory : public ErrorLogger
    {
      public:
        static IDeviceFactory*  Instance();

          //create
        virtual OutputDevice*     CreateDevice( const QString&,
                data::DeviceData& ) {Q_ASSERT(0 && "should not come here"); return 0;} ;

        virtual Filter*     CreateFilter( const QString& /*ac_sID*/,
                                          const QString& /*ac_sType*/,
                                          const QString& /*ac_sInput*/,
                                          const QString& /*ac_sOutput*/,
                                          ApexFilterParameters* const /*ac_pParams*/ ) {Q_ASSERT("should not come here"); return 0;};

        virtual DataBlock*  CreateDataBlock( const QUrl&      ,
                                             const QString&   ,
                                             const QString&   ,
                                             const QString&   ,
                                             const QDateTime& ,
                                             const data::DatablockData&  ) {Q_ASSERT(0&&"should not come here"); return 0;}; //  [Tom] removed virtualness

        virtual DataBlock* DuplicateDataBlock( const DataBlock& ac_Orig, const QString& ac_sNewID ) = 0;

          //FIXME implement default
        virtual data::DeviceData* CreateParameters( XERCES_CPP_NAMESPACE::DOMElement* ) { Q_ASSERT(0 && "should not come here"); return 0;} ;

/*        virtual FilterParameters* CreateFilterParameters( XERCES_CPP_NAMESPACE::DOMElement*, XERCES_CPP_NAMESPACE::DOMNode*, const tDeviceMap&, const tDataBlockMap&  )
        { return 0; }*/

          //destroy
        virtual void  DestroyElement( IDeviceFactoryElement* a_pElement ) = 0;
        virtual void  DestroyAll() = 0;

      protected:
        IDeviceFactory()
        {}
        virtual ~IDeviceFactory()
        {}
    };

  } //end ns stimulus
} //end ns apex

#endif //#ifndef __IApexDeviceFactory_H__
