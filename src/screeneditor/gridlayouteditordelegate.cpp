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
 
#include "gridlayouteditordelegate.h"

#include "properties.h"
#include "screenwidget.h"

#include "screen/gridlayoutelement.h"

#include <guicore/customlayout.h>

#include <QMessageBox>
#include <QVariant>
#include <QString>
#include <assert.h>

namespace apex
{
  namespace editor
  {

    ScreenElement* GridLayoutEditorDelegate::getScreenElement()
    {
      return element;
    }

    int GridLayoutEditorDelegate::getPropertyCount()
    {
      return LayoutEditorDelegate::getPropertyCount() + 2;
    }

    QString GridLayoutEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < LayoutEditorDelegate::getPropertyCount() )
        return LayoutEditorDelegate::getPropertyName( nr );
      switch( nr - LayoutEditorDelegate::getPropertyCount() )
      {
      case 0: return tr( "width" );
      case 1: return tr( "height" );
      default: qFatal( "unknown parameter number" ); return QString();
      }
    }

    QVariant GridLayoutEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < LayoutEditorDelegate::getPropertyCount() )
        return LayoutEditorDelegate::getPropertyData( nr, role );
      switch( nr - LayoutEditorDelegate::getPropertyCount() )
      {
      case 0:
        if ( role == Qt::DisplayRole )
          return element->getWidth();
        else return QVariant();
      case 1:
        if ( role == Qt::DisplayRole )
          return element->getHeight();
        else return QVariant();
      default:
        qFatal( "unknown parameter number" ); return QVariant();
      }
    }

    PropertyType GridLayoutEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < LayoutEditorDelegate::getPropertyCount() )
        return LayoutEditorDelegate::getPropertyType( nr );
      switch( nr - LayoutEditorDelegate::getPropertyCount() )
      {
      case 0: return IntPropertyType;
      case 1: return IntPropertyType;
      default:
        qFatal( "unknown parameter number" );
        return IntPropertyType;
      }
    }

    bool GridLayoutEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < LayoutEditorDelegate::getPropertyCount() )
        return LayoutEditorDelegate::setProperty( nr, v );
      // no element declarations in switch
      bool harmlesschange;
      int nval;
      int oldval;
      int msgret;
      switch( nr - LayoutEditorDelegate::getPropertyCount() )
      {
      case 0:
        if ( v.type() != QVariant::Int ) return false;
        nval = v.toInt();
        if ( nval <= 0 ) return false;
        oldval = element->getWidth();
        if ( nval < oldval )
        {
          element->setWidth( nval );
          harmlesschange =
            checkHarmlessChange( element );
          element->setWidth( oldval );
        }
        else harmlesschange = true;

        if ( !harmlesschange )
        {
          msgret = QMessageBox::warning(
            this, tr( "GridLayout width change" ),
            tr( "Changing the gridlayout width will delete child elements.\n"
                "Do you want to continue ?" ),
            tr( "Continue" ), tr( "Cancel" ), QString(), 0, 1 );
          harmlesschange = ( msgret == 0 );
        }

        if ( harmlesschange )
        {
          element->setWidth( nval );
          fixupChildren( element );
          updateLayout( screenWidget->getElementToDelegateMap() );
          return true;
        }
        else
          return false;
      case 1: // height
        if ( v.type() != QVariant::Int ) return false;
        nval = v.toInt();
        if ( nval <= 0 ) return false;
        oldval = element->getHeight();
        if ( nval < oldval )
        {
          element->setHeight( nval );
          harmlesschange =
            checkHarmlessChange( element );
          element->setHeight( oldval );
        }
        else harmlesschange = true;

        if ( !harmlesschange )
        {
          msgret = QMessageBox::warning(
            this, tr( "GridLayout width change" ),
            tr( "Changing the gridlayout height will delete child elements.\n"
                "Do you want to continue ?" ),
            tr( "Continue" ), tr( "Cancel" ), QString(), 0, 1 );
          harmlesschange = ( msgret == 0 );
        }

        if ( harmlesschange )
        {
          element->setHeight( nval );
          fixupChildren( element );
          updateLayout( screenWidget->getElementToDelegateMap() );
          return true;
        }
        else
          return false;
      default:
        qFatal( "unknown parameter number" );
        return false;
      }
    }

    GridLayoutEditorDelegate::GridLayoutEditorDelegate(
      GridLayoutElement* e, QWidget* parent, ScreenWidget* widget,
      elementToDelegateMapT& elementToDelegateMap )
      : LayoutEditorDelegate( parent, widget ),
        element( e ),
        gridlayout( 0 )
    {
      e->fillChildrenWithEmpties( widget->getScreen() );
      updateLayout( elementToDelegateMap );
    }

    void GridLayoutEditorDelegate::updateLayout( elementToDelegateMapT& elementToDelegateMap )
    {
      QGridLayout* oldlayout = gridlayout;
      gridlayout = new QGridLayout( element->getHeight(), element->getWidth() );
      setLayout( gridlayout );
      delete oldlayout;
      updateLayoutChildren( elementToDelegateMap );
    }

    QGridLayout* GridLayoutEditorDelegate::getLayout()
    {
      return gridlayout;
    }

    void GridLayoutEditorDelegate::addItemToLayout( QWidget* widget, ScreenElement* e )
    {
        gridlayout->addWidget( widget, e->getY(), e->getX() );
    }

    GridLayoutEditorDelegate::~GridLayoutEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }
  }
}
