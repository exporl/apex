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

#include "screenelementvisitor.h"

void apex::data::ScreenElementVisitor::visitAnswerLabel( AnswerLabelElement* e )
{
    visitAnswerLabel( const_cast<const AnswerLabelElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitArcLayout( ArcLayoutElement* e )
{
    visitArcLayout( const_cast<const ArcLayoutElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitButton( ButtonElement* e )
{
    visitButton( const_cast<const ButtonElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitMatrix( MatrixElement* e )
{
    visitMatrix( const_cast<const MatrixElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitSpinBox( SpinBoxElement* e )
{
    visitSpinBox( const_cast<const SpinBoxElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitSlider( SliderElement* e )
{
    visitSlider( const_cast<const SliderElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitCheckBox( CheckBoxElement* e )
{
    visitCheckBox( const_cast<const CheckBoxElement*>( e ) );
}
void apex::data::ScreenElementVisitor::visitEmpty( EmptyElement* e )
{
    visitEmpty( const_cast<const EmptyElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitFlashPlayer( FlashPlayerElement* e )
{
    visitFlashPlayer( const_cast<const FlashPlayerElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitGridLayout( GridLayoutElement* e )
{
    visitGridLayout( const_cast<const GridLayoutElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitLabel( LabelElement* e )
{
    visitLabel( const_cast<const LabelElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitParameterList( ParameterListElement* e )
{
    visitParameterList( const_cast<const ParameterListElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitParameterLabel( ParameterLabelElement* e )
{
    visitParameterLabel( const_cast<const ParameterLabelElement*>( e ) );
}


void apex::data::ScreenElementVisitor::visitPicture( PictureElement* e )
{
    visitPicture( const_cast<const PictureElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitPictureLabel( PictureLabelElement* e )
{
    visitPictureLabel( const_cast<const PictureLabelElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitTextEdit( TextEditElement* e )
{
    visitTextEdit( const_cast<const TextEditElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitTwoPartLayout( TwoPartLayoutElement* e )
{
    visitTwoPartLayout( const_cast<const TwoPartLayoutElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitHtml( HtmlElement* e )
{
    visitHtml( const_cast<const HtmlElement*>( e ) );
}

void apex::data::ScreenElementVisitor::visitAnswerLabel( const AnswerLabelElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitArcLayout( const ArcLayoutElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitButton( const ButtonElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitHtml( const HtmlElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitMatrix( const MatrixElement* /*e*/ )
{

}

void apex::data::ScreenElementVisitor::visitSpinBox( const SpinBoxElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitSlider( const SliderElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitCheckBox( const CheckBoxElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitEmpty( const EmptyElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitFlashPlayer( const FlashPlayerElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitGridLayout( const GridLayoutElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitLabel( const LabelElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitParameterList( const ParameterListElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitParameterLabel( const ParameterLabelElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitPicture( const PictureElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitPictureLabel( const PictureLabelElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitTextEdit( const TextEditElement* /*e*/ )
{
}

void apex::data::ScreenElementVisitor::visitTwoPartLayout( const TwoPartLayoutElement* /*e*/ )
{
}

apex::data::ScreenElementVisitor::~ScreenElementVisitor()
{
}
