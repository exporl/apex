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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_LAYOUTEDITORDELEGATE_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_LAYOUTEDITORDELEGATE_H_

#include "screenelementeditordelegate.h"
#include "screentypedefs.h"

#include <QWidget>
#include <QLayout>
#include <QFrame>


namespace apex
{
  namespace data
  {
    class ArcLayoutElement;
    class GridLayoutElement;
    class ScreenElement;
    class ScreenLayoutElement;
  }

  namespace editor
  {

    using data::ArcLayoutElement;
    using data::GridLayoutElement;
    using data::ScreenElement;
    using data::ScreenLayoutElement;


    /**
     * This class is a convenience parent class for
     * ScreenElementEditorDelegate's representing a layout element. It
     * is inherited by both GridLayoutEditorDelegate and
     * ArcLayoutEditorDelegate, and contains functions common to both
     * classes.
     */
    class LayoutEditorDelegate :
        public QFrame,
        public ScreenElementEditorDelegate
    {
      Q_OBJECT
    protected:
      LayoutEditorDelegate(
        QWidget* parent,
        ScreenWidget* widget );

      QFrame* getWidget();

      void replaceChild(
        QWidget* oldChildWidget, ScreenElementEditorDelegate* oldChildRep,
        data::ScreenElement* newElement );

      bool setProperty( int nr, const QVariant& v );
    protected:
      virtual QLayout* getLayout() = 0;
      virtual void addItemToLayout( QWidget* widget, ScreenElement* e ) = 0;

      void fixupChildren( ScreenLayoutElement* element );
      bool checkHarmlessChange( const ScreenLayoutElement* element );
      void updateLayoutChildren( elementToDelegateMapT& elementToDelegateMap );

      void mouseReleaseEvent( QMouseEvent* ev );
    };

  }
}
#endif
