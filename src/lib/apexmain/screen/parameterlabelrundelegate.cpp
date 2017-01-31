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
 
#include "parameterlabelrundelegate.h"

#include "screen/parameterlabelelement.h"
#include "screen/screenrundelegate.h"

#include "runner/experimentrundelegate.h"
#include "parameters/parametermanager.h"
#include "stimulus/stimulusparameters.h"
#include "stimulus/stimulus.h"

#include <QVariant>
#include <QDebug>

namespace apex
{
namespace rundelegates
{


ParameterLabelRunDelegate::ParameterLabelRunDelegate(
    ExperimentRunDelegate* p_exprd,
    QWidget* parent, const ParameterLabelElement* e, const QFont& defaultFont ) :
        LabelRunDelegateBase( p_exprd, parent, e, defaultFont ), element( e )
{
    qDebug() << "creating param label";
}



const ScreenElement* ParameterLabelRunDelegate::getScreenElement() const
{
    return element;
}


void ParameterLabelRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( d, SIGNAL( newStimulus( stimulus::Stimulus* ) ),
             this, SLOT( newStimulus( stimulus::Stimulus* ) ) );
}


void ParameterLabelRunDelegate::newStimulus( stimulus::Stimulus* stimulus ) {
    const data::StimulusParameters* varParam = stimulus->GetVarParameters();
    const data::StimulusParameters* fixParam = stimulus->GetFixParameters();


    QVariant value;
    QString id( element->getParameter().id );
    QString expression (  element->getParameter().expression );

    QVariant pmvalue( m_rd->GetParameterManager()->
                      parameterValue(id));
    qDebug() << "ParameterLabelRunDelegate: param value for" << id << "=" << pmvalue;
    if (pmvalue.isValid()){
        value=pmvalue.toString();
    } else if ( varParam->contains(id))
    {
        value=varParam->value(id).toString();
    } else if (fixParam->contains(id)) {
        value=fixParam->value(id).toString();
    }

    if (! expression.isEmpty()) {
        double newvalue=ParseExpression(expression,
                                        value.toDouble());
        value=QString::number(newvalue);
    }

    setText(value.toString());

}


}
}

