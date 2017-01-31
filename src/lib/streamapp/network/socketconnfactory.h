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
 
#ifndef __SOCKETCONNFACTORY_H__
#define __SOCKETCONNFACTORY_H__

namespace network
{

  class ISocket;
  class ISocketConnection;

    /**
      * ISocketConnectionFactory
      *   interface for classes that can create an ISocketConnection.
      *************************************************************** */
  class ISocketConnectionFactory
  {
  protected:
      /**
        * Protected Constructor.
        */
    ISocketConnectionFactory()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~ISocketConnectionFactory()
    {
    }

      /**
        * Create a new connection.
        * Implementers should return a new connection here,
        * and make sure it gets deleted (or reused) when
        * after it is disconnected.
        * Is allowed to return 0 if no more connections
        * are allowed.
        * @param ac_pSocketToInitWith a connected socket acquired through listening.
        */
    virtual ISocketConnection* mf_pCreateConnection( ISocket* const ac_pSocketToInitWith ) = 0;
  };

}

#endif //#ifndef __SOCKETCONNFACTORY_H__
