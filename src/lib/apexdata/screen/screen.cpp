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

#include "apextools/apextools.h"

#include "buttongroup.h"
#include "gridlayoutelement.h"
#include "screen.h"

namespace apex
{
namespace data
{
ScreenElement* Screen::getRootElement()
{
    return rootElement;
}

Screen::Screen( const QString& i, ScreenElement* re,
                const ScreenElementMap& elmap,
                ButtonGroup* bg, const QString& da )
    : m_idToElementMap( elmap ),
      rootElement( re ), buttonGroup( bg ),
      defaultAnswer( da ), id( i )
{
}

Screen::~Screen()
{
    deleteElement( rootElement );
    delete buttonGroup;
}

ButtonGroup* apex::data::Screen::getButtonGroup()
{
    return buttonGroup;
}

const QString Screen::getID() const
{
    return id;
}

const ScreenElementMap& Screen::idToElementMap() const
{
    return m_idToElementMap;
}

const QString Screen::getDefaultAnswerElement() const
{
    return defaultAnswer;
}

const ScreenElement* Screen::getRootElement() const
{
    return rootElement;
}

const ButtonGroup* Screen::getButtonGroup() const
{
    return buttonGroup;
}

void Screen::setRootElement( ScreenElement* e )
{
    if ( rootElement )
        deleteElement( rootElement );
    rootElement = e;
}

void Screen::deleteElement( ScreenElement* e )
{
    if ( e->canHaveChildren() )
    {
        std::vector<ScreenElement*> children = *e->getChildren();
        for ( unsigned i = 0; i < children.size(); ++i )
            deleteElement( children[i] );
    }
    m_idToElementMap.remove( e->getID() );
    delete e;
    if ( rootElement == e )
        rootElement = 0;
}

void Screen::setElementID( ScreenElement* e, const QString& id )
{
    QString oldid = e->getID();
    m_idToElementMap.remove( oldid );
    e->setID( id );
    m_idToElementMap[id] = e;
}

void Screen::addElement( ScreenElement* e )
{
    m_idToElementMap[e->getID()] = e;
}


const ScreenElement* Screen::elementById(const QString& id) const
{
    return m_idToElementMap[id];
}


QString Screen::elementTextById(const QString& id) const
{
    return elementById(id)->text();
}


bool Screen::operator==(const Screen& other) const
{
    //special check for the buttongroup because it can be 0
    if (buttonGroup == 0 || other.buttonGroup == 0)
    {
        if (buttonGroup != other.buttonGroup)
            return false;
    }
    else if (*buttonGroup != *other.buttonGroup)
        return false;

    return  defaultAnswer == other.defaultAnswer &&
            id == other.id &&
            *rootElement == *other.rootElement &&
            ApexTools::areEqualPointerMaps(m_idToElementMap, other.m_idToElementMap);
}

bool Screen::operator!=(const Screen& other) const
{
    return !(*this == other);
}

}
}
















