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

#ifndef SCREENLAYOUTELEMENT_H
#define SCREENLAYOUTELEMENT_H

#include "screenelement.h"

#include "global.h"

namespace apex
{

namespace data
{

/**
 * The ScreenLayoutElement class is a common base class of a \ref
 * ScreenElement which represents a layout element.  A layout
 * element is a \ref ScreenElement which can have children, and
 * knows about how to layout them on screen.
 */
class APEXDATA_EXPORT ScreenLayoutElement
            : public ScreenElement
{
    protected:
        virtual void deleteChild(ScreenElement* child);
    public:
        ScreenLayoutElement(const QString& id, ScreenElement* parent);
        ~ScreenLayoutElement();

        virtual int getNumberOfChildren() const;
        virtual ScreenElement* getChild(int i);
        virtual const ScreenElement* getChild(int i) const;
        virtual void addChild(ScreenElement* child);
        const std::vector<ScreenElement*>* getChildren() const;
        std::vector<ScreenElement*>* getChildren();
        /**
         * check whether the x and y values of this child are valid..
         * throw an ApexStringException if not...
         */
        virtual void checkChild(const ScreenElement* childel) const = 0;
        
        bool operator==(const ScreenLayoutElement& other) const;
        
    private:
        std::vector<ScreenElement*> children;
};

}

}

#endif
