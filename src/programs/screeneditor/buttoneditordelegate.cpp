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

#include "apexdata/screen/buttonelement.h"

#include "apexmain/gui/guidefines.h"

#include "apextools/apextools.h"

#include "buttoneditordelegate.h"
#include "screeneditor.h"
#include "screenwidget.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QVariant>

namespace apex
{
  namespace editor
  {
    using gui::sc_nDefaultFontSize;

    ButtonEditorDelegate::ButtonEditorDelegate(
      ButtonElement* e, QWidget* parent, ScreenWidget* w )
      : QFrame( parent ),
        ScreenElementEditorDelegate( w ),
        element( e ),
        button( new QPushButton( this ) )
    {
      button->setText( element->text() );
      button->installEventFilter( this );
      button->setFocusPolicy( Qt::NoFocus );

      button->setGeometry( contentsRect().adjusted( 2, 2, -2, -2 ) );
    }

    QFrame* ButtonEditorDelegate::getWidget()
    {
      return this;
    }

    ScreenElement* ButtonEditorDelegate::getScreenElement()
    {
      return element;
    }

    int ButtonEditorDelegate::getPropertyCount()
    {
      return ScreenElementEditorDelegate::getPropertyCount() + 3;
    }

    QString ButtonEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyName( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() ) {
      case 0: return tr( "Text" );
      case 1: return tr( "Font Size" );
      case 2: return tr( "Shortcut" );
      default: return QString();
      }
    }

    QVariant ButtonEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyData( nr, role );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0:
        if ( role == Qt::DisplayRole )
          return element->text();
      case 1:
        if ( role == Qt::DisplayRole )
          return element->getFontSize();
      case 2:
        if ( role == Qt::DisplayRole )
          return element->getShortCut()/*.toString()*/;
      default:
        return QVariant();
      }

    }

    PropertyType ButtonEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyType( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: return StringPropertyType;
      case 1: return FontSizePropertyType;
      case 2: return ShortcutPropertyType;
      default: return IntPropertyType;
      }
    }

    bool ButtonEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::setProperty( nr, v );
      QFont font;
      QKeySequence seq;
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // text
        if ( v.type() != QVariant::String ) return false;
        setText( v.toString() );
        return true;
      case 1: // font size
        if ( v.type() != QVariant::Int ) return false;
        element->setFontSize( v.toInt() );
        resetText();
        return true;
      case 2: // shortcut
        if ( v.type() != QVariant::KeySequence ) return false;
        seq = v.value<QKeySequence>();
        element->setShortCut( seq.toString() );
        return true;
      default: return false;
      }
    }

    void ButtonEditorDelegate::mouseReleaseEvent( QMouseEvent* ev )
    {
      handleMouseReleaseEvent( ev, this );
    }

    bool ButtonEditorDelegate::eventFilter( QObject* o, QEvent* e )
    {
      if ( o == button )
      {
        if ( e->type() == QEvent::MouseButtonRelease )
        {
          handleMouseReleaseEvent( static_cast<QMouseEvent*>( e ), this );
          return true;
        } else if ( e->type() == QEvent::MouseButtonDblClick ) {
          mouseDoubleClickEvent( static_cast<QMouseEvent*>( e ) );
          return true;
        } else if ( e->type() == QEvent::MouseButtonPress ||
                    e->type() == QEvent::MouseMove ) {
          return true;
        }
      }
      return false;
    }

    void ButtonEditorDelegate::resizeEvent( QResizeEvent* e )
    {
      QFrame::resizeEvent( e );
      setButtonGeometry();

      // reset font size etc.
      resetText();
    }

    void ButtonEditorDelegate::setButtonGeometry()
    {
      button->setGeometry( contentsRect().adjusted( 2,2,-2,-2 ) );
    }

    void ButtonEditorDelegate::paintEvent( QPaintEvent* e )
    {
      setButtonGeometry();
      QFrame::paintEvent( e );
    }

    void ButtonEditorDelegate::setText( const QString& s )
    {
      element->setText( s );
      resetText();
    }

// reset the button text, adjusting font size etc to match..
// called after resizing and resetting text..
    void ButtonEditorDelegate::resetText()
    {
      button->setFont( getFont() );

      QString text = getText();
      ApexTools::shrinkTillItFits (button, text, QSize (2, 2));
      button->setText( text );
    }

    QFont ButtonEditorDelegate::getFont()
    {
      QFont font = button->font();
      if ( getScreenElement()->getFontSize() > 0 )
        font.setPointSize( getScreenElement()->getFontSize() );
      else
        font.setPointSize( sc_nDefaultFontSize );
      return font;
    }

    const QString ButtonEditorDelegate::getText()
    {
      return element->text();
    }

    ButtonEditorDelegate::~ButtonEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }

    void ButtonEditorDelegate::mouseDoubleClickEvent( QMouseEvent* /*ev*/ )
    {
      bool ok = false;
      QString text = QInputDialog::getText(
        this,
        tr( "Button Text" ),
        tr( "Enter the text for the button with id %1." ).arg( element->getID() ),
        QLineEdit::Normal, element->text(), &ok );
      if (ok)
      {
        setText( text );
        screenWidget->getEditor()->selectWidget( getWidget(), this );
      }
    }

  }
}
