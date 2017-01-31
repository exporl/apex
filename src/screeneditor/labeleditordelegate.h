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
 
#ifndef LABELEDITORDELEGATE_H
#define LABELEDITORDELEGATE_H

#include "labeleditordelegatebase.h"

namespace apex
{
  namespace data
  {
    class LabelElement;
    class ScreenElement;
  }

  namespace editor
  {
    using data::LabelElement;
    using data::ScreenElement;

    /**
     * This class is an ScreenElementEditorDelegate representing a
     * LabelElement.
     */
    class LabelEditorDelegate
      : public LabelEditorDelegateBase
    {
      Q_OBJECT

      LabelElement* element;
    public:
      LabelEditorDelegate( LabelElement* e, QWidget* parent, ScreenWidget* w );
      ~LabelEditorDelegate();
      ScreenElement* getScreenElement();

      const QString getText() const;
      void setText( const QString& s );

      int getPropertyCount();
      QString getPropertyName( int nr );
      QVariant getPropertyData( int nr, int role );
      PropertyType getPropertyType( int nr );
      bool setProperty( int nr, const QVariant& v );

      void mouseDoubleClickEvent ( QMouseEvent * event );
    };

  }
}
#endif
