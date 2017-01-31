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
 
#ifndef CUSTOMLAYOUT_H
#define CUSTOMLAYOUT_H

#include <qglobal.h>
#include "customlayoutinfo.h"
#include "utils/macros.h"
#include "defines.h"

class QRect;

  /**
    * namespace containing all graphical stuff
    ****************************************** */
namespace guicore
{

    /**
      * CustomLayout
      *   base class for calculating item layouts and
      *   resizing the corresponding layout items.
      *   Subclasses will provide extra methods to
      *   automatically calculate a tItemLayout.
      *********************************************** */
  class CustomLayout
  {
  public:
      /**
        * Constructor.
        * @param ac_bDeleteItemLayout if true, deletes all tItemLayouts at destruction
        */
    CustomLayout( const bool ac_bDeleteItemLayout = true );

      /**
        * Destructor.
        */
    virtual ~CustomLayout();

      /**
        * Add a widget to the layout.
        * @param a_pWidget the widget to calculate layout for
        * @param a_pLayout the layout info.
        */
    void mp_AddItem( QWidget* a_pWidget, tItemLayout* a_pLayout );

      /**
        * Add a layout to the layout.
        * @param a_pWidget the layout to calculate layout for
        * @param a_pLayout the layout info.
        */
    void mp_AddItem( CustomLayout* a_pWidget, tItemLayout* a_pLayout );

      /**
        * Remove a widget from the layout.
        * @param a_pWidget the widget to remove.
        */
    void mp_RemoveItem( QWidget* a_pWidget );

      /**
        * Remove a layout from the layout.
        * @param a_pWidget the layout to remove.
        */
    void mp_RemoveItem( CustomLayout* a_pWidget );

      /**
        * Remove everything from the layout.
        */
    void mp_RemoveAll();

      /**
        * Get the layout for the specified widget.
        * @param a_pWidget the widget to ge tthe layout for
        * @return the layout or 0 if not found
        */
    tItemLayout* mf_pGetLayoutFor( QWidget* a_pWidget );

      /**
        * Get the layout for the specified widget.
        * @param a_pWidget the widget to ge tthe layout for
        * @return the layout or 0 if not found
        */
    tItemLayout* mf_pGetLayoutFor( CustomLayout* a_pWidget );

      /**
        * Do the resizing.
        * For all items added, calculates the layout relative to ac_WhereToPutThem,
        * then resizes the item, either by calling resize() for q widget, or by calling
          * mp_ResizeThemAll() for a layout.
        * @param ac_WhereToPutThem the parent rectangle
        */
    virtual void mp_ResizeThemAll( const QRect& ac_WhereToPutThem );

      /**
        * Map with items and layout.
        */
    tProtectedMap( tLayoutItem*, tItemLayout* )

      /**
        * Get the item/layout map.
        * @return the map.
        */
    const mt_ItemMap& mf_GetItems() const
    { return m_Items; }

  private:
    const bool mc_bDeleteItem;

    CustomLayout operator=( const CustomLayout& );
    CustomLayout ( const CustomLayout& );
  };

    /**
      * GridLayout
      *   puts items in a grid.
      *   item width = parent width / number of columns.
      *   item height = parent height / number of rows.
      ************************************************** */
  class GridLayout : public CustomLayout
  {
  public:
      /**
        * Constructor.
        * @param ac_nRows the number of rows
        * @param ac_nCols the number of columns
        */
    GridLayout( const unsigned ac_nRows, const unsigned ac_nCols );

      /**
        * Destructor.
        */
    ~GridLayout();

      /**
        * Add a widget.
        * @param a_pWidget the widget
        * @param ac_nRow the row to place it at
        * @param ac_nCol the column to place it at
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                         const double ac_dXSpacing = 0.85, const double ac_dYSpacing = 0.85 );

      /**
        * Add a widget using phi ratio.
        * @param a_pWidget the widget
        * @param ac_nRow the row to place it at
        * @param ac_nCol the column to place it at
        * @param ac_dXSpacing the x spacing
        * @see tItemLayout::mp_SetUsePhi()
        */
    void mp_AddPhiItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                            const double ac_dXSpacing = 0.85 );

