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
 
#include "guicore/customlayout.h"
#include "guicore/colors.h"
#include "utils/math.h"
#include "utils/dataconversion.h"

#if QT_VERSION < 0x040000
#include <qwidget.h>
#include <qapplication.h>
#else
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#endif

#include <assert.h>

using namespace guicore;
using namespace dataconversion;

tLayoutItem::tLayoutItem( QWidget* a_pWidget ) :
  mc_pWidget( a_pWidget),
  mc_pLayout( 0 ),
  mc_bIsWidget( true )
{
}

tLayoutItem::tLayoutItem( CustomLayout* a_pWidget ) :
  mc_pWidget( 0 ),
  mc_pLayout( a_pWidget ),
  mc_bIsWidget( false )
{
}

tLayoutItem::tLayoutItem( const tLayoutItem& ac_Rh ) :
  mc_pWidget( ac_Rh.mc_pWidget ),
  mc_pLayout( ac_Rh.mc_pLayout ),
  mc_bIsWidget( ac_Rh.mc_bIsWidget )
{
}

tLayoutItem::~tLayoutItem()
{
}

/****************************************************** CUSTOM *****************************************************/

CustomLayout::CustomLayout( const bool ac_bDeleteLayoutItem /*= true */ ) :
  mc_bDeleteItem( ac_bDeleteLayoutItem )
{
}

CustomLayout::~CustomLayout()
{
  mt_VIt it = m_Items.begin();
  while( it != m_Items.end() )
  {
    delete (*it).first;
    if( mc_bDeleteItem )
      delete (*it).second;
    ++it;
  }
}

void CustomLayout::mp_AddItem( QWidget* a_pWidget, tItemLayout* ac_Layout )
{
  m_Items.insert( mt_ItemPair( new tLayoutItem( a_pWidget ), ac_Layout ) );
}

void CustomLayout::mp_AddItem( CustomLayout* a_pWidget, tItemLayout* ac_Layout )
{
  m_Items.insert( mt_ItemPair( new tLayoutItem( a_pWidget ), ac_Layout ) );
}

void CustomLayout::mp_RemoveItem( QWidget* a_pWidget )
{
  mt_VIt itE = m_Items.end();
  for( mt_VIt it = m_Items.begin() ; it != itE ; ++it )
  {
    if( (*it).first->mc_pWidget == a_pWidget )
    {
      delete (*it).second;
      m_Items.erase( it );
      return;
    }
  }
}

void CustomLayout::mp_RemoveItem( CustomLayout* a_pWidget )
{
  mt_VIt itE = m_Items.end();
  for( mt_VIt it = m_Items.begin() ; it != itE ; ++it )
  {
    if( (*it).first->mc_pLayout == a_pWidget )
    {
      delete (*it).second;
      m_Items.erase( it );
      return;
    }
  }
}

void CustomLayout::mp_RemoveAll()
{
  mt_VIt itE = m_Items.end();
  for( mt_VIt it = m_Items.begin() ; it != itE ; ++it )
    delete (*it).second;
  m_Items.clear();
}

tItemLayout* CustomLayout::mf_pGetLayoutFor( QWidget* a_pWidget )
{
  mt_CIt itE = m_Items.end();
  for( mt_CIt it = m_Items.begin() ; it != itE ; ++it )
  {
    if( (*it).first->mc_pWidget == a_pWidget )
      return (*it).second;
  }
  return 0;
}

tItemLayout* CustomLayout::mf_pGetLayoutFor( CustomLayout* a_pWidget )
{
  mt_CIt itE = m_Items.end();
  for( mt_CIt it = m_Items.begin() ; it != itE ; ++it )
  {
    if( (*it).first->mc_pLayout == a_pWidget )
      return (*it).second;
  }
  return 0;
}

