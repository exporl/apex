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
 
#include "connect_item.h"
#include "colors.h"
#include "utils/dataconversion.h"
#include "guicore/widgetattributes.h"
#include "utils/math.h"
#include <iostream>

#if QT_VERSION < 0x040000
#include <qlabel.h>
#include <qpainter.h>
#else
#include <QLabel>
#include <QPainter>
#endif

using namespace synthgui;

namespace
{
  QRect sf_ShrinkRect( const QRect& ac_Orig, const double ac_dShrinkX, const double ac_dShrinkY )
  {
    int nW = dataconversion::roundDoubleToInt( (double) ac_Orig.width() * ac_dShrinkX );
    int nH = dataconversion::roundDoubleToInt( (double) ac_Orig.height() * ac_dShrinkY );
    int nT = ac_Orig.top() + ( ( ac_Orig.height() - nH ) / 2 );
    int nL = ac_Orig.left() + ( ( ac_Orig.width() - nW ) / 2 );
    return QRect( nL, nT, nW, nH );
  }

  QRect sf_RectPart( const QRect& ac_Orig, const double ac_dShrink, const guicore::FriendLayout::mt_eFriendPosition ac_eWhere )
  {
    if( ac_eWhere == FriendLayout::left )
    {
      int nW = dataconversion::roundDoubleToInt( (double) ac_Orig.width() * ac_dShrink );
      int nH = ac_Orig.height();
      return QRect( ac_Orig.left(), ac_Orig.top(), nW, nH );
    }
    else if( ac_eWhere == FriendLayout::right )
    {
      int nW = dataconversion::roundDoubleToInt( (double) ac_Orig.width() * ac_dShrink );
      int nH = ac_Orig.height();
      return QRect( ac_Orig.right() - nW, ac_Orig.top(), nW, nH );
    }
    else
      return QRect();
  }
}

Socket::Socket( QWidget* a_pParent, const unsigned ac_nChannel, const bool ac_bInput ) :
    CustomLayoutWidget<QPushButton>( a_pParent ),
  mc_bInput( ac_bInput ),
  mc_nChannel( ac_nChannel )
{
  QPushButton::setToggleButton( true );
  connect( this, SIGNAL( toggled(bool) ), SLOT( mp_SetState(bool) ) );
}

Socket::~Socket()
{

}

void Socket::paintEvent( QPaintEvent* /*e*/ )
{
  QPainter painting( this );

  const QRect& zork( rect() );

  painting.setBrush( QBrush( gc_Socket ) );
  painting.drawRect( zork );

  QRect conn;
  QRect text;
  if( mc_bInput )
  {
    conn = sf_RectPart( zork, 0.5, FriendLayout::left );
    text = sf_RectPart( zork, 0.5, FriendLayout::right );
  }
  else
  {
    conn = sf_RectPart( zork, 0.5, FriendLayout::right );
    text = sf_RectPart( zork, 0.5, FriendLayout::left );
  }

  mp_DrawConnector( &painting, conn );
  painting.setBrush( Qt::NoBrush );
  mp_DrawChannel( &painting, text );
}

void Socket::mp_DrawConnector( QPainter* a_pPainter, const QRect& ac_eWhere )
{
  m_Point = ac_eWhere.center();

  QBrush brush;
  brush.setStyle( QBrush::SolidPattern );
  brush.setColor( gc_SocketBG );

  a_pPainter->setBrush( brush );
  a_pPainter->drawEllipse( ac_eWhere );

  const QRect sork( sf_ShrinkRect( ac_eWhere, 0.7, 0.7 ) );

  if( state() == false )
  {
    brush.setColor( gc_SocketUFG );
  }
  else
  {
    brush.setColor( gc_SocketCFG );
  }

  a_pPainter->setBrush( brush );
  a_pPainter->drawEllipse( sork );
}

void Socket::mp_DrawChannel( QPainter* a_pPainter, const QRect& ac_eWhere )
{
  QPen pen( gc_SocketText );
  a_pPainter->setPen( pen );
  QFont fontr( a_pPainter->font() );
  fontr.setPixelSize( ac_eWhere.width() );
  a_pPainter->setFont( fontr );
  a_pPainter->drawText( ac_eWhere, Qt::AlignCenter, QString::number( mc_nChannel ) );
}

/*************************************************************************************************************************************/

