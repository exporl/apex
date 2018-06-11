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

#include "apexdata/screen/answerlabelelement.h"
#include "apexdata/screen/arclayoutelement.h"
#include "apexdata/screen/buttonelement.h"
#include "apexdata/screen/checkboxelement.h"
#include "apexdata/screen/flashplayerelement.h"
#include "apexdata/screen/gridlayoutelement.h"
#include "apexdata/screen/htmlelement.h"
#include "apexdata/screen/labelelement.h"
#include "apexdata/screen/matrixelement.h"
#include "apexdata/screen/parameterlabelelement.h"
#include "apexdata/screen/parameterlistelement.h"
#include "apexdata/screen/pictureelement.h"
#include "apexdata/screen/picturelabelelement.h"
#include "apexdata/screen/sliderelement.h"
#include "apexdata/screen/spinboxelement.h"
#include "apexdata/screen/texteditelement.h"

#include "screen/answerlabelrundelegate.h"
#include "screen/arclayoutrundelegate.h"
#include "screen/buttonrundelegate.h"
#include "screen/checkboxrundelegate.h"
#include "screen/gridlayoutrundelegate.h"

#include "screen/htmlrundelegate.h"

#include "screen/labelrundelegate.h"
#include "screen/matrixrundelegate.h"
#include "screen/parameterlabelrundelegate.h"
#include "screen/parameterlistrundelegate.h"
#include "screen/picturelabelrundelegate.h"
#include "screen/picturerundelegate.h"
#include "screen/sliderrundelegate.h"
#include "screen/spinboxrundelegate.h"
#include "screen/texteditrundelegate.h"

#include "rundelegatecreatorvisitor.h"

#ifdef FLASH
#include "screen/flashplayerrundelegate.h"
#endif

namespace apex
{
namespace rundelegates
{
RunDelegateCreatorVisitor::RunDelegateCreatorVisitor(
    ExperimentRunDelegate *p_exprd, QWidget *p, ElementToRunningMap &etrm,
    const QFont &df)
    : parent(p),
      elementToRunningMap(etrm),
      defaultFont(df),
      lastcreated(0),
      m_rd(p_exprd)
{
}

RunDelegateCreatorVisitor::~RunDelegateCreatorVisitor()
{
}

void RunDelegateCreatorVisitor::visitAnswerLabel(const AnswerLabelElement *e)
{
    AnswerLabelRunDelegate *d =
        new AnswerLabelRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitArcLayout(const ArcLayoutElement *e)
{
    ArcLayoutRunDelegate *d = new ArcLayoutRunDelegate(
        m_rd, e, parent, elementToRunningMap, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitButton(const ButtonElement *e)
{
    ButtonRunDelegate *d = new ButtonRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitMatrix(const MatrixElement *e)
{
    MatrixRunDelegate *d = new MatrixRunDelegate(m_rd, parent, e);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitCheckBox(const CheckBoxElement *e)
{
    CheckBoxRunDelegate *d =
        new CheckBoxRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitSpinBox(const SpinBoxElement *e)
{
    SpinBoxRunDelegate *d =
        new SpinBoxRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    // spinBoxList.push_back(d);
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitSlider(const SliderElement *e)
{
    SliderRunDelegate *d = new SliderRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitEmpty(const EmptyElement * /*e*/)
{
    // this is intentional..  we don't show empty elements, in fact
    // they really shouldn't be here at all
    qCDebug(APEX_RS, "Empty Screen element in running experiment");
    lastcreated = 0;
}

#ifdef FLASH
void RunDelegateCreatorVisitor::visitFlashPlayer(const FlashPlayerElement *e)
{
    lastcreated = new FlashPlayerRunDelegate(m_rd, parent, e);
    elementToRunningMap[e] = lastcreated;
}
#else
void RunDelegateCreatorVisitor::visitFlashPlayer(
    const FlashPlayerElement * /*e*/)
{
    lastcreated = 0;
}
#endif

void RunDelegateCreatorVisitor::visitGridLayout(const GridLayoutElement *e)
{
    GridLayoutRunDelegate *glrd = new GridLayoutRunDelegate(
        m_rd, e, parent, elementToRunningMap, defaultFont);
    elementToRunningMap[e] = glrd;
    lastcreated = glrd;
}

void RunDelegateCreatorVisitor::visitLabel(const LabelElement *e)
{
    LabelRunDelegate *d = new LabelRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitParameterList(
    const ParameterListElement *e)
{
    ParameterListRunDelegate *d = new ParameterListRunDelegate(m_rd, e, parent);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitParameterLabel(
    const ParameterLabelElement *e)
{
    ParameterLabelRunDelegate *d =
        new ParameterLabelRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitPicture(const PictureElement *e)
{
    PictureRunDelegate *d = new PictureRunDelegate(m_rd, parent, e);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitPictureLabel(const PictureLabelElement *e)
{
    PictureLabelRunDelegate *d = new PictureLabelRunDelegate(m_rd, parent, e);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitTextEdit(const TextEditElement *e)
{
    TextEditRunDelegate *d =
        new TextEditRunDelegate(m_rd, parent, e, defaultFont);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

void RunDelegateCreatorVisitor::visitHtml(const HtmlElement *e)
{
    HtmlRunDelegate *d = new HtmlRunDelegate(m_rd, parent, e);
    elementToRunningMap[e] = d;
    lastcreated = d;
}

ScreenElementRunDelegate *
RunDelegateCreatorVisitor::createRunDelegate(const ScreenElement *e)
{
    e->visit(this);
    ScreenElementRunDelegate *ret = lastcreated;
    if (ret->getWidget())
        ret->getWidget()->hide();
    lastcreated = 0;
    return ret;
}
}
}