void CustomLayout::mp_ResizeThemAll( const QRect& ac_WhereToPutThem )
{
    //get width and height
  const int nWidth  = ac_WhereToPutThem.width();      //according to the Qt makers, width can be negative. ROFLMAO.
  const int nHeight = ac_WhereToPutThem.height();

    //loop them all
  mt_VIt it = m_Items.begin();
  while( it != m_Items.end() )
  {
    tItemLayout* pL = (*it).second;
    tLayoutItem* pI = (*it).first;

      //get the left and top points, and size
    int nL = ac_WhereToPutThem.left() + roundDoubleToInt( (double) nWidth * pL->m_dXoff );
    int nT = ac_WhereToPutThem.top() + roundDoubleToInt( (double) nHeight * pL->m_dYoff );
    double dW = pL->m_dXsiz;
    double dH = pL->m_dYsiz;

      //calculate layout in pixels
    if( pL->m_bPhiRatio )
    {
        //calculate width and height based on size ratio
      dW *= nWidth;
      dH *= nHeight;

        //calculate new width, left and top based on spacing
        //height = width * phi
      nL += roundDoubleToInt( ( ( 1.0 - pL->m_dXspa ) * dW ) / 2.0 );
      nT += roundDoubleToInt( ( dH - dW * pL->m_dXspa * math::gc_dPhiR ) / 2.0 );
      dW *= pL->m_dXspa;
      dH = math::gc_dPhiR * dW;
    }
    else
    {
        //calculate width and height based on size ratio
      dW *= nWidth;
      dH *= nHeight;

        //calculate new width, height, left and top based on spacing
      nL += roundDoubleToInt( ( ( 1.0 - pL->m_dXspa ) * dW ) / 2.0 );
      nT += roundDoubleToInt( ( ( 1.0 - pL->m_dYspa ) * dH ) / 2.0 );
      dW *= pL->m_dXspa;
      dH *= pL->m_dYspa;
    }

    int nW = roundDoubleToInt( dW );
    int nH = roundDoubleToInt( dH );

      //check for bounds
    if( pL->m_bMinMax )
    {
      //...
    }

      //scale
    if( pI->mc_bIsWidget )
      pI->mc_pWidget->setGeometry( QRect( nL, nT, nW, nH ) );
    else
      pI->mc_pLayout->mp_ResizeThemAll( QRect( nL, nT, nW, nH ) );

    ++it;
  }
}

/****************************************************** GRID *****************************************************/

GridLayout::GridLayout( const unsigned ac_nRows, const unsigned ac_nCols ) :
    CustomLayout(),
  mc_nRows( ac_nRows ),
  mc_nCols( ac_nCols )
{
}

GridLayout::~GridLayout()
{
}

tItemLayout* GridLayout::mf_GenerateLayout( const unsigned ac_nRow, const unsigned ac_nCol, const double ac_dXSpacing, const double ac_dYSpacing ) const
{
  assert( ac_nRow < mc_nRows );
  assert( ac_nCol < mc_nCols );

  const double dRowDiv = 1.0 / (double) mc_nRows;
  const double dColDiv = 1.0 / (double) mc_nCols;

  const double dXOff = ac_nCol * dColDiv;
  const double dYOff = ac_nRow * dRowDiv;
  const double dXSiz = dColDiv;
  const double dYSiz = dRowDiv;

  return new tItemLayout( dXOff, dYOff, dXSiz, dYSiz, ac_dXSpacing, ac_dYSpacing );
}

tItemLayout* GridLayout::mf_GenerateMultiLayout( const unsigned ac_nRow, const unsigned ac_nCol, const unsigned ac_nToRow, const unsigned ac_nToCol, const double ac_dXSpacing, const double ac_dYSpacing ) const
{
  assert( ac_nRow < mc_nRows );
  assert( ac_nCol < mc_nCols );
  assert( ac_nToRow < mc_nRows );
  assert( ac_nToCol < mc_nCols );

  const double dRowDiv = 1.0 / (double) mc_nRows;
  const double dColDiv = 1.0 / (double) mc_nCols;

  const double dXOff = ac_nCol * dColDiv;
  const double dYOff = ac_nRow * dRowDiv;
  const double dXSiz = dColDiv * (double) ( ac_nToCol - ac_nCol + 1 );
  const double dYSiz = dRowDiv * (double) ( ac_nToRow - ac_nRow + 1 );

  return new tItemLayout( dXOff, dYOff, dXSiz, dYSiz, ac_dXSpacing, ac_dYSpacing );
}

