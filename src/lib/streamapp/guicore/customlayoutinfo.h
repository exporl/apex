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
 
#ifndef __LAYOUTINFO_H__
#define __LAYOUTINFO_H__

class QWidget;

namespace guicore
{

    /**
      * tItemLayout
      *   all layout information for (rectangular) widgets/layouts ("items").
      *   A tItemLayout contains all info needed to calculate the size and
      *   position of an item on the screen. An item can be a widget, or
      *   another tItemLayout.
      *   Everything is expressed with ratios between 0 and 1, which are
      *   relative to the parent item (so not to the entire screen).
      *   The main info constists of offset and size:
          @verbatim

              0   Xoff              1
            0 |---------------------|
              |                     |
         Yoff |     |-----|         |   -
              |     |     |         |   | Ysize
              |     |-----|         |   -
              |                     |
            1 |---------------------|

                    |-----| Xsize

          @endverbatim
      *
      *   Offset and size are simply used to calculate the space the item
      *   gets in the parent rectangle.
      *
      *   Furthermore there is spacing, which tells how much space
      *   the item actually uses. This is used to avoid sticking items
      *   to each other: suppose a 2*2 grid for 100*100 pixels with 4 buttons,
      *   then every button gets exactly 50*50 pixels, hence each button touches
      *   3 other buttons. By setting spacing to 0.9, the 50*50 rectangle is
      *   shrinked to 45*45 pixels, but the center remains the same and the buttons
      *   do not touch each other.
      *
      *   It is also possible to define a minimum/maximum/preferred size in pixels
      *   to get exactly the same functionality as Qt. Setting absolute size
      *   is possible by setting minimum and maximum to the same value.
      *   tItemLayout is used by CustomLayout, which calculates the layout of
      *   items based on the parent recatangle.
      *   @see tLayoutItem
      *   @see CustomLayout
      ***************************************************************************** */
  class tItemLayout
  {
  public:
      /**
        * Constructor.
        * Sets the basic info.
        * @param ac_Xoff the x offset
        * @param ac_Yoff the y offset
        * @param ac_Xsiz the x size
        * @param ac_Ysiz the y size
        * @param ac_Xspa the x spacing
        * @param ac_Yspa the y spacing
        */
    tItemLayout( const double ac_Xoff, const double ac_Yoff,
                 const double ac_Xsiz, const double ac_Ysiz,
                 const double ac_Xspa = 1.0, const double ac_Yspa = 1.0 ) :
      m_dXoff( ac_Xoff ), m_dYoff( ac_Yoff ),
      m_dXsiz( ac_Xsiz ), m_dYsiz( ac_Ysiz ),
      m_dXspa( ac_Xspa ), m_dYspa( ac_Yspa ),
      m_bMinMax( false ),
      m_bPhiRatio( false )
    {}

      /**
        * Copy Constructor.
        */
    tItemLayout( const tItemLayout& ac_Rh ) :
      m_dXoff( ac_Rh.m_dXoff ), m_dYoff( ac_Rh.m_dYoff ),
      m_dXsiz( ac_Rh.m_dXsiz ), m_dYsiz( ac_Rh.m_dYsiz ),
      m_dXspa( ac_Rh.m_dXspa ), m_dYspa( ac_Rh.m_dYspa ),
      m_bMinMax( ac_Rh.m_bMinMax ),
      m_bPhiRatio( ac_Rh.m_bPhiRatio )
    {}

      /**
        * Default Constructor.
        * Initializes all values to use maximum size possible.
        */
    tItemLayout() :
      m_dXoff( 0.0 ), m_dYoff( 0.0 ),
      m_dXsiz( 1.0 ), m_dYsiz( 1.0 ),
      m_dXspa( 1.0 ), m_dYspa( 1.0 ),
      m_bMinMax( false ),
      m_bPhiRatio( false )
    {}

      /**
        * Operator =.
        */
    tItemLayout& operator=( const tItemLayout& )
    { return *this; }

      /**
        * Set to use Phi ratio.
        * If set, the element's height == phi * width.
        * This looks best since the whole nature is based on it, go figure.
        * @param ac_bSet false if not using ratio
        */
    void mp_SetUsePhi( const bool ac_bSet )
    { m_bPhiRatio = ac_bSet; }

      /**
        * Set to use min/max pixel restrictions.
        * If set, the element's size will be at least the minimum,
        * and no bigger then the maximum.
        * @param ac_bSet false if not using min/max
        */
    void mp_SetUseMinMax( const bool ac_bSet )
    { m_bMinMax = ac_bSet; }

      /**
        * Offset ratio.
        */
    double  m_dXoff;
    double  m_dYoff;

      /**
        * Size ratio.
        */
    double  m_dXsiz;
    double  m_dYsiz;

      /**
        * Spacing ratio.
        */
    double  m_dXspa;
    double  m_dYspa;

      /**
        * Minimum, maximum and preferred *in pixels*.
        */
    int     m_nMinX;
    int     m_nMinY;
    int     m_nPrefX;
    int     m_nPrefY;
    int     m_nMaxX;
    int     m_nMaxY;

      /**
        * Layout flags.
        */
    bool    m_bMinMax;
    bool    m_bPhiRatio;
  };

  class CustomLayout;

    /**
      * tLayoutItem
      *   class representing the items that can be used by CustomLayout.
      *   An item is either a widget or a layout, to allow nesting layouts.
      ********************************************************************* */
  class tLayoutItem
  {
  public:
      /**
        * Constructor for widgets.
        * @param a_pWidget the layout item
        */
    tLayoutItem( QWidget* a_pWidget );

      /**
        * Constructor for layouts.
        * @param a_pWidget the layout item
        */
    tLayoutItem( CustomLayout* a_pWidget );

      /**
        * Copy Constructor.
        */
    tLayoutItem( const tLayoutItem& ac_Rh );

      /**
        * Destructor.
        */
    ~tLayoutItem();

      /**
        * Operator =.
        */
    tLayoutItem& operator=( const tLayoutItem& )
    { return *this; }

    friend class CustomLayout;

  protected:
    QWidget* const        mc_pWidget;
    CustomLayout* const   mc_pLayout;
    const bool            mc_bIsWidget;
  };

}

#endif //#ifndef __LAYOUTINFO_H__
