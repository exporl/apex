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

#include "apexdata/screen/arclayoutelement.h"

#include "apextools/gui/arclayout.h"

#include "arclayouteditordelegate.h"
#include "screenwidget.h"

#include <QMessageBox>
#include <QVariant>

namespace apex
{
namespace editor
{
ArcLayoutEditorDelegate::ArcLayoutEditorDelegate(
    ArcLayoutElement *e, QWidget *parent, ScreenWidget *widget,
    elementToDelegateMapT &elementToDelegateMap)
    : LayoutEditorDelegate(parent, widget), element(e), arclayout(0)
{
    e->fillChildrenWithEmpties(widget->getScreen());
    updateLayout(elementToDelegateMap);
}

ScreenElement *ArcLayoutEditorDelegate::getScreenElement()
{
    return element;
}

int ArcLayoutEditorDelegate::getPropertyCount()
{
    return LayoutEditorDelegate::getPropertyCount() + 2;
}

QString ArcLayoutEditorDelegate::getPropertyName(int nr)
{
    if (nr < LayoutEditorDelegate::getPropertyCount())
        return LayoutEditorDelegate::getPropertyName(nr);
    switch (nr - LayoutEditorDelegate::getPropertyCount()) {
    case 0:
        return tr("width");
    case 1:
        return tr("type");
    default:
        return QString();
    }
}

QVariant ArcLayoutEditorDelegate::getPropertyData(int nr, int role)
{
    if (nr < LayoutEditorDelegate::getPropertyCount())
        return LayoutEditorDelegate::getPropertyData(nr, role);
    switch (nr - LayoutEditorDelegate::getPropertyCount()) {
    case 0: // width
        if (role == Qt::DisplayRole)
            return element->getWidth();
    case 1: // type
        if (role == Qt::DisplayRole) {
            switch (element->getType()) {
            case ArcLayout::ARC_TOP:
                return QString::fromUtf8("Upper");
            case ArcLayout::ARC_BOTTOM:
                return QString::fromUtf8("Lower");
            case ArcLayout::ARC_LEFT:
                return QString::fromUtf8("Left");
            case ArcLayout::ARC_RIGHT:
                return QString::fromUtf8("Right");
            case ArcLayout::ARC_FULL:
                return QString::fromUtf8("Full");
            }
        }
    default:
        return QVariant();
    }
}

PropertyType ArcLayoutEditorDelegate::getPropertyType(int nr)
{
    if (nr < LayoutEditorDelegate::getPropertyCount())
        return LayoutEditorDelegate::getPropertyType(nr);
    switch (nr - LayoutEditorDelegate::getPropertyCount()) {
    case 0: // width
        return IntPropertyType;
    case 1:
        return ArcLayoutTypePropertyType;
    default:
        return IntPropertyType;
    }
}

bool ArcLayoutEditorDelegate::setProperty(int nr, const QVariant &v)
{
    if (nr < LayoutEditorDelegate::getPropertyCount())
        return LayoutEditorDelegate::setProperty(nr, v);
    // no element declarations in switch
    bool harmlesschange;
    int nval;
    int oldval;
    int msgret;
    ArcLayout::ArcType type;
    QString s;
    switch (nr - LayoutEditorDelegate::getPropertyCount()) {
    case 0:
        if (v.type() != QVariant::Int)
            return false;
        nval = v.toInt();
        if (nval <= 0)
            return false;
        oldval = element->getWidth();
        if (nval < oldval) {
            element->setWidth(nval);
            harmlesschange = checkHarmlessChange(element);
            element->setWidth(oldval);
        } else
            harmlesschange = true;

        if (!harmlesschange) {
            msgret = QMessageBox::warning(
                this, tr("Arc Layout width change"),
                tr("Changing the arc layout width will delete child elements.\n"
                   "Do you want to continue ?"),
                tr("Continue"), tr("Cancel"), QString(), 0, 1);
            harmlesschange = (msgret == 0);
        }

        if (harmlesschange) {
            element->setWidth(nval);
            fixupChildren(element);
            updateLayout(screenWidget->getElementToDelegateMap());
            return true;
        } else
            return false;
    case 1:
        if (v.type() != QVariant::String)
            return false;
        s = v.toString();
        if (s == "Upper")
            type = ArcLayout::ARC_TOP;
        else if (s == "Lower")
            type = ArcLayout::ARC_BOTTOM;
        else if (s == "Left")
            type = ArcLayout::ARC_LEFT;
        else if (s == "Right")
            type = ArcLayout::ARC_RIGHT;
        else if (s == "Full")
            type = ArcLayout::ARC_FULL;
        else
            return false;
        element->setType(type);
        arclayout->setArcType(type);
        updateLayout(screenWidget->getElementToDelegateMap());
    default:
        return false;
    }
}

void ArcLayoutEditorDelegate::updateLayout(
    elementToDelegateMapT &elementToDelegateMap)
{
    ArcLayout *oldlayout = arclayout;
    arclayout = new ArcLayout((ArcLayout::ArcType)element->getType());
    setLayout(arclayout);
    delete oldlayout;
    updateLayoutChildren(elementToDelegateMap);
}

ArcLayout *ArcLayoutEditorDelegate::getLayout()
{
    return arclayout;
}

void ArcLayoutEditorDelegate::addItemToLayout(QWidget *w, ScreenElement *e)
{
    int x = e->getX();
    arclayout->insertWidget(x, w);
}

ArcLayoutEditorDelegate::~ArcLayoutEditorDelegate()
{
    screenWidget->delegateDeleted(this);
}
}
}
