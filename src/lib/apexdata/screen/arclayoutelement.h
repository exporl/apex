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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_ARCLAYOUTELEMENT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_ARCLAYOUTELEMENT_H_

#include "screenlayoutelement.h"
//#include "gui/arclayout.h"

#include "apextools/global.h"

namespace apex
{
namespace data
{
/**
 * The ArcLayoutElement is an implementation of a \ref
 * ScreenElement representing an "arclayout".
 */
class APEXDATA_EXPORT ArcLayoutElement : public ScreenLayoutElement
{
    int width;
    /*ArcLayout::ArcType*/ int arctype;

public:
    ArcLayoutElement(const QString &id, ScreenElement *parent, int width,
                     /*::ArcLayout::ArcType*/ int type);
    ArcLayoutElement(const QString &id, ScreenElement *parent);

    void fillChildrenWithEmpties(Screen *s);
    int getNumberOfChildPlaces() const;

    ElementTypeT elementType() const;

    void visit(ScreenElementVisitor *v);
    void visit(ScreenElementVisitor *v) const;

    int getWidth() const
    {
        return width;
    }
    void setWidth(int w);
    /*ArcLayout::ArcType*/ int getType() const
    {
        return arctype;
    }
    void setType(/*ArcLayout::ArcType*/ int t);

    void checkChild(const ScreenElement *childel) const;

    bool operator==(const ArcLayoutElement &other) const;
};
}
}

#endif
