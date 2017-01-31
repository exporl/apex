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
 
#include "gridlayoutrundelegate.h"

#include "screen/gridlayoutelement.h"
#include "rundelegatecreatorvisitor.h"

namespace apex
{
namespace rundelegates
{

const ScreenElement* GridLayoutRunDelegate::getScreenElement() const
{
    return element;
}

QLayout* GridLayoutRunDelegate::getLayout()
{
    return this;
}

QWidget* GridLayoutRunDelegate::getWidget()
{
    return 0;
}

GridLayoutRunDelegate::GridLayoutRunDelegate(
    ExperimentRunDelegate* p_exprd,
    const GridLayoutElement* e, QWidget* parent,
    ElementToRunningMap& elementToRunningMap,
    const QFont& font ) :
      QGridLayout(),
      ScreenElementRunDelegate(p_exprd, e),
      element( e )
{
    setMargin(5);
    setObjectName(element->getID());

//    qDebug("Creating gridlayout with parent %p", parent);

    for ( int i = 0; i < element->getNumberOfChildren(); ++i )
    {
        const ScreenElement* child = element->getChild( i );
        RunDelegateCreatorVisitor delcreator( p_exprd, parent, elementToRunningMap, font );
        ScreenElementRunDelegate* childdel = delcreator.createRunDelegate( child );
        if ( childdel )
            childdel->addToGridLayout( this );
    }

    for (int i=0; i< element->getColumnStretches().size();++i) {
        setColumnStretch(i, element->getColumnStretches().at(i));
    }
    for (int i=0; i< element->getRowStretches().size();++i) {
        setRowStretch(i, element->getRowStretches().at(i));
    }
}

}
}

