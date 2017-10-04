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

#include "apexdata/screen/parameterlabelelement.h"

#include "apexdata/stimulus/stimulusparameters.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "screen/screenrundelegate.h"

#include "stimulus/stimulus.h"

#include "parameterlabelrundelegate.h"

#include <QDebug>
#include <QVariant>

namespace apex
{
namespace rundelegates
{

ParameterLabelRunDelegate::ParameterLabelRunDelegate(
    ExperimentRunDelegate *p_exprd, QWidget *parent,
    const ParameterLabelElement *e, const QFont &defaultFont)
    : LabelRunDelegateBase(p_exprd, parent, e, defaultFont), element(e)
{
    ParameterManager *mgr = p_exprd->GetParameterManager();
    setText(mgr->parameterValue(element->getParameter().id).toString());

    connect(mgr, SIGNAL(parameterChanged(QString, QVariant)), this,
            SLOT(updateParameter(QString, QVariant)));
}

const ScreenElement *ParameterLabelRunDelegate::getScreenElement() const
{
    return element;
}

void ParameterLabelRunDelegate::connectSlots(gui::ScreenRunDelegate *)
{
}

void ParameterLabelRunDelegate::updateParameter(const QString &id,
                                                const QVariant &value)
{
    if (id == element->getParameter().id) {
        setText(value.toString());
    }
}

void ParameterLabelRunDelegate::setEnabled(const bool e)
{
    QLabel::setEnabled(e);
}
}
}
