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
 
#ifndef __CONNECTIONRUNDELEGATECREATOR_H__
#define __CONNECTIONRUNDELEGATECREATOR_H__

#include "apextypedefs.h"
#include "connection/connection.h"
#include <defines.h>

//from libtools
#include "status/errorlogger.h"

namespace apex
{
  namespace stimulus
  {

      /**
        * ConnectionsFactory
        *   parses and checks connections.
        *   Not really a factory in the pattern sense of the word ;-
        *   Logs all errors and warnings directly to the ErrorLogger.
        ************************************************************* */
    class ConnectionRunDelegateCreator : public ErrorLogger
    {
    public:
        /**
          * Constructor.
          * Sets up.
          * @param a_VectorToFill the map to fill with parsed connections
          * @param a_Devs the map with devices for the current experiment
          * @param a_Filters the map with filters for the current experiment
          * @param a_DBlocks the map with datablocks for the current experiment
          */
      ConnectionRunDelegateCreator ( tConnectionsMap& a_VectorToFill, const tDeviceMap& a_Devs, const tFilterMap& a_Filters, const tDataBlockMap& a_DBlocks );

      bool AddConnection (const data::ConnectionData& data);

        /**
          * Try to make default connections.
          * This only works if there are only datablocks and devices.
          * All DataBlock channels are connected in order to the corresponding
          * Device's channels.
          * @return false if failed (eg if filters are present)
          */
      bool mp_bMakeDefaultConnections();

        /**
          * Report all unconnected and not completely conneted.
          * Throw exception when datablock is not connected at all.
          */
      void mf_ReportUnconnectedItems();

    private:
        /**
          * Check if the connection is valid.
          * Throws an error if not valid.
          * @param ac_Connection connection to check.
          * @return the same connection.
          */
      const tConnection& mf_IsValid( const tConnection& ac_Connection ) const;

        /**
          * Add a Connection to the map.
          * Calls mf_IsValid().
          * @param ac_Connection the connection to add.
          */
      void mp_AddConnection( const tConnection& ac_Connection );

      const tDeviceMap&     m_Devices;
      const tFilterMap&     m_Filters;
      const tDataBlockMap&  m_DBlocks;
      tConnectionsMap&      m_Connections;
    };

  }
}

#endif
