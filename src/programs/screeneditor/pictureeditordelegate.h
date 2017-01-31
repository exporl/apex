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

#ifndef _EXPORL_SRC_PROGRAMS_SCREENEDITOR_PICTUREEDITORDELEGATE_H_
#define _EXPORL_SRC_PROGRAMS_SCREENEDITOR_PICTUREEDITORDELEGATE_H_

#include "labeleditordelegatebase.h"

namespace apex
{
  namespace data
  {
    class PictureElement;
  }
  namespace editor
  {

    using data::PictureElement;

    /**
     * This class is an ScreenElementEditorDelegate representing a
     * PictureElement.
     */
    class PictureEditorDelegate
      : public LabelEditorDelegateBase
    {
      Q_OBJECT

      PictureElement* element;
    public:
      PictureEditorDelegate( PictureElement* e, QWidget* parent, ScreenWidget* w );
      ~PictureEditorDelegate();

      ScreenElement* getScreenElement();

      int getPropertyCount();
      QString getPropertyName( int nr );
      QVariant getPropertyData( int nr, int role );
      PropertyType getPropertyType( int nr );
      bool setProperty( int nr, const QVariant& v );

      const QString getText() const;

    protected:
      void mouseReleaseEvent( QMouseEvent* ev );
    };

  }
}
#endif
