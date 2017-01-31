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
 
#include "screenwidget.h"

#include "screeneditor.h"
#include "screen/screen.h"
#include "screen/screenelement.h"
#include "gui/guidefines.h"
#include "editordelegatecreatorvisitor.h"
#include "screenelementeditordelegate.h"

#include <QLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QLabel>
#include <QInputDialog>
#include <QSizePolicy>

#include <guicore/customlayout.h>
#include <guicore/widgetattributes.h>

namespace apex
{
  namespace editor
  {
    using data::ScreenElement;
    using gui::sc_DefaultBGColor;

    void ScreenWidget::buildScreenReps()
    {
      ScreenElement* root = getScreen()->getRootElement();
      EditorDelegateCreatorVisitor delcreator( this, this, elementToDelegateMap );
      rootDelegate =
        delcreator.createEditorDelegate( root );
      setRootDelegateGeometry();
      rootDelegate->getWidget()->show();
    }

    ScreenWidget::ScreenWidget( ScreenEditor* ed, Screen* s, QFont font )
      : QWidget( ed ),
        editor( ed ),
        screen( s ),
        defaultFont( font )
    {
//      guicore::widgetattributes::gf_SetBackGroundColor( this, sc_DefaultBGColor );
      setAutoFillBackground( true );
      setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    }

    ScreenEditor* ScreenWidget::getEditor()
    {
      return editor;
    }

    Screen* ScreenWidget::getScreen()
    {
      return screen;
    }

    const Screen* ScreenWidget::getScreen() const
    {
      return screen;
    }

    QFont ScreenWidget::getDefaultFont()
    {
      return defaultFont;
    }

    ScreenElementEditorDelegate* ScreenWidget::getEditorDelegate( ScreenElement* el )
    {
      elementToDelegateMapT::const_iterator it = elementToDelegateMap.find( el );
      if ( it == elementToDelegateMap.end() )
        return 0;
      else
        return it->second;
    }

    elementToDelegateMapT& ScreenWidget::getElementToDelegateMap()
    {
      return elementToDelegateMap;
    }

    void ScreenWidget::setRootDelegateGeometry()
    {
      rootDelegate->getWidget()->setGeometry( rect().adjusted( 3,3,-3,-3 ) );
    }

    void ScreenWidget::resizeEvent( QResizeEvent* e )
    {
      QWidget::resizeEvent( e );
      setRootDelegateGeometry();
    }

    void ScreenWidget::deleteDelegate( ScreenElementEditorDelegate* eldel, bool delEl )
    {
      ScreenElement* el = eldel->getScreenElement();
      // first delete the children...
      if ( el->canHaveChildren() )
      {
          for ( int i = 0; i < el->getNumberOfChildren(); ++i )
          {
              ScreenElement* childel = el->getChild( i );
              elementToDelegateMapT::iterator it = elementToDelegateMap.find( childel );
              Q_ASSERT( it != elementToDelegateMap.end() );
              ScreenElementEditorDelegate* childdel = it->second;
              deleteDelegate( childdel, delEl );
          }
      }
      delete eldel;
      if ( delEl )
        screen->deleteElement( el );
    }

    void ScreenWidget::delegateDeleted( ScreenElementEditorDelegate* eldel )
    {
      ScreenElement* el = eldel->getScreenElement();
      elementToDelegateMap.erase( el );
      if ( eldel == rootDelegate )
        rootDelegate = 0;
    }

    ScreenWidget::~ScreenWidget()
    {
      deleteDelegate( rootDelegate );
    }

  }
}
