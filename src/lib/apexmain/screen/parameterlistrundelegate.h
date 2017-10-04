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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_PARAMETERLISTRUNDELEGATE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_PARAMETERLISTRUNDELEGATE_H_

#include "parameterscontainerrundelegate.h"

#include <QTableView>

namespace apex
{
namespace data
{
class ScreenElement;
class ParameterListElement;
}

namespace stimulus
{
class Stimulus;
}

namespace rundelegates
{
using data::ParameterListElement;
using data::ScreenElement;

/**
 * The ParameterListRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a ParameterListElement.
 */
class ParameterListRunDelegate : public QTableView,
                                 public ScreenElementRunDelegate,
                                 public ParametersContainerRunDelegate
{
    Q_OBJECT
public:
    ParameterListRunDelegate(ExperimentRunDelegate *p_exprd,
                             const ParameterListElement *e, QWidget *parent);
    ~ParameterListRunDelegate();

    const ScreenElement *getScreenElement() const;

    QWidget *getWidget();
    // void connectSlots( gui::ScreenRunDelegate* d );
public slots:
    // void newStimulus( stimulus::Stimulus* stimulus );
    void updateParameter(const QString &id, const QVariant &value);

private:
    class Model;
    const ParameterListElement *element;
    Model *model;
};
}
}

#endif