ConnectItem::ConnectItem( QWidget* a_pParent, const QString& ac_sName, const unsigned ac_nInputs, const unsigned ac_nOutputs ) :
    gt_CustomLayoutWidget( a_pParent ),
  mc_sName( ac_sName ),
  mc_nInputs( ac_nInputs ),
  mc_nOutputs( ac_nOutputs ),
  m_pName( new QLabel( ac_sName, this ) )
{
  QFont fontr( m_pName->font() );
  fontr.setBold( true );
  fontr.setPixelSize( 16 );
  m_pName->setFont( fontr );
  m_pName->setFrameShape( QLabel::Panel );
  m_pName->setFrameShadow( QLabel::Plain );
  widgetattributes::gf_SetBackGroundColor( this, gc_ItemBG );
  widgetattributes::gf_SetBackGroundColor( m_pName, gc_ItemLabelBG );
  widgetattributes::gf_ApplyDefaultAlignment( m_pName );

  m_pLayout = new CustomLayout();
  m_pLayout->mp_AddItem( m_pName, new tItemLayout( 0.0, 0.0, 1.0, 0.2 ) );
  const unsigned nDiv = s_max( ac_nInputs, ac_nOutputs );
  const double dYsize = 0.7 / nDiv;
  for( unsigned i = 0 ; i < ac_nInputs ; ++i )
  {
    Socket* pCur = new Socket( this, i, true );
    connect( pCur, SIGNAL( ms_Clicked( Socket* ) ), this, SLOT( ms_SocketClicked( Socket* ) ) );
    m_pLayout->mp_AddItem( pCur, new tItemLayout( 0.0, 0.3 + dYsize * i, 0.3, dYsize, 1.0, 0.9 ) );
  }
  for( unsigned i = 0 ; i < ac_nOutputs ; ++i )
  {
    Socket* pCur = new Socket( this, i, false );
    connect( pCur, SIGNAL( ms_Clicked( Socket* ) ), this, SLOT( ms_SocketClicked( Socket* ) ) );
    m_pLayout->mp_AddItem( pCur, new tItemLayout( 0.7, 0.3 + dYsize * i, 0.3, dYsize, 1.0, 0.9 ) );
  }
}

ConnectItem::~ConnectItem()
{

}

void ConnectItem::paintEvent( QPaintEvent* e )
{
  gt_CustomLayoutWidget::paintEvent( e );
  //QPainter paint( this );
  //paint.se
}

void ConnectItem::ms_SocketClicked( Socket* a_pSocket )
{
  emit ms_SocketClicked( this, a_pSocket );
}

QPoint ConnectItem::mf_GetSocketPoint( Socket* a_pSocket )
{
  const QRect& geo( a_pSocket->geometry() );

  return QPoint( geo.left() + a_pSocket->mf_GetSocketPoint().x(), geo.top() + a_pSocket->mf_GetSocketPoint().y() );
}

/*************************************************************************************************************************************/

ConnectArea::ConnectArea( QWidget* a_pParent ) :
    gt_CustomLayoutWidget( a_pParent ),
  mv_eState( mc_eInit )
{
  m_pLayout = new GridLayout( 5, 5 );
}

ConnectArea::~ConnectArea()
{

}

void ConnectArea::mp_AddItem( ConnectItem* a_pItem )
{
  a_pItem->show();
  a_pItem->lower();
  /*unsigned x;
  unsigned y = 0;

    //....... YOU LAZY BASTARD
  if( m_Items.size() < 5 )
    x = m_Items.size();

  ((GridLayout*)m_pLayout)->mp_AddItem( a_pItem, x, y );
  mp_DoResize();
  m_Items.push_back( a_pItem );
  connect( a_pItem, SIGNAL( ms_SocketClicked( ConnectItem*, Socket* ) ), this, SLOT( ms_SocketClicked( ConnectItem*, Socket* ) ) );

  if( x == 1 )
  {
    mv_eState = mc_eIdle;
  }*/
}

void ConnectArea::ms_SocketClicked( ConnectItem* a_pItem, Socket* a_pSocket )
{
  std::cout << "clicked " << a_pSocket->mf_bInput() << " channel " << a_pSocket->mf_nChannel() << " on " << a_pItem->mf_sGetName().data() << std::endl;
  if( mv_eState == mc_eIdle )
  {
    mv_eState = mc_eConnecting;
    cur.m_pFromItem = a_pItem;
    cur.m_pFromSocket = a_pSocket;
  }
  else if( mv_eState == mc_eConnecting )
  {
    cur.m_pToItem = a_pItem;
    cur.m_pToSocket = a_pSocket;
    m_Connections.push_back( new mt_Connection( cur ) );
    mv_eState = mc_eIdle;
  }
}

void ConnectArea::paintEvent( QPaintEvent* /*e*/ )
{
  //gt_CustomLayoutWidget::paintEvent( e );

  if( mv_eState == mc_eIdle )
  {
    QPainter paint( this );
    /*mt_Connections::size_type size = m_Connections.size();
    for( mt_Connections::size_type i = 0 ; i < size ; ++i )
    {
      mt_Connection* pCur = m_Connections.at( i );
      QPoint start = pCur->m_pFromItem->mf_GetSocketPoint( pCur->m_pFromSocket );
      QPoint stop  = pCur->m_pToItem->mf_GetSocketPoint( pCur->m_pToSocket );
      mp_DrawCable( &paint, start.x(), start.y(), stop.x(), stop.y() );
    } */
    mp_DrawCable( &paint, 50, 50, 300, 300 );
  }
}

void ConnectArea::mp_DrawCable( QPainter* a_pPainter, int ac_nXstart, int ac_nYstart, int ac_nXend, int ac_nYend )
{
  QPen pen;
  pen.setWidth( 20 );
  pen.setColor( gc_CableColor );
  a_pPainter->setPen( pen );
  a_pPainter->drawLine( ac_nXstart, ac_nYstart, ac_nXend, ac_nYend );
}


