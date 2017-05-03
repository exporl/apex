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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_EDITORDELEGATECREATORVISITOR_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_EDITORDELEGATECREATORVISITOR_H_

#include "apexdata/screen/screenelementvisitor.h"

#include "screentypedefs.h"

class QWidget;

namespace apex
{
  namespace editor
  {
    using data::ScreenElementVisitor;
    using data::AnswerLabelElement;
    using data::ArcLayoutElement;
    using data::ButtonElement;
    using data::EmptyElement;
    using data::FlashPlayerElement;
    using data::GridLayoutElement;
    using data::LabelElement;
    using data::ScreenElement;
    using data::ParameterListElement;
    using data::PictureElement;
    using data::PictureLabelElement;
    using data::TextEditElement;

    class ScreenElementEditorDelegate;
    class ScreenWidget;

    /**
     * The EditorDelegateCreatorVisitor class is a class that is used
     * to create a ScreenElementEditorDelegate for a given
     * ScreenElement.  I did not name it
     * ScreenElementEditorDelegateCreatorVisitor for brevity ;)
     *
     * It is an implementation of the ScreenElementVisitor class in
     * order to avoid having a big ugly switch in the code...
     */
    class EditorDelegateCreatorVisitor
      : public ScreenElementVisitor
    {
      QWidget* parent;
      ScreenWidget* widget;
      ScreenElementEditorDelegate* lastcreated;
      elementToDelegateMapT& elementToDelegateMap;
    public:
      EditorDelegateCreatorVisitor(
        QWidget* parent, ScreenWidget* widget, elementToDelegateMapT& elementToDelegateMap );

      ScreenElementEditorDelegate* createEditorDelegate(
        ScreenElement* e );

      ~EditorDelegateCreatorVisitor();
      void visitAnswerLabel( AnswerLabelElement* e );
      void visitArcLayout( ArcLayoutElement* e );
      void visitButton( ButtonElement* e );
      void visitEmpty( EmptyElement* e );
      void visitFlashPlayer( FlashPlayerElement* e );
      void visitGridLayout( GridLayoutElement* e );
      void visitLabel( LabelElement* e );
      void visitParameterList( ParameterListElement* e );
      void visitPicture( PictureElement* e );
      void visitPictureLabel( PictureLabelElement* e );
      void visitTextEdit( TextEditElement* e );
    };

  }
}
#endif
