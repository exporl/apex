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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENELEMENTCREATORTEMPLATE_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENELEMENTCREATORTEMPLATE_H_

#include "functions.h"
#include "screenelementcreator.h"

namespace apex
{
namespace editor
{
/**
 * The ScreenElementCreatorTemplate class is a template
 * implementation of the \ref ScreenElementCreator abstract base
 * class.  It will create an instance of type ElementType in its
 * create method.  The ElementType should be a subclass of
 * ScreenElement.
 */
template <typename ElementType>
class ScreenElementCreatorTemplate : public ScreenElementCreator
{
    const QString elName;

public:
    ScreenElementCreatorTemplate(const QString &name);
    ScreenElement *create(ScreenElement *parent, Screen *screen) const;
    const QString getElementName() const
    {
        return elName;
    };
    const QString getFreeID(const ScreenElementMap &takenIDs) const;
};

template <typename ElementType>
ScreenElementCreatorTemplate<ElementType>::ScreenElementCreatorTemplate(
    const QString &name)
    : elName(name)
{
}

template <typename ElementType>
ScreenElement *
ScreenElementCreatorTemplate<ElementType>::create(ScreenElement *parent,
                                                  Screen *screen) const
{
    ElementType *ret =
        new ElementType(getFreeID(screen->idToElementMap()), parent);
    ret->fillChildrenWithEmpties(screen);
    return ret;
}

template <typename ElementType>
const QString ScreenElementCreatorTemplate<ElementType>::getFreeID(
    const ScreenElementMap &takenIDs) const
{
    return findFreeID(getElementName(), takenIDs);
}
}
}
#endif
