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
 
#include "answerlabeleditordelegate.h"

#include "screenwidget.h"

#include "screen/answerlabelelement.h"

#include <QVariant>
#include <QLabel>

namespace apex
{
  namespace editor
  {
    ScreenElement* AnswerLabelEditorDelegate::getScreenElement()
    {
      return element;
    }

    int AnswerLabelEditorDelegate::getPropertyCount()
    {
      return LabelEditorDelegateBase::getPropertyCount() + 1;
    }

    QString AnswerLabelEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::getPropertyName( nr );
      switch( nr - LabelEditorDelegateBase::getPropertyCount() )
      {
      case 0:
        return tr( "Font size" );
      default:
        return QString();
      };
    }

    QVariant AnswerLabelEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::getPropertyData( nr, role );
      switch( nr - LabelEditorDelegateBase::getPropertyCount() )
      {
      case 0: // font size
        if ( role == Qt::DisplayRole )
          return element->getFontSize();
      }
      return QVariant();
    }

    PropertyType AnswerLabelEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::getPropertyType( nr );
      return FontSizePropertyType;
    }

    bool AnswerLabelEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::setProperty( nr, v );
      switch( nr - LabelEditorDelegateBase::getPropertyCount() )
      {
      case 0:
        if ( v.type() == QVariant::Int )
        {
          element->setFontSize( v.toInt() );
          resetText();
          return true;
        }
      }
      return false;
    }

    AnswerLabelEditorDelegate::AnswerLabelEditorDelegate(
      AnswerLabelElement* e, QWidget* parent, ScreenWidget* w )
      : LabelEditorDelegateBase( e, parent, w ),
        element( e )
    {
      resetText();
    }

    const QString AnswerLabelEditorDelegate::getText() const
    {
      return tr( "Answer label" );
    }

    AnswerLabelEditorDelegate::~AnswerLabelEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }
  }
}
