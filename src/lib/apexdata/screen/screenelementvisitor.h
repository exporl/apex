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

#ifndef SCREENELEMENTVISITOR_H
#define SCREENELEMENTVISITOR_H

#include "global.h"

namespace apex
{
namespace data
{
class AnswerLabelElement;
class ArcLayoutElement;
class ButtonElement;
class MatrixElement;
class SpinBoxElement;
class SliderElement;
class CheckBoxElement;
class EmptyElement;
class FlashPlayerElement;
class GridLayoutElement;
class LabelElement;
class ParameterListElement;
class ParameterLabelElement;
class PictureElement;
class PictureLabelElement;
class ScreenElement;
class TextEditElement;
class TwoPartLayoutElement;

/**
 * The ScreenElementVisitor class is a visitor interface for
 * ScreenElement's.  Check out
 * http://en.wikipedia.org/wiki/Visitor_pattern for a general
 * explanation of the Visitor design pattern.
 *
 * Concretely: this class's purpose is to replace the following c-style code
 * \code
 *   switch( element->elementType() )
 *   {
 *      case ScreenElement::Button:
 *         // do something
 *         break;
 *      case ScreenElement::Label:
 *         // do something
 *         break;
 *   }
 * \endcode
 * with a nicer, more object-oriented approach:
 * \code
 *   class ConcreteElementVisitor
 *   {
 *     void visitButton( ButtonElement* b )
 *     {
 *        // do something
 *     }
 *     void visitLabel( LabelElement* l )
 *     {
 *        // do something
 *     }
 *   };
 *   ConcreteElementVisitor v;
 *   element->visit( &v );
 * \endcode
 */
class APEXDATA_EXPORT ScreenElementVisitor
{
    public:
        virtual ~ScreenElementVisitor();

        virtual void visitAnswerLabel(const AnswerLabelElement* e);
        virtual void visitArcLayout(const ArcLayoutElement* e);
        virtual void visitButton(const ButtonElement* e);
        virtual void visitMatrix(const MatrixElement* e);
        virtual void visitSpinBox(const SpinBoxElement* e);
        virtual void visitSlider(const SliderElement* e);
        virtual void visitCheckBox(const CheckBoxElement* e);
        virtual void visitEmpty(const EmptyElement* e);
        virtual void visitFlashPlayer(const FlashPlayerElement* e);
        virtual void visitGridLayout(const GridLayoutElement* e);
        virtual void visitLabel(const LabelElement* e);
        virtual void visitParameterList(const ParameterListElement* e);
        virtual void visitParameterLabel(const ParameterLabelElement* e);
        virtual void visitPicture(const PictureElement* e);
        virtual void visitPictureLabel(const PictureLabelElement* e);
        virtual void visitTextEdit(const TextEditElement* e);
        virtual void visitTwoPartLayout(const TwoPartLayoutElement* e);

        virtual void visitAnswerLabel(AnswerLabelElement* e);
        virtual void visitArcLayout(ArcLayoutElement* e);
        virtual void visitButton(ButtonElement* e);
        virtual void visitMatrix(MatrixElement* e);
        virtual void visitSpinBox(SpinBoxElement* e);
        virtual void visitSlider(SliderElement* e);
        virtual void visitCheckBox(CheckBoxElement* e);
        virtual void visitEmpty(EmptyElement* e);
        virtual void visitFlashPlayer(FlashPlayerElement* e);
        virtual void visitGridLayout(GridLayoutElement* e);
        virtual void visitLabel(LabelElement* e);
        virtual void visitParameterList(ParameterListElement* e);
        virtual void visitParameterLabel(ParameterLabelElement* e);
        virtual void visitPicture(PictureElement* e);
        virtual void visitPictureLabel(PictureLabelElement* e);
        virtual void visitTextEdit(TextEditElement* e);
        virtual void visitTwoPartLayout(TwoPartLayoutElement* e);
};
}
}


#endif
