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

#ifndef _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENELEMENTCREATOR_H_
#define _APEX_SRC_PROGRAMS_SCREENEDITOR_SCREENELEMENTCREATOR_H_

#include "apextools/apextypedefs.h"

class QString;

namespace apex
{
namespace data
{
class ScreenElement;
class Screen;
}

namespace editor
{
using data::ScreenElement;
using data::Screen;
using gui::ScreenElementMap;

/**
 * The ScreenElementCreator class is a simple abstract base class.
 * It represents an object that knows how to create a new \ref
 * ScreenElement of a certain type, given a parent \ref
 * ScreenElement and a parent \ref Screen.
 *
 * There is a template implementation of this class, called
 * \ref ScreenElementCreatorTemplate.
 */
class ScreenElementCreator
{
public:
    virtual ScreenElement *create(ScreenElement *parent,
                                  Screen *screen) const = 0;
    virtual ~ScreenElementCreator();
    virtual const QString getElementName() const = 0;
};
}
}
#endif
