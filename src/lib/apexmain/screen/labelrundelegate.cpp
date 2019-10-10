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

#include "apexdata/screen/labelelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "labelrundelegate.h"

namespace apex
{
namespace rundelegates
{

LabelRunDelegate::LabelRunDelegate(ExperimentRunDelegate *p_exprd,
                                   QWidget *parent, const LabelElement *e,
                                   const QFont &defaultFont)
    : LabelRunDelegateBase(p_exprd, parent, e, defaultFont), element(e)
{
    setObjectName(element->getID());

    setText(e->getText());

    // set minimum size hint
    // get line of text with maximum width

    // font with minimum size:
    QFont minfont(font());
    minfont.setPointSize(10);

    QStringList l(e->getText().split("\n"));
    int maxlen = 0;
    int pos = -1;
    for (int i = 0; i < l.size(); ++i) {
        if (l.at(i).length() > maxlen) {
            maxlen = l.at(i).length();
            pos = i;
        }
    }
    if (pos == -1)
        return;
    QFontMetrics fm(minfont);
    setMinimumWidth(fm.boundingRect(l.at(pos)).width());
    setMinimumHeight((fm.height() + fm.lineSpacing()) * l.size());
}

const ScreenElement *LabelRunDelegate::getScreenElement() const
{
    return element;
}
}
}
