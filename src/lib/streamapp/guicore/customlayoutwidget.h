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
 
#ifndef __CUSTOMLAYOUTWIDGET_H__
#define __CUSTOMLAYOUTWIDGET_H__

#include "customlayout.h"

#if QT_VERSION < 0x040000
#include <qwidget.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#define _QButtonGroup QButtonGroup
#else
#include <QWidget>
#include <QGroupBox>
#define _QButtonGroup QGroupBox
#endif

namespace guicore
{
    /**
      * CustomLayoutWidget
      *   template base class for widgets using CustomLayout.
      ******************************************************* */
  template<class tType = QWidget>
  class CustomLayoutWidget : public tType
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget or 0 for desktop level
        */
    CustomLayoutWidget( QWidget* a_pParent ) :
        tType( a_pParent ),
      m_pLayout( 0 ),
      mv_bDeleteLayout( true )
    {
    }

      /**
        * Destructor.
        * Deletes layout if mv_bDeleteLayout == true
        */
    virtual ~CustomLayoutWidget()
    {
      if( mv_bDeleteLayout )
        delete m_pLayout;
    }

      /**
        * The actual resizing.
        * Only has effect when the layout is set.
        */
    virtual void mp_DoResize()
    {
      if( m_pLayout )
        m_pLayout->mp_ResizeThemAll( tType::rect() );
    }

      /**
        * Geometry override.
        * @param ac_Rect the new geometry
        */
    virtual void setGeometry( const QRect &ac_Rect )
    {
      tType::setGeometry( ac_Rect );
      mp_DoResize();
    }

      /**
        * Set the layout.
        * @param a_pLayout the layout to use.
        * @param ac_bDelete if true, a_pLayout will be deleted upon destruction
        */
    void mp_SetLayout( CustomLayout* a_pLayout, const bool ac_bDelete = true )
    {
      m_pLayout = a_pLayout;
      mv_bDeleteLayout = ac_bDelete;
    }

  protected:
      /**
        * ResizeEvent override.
        * @param e the new size
        */
    virtual void resizeEvent( QResizeEvent* e )
    {
      tType::resizeEvent( e );
      mp_DoResize();
    }

    CustomLayout* m_pLayout;
    bool          mv_bDeleteLayout;
  };

    /**
      * Default CustomLayoutWidget
      */
  typedef CustomLayoutWidget<> gt_CustomLayoutWidget;

    /**
      * ButtonGroup
      *   template specialization of CustomLayoutWidget for QButtonGroup.
      ******************************************************************* */
  class ButtonGroup : public CustomLayoutWidget<_QButtonGroup>
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget or 0 for desktop level
        */
    ButtonGroup( QWidget* a_pParent ) :
      CustomLayoutWidget<_QButtonGroup>( a_pParent )
    {}

      /**
        * Destructor.
        */
    virtual ~ButtonGroup()
    {}

      /**
        * The actual resizing.
        * Uses contentsRect instead of rect().
        */
    virtual void mp_DoResize()
    {
      if( m_pLayout )
        m_pLayout->mp_ResizeThemAll( _QButtonGroup::contentsRect() );
    }
  };

}

#endif //#ifndef __CUSTOMLAYOUTWIDGET_H__
