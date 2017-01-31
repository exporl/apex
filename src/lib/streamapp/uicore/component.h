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
 
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "gfx.h"
#include "menuaction.h"

  /**
    * namespace with low-level rendering classes.
    * Don't expect fancy widgets here, the main purpose of the classes
    * is to provide a simple and fast gui control on top of demanding
    * applications like real-time video and audio processing.
    */
namespace uicore
{

  class Screen;
  class Painter;

    /**
      * Component
      *   anything that can be rendered on a Screen.
      *   All set methods can be called safely before
      *   mf_bCreate() is called. However, all changes
      *   made using these methods are only applied on
      *   on the next mf_bRender() call.
      *   Hence, the safe way to use a Component, is to
      *   call mf_bCreate() in the constructor of the
      *   implementation, eventually raise an error when this
      *   fails, then set at least size and position,
      *   and then call any of the rendering methods.
      *   The safe way to implement a Component, is to make sure
      *   never to draw off screen, as this leads to a fatal crash
      *   on most platforms. So it's best to check bounds, and
      *   print a debug message when this happens.
      ************************************************************* */
  class Component : public ActionComponent
  {
  protected:
      /**
        * Protected constructor.
        * Instance will be added to a_pParent.
        * @param a_pParent the parent screen, must be non-zero
        */
    Component( Screen* a_pParent );

  public:
      /**
        * Destructor.
        * Always calls mp_RemoveComponent() on the parent.
        */
    virtual ~Component();

      /**
        * Render the component.
        * A component must always be rendered completely.
        * @return false on render error
        */
    virtual void mf_Render() = 0;

      /**
        * Render the Component, as called by the application.
        * Applications will normally not use mf_bRender() directly,
        * but call the setter methods to tell the Component what they want it to look like.
        * The application loop will then call this method on a regular basis to
        * make sure the screen gets updated when needed.
        * The default implementation just calls show, or nothing,
        * depending on the flags set.
        * @return true for success
        */
    virtual void mf_Draw();

      /**
        * Render the Component, even if the dirt flag is not set.
        * @see mf_Draw();
        */
    virtual void mf_ReDraw();

      /**
        * Show the Component.
        * This sets the show flag, so the item will be rendered
        * on the next mf_bDraw() call.
        * @param ac_bShow true for show, false for hide.
        */
    INLINE virtual void mp_Show( const bool ac_bShow = true )
    {
      mp_SetDirty();
      mv_bShow = ac_bShow;
    }

      /**
        * HighLight the component.
        * This renders the component with a higher color value.
        * @param ac_bHighLight true for highlight
        */
    INLINE virtual void mp_HighLight( const bool ac_bHighLight = true )
    {
      mp_SetDirty();
      mv_bHighLight = ac_bHighLight;
    }

      /**
        * Set dirty, aka trigger a redraw.
        */
    INLINE void mp_SetDirty()
    {
      mv_bDirty = true;
    }

      /**
        * Unset the dirty flag.
        */
    INLINE void mp_SetNotDirty()
    {
      mv_bDirty = false;
    }

      /**
        * Set the size.
        * This method is always safe, since it clips the
        * size to the parent Screen's width and height.
        * @param ac_nX the width
        * @param ac_nY the height
        */
    virtual void mp_SetSize( const unsigned ac_nX, const unsigned ac_nY );

      /**
        * Set the position.
        * This method is always safe, since it clips the
        * position to the parent Screen's width and height.
        * @param ac_nX the x position
        * @param ac_nY the y position
        */
    virtual void mp_SetPosition( const unsigned ac_nX, const unsigned ac_nY );

      /**
        * Set the entire geometry in one go.
        * Provided for convenience.
        * @param ac_Rect the size and position
        */
    virtual void mp_SetGeometry( const tRect& ac_Rect );

      /**
        * Get the geometry rectangle.
        * @return a const reference.
        */
    INLINE const tRect& mf_tGetRect() const
    {
      return mv_Rect;
    }

      /**
        * Move a Component to another Screen.
        * The new Screen must be non-zero, a Component may *never* exist outside
        * a Screen's management since that would screw up the painting.
        * @param a_pNewParent the new parent Screen.
        */
    virtual void mp_ReparentComponent( Screen* a_pNewParent );

  protected:
    tRect   mv_Rect;
    bool    mv_bDirty;
    bool    mv_bShow;
    bool    mv_bHighLight;
    Screen* m_pParent;
  };

}

#endif //#ifndef __COMPONENT_H__
