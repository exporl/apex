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

#include "apexdata/screen/arclayoutelement.h"

#include "apextools/gui/arclayout.h"

#include "arclayoutrundelegate.h"
#include "rundelegatecreatorvisitor.h"

#include <QDebug>
#include <QWidget>

class QFont;

namespace apex
{
namespace rundelegates
{

ArcLayoutRunDelegate::ArcLayoutRunDelegate(
    ExperimentRunDelegate *p_parent, const ArcLayoutElement *e, QWidget *parent,
    ElementToRunningMap &elementToRunningMap, const QFont &font)
    : ScreenElementRunDelegate(p_parent, e),
      ArcLayout((ArcLayout::ArcType)e->getType(), 0),
      element(e)
{

    // insert dummy elements to create the right size
    int n = e->getWidth();
    for (int i = 0; i < n + 1; ++i) { // 1 extra element for center
        QWidget *t = new QWidget(parent);
        addWidget(t);
    }

    setArcLayoutFlags(ArcLayout::ARC_RECTANGULAR);

    for (int i = 0; i < element->getNumberOfChildren(); ++i) {
        const ScreenElement *child = element->getChild(i);
        RunDelegateCreatorVisitor delcreator(p_parent, parent,
                                             elementToRunningMap, font);
        ScreenElementRunDelegate *childdel =
            delcreator.createRunDelegate(child);
        if (childdel)
            childdel->addToArcLayout(this);
    }
}

const ScreenElement *ArcLayoutRunDelegate::getScreenElement() const
{
    return element;
}

QWidget *ArcLayoutRunDelegate::getWidget()
{
    return 0;
}

QLayout *ArcLayoutRunDelegate::getLayout()
{
    return this;
}
}
}
