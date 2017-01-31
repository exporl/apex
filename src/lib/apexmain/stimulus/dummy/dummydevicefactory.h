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

#ifndef DUMMYDEVICEFACTORY_H
#define DUMMYDEVICEFACTORY_H

#include "../idevicefactory.h"
#include "appcore/raiifactory.h"   //for managing created resources
#include "../idevicefactoryelement.h"

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
};

namespace apex{
    namespace data {
        class DatablockData;
    }

  namespace stimulus{

    class DummyDeviceFactory : public IDeviceFactory
    {
    public:
      static IDeviceFactory*  Instance();

        //create
      virtual OutputDevice*     CreateDevice( const QString&    ac_sID,
                                              data::DeviceData& ac_Params );
      /*virtual Filter*     CreateFilter( const QString& ac_sID,
                                        const QString& ac_sType,
                                        const QString& ac_sInput,
                                        const QString& ac_sOutput,
                                        FilterParameters* const ac_pParams );*/
      /*virtual DataBlock*  CreateDataBlock(  const QUrl&      ac_Url,
                                            const QString&   ac_sID,
                                            const QString&   ac_sDesc,
                                            const QString&   ac_sDevice,
                                            const QDateTime& ac_Birth,
                                            const apex::data::DatablockData& ac_Params );*/

      virtual DataBlock* DuplicateDataBlock( const DataBlock& ac_Orig, const QString& ac_sNewID );

      virtual data::DeviceData* CreateParameters( XERCES_CPP_NAMESPACE::DOMElement* p_base);

        //destroy
      virtual void  DestroyElement( IDeviceFactoryElement* a_pDevice );
      virtual void  DestroyAll();

    protected:
      DummyDeviceFactory ();
      virtual ~DummyDeviceFactory ();

    private:
      RaiiFactoryImp<IDeviceFactoryElement> m_Rimp;

      friend class Destroyer<DummyDeviceFactory>;

      static DummyDeviceFactory*            sm_pInstance;
      static Destroyer<DummyDeviceFactory>  sm_Destroyer;
    };

  } //end ns stimulus
} //end ns apex
#endif
