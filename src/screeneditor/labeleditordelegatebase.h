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
 
#ifndef LABELEDITORDELEGATEBASE_H
#define LABELEDITORDELEGATEBASE_H

#include "screenelementeditordelegate.h"

#include <QFrame>

class QLabel;

namespace apex
{
  namespace data
  {
    class ScreenElement;
  }

  namespace editor
  {
    using data::ScreenElement;

    /**
     * The LabelEditorDelegateBase class is a common base class for
     * ScreenElementEditorDelegate classes that show a label on
     * screen.  Classes inheriting this class don't necessarily
     * represent a ScreenElement that is some kind of label, but
     * e.g. PictureElement uses this class in order to show a label
     * showing the information about the picture.
     */
    class LabelEditorDelegateBase
      : public QFrame, public ScreenElementEditorDelegate
    {
      Q_OBJECT

    public:
      LabelEditorDelegateBase( ScreenElement* e, QWidget* parent, ScreenWidget* w );
      QFrame* getWidget();
      virtual const QString getText() const = 0;

      bool setProperty( int nr, const QVariant& v );
    protected:
      bool eventFilter( QObject* o, QEvent* e );

      void mouseReleaseEvent( QMouseEvent* ev );
      void resizeEvent( QResizeEvent* e );
      void paintEvent( QPaintEvent* e );
      void resetText();
    private:
      QLabel* label;
      void setLabelGeometry();
      QFont getFont();
    };

  }
}
#endif
