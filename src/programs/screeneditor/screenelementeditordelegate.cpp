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

#include "apexdata/screen/screen.h"
#include "apexdata/screen/screenelement.h"

#include "screeneditor.h"
#include "screenelementeditordelegate.h"
#include "screenwidget.h"

#include <QFrame>
#include <QMouseEvent>

namespace apex
{
  namespace editor
  {
    ScreenElementEditorDelegate::ScreenElementEditorDelegate( ScreenWidget* w )
      : screenWidget( w )
    {
    }

    ScreenElementEditorDelegate::~ScreenElementEditorDelegate()
    {
    }

    void ScreenElementEditorDelegate::handleMouseReleaseEvent( QMouseEvent* me, QFrame* widget )
    {
      // if the user changes his mind and releases the mouse
      // not above our widget, we ignore...
      if ( !widget->rect().contains( me->pos() ) )
        return;
      // otherwise, the user wants to select this widget...
      screenWidget->getEditor()->selectWidget( widget, this );
    }

    void ScreenElementEditorDelegate::replaceChild(
      QWidget*, ScreenElementEditorDelegate*,
      ScreenElement* )
    {
      qFatal( "replaceChild on a non-layout delegate" );
    }

    int ScreenElementEditorDelegate::getPropertyCount()
    {
      return 1;
    }

    QString ScreenElementEditorDelegate::getPropertyName( int nr )
    {
      switch( nr )
      {
      case 0: return QObject::tr( "id" );
      default:
        qFatal( "should not get here.." );
        return QString();
      }
    }

    QVariant ScreenElementEditorDelegate::getPropertyData( int nr, int role )
    {
      switch( nr )
      {
      case 0:
        if ( role == Qt::DisplayRole )
          return getScreenElement()->getID();
        else return QVariant();
      default:
        qFatal( "should not get here.." );
        return QVariant();
      }
    }

    PropertyType ScreenElementEditorDelegate::getPropertyType( int nr )
    {
      switch( nr )
      {
      case 0:
        return StringPropertyType;
      default:
        qFatal( "should not get here.." );
        return IntPropertyType;
      }
    }

    bool ScreenElementEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      QString s;
      switch( nr )
      {
      case 0:
        if ( v.type() != QVariant::String )
          return false;
        s = v.toString();
        screenWidget->getScreen()->setElementID( getScreenElement(), s );
        return true;
      default:
        qFatal( "should not get here.." );
        return false;
      }
    }

  }
}
