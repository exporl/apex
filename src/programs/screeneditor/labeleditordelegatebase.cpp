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

#include "apexdata/screen/screenelement.h"

#include "apexmain/gui/guidefines.h"

#include "apextools/apextools.h"

#include "labeleditordelegatebase.h"
#include "screenwidget.h"

#include <QLabel>
#include <QMouseEvent>

namespace apex
{
namespace editor
{
using gui::sc_nDefaultFontSize;
using gui::sc_DefaultBGColor;

QFrame *LabelEditorDelegateBase::getWidget()
{
    return this;
}

LabelEditorDelegateBase::LabelEditorDelegateBase(ScreenElement *e,
                                                 QWidget *parent,
                                                 ScreenWidget *w)
    : QFrame(parent), ScreenElementEditorDelegate(w), label(new QLabel(this))
{
    label->setAlignment(Qt::AlignCenter);
    label->setAutoFillBackground(true);
    label->setFrameShape(QLabel::Box);

    QFont font = w->getDefaultFont();
    if (e->getFontSize() > 0)
        font.setPointSize(e->getFontSize());
    label->setFont(font);
}

void LabelEditorDelegateBase::mouseReleaseEvent(QMouseEvent *ev)
{
    handleMouseReleaseEvent(ev, this);
}

bool LabelEditorDelegateBase::eventFilter(QObject *o, QEvent *e)
{
    if (o == label) {
        if (e->type() == QEvent::MouseButtonRelease) {
            handleMouseReleaseEvent(static_cast<QMouseEvent *>(e), this);
            return true;
        }
    }
    return false;
}

void LabelEditorDelegateBase::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    setLabelGeometry();

    resetText();
}

void LabelEditorDelegateBase::paintEvent(QPaintEvent *e)
{
    setLabelGeometry();
    QFrame::paintEvent(e);
}

void LabelEditorDelegateBase::setLabelGeometry()
{
    label->setGeometry(contentsRect().adjusted(2, 2, -2, -2));
}

void LabelEditorDelegateBase::resetText()
{
    label->setFont(getFont());

    QString text = getText();
    ApexTools::shrinkTillItFits(label, text, QSize(2, 2));
    label->setText(text);
}

QFont LabelEditorDelegateBase::getFont()
{
    QFont font = label->font();
    if (getScreenElement()->getFontSize() > 0)
        font.setPointSize(getScreenElement()->getFontSize());
    else
        font.setPointSize(sc_nDefaultFontSize);
    return font;
}

bool LabelEditorDelegateBase::setProperty(int nr, const QVariant &v)
{
    return ScreenElementEditorDelegate::setProperty(nr, v);
}
}
}
