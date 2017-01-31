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

#ifndef GRIDLAYOUTELEMENT_H
#define GRIDLAYOUTELEMENT_H

#include "screenlayoutelement.h"
#include "apextypedefs.h"

#include "global.h"

namespace apex
{
namespace data
{

using gui::tScreenElementMap;

/**
 * The GridLayoutElement is an implementation of a \ref
 * ScreenElement representing an "grid layout".
 */
class APEXDATA_EXPORT GridLayoutElement
            : public ScreenLayoutElement
{
        int width;
        int height;
        tStretchList rowStretch;
        tStretchList colStretch;

    public:
        GridLayoutElement(const QString& id, ScreenElement* parent, int width, int height, tStretchList rowstretch, tStretchList colstretch);
        GridLayoutElement(const QString& id, ScreenElement* parent);

        ElementTypeT elementType() const;

        void visit(ScreenElementVisitor* v);
        void visit(ScreenElementVisitor* v) const;

        void fillChildrenWithEmpties(Screen* s);

        int getNumberOfChildPlaces() const;

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        void setWidth(int w);
        void setHeight(int h);

        tStretchList getColumnStretches() const { return colStretch; };
        tStretchList getRowStretches() const { return rowStretch; };

        void checkChild(const ScreenElement* childel) const;
        
        bool operator==(const GridLayoutElement& other) const;
};
}
}
#endif
