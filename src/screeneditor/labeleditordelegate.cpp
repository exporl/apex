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
 
#include "labeleditordelegate.h"

#include "screenwidget.h"
#include "screeneditor.h"

#include "screen/labelelement.h"
#include "gui/guidefines.h"

#include <QVariant>
#include <QLabel>

#include <QInputDialog>
#include <QLineEdit>

namespace apex
{
  namespace editor
  {
    using gui::sc_nDefaultFontSize;
    using gui::sc_DefaultBGColor;

    ScreenElement* LabelEditorDelegate::getScreenElement()
    {
      return element;
    }

    int LabelEditorDelegate::getPropertyCount()
    {
      return LabelEditorDelegateBase::getPropertyCount() + 2;
    }

    QString LabelEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::getPropertyName( nr );
      switch( nr - LabelEditorDelegateBase::getPropertyCount() )
      {
      case 0:
        return tr( "Text" );
      case 1:
        return tr( "Font Size" );
      default:
        qFatal( "unknown parameter number" );
        return QString();
      }
    }

    QVariant LabelEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::getPropertyData( nr, role );
      switch( nr - LabelEditorDelegateBase::getPropertyCount() )
      {
      case 0:
        if ( role == Qt::DisplayRole )
          return element->getText();
        break;
      case 1:
        if ( role == Qt::DisplayRole )
          return element->getFontSize();
        break;
      default:
        qFatal( "unknown parameter number" );
        return QVariant();
      }
      return QVariant();
    }

    PropertyType LabelEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::getPropertyType( nr );
      switch( nr - LabelEditorDelegateBase::getPropertyCount() ) {
      case 0: return StringPropertyType;
      case 1: return FontSizePropertyType;
      default:
        qFatal( "unknown parameter number" );
        return IntPropertyType;

      }
    }

    bool LabelEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < LabelEditorDelegateBase::getPropertyCount() )
        return LabelEditorDelegateBase::setProperty( nr, v );
      int fs;
      QFont font;
      switch( nr - LabelEditorDelegateBase::getPropertyCount() )
      {
      case 0:
        if ( v.type() != QVariant::String ) return false;
        setText( v.toString() );
        return true;
      case 1:
        if ( v.type() != QVariant::Int ) return false;
        fs = v.toInt();
        if ( fs <= 0 )
          fs = sc_nDefaultFontSize;
        element->setFontSize( v.toInt() );
        resetText();
        return true;
      default:
        qFatal( "unknown parameter number" );
        return false;
      }
    }

    LabelEditorDelegate::LabelEditorDelegate( LabelElement* e, QWidget* parent, ScreenWidget* w )
      : LabelEditorDelegateBase( e, parent, w ),
        element( e )
    {
      resetText();
    }

    const QString LabelEditorDelegate::getText() const
    {
      return element->getText();
    }

    void LabelEditorDelegate::setText( const QString& s )
    {
      element->setText( s );
      resetText();
    }

    void LabelEditorDelegate::mouseDoubleClickEvent( QMouseEvent * )
    {
      bool ok = false;
      QString text = QInputDialog::getText(
        tr( "Label Text" ),
        tr( "Enter the text for the label with id %1." ).arg( element->getID() ),
        QLineEdit::Normal, element->getText(), &ok);
      if (ok)
      {
        setText( text );
        screenWidget->getEditor()->selectWidget( getWidget(), this );
      }
    }

    LabelEditorDelegate::~LabelEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }
  }
}