void GridLayout::mp_AddItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol, const double ac_dXSpacing, const double ac_dYSpacing  )
{
  CustomLayout::mp_AddItem( a_pWidget, mf_GenerateLayout( ac_nRow, ac_nCol, ac_dXSpacing, ac_dYSpacing ) );
}

void GridLayout::mp_AddPhiItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol, const double ac_dXSpacing )
{
  tItemLayout* p = mf_GenerateLayout( ac_nRow, ac_nCol, ac_dXSpacing, 1.0 );
  p->m_bPhiRatio = true;
  CustomLayout::mp_AddItem( a_pWidget, p );
}

void GridLayout::mp_AddItem( CustomLayout* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol, const double ac_dXSpacing, const double ac_dYSpacing  )
{
  CustomLayout::mp_AddItem( a_pWidget, mf_GenerateLayout( ac_nRow, ac_nCol, ac_dXSpacing, ac_dYSpacing ) );
}

void GridLayout::mp_AddCenteredItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol, const double ac_dXSpacing, const double ac_dYSpacing )
{
  tItemLayout* p = mf_GenerateLayout( ac_nRow, ac_nCol, ac_dXSpacing, ac_dYSpacing );
  p->m_dXoff += ( ( p->m_dXsiz * ac_dXSpacing ) / 2.0 );
  //p->m_dYoff += ( ( p->m_dYsiz * ac_dYSpacing ) / 2.0 );
  CustomLayout::mp_AddItem( a_pWidget, p );
}

void GridLayout::mp_AddMultiItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
  const unsigned ac_nToRow, const unsigned ac_nToCol,
  const double ac_dXSpacing, const double ac_dYSpacing )
{
  CustomLayout::mp_AddItem( a_pWidget, mf_GenerateMultiLayout( ac_nRow, ac_nCol, ac_nToRow, ac_nToCol, ac_dXSpacing, ac_dYSpacing ) );
}

void GridLayout::mp_AddMultiItem( CustomLayout* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
  const unsigned ac_nToRow, const unsigned ac_nToCol,
  const double ac_dXSpacing, const double ac_dYSpacing )
{
  CustomLayout::mp_AddItem( a_pWidget, mf_GenerateMultiLayout( ac_nRow, ac_nCol, ac_nToRow, ac_nToCol, ac_dXSpacing, ac_dYSpacing ) );
}

/****************************************************** FRIEND *****************************************************/

FriendLayout::FriendLayout( QWidget* a_pItem, QWidget* a_pFriend, const mt_eFriendPosition ac_eWhere, const double ac_FriendSize /*= 0.5*/ ) :
  CustomLayout(),
  m_pLayout( new CustomLayout() )
{
  const double ac_ItemSize = 1.0 - ac_FriendSize;
  switch( ac_eWhere )
  {
  case left:
    m_pLayout->mp_AddItem( a_pItem, new tItemLayout( ac_FriendSize, 0.0, ac_ItemSize, 1.0 ) );
    m_pLayout->mp_AddItem( a_pFriend, new tItemLayout( 0.0, 0.0, ac_FriendSize, 1.0 ) );
    break;
  case right:
    m_pLayout->mp_AddItem( a_pFriend, new tItemLayout( 0.0, 0.0, ac_FriendSize, 1.0 ) );
    m_pLayout->mp_AddItem( a_pItem, new tItemLayout( ac_FriendSize, 0.0, ac_ItemSize, 1.0 ) );
    break;
  case top:
    m_pLayout->mp_AddItem( a_pItem, new tItemLayout( 0.0, 0.5, 0.1, 0.5 ) );
    m_pLayout->mp_AddItem( a_pFriend, new tItemLayout( 0.0, 0.0, 0.1, 0.5 ) );
    break;
  case bottom:
    m_pLayout->mp_AddItem( a_pItem, new tItemLayout( 0.0, 0.5, 0.1, 0.5 ) );
    m_pLayout->mp_AddItem( a_pFriend, new tItemLayout( 0.0, 0.0, 0.1, 0.5 ) );
    break;
  default:
    break;
  }
  CustomLayout::mp_AddItem( m_pLayout, new tItemLayout( 0.0, 0.0, 1.0, 1.0 ) );
}

