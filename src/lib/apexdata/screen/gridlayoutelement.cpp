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

#include "apextools/exceptions.h"

#include "emptyelement.h"
#include "gridlayoutelement.h"
#include "screen.h"
#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

GridLayoutElement::GridLayoutElement(const QString &i, ScreenElement *parent,
                                     int w, int h, tStretchList rs,
                                     tStretchList cs)
    : ScreenLayoutElement(i, parent),
      width(w),
      height(h),
      rowStretch(rs),
      colStretch(cs)
{
}

GridLayoutElement::GridLayoutElement(const QString &id, ScreenElement *parent)
    : ScreenLayoutElement(id, parent), width(3), height(2)
{
}

void GridLayoutElement::checkChild(const ScreenElement *childel) const
{
    int x = childel->getX();
    int y = childel->getY();
    if (x >= getWidth() || x < 0)
        throw ApexStringException("column for element '" + childel->getID() +
                                  "' out of range.");
    else if (y >= getHeight() || y < 0)
        throw ApexStringException("row for element '" + childel->getID() +
                                  "' out of range.");
}

void GridLayoutElement::visit(ScreenElementVisitor *v)
{
    v->visitGridLayout(this);
}

void GridLayoutElement::visit(ScreenElementVisitor *v) const
{
    v->visitGridLayout(this);
}

int GridLayoutElement::getNumberOfChildPlaces() const
{
    return getWidth() * getHeight();
}

void GridLayoutElement::fillChildrenWithEmpties(Screen *s)
{
    const ScreenElementMap &takenIDs = s->idToElementMap();
    std::vector<std::vector<bool>> filled(getWidth());
    for (int i = 0; i < getWidth(); ++i)
        filled[i].resize(getHeight(), false);
    for (int i = 0; i < getNumberOfChildren(); ++i) {
        ScreenElement *child = getChild(i);
        filled[child->getX()][child->getY()] = true;
    }
    for (int i = 0; i < getWidth(); ++i) {
        for (int j = 0; j < getHeight(); ++j) {
            if (!filled[i][j]) {
                EmptyElement *e =
                    new EmptyElement(EmptyElement::findFreeID(takenIDs), this);
                e->setX(i);
                e->setY(j);
                s->addElement(e);
            }
        }
    }
}

void GridLayoutElement::setWidth(int w)
{
    width = w;
}

void GridLayoutElement::setHeight(int h)
{
    height = h;
}

ScreenElement::ElementTypeT GridLayoutElement::elementType() const
{
    return GridLayout;
}

bool GridLayoutElement::operator==(const GridLayoutElement &other) const
{
    return ScreenLayoutElement::operator==(other) && width == other.width &&
           height == other.height && rowStretch == other.rowStretch &&
           colStretch == other.colStretch;
}
}
}
