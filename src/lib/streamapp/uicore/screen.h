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
 
#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "gfx.h"
#include "component.h"
#include "framebuffer.h"
#include "containers/ownedarray.h"
using namespace streamapp;

namespace uicore
{
  class Painter;
  class Component;
  class FrameBuffer;

    /**
      * Screen
      *   manages rendering for a FrameBuffer.
      *   Usage is straightforward: first call mf_bCreate(),
      *   then add Components you want to draw, then call
      *   mf_bDraw periodically to update the screen.
      *   Always deletes Components added, so don't go
      *   deleting them yourself.
      ****************************************************** */
  class Screen
  {
  public:
      /**
        * Constructor.
        * @param a_FrameBuffer the display to draw on.
        */
    Screen( FrameBuffer& a_FrameBuffer );

      /**
        * Destructor.
        */
    virtual ~Screen();

      /**
        * Array of components.
        */
    typedef OwnedArray<Component> mt_Components;

      /**
        * Draw the screen.
        * Default calls mf_bClear(),
        * then mf_bDraw() on each Component.
        * @return false if an error occurred.
        */
    virtual void mf_Draw();

      /**
        * Draw the screen without erasing it first.
        * This will save a lot of time and reduces flickering.
        * Components should use this in combination with a partial
        * mf_bClear().
        * Calls mf_bDraw() on each Component.
        * @return false if an error occurred.
        */
    virtual void mf_DrawNoErase();

      /**
        * Erase the entire screen to the color set.
        */
    virtual void mf_Erase();

      /**
        * Set the erase aka background color of the screen.
        * @param ac_nR red value
        * @param ac_nG green value
        * @param ac_nB blue value
        */
    void mp_SetEraseColor( const u8 ac_nR, const u8 ac_nG, const u8 ac_nB );

      /**
        * Set the erase aka background color of the screen.
        * @param ac_Color the color
        */
    void mp_SetEraseColor( const tRgb& ac_Color );

      /**
        * Get the erase color.
        * @return color as unsigned short
        */
    INLINE unsigned short mf_nGetEraseColor() const
    {
      return mv_EraseColor.mf_ColorConvert();
    }

      /**
        * Get the erase color.
        * @return the color as tRgb
        */
    INLINE const tRgb& mf_tGetEraseColor() const
    {
      return mv_EraseColor;
    }

      /**
        * Add a Component to the list of items to draw.
        * @param a_pComponent the component, must be non-zero
        */
    virtual void mp_AddComponent( Component* a_pComponent );

      /**
        * Remove a Component.
        * @param a_pComponent the one to remove
        */
    virtual void mp_RemoveComponent( Component* a_pComponent );

      /**
        * Remove all Components.
        */
    virtual void mp_RemoveAllComponents();

      /**
        * Get the width.
        * @return width in pixels.
        */
    INLINE const unsigned& mf_nWidth() const
    {
      return mc_nW;
    }

      /**
        * Get the height.
        * @return height in pixels.
        */
    INLINE const unsigned& mf_nHeight() const
    {
      return mc_nH;
    }

      /**
        * Get the framebuffer pointer.
        * @return the pointer.
        */
    INLINE unsigned short* mf_pGetDisplay() const
    {
      return m_FrameBuf.mf_pGetDisplay();
    }

      /**
        * Get a reference to the Component list.
        * @return const reference
        */
    INLINE const mt_Components& mf_GetComponents() const
    {
      return m_Comps;
    }

  private:
    friend void Component::mp_ReparentComponent( Screen* );

      /**
        * Remove a Component from the list without deleting it.
        * May only be accessed by Component::mp_ReparentComponent().
        * @param a_pComponent the item to remove
        */
    void mp_RemoveCompFromList( Component* a_pComponent );

    const unsigned  mc_nW;
    const unsigned  mc_nH;
    bool            mv_bDirty;
    tRgb            mv_EraseColor;
    Painter*        m_pEraser;
    FrameBuffer&    m_FrameBuf;
    mt_Components   m_Comps;
  };

}

#endif //#ifndef __SCREEN_H__
