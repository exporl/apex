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
 
#ifndef __WIDGETATTRIBUTES_H__
#define __WIDGETATTRIBUTES_H__

class QRect;
class QColor;
class QLabel;
class QString;
class QWidget;
class QPalette;

namespace guicore
{

  class tItemLayout;

    /**
      * namespace for operations on widgets
      ************************************* */
  namespace widgetattributes
  {
      /**
        * Set horizontal and vertical center alignment.
        * @param a_pVictim the label to act on
        */
    void gf_ApplyDefaultAlignment( QLabel* a_pVictim );

      /**
        * Set the text size on a widget.
        * @param a_pVictim the label to act on
        * @param ac_nHowPhat the text size
        */
    void gf_ApplyPhatText( QLabel* a_pVictim, const int ac_nHowPhat = 24 );

      /**
        * Apply size relative to the desktop.
        * @param a_pVictim the widget to act on
        * @param ac_dXSize the x scale factor
        * @param ac_dYSize the y scale factor
        */
    void gf_SizeToDesktop( QWidget* a_pVictim, const double ac_dXSize = 1.0, const double ac_dYSize = 1.0 );

      /**
        * Apply layout relative to the desktop.
        * @param a_pVictim the widget to act on
        * @param ac_Layout the layout info
        */
    void gf_ScaleToDesktop( QWidget* a_pVictim, tItemLayout& ac_Layout );

      /**
        * Set fixed pixel size.
        * @param a_pVictim the widget to act on
        * @param ac_nWidth the x size in pixels
        * @param ac_nHeight the y size in pixels
        */
    void gf_SetFixedSize( QWidget* a_pVictim, const unsigned ac_nWidth, const unsigned ac_nHeight );

      /**
        * Set background color.
        * @param a_pVictim the widget to act on
        * @param ac_Color the color
        */
    void gf_SetBackGroundColor( QWidget* a_pVictim, const QColor& ac_Color );

      /**
        * Set foreground color.
        * @param a_pVictim the widget to act on
        * @param ac_Color the color
        */
    void gf_SetForeGroundColor( QWidget* a_pVictim, const QColor& ac_Color );

      /**
        * Shrink text so it fits the widget.
        * Doesn't shrink if it fits already.
        * Shrinks until fontize = 1 if it doesn't fit.
        * @param a_pVictim the widget the text is on
        * @param ac_sText the text to fit on the widget
        * @param ac_nWidthBorder the number of pixels to reserve from the left/right side
        * @param ac_nHeightBorder the number of pixels to reserve from the top/bottom side
        */
    void gf_ShrinkTillItFits( QWidget* a_pVictim, const QString& ac_sText, const int ac_nWidthBorder = 2, const int ac_nHeightBorder = 2 );

      /**
        * Set the disabled background color of the palette.
        * @param ac_DisabledBackGroundColor the color
        * @return the palette, must be deleted by acquirer
        */
    QPalette* gf_pInitPaletteWithBGColor( const QColor& ac_DisabledBackGroundColor );

      /**
        * Set all backgroundcolors of the palette.
        * @param ac_DisabledBackGroundColor the color
        * @return the palette, must be deleted by acquirer
        */
    QPalette* gf_pInitEntirePaletteWithBGColor( const QColor& ac_DisabledBackGroundColor );

      /**
        * Set the global palette used for gf_Indicator.
        * @param ac_pPalette the palette or 0 to reset
        */
    void gf_SetIndicatorPalette( const QPalette* ac_pPalette );

      /**
        * Get the global palette used for gf_Indicator.
        * @return the current palette
        */
    const QPalette* gf_pGetIndicatorPalette();

      /**
        * Disables the widget.
        * Sets palette to ac_pPalette if given, else to the global palette.
        * @param a_pVictim the widget to disable
        * @param ac_pPalette the optional palette
        */
    void gf_Indicator( QWidget* a_pVictim, const QPalette* ac_pPalette = 0 );

      /**
        * Enables the widget.
        * Sets palette to ac_pPalette if given.
        * @param a_pVictim the widget to disable
        * @param ac_pPalette the optional palette
        */
    void gf_Control( QWidget* a_pVictim, const QPalette* ac_pPalette = 0  );
  };

}

#endif //#ifndef __WIDGETATTRIBUTES_H__
