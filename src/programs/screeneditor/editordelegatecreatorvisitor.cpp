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

#include "apexdata/screen/screenelement.h"

#include "answerlabeleditordelegate.h"
#include "arclayouteditordelegate.h"
#include "buttoneditordelegate.h"
#include "editordelegatecreatorvisitor.h"
#include "emptyelementeditordelegate.h"
#include "flashplayereditordelegate.h"
#include "gridlayouteditordelegate.h"
#include "labeleditordelegate.h"
#include "parameterlisteditordelegate.h"
#include "pictureeditordelegate.h"
#include "picturelabeleditordelegate.h"
#include "textediteditordelegate.h"

namespace apex
{
  namespace editor
  {
    EditorDelegateCreatorVisitor::EditorDelegateCreatorVisitor(
      QWidget* p, ScreenWidget* w, elementToDelegateMapT& etdm )
      : parent( p ), widget( w ), lastcreated( 0 ), elementToDelegateMap( etdm )
    {
    }

    ScreenElementEditorDelegate* EditorDelegateCreatorVisitor::createEditorDelegate(
      ScreenElement* e )
    {
      e->visit( this );
      ScreenElementEditorDelegate* ret = lastcreated;
      lastcreated = 0;
      elementToDelegateMap[e] = ret;
      return ret;
    }

    EditorDelegateCreatorVisitor::~EditorDelegateCreatorVisitor()
    {
    }

    void EditorDelegateCreatorVisitor::visitAnswerLabel( AnswerLabelElement* e )
    {
      lastcreated = new AnswerLabelEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitArcLayout( ArcLayoutElement* e)
    {
      lastcreated = new ArcLayoutEditorDelegate( e, parent, widget, elementToDelegateMap );
    }

    void EditorDelegateCreatorVisitor::visitButton( ButtonElement* e )
    {
      lastcreated = new ButtonEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitEmpty( EmptyElement* e )
    {
      lastcreated = new EmptyElementEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitFlashPlayer( FlashPlayerElement* e )
    {
      lastcreated = new FlashPlayerEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitGridLayout( GridLayoutElement* e )
    {
      lastcreated = new GridLayoutEditorDelegate( e, parent, widget, elementToDelegateMap );
    }

    void EditorDelegateCreatorVisitor::visitLabel( LabelElement* e )
    {
      lastcreated = new LabelEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitParameterList( ParameterListElement* e )
    {
      lastcreated = new ParameterListEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitPicture( PictureElement* e )
    {
      lastcreated = new PictureEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitPictureLabel( PictureLabelElement* e )
    {
      lastcreated = new PictureLabelEditorDelegate( e, parent, widget );
    }

    void EditorDelegateCreatorVisitor::visitTextEdit( TextEditElement* e )
    {
      lastcreated = new TextEditEditorDelegate( e, parent, widget );
    }
  }
}

