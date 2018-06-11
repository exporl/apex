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

#include "apextools/exceptions.h"

#include "arclayoutelement.h"
#include "emptyelement.h"
#include "screen.h"
#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

ArcLayoutElement::ArcLayoutElement(const QString &id, ScreenElement *parent,
                                   int w, /*ArcLayout::ArcType*/ int t)
    : ScreenLayoutElement(id, parent), width(w), arctype(t)
{
}

void ArcLayoutElement::checkChild(const ScreenElement *childel) const
{
    int x = childel->getX();
    if (x < -1 || x >= getWidth())
        throw ApexStringException("x value for element '" + childel->getID() +
                                  "' out of range.");
}

void ArcLayoutElement::visit(ScreenElementVisitor *v)
{
    v->visitArcLayout(this);
}

void ArcLayoutElement::visit(ScreenElementVisitor *v) const
{
    v->visitArcLayout(this);
}

int ArcLayoutElement::getNumberOfChildPlaces() const
{
    return getWidth() + 1;
}

ArcLayoutElement::ArcLayoutElement(const QString &id, ScreenElement *parent)
    : ScreenLayoutElement(id, parent),
      width(5),
      arctype(/*ArcLayout::ARC_TOP*/ 0x0001)
{
}

void ArcLayoutElement::fillChildrenWithEmpties(Screen *screen)
{
    const ScreenElementMap &takenIDs = screen->idToElementMap();
    bool centertaken = false;
    std::vector<bool> filled(getWidth(), false);

    for (int i = 0; i < getNumberOfChildren(); ++i) {
        ScreenElement *child = getChild(i);
        int x = child->getX();
        if (x < 0)
            centertaken = true;
        else
            filled[x] = true;
    }
    if (!centertaken) {
        EmptyElement *e =
            new EmptyElement(EmptyElement::findFreeID(takenIDs), this);
        e->setX(-1);
        screen->addElement(e);
    }
    for (int i = 0; i < getWidth(); ++i) {
        if (!filled[i]) {
            EmptyElement *e =
                new EmptyElement(EmptyElement::findFreeID(takenIDs), this);
            e->setX(i);
            screen->addElement(e);
        }
    }
}

void ArcLayoutElement::setWidth(int w)
{
    width = w;
}

void ArcLayoutElement::setType(int t)
{
    arctype = t;
}

ScreenElement::ElementTypeT ArcLayoutElement::elementType() const
{
    return ArcLayout;
}

bool ArcLayoutElement::operator==(const ArcLayoutElement &other) const
{
    return ScreenLayoutElement::operator==(other) && width == other.width &&
           arctype == other.arctype;
}
}
}
