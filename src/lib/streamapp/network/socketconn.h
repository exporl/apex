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
 
#ifndef __SOCKETCONN_H__
#define __SOCKETCONN_H__

#include "socketfunc.h"
#include "appcore/threads/thread.h"
using namespace appcore;

namespace streamapp
{
  class RawMemoryAccess;
}
using namespace streamapp;

namespace network
{

  class ISocket;
  class ISocketConnectionServer;

    /**
      * ISocketConnection
      *   simple two-way communication protocol using ISocket.
      *   After connecting to a remote socket, messages can be sent
      *   by calling mp_SendMessage(), and incoming messages result
      *   in a call to the mf_MessageReceived() method.
      *   Some info: there is always a clinet and a host side. The
      *   host side is the one that listens for a connection, and
      *   creates an ISocketConnection using mp_InitWithSocket(),
      *   while the client side is the one that uses mp_bConnectToSocket().
      *   About the negotiate flag: if this is set to true, the host side
      *   will send it's MagicID to the client side upon connecting.
      *   The client takes over this ID, so now both are ready to send
      *   and receive data. If not using negotiation, both sides must
      *   have the same ID. Note that this is ok for eg a single dedicated
      *   connection, but if the host side has multiple connections at once,
      *   it is way handier to let each one have a unique ID, since this
      *   can be used by the server app to send data to a specific connection
      *   only.
      *   When using negotation, it's the responsibility of the factory
      *   creating the host side to supply usable IDs, and both sides must
      *   have their flag set to true.
      *   @see ISocketConnectionServer for a socket server
      *   @note these classes are experimental
      *   @TODO allow handling message in callee thread instead of
      *   in server thread.
      *   @TODO make an ISocket impl with pipes, then this can be
      *   used to communicate with other processes
      *   @TODO respect MTU limit (576?) by using 512bytes or so
      ********************************************************************* */
  class ISocketConnection : public IThread
  {
    friend class ISocketConnectionServer;
  protected:
      /**
        * Constructor.
        * Doesn't connect, just sets up.
        * @param ac_nMagicID message ID used in header to check validity.
        * The other end should use the same number in order to be recognized.
        * @param ac_bNegotiate true to negotiate for the MagicID
        */
    ISocketConnection( const unsigned ac_nMagicID = 0, const bool ac_bNegotiate = true );

  public:
      /**
        * Destructor.
        */
    virtual ~ISocketConnection();

      /**
        * Connect to a remote socket.
        * Disconnects if already connected.
        * The remote side should have an ISocketConnectionServer running,
        * listening on the same port, and using the same kind of socket.
        * FIXME might need to change address to string if using IR socket?
        * @param ac_nAddress the remote address (for Tcp), else??
        * @param ac_nPort the port number to connect to
        * @param a_nTimeOut the timeout for connecting
        * @return true if succeeded
        */
    bool mp_bConnectToSocket( const int ac_nAddress, const unsigned short ac_nPort, int a_nTimeOut );

      /**
        * Try to disconnect.
        * Closes the socket, then stops the thread.
        * Calls mf_Disconnected().
        */
    void mp_DisConnect();

      /**
        * See if a connection is established.
        * @return true if connected
        */
    bool mf_bIsConnected() const;

      /**
        * Send a message to the other end.
        * Must be connected for this.
        * If it succeeds, the other end will call back
        * it's mf_MessageReceived() method.
        * @param ac_Data the data to send
        * @return false for error
        */
    bool mf_bSendMessage( const RawMemoryAccess& ac_Data );

      /**
        * Send a message to the other end.
        * @see mf_bSendMessage
        * @param ac_Data the data to send
        * @param ac_nBytesToSend number of bytes from ac_Data to send
        * @return false for error
        */
    bool mf_bSendMessage( const RawMemoryAccess& ac_Data, const unsigned ac_nBytesToSend );

      /**
        * Get the magic ID that was assigned.
        * @return the ID
        */
    INLINE const unsigned& mf_nGetID() const
    {
      return m_nMagicID;
    }

      /**
        * Initialize with an already connected socket.
        * Used by ISocketConnectionServer when a connection
        * is made.
        * @param ac_pSocket a connected socket.
        */
    void mp_InitWithSocket( ISocket* const ac_pSocket );

  protected:
      /**
        * Called when a message arrives.
        * @see mf_bSendMessage()
        * @param ac_Data the message
        */
    virtual void mf_MessageReceived( const RawMemoryAccess& ac_Data ) = 0;

      /**
        * Called when a connection is made.
        */
    virtual void mf_Connected() = 0;

      /**
        * Called when a connection is broken.
        */
    virtual void mf_DisConnected() = 0;

      /**
        * Called when an error occurred
        * @param a_eErrorCode the error returned by CommSocket
        */
    virtual void mf_Error( CommSocket::mt_eStatus a_eErrorCode ) = 0;

      /**
        * Read a message.
        */
    bool mf_bReadMessage();

      /**
        * The ID.
        * Protected to allow implementations to negotiate for the ID.
        */
    unsigned m_nMagicID;

  private:
      /**
        * Implementation of the IThread method.
        */
    void mp_Run();

    bool mp_bNegotiation();

    const bool            mc_bNegotiate;
    ISocket*              m_pSocket;
    CommSocket*           m_pCommSocket;
    const CriticalSection mc_Lock;
  };

}

#endif //#ifndef __SOCKETCONN_H__
