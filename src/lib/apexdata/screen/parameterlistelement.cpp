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
 
#include "parameterlistelement.h"

#include "screenelementvisitor.h"

#include <assert.h>
#include <QRegExp>
#include <QStringList>

#include "apextools.h"

namespace apex
{
namespace data
{

ParameterListElement::ParameterListElement(
    const QString& id, ScreenElement* parent )
    : ParametersContainerElement( id, parent )
{
}

ParameterListElement::~ParameterListElement()
{
}

bool ParameterListElement::addParameter(
    const QString& id, const QString& name,
    const QString& expression )
{
    assert( !id.isEmpty() );
#ifdef PRINTPARAMETERLIST
    qDebug("ParameterlistElement::AddParameter id=" + id);
#endif
    parameterList.push_back( ParameterData( id, name, expression ) );
    return CheckExpression( expression );
}


const ParameterListElement::ParameterListT& ParameterListElement::getParameterList() const
{
    return parameterList;
}


void ParameterListElement::visit( ScreenElementVisitor* v )
{
    v->visitParameterList( this );
}

void ParameterListElement::visit( ScreenElementVisitor* v ) const
{
    v->visitParameterList( this );
}

bool ParameterListElement::setParameterID( int param, const QString& s )
{
    // check id already taken...
    for ( ParameterListT::iterator i = parameterList.begin();
          i != parameterList.end(); ++i )
    {
        if ( i->id == s && i - parameterList.begin() != param )
            return false;
    }
    parameterList[param].id = s;
    return true;
}

void ParameterListElement::setParameterName( int param, const QString& s )
{
    parameterList[param].name = s;
}

bool ParameterListElement::setParameterExpression( int param, const QString& s )
{
    if ( !CheckExpression( s ) ) return false;
    else {
        parameterList[param].expression = s;
        return true;
    }
}

ScreenElement::ElementTypeT ParameterListElement::elementType() const
{
    return ParameterList;
}

void ParameterListElement::removeParameter( int param )
{
    parameterList.erase( parameterList.begin() + param );
}

}
}

bool apex::data::ParameterListElement::parameterIDTaken( const QString& s ) const
{
    for ( ParameterListT::const_iterator i = parameterList.begin();
          i != parameterList.end(); ++i ) {
        if ( i->id == s )
            return true;
    }
    return false;
}

bool apex::data::ParameterListElement::
        operator==(const ParameterListElement& other) const
{
    if (!ApexTools::haveSameContents(parameterList, other.parameterList))
    {
        qDebug("parameterlists are not equal");
        return false;
    }
    
    return  ScreenElement::operator==(other);
}