      /**
        * Add a layout.
        * @param a_pWidget the layout
        * @param ac_nRow the row to place it at
        * @param ac_nCol the column to place it at
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddItem( CustomLayout* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                              const double ac_dXSpacing = 1.0, const double ac_dYSpacing = 1.0  );

      /**
        * Add a widget and center it horizontally.
        * @param a_pWidget the widget
        * @param ac_nRow the row to place it at
        * @param ac_nCol the column to place it at
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddCenteredItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                                 const double ac_dXSpacing = 0.85, const double ac_dYSpacing = 0.85 );

      /**
        * Add a widget of fixed size.
        * @param a_pWidget the widget
        * @param ac_nRow the row to place it at
        * @param ac_nCol the column to place it at
        * @param ac_dXPixels the x size
        * @param ac_dYPixels the y size
        */
    void mp_AddFixedItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                              const double ac_dXPixels, const double ac_dYPixels );

      /**
        * Add a layout of fixed size.
        * @param a_pWidget the layout
        * @param ac_nRow the row to place it at
        * @param ac_nCol the column to place it at
        * @param ac_dXPixels the x size
        * @param ac_dYPixels the y size
        */
    void mp_AddFixedItem( CustomLayout* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                                    const double ac_dXPixels, const double ac_dYPixels );

      /**
        * Add a widget spanning multiple columns and/or rows
        * @param a_pWidget the widget
        * @param ac_nRow the starting row
        * @param ac_nCol the starting column
        * @param ac_nToRow the ending row
        * @param ac_nToCol the ending column
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddMultiItem( QWidget* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                              const unsigned ac_nToRow, const unsigned ac_nToCol,
                                              const double ac_dXSpacing = 0.85, const double ac_dYSpacing = 0.85 );

      /**
        * Add a widget spanning multiple columns and/or rows
        * @param a_pWidget the widget
        * @param ac_nRow the starting row
        * @param ac_nCol the starting column
        * @param ac_nToRow the ending row
        * @param ac_nToCol the ending column
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddMultiItem( CustomLayout* a_pWidget, const unsigned ac_nRow, const unsigned ac_nCol,
                                                    const unsigned ac_nToRow, const unsigned ac_nToCol,
                                                    const double ac_dXSpacing = 1.0, const double ac_dYSpacing = 1.0  );

      /**
        * Get the number of rows.
        * @return the number
        */
    INLINE const unsigned& mf_nRows() const
    {
      return mc_nRows;
    }

      /**
        * Get the number of columns.
        * @return the number
        */
    INLINE const unsigned& mf_nCols() const
    {
      return mc_nCols;
    }

  private:
    tItemLayout* mf_GenerateLayout( const unsigned ac_nRow, const unsigned ac_nCol, const double ac_dXSpacing, const double ac_dYSpacing ) const;
    tItemLayout* mf_GenerateMultiLayout( const unsigned ac_nRow, const unsigned ac_nCol, const unsigned ac_nToRow, const unsigned ac_nToCol, const double ac_dXSpacing, const double ac_dYSpacing ) const;

    const unsigned mc_nRows;
    const unsigned mc_nCols;

    GridLayout operator=( const GridLayout& );
    GridLayout ( const GridLayout& );
  };

    /**
      * FriendLayout
      *   simple way to bind one widget to another.
      *   Uses 50% space for both items by default.
      ********************************************* */
  class FriendLayout : public CustomLayout
  {
  public:
    enum mt_eFriendPosition
    {
      top,
      bottom,
      left,
      right
    };

    FriendLayout( QWidget* a_pItem, QWidget* a_pFriend, const mt_eFriendPosition ac_eWhere, const double ac_FriendSize = 0.5 );
    ~FriendLayout();

  private:
    CustomLayout* m_pLayout;
  };

    /**
      * ArcLayout.
      *   puts items in an arc shaped layout.
      *   item width = parent width / number of items
      *   item height = parent height / number of items
      ************************************************* */
  class ArcLayout : public CustomLayout
  {
  public:
      /**
        * Constructor.
        * @param ac_nItems the number of items on the arc
        */
    ArcLayout( const unsigned ac_nItems );

      /**
        * Destructor.
        */
    ~ArcLayout();

      /**
        * Arc type.
        * First four use half the arc.
        */
    enum mt_eType
    {
      mc_eUpper,
      mc_eLower,
      mc_eLeft,
      mc_eRight,
      mc_eFull
    };

      /**
        * Set the arc type.
        * Only affects the items added after this call.
        * @param ac_eType the type
        */
    void mp_SetType( const mt_eType ac_eType )
    { mv_eType = ac_eType; }

      /**
        * Add a widget.
        * @param a_pWidget the widget
        * @param ac_nPosition where to put it
        * @param ac_bFollowThePhi if true, height = width * phi
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddItem( QWidget* a_pWidget, const unsigned ac_nPosition, const bool ac_bFollowThePhi = true, const double ac_dXSpacing = 1.0, const double ac_dYSpacing = 1.0 );

      /**
        * Add a layout.
        * @param a_pWidget the widget
        * @param ac_nPosition where to put it
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_AddItem( CustomLayout* a_pWidget, const unsigned ac_nPosition, const double ac_dXSpacing = 1.0, const double ac_dYSpacing = 1.0 );

      /**
        * Set the widget in the center of the arc
        * @param a_pWidget the widget
        * @param ac_bFollowThePhi if true, height = width * phi
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_SetCenterItem( QWidget* a_pWidget, const bool ac_bFollowThePhi = true, const double ac_dXSpacing = 1.0, const double ac_dYSpacing = 1.0 );

      /**
        * Set the layout in the center of the arc
        * @param a_pWidget the layout
        * @param ac_dXSpacing the x spacing
        * @param ac_dYSpacing the y spacing
        */
    void mp_SetCenterItem( CustomLayout* a_pWidget, const double ac_dXSpacing = 1.0, const double ac_dYSpacing = 1.0 );


      /**
        * Get the maximum number of items.
        * @return the number
        */
    const unsigned& mf_nGetNumItems() const
    { return mc_nItems; }

  private:
    tItemLayout* mf_pGenerateLayout( const unsigned ac_nWhere, const double ac_dXSpacing, const double ac_dYSpacing ) const;
    tItemLayout* mf_pGenerateCenterLayout( const double ac_dXSpacing, const double ac_dYSpacing ) const;

    const unsigned  mc_nItems;
    mt_eType        mv_eType;
  };

    /**
      * Apply a scale factor to all elements from the given layout.
      * Use after adding all elements.
      * Can be used eg to scale all elements to the desktop size if the
      * layout was written for a different size.
      * @param a_Layout the layout to scale
      * @param ac_dXScale the scale for the X direction
      * @param ac_dYScale the scale for the Y direction
      */
  void gf_ApplyScale( CustomLayout& a_Layout, const double ac_dXScale, const double ac_dYScale );

    /**
      * Apply an offset factor to all elements from the given layout.
      * Use after adding all elements.
      * @param a_Layout the layout to scale
      * @param ac_dXOffset the offset for the X direction
      * @param ac_dYOffset the offset for the Y direction
      */
  void gf_ApplyOffset( CustomLayout& a_Layout, const double ac_dXOffset, const double ac_dYOffset );

    /**
      * tDesktopInfo
      *   contains monitor info for dual screen computers
      *************************************************** */
  struct tDesktopInfo
  {
      /**
        * Define the 4 possible positions.
        */
    enum mt_Position
    {
      mc_e2RightOf1,
      mc_e2LeftOf1,
      mc_e2Under1,
      mc_e2Above1
    };

      /**
        * Get the coordinates of the specified screen.
        * Eg suppose mc_e2RightOf1 and 3200*1200 desktop, returns
        * QRect( 1599, 0, 1600, 1200 ) for screen 1.
        * @param a_Screen the rect to fill with coordinates.
        * @param ac_nScreen the screen for which to get coordinates. Only 0 or 1 for now.
        */
    void mf_GetScreen( QRect& a_Screen, const unsigned ac_nScreen ) const;

      /**
        * Set to true when using dual monitors. //FIXME how to find this out in code?
        */
    bool m_bDualMonitor;

      /**
        * Set to appropriate position. //FIXME how to find this out in code?
        */
    mt_Position m_ePosition;
  };

    /**
      * Apply desktop scale as described above.
      * @param a_Layout the layout to scale
      * @param ac_nOriginalX the X size a_Layout was designed for
      * @param ac_nOriginalY the Y size a_Layout was designed for
        @param a_pMonInfo the monitor info or 0 if only one screen is wanted
      * @param ac_bFirstScreen for a multi-screen setup, place the layout on the second screen if true.
      * @see tDesktopInfo
      */
  void gf_ApplyDesktopScale( CustomLayout& a_Layout, const int ac_nOriginalX, const int ac_nOriginalY, tDesktopInfo* a_pMonInfo = 0, const bool ac_bFirstScreen = true );

}

#endif //#ifndef CUSTOMLAYOUT_H
