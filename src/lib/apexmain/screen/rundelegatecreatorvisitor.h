/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_RUNDELEGATECREATORVISITOR_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_RUNDELEGATECREATORVISITOR_H_

#include "apexdata/screen/screenelementvisitor.h"

#include "rundelegatedefines.h"

#include <QFont>

namespace apex
{
class ExperimentRunDelegate;
namespace rundelegates
{
using namespace data;

/**
 * The RunDelegateCreatorVisitor class is a class that is used to
 * create a ScreenElementRunDelegate for a given ScreenElement.  I
 * did not name it ScreenElementRunDelegateCreatorVisitor for
 * brevity ;)
 *
 * It is an implementation of the ScreenElementVisitor class in
 * order to avoid having a big ugly switch in the code...
 */
class RunDelegateCreatorVisitor : public ScreenElementVisitor
{
    QWidget *parent;
    ElementToRunningMap &elementToRunningMap;
    QFont defaultFont;
    ScreenElementRunDelegate *lastcreated;

public:
    RunDelegateCreatorVisitor(ExperimentRunDelegate *p_exprd, QWidget *parent,
                              ElementToRunningMap &elementToRunningMap,
                              const QFont &defaultFont);

    ScreenElementRunDelegate *createRunDelegate(const ScreenElement *e);
    ~RunDelegateCreatorVisitor();

private:
    void visitAnswerLabel(const AnswerLabelElement *e);
    void visitArcLayout(const ArcLayoutElement *e);
    void visitButton(const ButtonElement *e);
    void visitMatrix(const MatrixElement *e);
    void visitSpinBox(const SpinBoxElement *e);
    void visitSlider(const SliderElement *e);
    void visitCheckBox(const CheckBoxElement *e);
    void visitEmpty(const EmptyElement *e);
    void visitFlashPlayer(const FlashPlayerElement *e);
    void visitGridLayout(const GridLayoutElement *e);
    void visitLabel(const LabelElement *e);
    void visitParameterList(const ParameterListElement *e);
    void visitParameterLabel(const ParameterLabelElement *e);
    void visitPicture(const PictureElement *e);
    void visitPictureLabel(const PictureLabelElement *e);
    void visitTextEdit(const TextEditElement *e);
    void visitHtml(const HtmlElement *e);
    void visitNumericKeypad(const NumericKeypadElement *e);
    ExperimentRunDelegate *m_rd;
};
}
}

#endif
