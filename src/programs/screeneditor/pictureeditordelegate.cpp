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

#include "apexdata/screen/pictureelement.h"

#include "functions.h"
#include "pictureeditordelegate.h"
#include "screenwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QVariant>

namespace apex
{
  namespace editor
  {
    ScreenElement* PictureEditorDelegate::getScreenElement()
    {
      return element;
    }


    PictureEditorDelegate::PictureEditorDelegate(
      PictureElement* e, QWidget* parent, ScreenWidget* w )
      : LabelEditorDelegateBase( e, parent, w ),
        element( e )
    {
      resetText();
    }

    int PictureEditorDelegate::getPropertyCount()
    {
      return ScreenElementEditorDelegate::getPropertyCount() + 5;
    }

    QString PictureEditorDelegate::getPropertyName( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyName( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // default
        return tr( "default" );
      case 1: // overrideFeedback
        return tr( "override feedback" );
      case 2: // highlight
        return tr( "highlight" );
      case 3: // positive
        return tr( "positive" );
      case 4: // negative
        return tr( "negative" );
      default:
        return QString();
      }
    }

    QVariant PictureEditorDelegate::getPropertyData( int nr, int role )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyData( nr, role );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // default
        if ( role == Qt::DisplayRole )
          return element->getDefault();
      case 1: // overrideFeedback
        if ( role == Qt::DisplayRole )
          return element->getOverrideFeedback();
      case 2: // highlight
        if ( role == Qt::DisplayRole )
          return element->getHighlight();
      case 3: // positive
        if ( role == Qt::DisplayRole )
          return element->getPositive();
      case 4: // negative
        if ( role == Qt::DisplayRole )
          return element->getNegative();
      default:
        return QVariant();
      }
    }

    PropertyType PictureEditorDelegate::getPropertyType( int nr )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::getPropertyType( nr );
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // default
      case 2: // highlight
      case 3: // positive
      case 4: // negative
        return ImagePropertyType;
      case 1: // overrideFeedback
        return BooleanPropertyType;
      default:
        return IntPropertyType;
      }
    }

    bool PictureEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      if ( nr < ScreenElementEditorDelegate::getPropertyCount() )
        return ScreenElementEditorDelegate::setProperty( nr, v );
      bool b;
      QString s;
      switch( nr - ScreenElementEditorDelegate::getPropertyCount() )
      {
      case 0: // default
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        element->setDefault( s );
        resetText();
        return true;
      case 1: // overrideFeedback
        if ( v.type() != QVariant::Bool ) return false;
        b = v.toBool();
        element->setOverrideFeedback( b );
        resetText();
        return true;
      case 2: // highlight
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        element->setHighlight( s );
        resetText();
        return true;
      case 3: // positive
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        element->setPositive( s );
        resetText();
        return true;
      case 4: // negative
        if ( v.type() != QVariant::String ) return false;
        s = v.toString();
        element->setNegative( s );
        resetText();
        return true;
      default: return false;
      }
    }

    void PictureEditorDelegate::mouseReleaseEvent( QMouseEvent* ev )
    {
      handleMouseReleaseEvent( ev, this );
    }

    const QString PictureEditorDelegate::getText() const
    {
      QString def = element->getDefault();
      if ( def.isEmpty() ) def = tr( "Not set" );
      def = shortenString( def, 30 );
      QString overrideFeedback = element->getOverrideFeedback()? tr( "yes" ) : tr( "no" );
      QString highlight = element->getHighlight();
      highlight = shortenString( highlight, 30 );
      if ( highlight.isEmpty() ) highlight = tr( "Not set" );
      QString positive = element->getPositive();
      positive = shortenString( positive, 30 );
      if ( positive.isEmpty() ) positive = tr( "Not set" );
      QString negative = element->getNegative();
      negative = shortenString( negative, 30 );
      if ( negative.isEmpty() ) negative = tr( "Not set" );
      return tr( "Picture\n"
                 "Default:\n%1\n"
                 "Override Feedback:\n %2\n"
                 "Highlight:\n%3\n"
                 "Positive:\n%4\n"
                 "Negative:\n%5\n" )
        .arg( def )
        .arg( overrideFeedback )
        .arg( highlight )
        .arg( positive )
        .arg( negative )
        ;
    }
    PictureEditorDelegate::~PictureEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }
  }
}

