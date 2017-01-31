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

#include "picturelabeleditordelegate.h"

#include "screenwidget.h"
#include "functions.h"

#include "apexdata/screen/picturelabelelement.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QVariant>
#include <QString>

namespace apex
{
  namespace editor
  {
    ScreenElement* PictureLabelEditorDelegate::getScreenElement()
    {
      return element;
    }


    PictureLabelEditorDelegate::PictureLabelEditorDelegate(
      PictureLabelElement* e, QWidget* parent, ScreenWidget* w )
      : LabelEditorDelegateBase( e, parent, w ),
        element( e )
    {
      resetText();
    }

    int PictureLabelEditorDelegate::getPropertyCount()
    {
      return ScreenElementEditorDelegate::getPropertyCount() + 1;
    }

    QString PictureLabelEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyName( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // picture
        return tr( "picture" );
      default:
        return QString();
      }
    }

    QVariant PictureLabelEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyData( nr, role );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // picture
        if ( role == Qt::DisplayRole )
          return element->getPicture();
      default:
        return QVariant();
      }
    }

    PropertyType PictureLabelEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyType( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // picture
        return ImagePropertyType;
      default:
        return IntPropertyType;
      }
    }

    bool PictureLabelEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::setProperty( nr, v );
      QString s;
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // default
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        element->setPicture( s );
        resetText();
        return true;
      default: return false;
      }
    }

    void PictureLabelEditorDelegate::mouseReleaseEvent( QMouseEvent* ev )
    {
      handleMouseReleaseEvent( ev, this );
    }

    const QString PictureLabelEditorDelegate::getText() const
    {
      QString def = element->getPicture();
      if ( def.isEmpty() ) def = tr( "Not set" );
      def = shortenString( def, 30 );
      return tr( "PictureLabel\n"
                 "Picture:\n%1" )
        .arg( def )
        ;
    }
    PictureLabelEditorDelegate::~PictureLabelEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }
  }
}

