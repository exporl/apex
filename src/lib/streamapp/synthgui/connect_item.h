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
 
#ifndef __CONNECT_ITEM_H__
#define __CONNECT_ITEM_H__

#include <vector>
#include "guicore/customlayoutwidget.h"
using namespace guicore;

class QLabel;

#if QT_VERSION < 0x040000
#include <qwidget.h>
#include <qpushbutton.h>
#else
#include <QPushButton>
#endif

  /**
    * namespace containing graphical representations of streamapp elements
    ********************************************************************** */
namespace synthgui
{

    /**
      * Socket
      *   represents a connectable socket
      *********************************** */
  class Socket : public CustomLayoutWidget<QPushButton>
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget
        * @param ac_nChannel the channel number
        * @param ac_bInput true for input socket
        */
    Socket( QWidget* a_pParent, const unsigned ac_nChannel, const bool ac_bInput = true );

      /**
        * Destructor.
        */
    virtual ~Socket();

      /**
        * Query connections state.
        * @return true for connected
        */
    bool mf_bGetState() const
    { return state(); }

      /**
        * Query in or out.
        * @return true for input
        */
    const bool& mf_bInput() const
    { return mc_bInput; }

      /**
        * Get the channel number.
        * @return the number
        */
    const unsigned& mf_nChannel() const
    { return mc_nChannel; }

      /**
        * Get the coordinates of the socket midpoint.
        * Used by parent to draw cables.
        * @return the point
        */
    const QPoint& mf_GetSocketPoint() const
    { return m_Point; }

  public slots:
      /**
        * Set state.
        * @param ac_eState the new state
        */
    virtual void mp_SetState( bool ac_eState )
    {
      setOn( ac_eState );
      ms_Clicked( this );
    }

  signals:
      /**
        * Emitted when the socket is clicked
        * @param a_pSocket this
        */
    void ms_Clicked( Socket* a_pSocket );

  protected:
      /**
        * Paints the Socket.
        * @param e a QPaintEvent, mostly ignored
        */
    virtual void paintEvent( QPaintEvent* e );

      /**
        * Paint the connector
        * @param a_pPainter the painter
        * @param ac_eWhere the area to draw in
        */
    virtual void mp_DrawConnector( QPainter* a_pPainter, const QRect& ac_eWhere );

      /**
        * Paint the channel number
        * @param a_pPainter the painter
        * @param ac_eWhere the area to draw in
        */
    virtual void mp_DrawChannel( QPainter* a_pPainter, const QRect& ac_eWhere );

  private:
    const bool      mc_bInput;
    const unsigned  mc_nChannel;
    QPoint          m_Point;

    Socket( const Socket& );
    Socket& operator = ( const Socket& );
  };

    /**
      * ConnectItem
      *   any item that can have connections.
      *   Contains a number input and/or output sockets.
      ************************************************** */
  class ConnectItem : public gt_CustomLayoutWidget
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget
        * @param ac_sName the item's name
        * @param ac_nInputs number of input channels
        * @param ac_nOutputs number of output channels
        */
    ConnectItem( QWidget* a_pParent, const QString& ac_sName, const unsigned ac_nInputs, const unsigned ac_nOutputs );

      /**
        * Destructor.
        */
    ~ConnectItem();

      /**
        * Get the name.
        * @return text string reference
        */
    const QString& mf_sGetName() const
    { return mc_sName; }

      /**
        * Get the number of inputs.
        * @return number
        */
    const unsigned& mf_nGetNumInputChannels() const
    { return mc_nInputs; }

      /**
        * Get the number of outputs.
        * @return number
        */
    const unsigned& mf_nGetNumOutputChannels() const
    { return mc_nOutputs; }

      /**
        * Get the coordinates of the socket's midpoint.
        * @param a_pSocket the socket
        * @return the point
        */
    QPoint mf_GetSocketPoint( Socket* a_pSocket );

  public slots:
      /**
        * Called when a socket is clicked.
        * @param a_pSocket the socket
        */
    void ms_SocketClicked( Socket* a_pSocket );

  signals:
      /**
        * Emitted when a socket is clicked.
        * @param a_pItem this
        * @param a_pSocket the socket
        */
    void ms_SocketClicked( ConnectItem* a_pItem, Socket* a_pSocket );

  protected:
      /**
        * Paints the item.
        * @param e a QPaintEvent, mostly ignored
        */
    virtual void paintEvent( QPaintEvent* e );

  private:
    const QString   mc_sName;
    const unsigned  mc_nInputs;
    const unsigned  mc_nOutputs;

    QLabel* m_pName;

    ConnectItem( const ConnectItem& );
    ConnectItem& operator = ( const ConnectItem& );
  };

    /**
      * ConnectArea
      *   the widget on which connections are made.
      ********************************************* */
  class ConnectArea : public gt_CustomLayoutWidget
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget
        */
    ConnectArea( QWidget* a_pParent );

      /**
        * Destructor.
        */
    ~ConnectArea();

      /**
        * State.
        */
    enum mt_eState
    {
      mc_eInit,
      mc_eIdle,
      mc_eConnecting
    };

      /**
        * Cnnection.
        */
    struct mt_Connection
    {
      ConnectItem*  m_pFromItem;
      Socket*       m_pFromSocket;
      ConnectItem*  m_pToItem;
      Socket*       m_pToSocket;
    };

      /**
        * Items.
        */
    typedef std::vector<ConnectItem*> mt_Items;

      /**
        * Connections.
        */
    typedef std::vector<mt_Connection*> mt_Connections;

      /**
        * Add an item.
        * Will be deleted.
        * @param a_pItem the item
        */
    void mp_AddItem( ConnectItem* a_pItem );

  public slots:
      /**
        * Called when a socket is clicked.
        * @param a_pItem the item
        * @param a_pSocket the item's socket
        */
    void ms_SocketClicked( ConnectItem* a_pItem, Socket* a_pSocket );

  signals:
      /**
        * Emitted when a connection is made.
        * @param a_pConnection the new connection
        */
    void ms_Connected( mt_Connection* a_pConnection );

      /**
        * Emitted when a connection is breaked.
        * @param a_pConnection the old connection
        */
    void ms_DisConnected( mt_Connection* a_pConnection );

  protected:
      /**
        * Paints the area.
        * @param e a QPaintEvent, mostly ignored
        */
    void paintEvent( QPaintEvent* e );

      /**
        * Draws a cable.
        * @param a_pPainter the painter
        * @param ac_nXstart the starting point's x coordinate
        * @param ac_nYstart the starting point's y coordinate
        * @param ac_nXend the endpoint's x coordinate
        * @param ac_nYend the endpoint's y coordinate
        */
    void mp_DrawCable( QPainter* a_pPainter, int ac_nXstart, int ac_nYstart, int ac_nXend, int ac_nYend );

  private:
    mt_Items m_Items;
    mt_Connections m_Connections;
    mt_eState mv_eState;

    mt_Connection cur;
  };

}

#endif //#ifndef __CONNECT_ITEM_H__
