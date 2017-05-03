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

#include "screenelement.h"

#include <QDebug>

namespace apex
{

namespace data
{

ScreenElement::ScreenElement( const QString& i, ScreenElement* p )
    : parent( p ), id( i ), x( 0 ), y( 0 ), fontSize( -1 ), disabled(false)
{
    if ( parent )
    {
        Q_ASSERT( parent->canHaveChildren() );
        parent->addChild( this );
    }
}

ScreenElement::~ScreenElement()
{
    if ( parent )
    {
        parent->deleteChild( this );
    }
}

const FilePrefix& ScreenElement::prefix() const
{
    return filePrefix;
}

void ScreenElement::setPrefix(const FilePrefix& prefix)
{
    filePrefix = prefix;
}

ScreenElement* ScreenElement::getParent()
{
    return parent;
}

bool ScreenElement::canHaveChildren() const
{
    return getNumberOfChildPlaces() > 0;
}

int ScreenElement::getNumberOfChildren() const
{
    qFatal( "should not be called.." );
    return 0;
}

ScreenElement* ScreenElement::getChild( int /*i*/ )
{
    qFatal( "should not be called.." );
    return 0;
}

void ScreenElement::addChild( ScreenElement* /*child*/ )
{
    qFatal( "should not be called.." );
}

const std::vector<ScreenElement*>* ScreenElement::getChildren() const
{
    qFatal( "Should not be called" );
    return 0;
}

std::vector<ScreenElement*>* ScreenElement::getChildren()
{
    qFatal( "Should not be called" );
    return 0;
}

const QString ScreenElement::getID() const
{
    return id;
}

const ScreenElement* ScreenElement::getChild( int /*i*/ ) const
{
    qFatal( "should not be called.." );
    return 0;
}

void ScreenElement::deleteChild( ScreenElement* /*child*/ )
{
    qFatal( "should not be called.." );
}

int ScreenElement::getNumberOfChildPlaces() const
{
    return 0;
}

void ScreenElement::fillChildrenWithEmpties( Screen* /*screen*/ )
{
    // default impl does nothing..
}


void ScreenElement::setID( const QString& i )
{
    id = i;
}

const ScreenElement* ScreenElement::getParent() const
{
    return parent;
}


QString ScreenElement::text() const
{
    return QString();
}

QString ScreenElement::getShortCut(const QString& action) const
{
    if (! shortcuts.contains(action))
    //    throw ApexStringException(tr("Could not find shortcut for action %1")
    //    .arg(action));
        return QString();
    return shortcuts[action];
}

void ScreenElement::setShortCut(const QString& key,
                 const QString action)
{
    shortcuts[action]=key;
}

bool ScreenElement::operator==(const ScreenElement& other) const
{
    return  elementType() == other.elementType() &&
            ((parent == other.parent) || (*parent == *other.parent)) &&
            id == other.id &&
            x == other.x &&
            y == other.y &&
            fontSize == other.fontSize &&
            fgcolor == other.fgcolor &&
            bgcolor == other.bgcolor &&
            style == other.style &&
            disabled == other.disabled &&
            filePrefix == other.filePrefix &&
            shortcuts == other.shortcuts;
    return true;
}

bool ScreenElement::operator!=(const ScreenElement& other) const
{
    return !(*this == other);
}

}
}
