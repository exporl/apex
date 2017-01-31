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
 
#include "emptyelementeditordelegate.h"

#include "screen/emptyelement.h"
#include "gui/guidefines.h"
#include "screenwidget.h"

#include <QVariant>
#include <QString>

namespace apex
{
  namespace editor
  {
    EmptyElementEditorDelegate::EmptyElementEditorDelegate( EmptyElement* e, QWidget* parent, ScreenWidget* w )
      : LabelEditorDelegateBase( e, parent, w ),
        element( e )
    {
      resetText();
    }

    ScreenElement* EmptyElementEditorDelegate::getScreenElement()
    {
      return element;
    }

    int EmptyElementEditorDelegate::getPropertyCount()
    {
      return 0;
    }

    QString EmptyElementEditorDelegate::getPropertyName( int )
    {
      qFatal( "should not be called" );
      return QString();
    }

    QVariant EmptyElementEditorDelegate::getPropertyData( int, int )
    {
      qFatal( "should not be called" );
      return QVariant();
    }

    PropertyType EmptyElementEditorDelegate::getPropertyType( int )
    {
      qFatal( "should not be called" );
      return IntPropertyType;
    }

    bool EmptyElementEditorDelegate::setProperty( int, const QVariant& )
    {
      qFatal( "should not be called" );
      return false;
    }

    const QString EmptyElementEditorDelegate::getText() const
    {
      return tr( "<Empty>" );
    }

    EmptyElementEditorDelegate::~EmptyElementEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }

  }
}
