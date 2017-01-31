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

#include "htmlelement.h"

#include "screenelementvisitor.h"

#include <QObject>

namespace apex
{
namespace data
{

HtmlElement::HtmlElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent )
{
}

HtmlElement::~HtmlElement()
{
}

void HtmlElement::setPage( const QString& t )
{
    m_page = t;
}

/*QString HtmlElement::text() const
{
    return QString();
}*/

const QString& HtmlElement::page() const
{
    return m_page;
}

void HtmlElement::visit( ScreenElementVisitor* v )
{
    v->visitHtml( this );
}

void HtmlElement::visit( ScreenElementVisitor* v ) const
{
    v->visitHtml( this );
}

ScreenElement::ElementTypeT HtmlElement::elementType() const
{
    return Html;
}

bool HtmlElement::operator==(const HtmlElement& other) const
{
    return ScreenElement::operator==(other) && m_page == other.m_page;
}

}
}

