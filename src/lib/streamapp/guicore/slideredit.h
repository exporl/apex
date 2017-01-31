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
 
#ifndef __SLIDEREDIT_H__
#define __SLIDEREDIT_H__

#include "guicore/labeledit.h"

class QSlider;

namespace guicore
{

  class FriendLayout;

    /**
      * SliderEdit
      *   a LineEdit with a slider attached.
      ************************************** */
  class SliderEdit : public LineEdit
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget or  for desktop level
        * @param ac_Orient the slider orientation, vertical or horizontal
        * @param ac_eWhere the slider's position relative to the edit
        */
    SliderEdit( QWidget* a_pParent, const Qt::Orientation ac_Orient = Qt::Horizontal, const guicore::FriendLayout::mt_eFriendPosition ac_eWhere = guicore::FriendLayout::right );

      /**
        * Destructor.
        */
    ~SliderEdit();

      /**
        * Set min, max and step.
        * @param ac_nMin the minimum position allowed
        * @param ac_nMax the maximum position allowed
        * @param ac_nStep the stepsize
        */
    void mp_SetSliderStuff( const int ac_nMin, const int ac_nMax, const int ac_nStep );

      /**
        * Set whether or not to continuously emit when the slider is moved.
        * @param ac_bSet if false, only emits after slider is released
        */
    void mp_SetTracking( const bool ac_bSet );

      /**
        * Set the divider for the position.
        * LineEdit value = emit value = position / divider
        * @param ac_dDivider the new divider value
        */
    INLINE void mp_SetDivider( const double ac_dDivider )
    {
      mv_dDivider = ac_dDivider;
    }

      /**
        * Set which signal to emit.
        * @param ac_bSet true for double, false for QString
        */
    INLINE void mp_SetEmitDouble( const bool ac_bSet = true )
    {
      mv_bEmitDouble = ac_bSet;
    }

      /**
        * Set the slider's position.
        * Ignores divider.
        * @param ac_nPos the new position
        */
    void mp_SetPosition( const int ac_nPos );

      /**
        * Set the value.
        * Uses divider, sets both slider and edit.
        * @param ac_dValue the new value
        */
    void mp_SetValue( const double ac_dValue );

      /**
        * Set the value and emit.
        * Uses divider, sets both slider and edit.
        * @param ac_dValue the new value
        */
    void mp_SetValueSignaling( const double ac_dValue );

      /**
        * Set the size.
        * @param ac_Rect the new geometry
        */
    void setGeometry( const QRect &ac_Rect );

  signals:
      /**
        * Emitted when the value changes
        */
    void ms_ValueChange( QString );

      /**
        * Emitted when the value changes
        */
    void ms_ValueChange( double );

  protected:
      /**
        * Override and ignore
        */
    void resizeEvent( QResizeEvent* )
    {}

  private slots:
    void mp_SliderReleased();
    void mp_SliderChanged( int );
    void mp_EditChanged();
    void mp_EditEntered();

  private:
    QSlider* const  mc_pButton;
    FriendLayout*   mc_pLayout;
    double          mv_dDivider;
    bool            mv_bEmitDouble;

    SliderEdit( const SliderEdit& );
    SliderEdit& operator = ( const SliderEdit& );
  };

}

#endif //#ifndef __SLIDEREDIT_H__
