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

#include "apexdata/screen/texteditelement.h"

#include "apexmain/gui/guidefines.h"

#include "propertieseditor.h"
#include "screeneditor.h"
#include "screenwidget.h"
#include "textediteditordelegate.h"

#include <QEvent>
#include <QIntValidator>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QVariant>

namespace apex
{
  namespace editor
  {
    using gui::sc_nDefaultFontSize;

    TextEditEditorDelegate::TextEditEditorDelegate(
      TextEditElement* e, QWidget* parent, ScreenWidget* w )
      : QFrame( parent ),
        ScreenElementEditorDelegate( w ),
        element( e ),
        lineedit( 0 ),
        ignoretextchangedsignal( false )
    {
      QVBoxLayout* l = new QVBoxLayout( this );
      l->addStretch();

      lineedit = new QLineEdit( this );
      lineedit->setText( e->getText() );
      connect( lineedit, SIGNAL( textEdited( const QString& ) ),
               this, SLOT( textEdited( const QString& ) ) );
      lineedit->installEventFilter( this );
      l->addWidget( lineedit );
      l->addStretch();
      setLayout( l );

      lineedit->setFont( getFont() );

      QString sMask = e->getInputMask();
      lineedit->setInputMask( sMask );
    }

    TextEditEditorDelegate::~TextEditEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }

    QFrame* TextEditEditorDelegate::getWidget()
    {
      return this;
    }

    ScreenElement* TextEditEditorDelegate::getScreenElement()
    {
      return element;
    }

    int TextEditEditorDelegate::getPropertyCount()
    {
      return ScreenElementEditorDelegate::getPropertyCount() + 3;
    }

    QString TextEditEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyName( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0:
        return tr( "Text" );
      case 1:
        return tr( "Input Mask" );
      case 2:
        return tr( "Font Size" );
      default:
        return QString();
      }
    }

    QVariant TextEditEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyData( nr, role );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0:
        if ( role == Qt::DisplayRole )
          return element->getText();
        return QVariant();
      case 1:
        if ( role == Qt::DisplayRole )
          return element->getInputMask();
        return QVariant();
      case 2:
        if ( role == Qt::DisplayRole )
          return element->getFontSize();
        return QVariant();
      default:
        return QVariant();
      }
    }

    PropertyType TextEditEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyType( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0:
        return StringPropertyType;
      case 1:
        return InputMaskPropertyType;
      case 2:
        return FontSizePropertyType;
      default:
        return IntPropertyType;
      }
    }

    bool TextEditEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::setProperty( nr, v );
      QString s;
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0:
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        setText( s );
        return true;
      case 1:
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        setInputMask( s );
        return true;
      case 2:
        if ( v.type() != QVariant::Int ) return false;
        element->setFontSize( v.toInt() );
        lineedit->setFont( getFont() );
        return true;
      default:
        return false;
      }
    }

    void TextEditEditorDelegate::mouseReleaseEvent( QMouseEvent* ev )
    {
      handleMouseReleaseEvent( ev, this );
    }

    void TextEditEditorDelegate::mouseDoubleClickEvent( QMouseEvent* )
    {
    }

    void TextEditEditorDelegate::setInputMask( const QString& s )
    {
      ignoretextchangedsignal = true;
      if ( s == "numbers" )
      {
        lineedit->setInputMask( "" );
        lineedit->setValidator( new QIntValidator( -32768, 32767, this ) );
      }
      else
      {
        lineedit->setValidator( 0 );
        lineedit->setInputMask( s );
      }
      element->setInputMask( s );
      ignoretextchangedsignal = false;
    }

    void TextEditEditorDelegate::setText( const QString& s )
    {
      lineedit->setText( s );
      element->setText( s );
    }

    QFont TextEditEditorDelegate::getFont()
    {
      QFont font = lineedit->font();
      if ( getScreenElement()->getFontSize() > 0 )
        font.setPointSize( getScreenElement()->getFontSize() );
      else
        font.setPointSize( sc_nDefaultFontSize );
      return font;
    }

    void TextEditEditorDelegate::textEdited( const QString& s )
    {
      if ( ignoretextchangedsignal ) return;
      setText( s );
      screenWidget->getEditor()->getPropertiesEditor()->reset();
    }

    bool TextEditEditorDelegate::eventFilter( QObject* o, QEvent* e )
    {
      if ( o == lineedit )
      {
        if ( e->type() == QEvent::FocusIn )
          screenWidget->getEditor()->selectWidget( getWidget(), this );
        return false;
      }
      else return false;
    }

  }
}
