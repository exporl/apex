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

#include "apexdata/screen/emptyelement.h"
#include "apexdata/screen/screen.h"
#include "apexdata/screen/screenlayoutelement.h"

#include "editordelegatecreatorvisitor.h"
#include "layouteditordelegate.h"
#include "screenwidget.h"

#include <QFrame>

namespace apex
{
  namespace editor
  {
    using data::EmptyElement;

    LayoutEditorDelegate::LayoutEditorDelegate(
      QWidget* parent,
      ScreenWidget* widget )
      : QFrame(parent),
        ScreenElementEditorDelegate( widget )
    {
    }

    QFrame* LayoutEditorDelegate::getWidget()
    {
      return this;
    }

    void LayoutEditorDelegate::mouseReleaseEvent( QMouseEvent* ev )
    {
      handleMouseReleaseEvent( ev, this );
    }

    bool LayoutEditorDelegate::checkHarmlessChange( const ScreenLayoutElement* element )
    {
      bool ret = true;
      for ( int i = 0; i < element->getNumberOfChildren(); ++i )
      {
        const ScreenElement* el = element->getChild( i );

        try {
          element->checkChild( el );
        } catch( ... ) {
          // problem with child, doesn't matter if it's an EmptyElement
          if ( !dynamic_cast<const EmptyElement*>( el ) )
          {
            ret = false;
          }
        }
      }
      return ret;
    }

    void LayoutEditorDelegate::fixupChildren(
      ScreenLayoutElement* element )
    {
      std::vector<ScreenElement*> children = *element->getChildren();
      for ( unsigned i = 0; i < children.size(); ++i )
      {
        ScreenElement* el = children[i];

        try {
          element->checkChild( el );
        } catch( ... ) {
          // delete this child element..
          ScreenElementEditorDelegate* eldel = screenWidget->getEditorDelegate( el );
          getLayout()->removeWidget( eldel->getWidget() );
          screenWidget->deleteDelegate( eldel );
        }
      }
      element->fillChildrenWithEmpties(screenWidget->getScreen());
    }

    void LayoutEditorDelegate::updateLayoutChildren( elementToDelegateMapT& elementToDelegateMap )
    {
      for ( int i = 0; i < getScreenElement()->getNumberOfChildren(); ++i )
      {
        ScreenElement* el = getScreenElement()->getChild( i );
        ScreenElementEditorDelegate* eldel = 0;
        elementToDelegateMapT::const_iterator eldelit = elementToDelegateMap.find( el );
        if ( eldelit == elementToDelegateMap.end() )
        {
          EditorDelegateCreatorVisitor delcreator( this, screenWidget, elementToDelegateMap );
          eldel = delcreator.createEditorDelegate( el );
          elementToDelegateMap[el] = eldel;
        } else {
          eldel = eldelit->second;
        }

        addItemToLayout( eldel->getWidget(), el );
        eldel->getWidget()->show();
      }
//      mp_DoResize();      // FIXME

    }

    void LayoutEditorDelegate::replaceChild(
      QWidget* oldChildWidget, ScreenElementEditorDelegate* oldChildRep,
      apex::data::ScreenElement* newElement )
    {
      int oldx = oldChildRep->getScreenElement()->getX();
      int oldy = oldChildRep->getScreenElement()->getY();
      getLayout()->removeWidget( oldChildWidget );
      screenWidget->deleteDelegate( oldChildRep );
      newElement->setX( oldx );
      newElement->setY( oldy );
      EditorDelegateCreatorVisitor delcreator( this, screenWidget, screenWidget->getElementToDelegateMap() );
      ScreenElementEditorDelegate* newRep =
        delcreator.createEditorDelegate( newElement );
      if ( newRep )
        addItemToLayout( newRep->getWidget(), newElement );
      update();
      if ( newRep )
        newRep->getWidget()->show();
    }

    bool LayoutEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      return ScreenElementEditorDelegate::setProperty( nr, v );
    }
  }
}
