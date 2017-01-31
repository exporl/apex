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

#ifndef WAVDEVICEFACTORY_H
#define WAVDEVICEFACTORY_H

#include "../idevicefactory.h"
#include "../idevicefactoryelement.h"
#include <appcore/raiifactory.h>

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
    class DOMNode;
};

namespace apex{
    namespace data {
        class DatablockData;
        class DevicesData;
        class DatablocksData;
        class FilterData;
    }

  namespace stimulus{

      /**
        * WavDeviceFactory
        *   IDeviceFactory for wav stuff.
        ********************************* */
    class WavDeviceFactory : public IDeviceFactory
    {
    public:
      static IDeviceFactory*  Instance();

      /*virtual Device*     CreateDevice( const QString&   ac_sID,
                                        DeviceParameters& ac_Params );   */               //

     /* virtual Filter*     CreateFilter( const QString& ac_sID,
                                        const QString& ac_sType,
      data::FilterData* const ac_pParams );         */

      /*virtual DataBlock*  CreateDataBlock(  const QUrl&      ac_Url,
                                            const QString&   ac_sID,
                                            const QString&   ac_sDesc,
                                            const QString&   ac_sDevice,
                                            const QDateTime& ac_Birth,
                                            const apex::data::DatablockData& ac_Params ); */

      virtual DataBlock* DuplicateDataBlock( const DataBlock& ac_Orig, const QString& ac_sNewID );

      //virtual DeviceParameters* CreateParameters( DOMElement* p_base);

      //virtual FilterParameters* CreateFilterParameters( XERCES_CPP_NAMESPACE::DOMElement* p_base, XERCES_CPP_NAMESPACE::DOMNode* a_pFirstChild, const tDeviceMap& ac_Devices, const tDataBlockMap& ac_DataBlocks );

      /*data::FilterData* CreateFilterParameters( XERCES_CPP_NAMESPACE::DOMElement* a_pBase, XERCES_CPP_NAMESPACE::DOMNode* , const data::DevicesData& ac_DevicesData, const data::DatablocksData ac_DataBlocks  );*/

        //destroy
      virtual void  DestroyElement( IDeviceFactoryElement* a_pDevice );
      virtual void  DestroyAll();


      WavDeviceFactory();
      virtual ~WavDeviceFactory();

    private:
      RaiiFactoryImp<IDeviceFactoryElement> m_Rimp;

      friend class appcore::Destroyer<WavDeviceFactory>;

      static WavDeviceFactory*              sm_pInstance;
      static appcore::Destroyer<WavDeviceFactory>    sm_Destroyer;
    };

  } //end ns stimulus
} //end ns apex
#endif