FriendLayout::~FriendLayout()
{
  delete m_pLayout;
}

/****************************************************** ARC *****************************************************/

ArcLayout::ArcLayout( const unsigned ac_nItems ) :
  mc_nItems( ac_nItems ),
  mv_eType( mc_eFull )
{
}

ArcLayout::~ArcLayout()
{

}

void ArcLayout::mp_AddItem( QWidget* a_pWidget, const unsigned ac_nPosition, const bool ac_bFollowThePhi, const double ac_dXSpacing, const double ac_dYSpacing )
{
  tItemLayout* pRet = mf_pGenerateLayout( ac_nPosition, ac_dXSpacing, ac_dYSpacing );
  pRet->m_bPhiRatio = ac_bFollowThePhi;
  CustomLayout::mp_AddItem( a_pWidget, pRet );
}

void ArcLayout::mp_AddItem( CustomLayout* a_pWidget, const unsigned ac_nPosition, const double ac_dXSpacing, const double ac_dYSpacing )
{
  tItemLayout* pRet = mf_pGenerateLayout( ac_nPosition, ac_dXSpacing, ac_dYSpacing );
  pRet->m_bPhiRatio = false;
  CustomLayout::mp_AddItem( a_pWidget, pRet );
}

void ArcLayout::mp_SetCenterItem( QWidget* a_pWidget, const bool ac_bFollowThePhi, const double ac_dXSpacing, const double ac_dYSpacing )
{
  tItemLayout* pRet = mf_pGenerateCenterLayout( ac_dXSpacing, ac_dYSpacing );
  pRet->m_bPhiRatio = ac_bFollowThePhi;
  CustomLayout::mp_AddItem( a_pWidget, pRet );
}

void ArcLayout::mp_SetCenterItem( CustomLayout* a_pWidget, const double ac_dXSpacing, const double ac_dYSpacing )
{
  tItemLayout* pRet = mf_pGenerateCenterLayout( ac_dXSpacing, ac_dYSpacing );
  pRet->m_bPhiRatio = false;
  CustomLayout::mp_AddItem( a_pWidget, pRet );
}

tItemLayout* ArcLayout::mf_pGenerateCenterLayout( const double ac_dXSpacing, const double ac_dYSpacing ) const
{
  tItemLayout* pRet = new tItemLayout();

  pRet->m_dXspa = ac_dXSpacing;
  pRet->m_dYspa = ac_dYSpacing;

  pRet->m_dXsiz = 1.0 / (double) mc_nItems;
  pRet->m_dYsiz = 1.0 / (double) mc_nItems;

  pRet->m_dXoff = 0.5 - ( pRet->m_dXsiz / 2.0 );
  pRet->m_dYoff = 0.5 - ( pRet->m_dYsiz / 2.0 );

  return pRet;
}

tItemLayout* ArcLayout::mf_pGenerateLayout( const unsigned ac_nWhere, const double ac_dXSpacing, const double ac_dYSpacing ) const
{
  assert( ac_nWhere < mc_nItems );

  tItemLayout* pRet = new tItemLayout();

  double dDegree = 0.0;
  if( mv_eType == mc_eFull )
    dDegree = ac_nWhere * math::gc_d360 / mc_nItems;
  else if( mv_eType == mc_eLeft )
    dDegree = math::gc_d90 + ac_nWhere * math::gc_d180 / ( mc_nItems - 1 );
  else if( mv_eType == mc_eRight )
    dDegree = ( ac_nWhere * math::gc_d180 / ( mc_nItems - 1 ) ) - math::gc_d90;
  else if( mv_eType == mc_eLower )
    dDegree = ac_nWhere * math::gc_d180 / ( mc_nItems - 1 );
  else if( mv_eType == mc_eUpper )
    dDegree = math::gc_d180 + ac_nWhere * math::gc_d180 / ( mc_nItems - 1 );

  pRet->m_dXoff = 0.5 + 0.4 * cos( math::f_dDegreesToRadians( dDegree ) );
  pRet->m_dYoff = 0.5 + 0.4 * sin( math::f_dDegreesToRadians( dDegree ) );

  pRet->m_dXsiz = 1.0 / (double) mc_nItems;
  pRet->m_dYsiz = 1.0 / (double) mc_nItems;

  pRet->m_dYoff = pRet->m_dYoff - pRet->m_dYsiz / 2.0;
  pRet->m_dXoff = pRet->m_dXoff - pRet->m_dXsiz / 2.0;

  pRet->m_dXspa = ac_dXSpacing;
  pRet->m_dYspa = ac_dYSpacing;

  return pRet;
}

/****************************************************** FUNC *****************************************************/

void tDesktopInfo::mf_GetScreen( QRect& a_Screen, const unsigned ac_nScreen ) const
{
  const QRect& DeskZork( QApplication::desktop()->geometry() );

  a_Screen = DeskZork;

  if( m_ePosition == mc_e2RightOf1 )
  {
    const int semix = DeskZork.width() / 2;
    if( ac_nScreen == 1 )
      a_Screen.setLeft( semix );
    a_Screen.setWidth( semix );
  }
  else if( m_ePosition == mc_e2LeftOf1 )
  {
    const int semix = DeskZork.width() / 2;
    if( ac_nScreen == 0 )
      a_Screen.setLeft( semix );
    a_Screen.setWidth( semix );
  }
  else if( m_ePosition == mc_e2Under1 )
  {
    const int semiy = DeskZork.height() / 2;
    if( ac_nScreen == 1 )
      a_Screen.setTop( semiy );
    a_Screen.setHeight( semiy );
  }
  else if( m_ePosition == mc_e2Above1 )
  {
    const int semiy = DeskZork.height() / 2;
    if( ac_nScreen == 0 )
      a_Screen.setTop( semiy );
    a_Screen.setHeight( semiy );
  }
}

void guicore::gf_ApplyScale( CustomLayout& a_Layout, const double ac_dXScale, const double ac_dYScale )
{
  if( ac_dXScale == 1.0 && ac_dYScale == 1.0 )
    return;

  const CustomLayout::mt_ItemMap& items = a_Layout.mf_GetItems();
  CustomLayout::mt_CIt itE = items.end();
  for( CustomLayout::mt_CIt i = items.begin() ; i != itE ; ++i )
  {
    tItemLayout* pCur = (*i).second;
    pCur->m_dXoff *= ac_dXScale;
    pCur->m_dYoff *= ac_dYScale;
    pCur->m_dXsiz *= ac_dXScale;
    pCur->m_dYsiz *= ac_dYScale;
  }
}

void guicore::gf_ApplyOffset( CustomLayout& a_Layout, const double ac_dXOffset, const double ac_dYOffset )
{
  if( ac_dXOffset == 0.0 && ac_dYOffset == 0.0 )
    return;

  const CustomLayout::mt_ItemMap& items = a_Layout.mf_GetItems();
  CustomLayout::mt_CIt itE = items.end();
  for( CustomLayout::mt_CIt i = items.begin() ; i != itE ; ++i )
  {
    tItemLayout* pCur = (*i).second;
    pCur->m_dXoff += ac_dXOffset;
    pCur->m_dYoff += ac_dYOffset;
  }
}

void guicore::gf_ApplyDesktopScale( CustomLayout& a_Layout, const int ac_nOriginalX, const int ac_nOriginalY,
                                    tDesktopInfo* a_pMonInfo, const bool ac_bScreen )
{
  QRect DeskZork( QApplication::desktop()->geometry() );

  if( a_pMonInfo )
    a_pMonInfo->mf_GetScreen( DeskZork, ac_bScreen ? 0 : 1 );

  double xr = 1.0;
  double yr = 1.0;

  if( DeskZork.width() != ac_nOriginalX )
    xr = (double) ac_nOriginalX / (double) DeskZork.width();
  if( DeskZork.height() != ac_nOriginalY )
    yr = (double) ac_nOriginalY / (double) DeskZork.height();

  gf_ApplyScale( a_Layout, xr, yr );

  a_Layout.mp_ResizeThemAll( DeskZork );
}
