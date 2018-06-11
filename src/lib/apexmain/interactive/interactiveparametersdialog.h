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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_INTERACTIVE_INTERACTIVEPARAMETERSDIALOG_H_
#define _EXPORL_SRC_LIB_APEXMAIN_INTERACTIVE_INTERACTIVEPARAMETERSDIALOG_H_

#include "interactiveparameters.h"
#include <QDialog>
#include <QList>

namespace Ui
{
class ParameterDialog;
}

namespace apex
{
namespace data
{
class ParameterDialogResults;
}

class InteractiveParametersDialog : public QDialog,
                                    public InteractiveParameters::Callback
{
    Q_OBJECT
public:
    InteractiveParametersDialog(InteractiveParameters *data, QWidget *parent);
    virtual ~InteractiveParametersDialog();

    bool apply();
    virtual void warning(const QString &message) Q_DECL_OVERRIDE;

private:
    void buildWidgets();
    bool dialogResult();

    QList<QWidget *> widgets;
    QScopedPointer<Ui::ParameterDialog> ui;
    InteractiveParameters *data;
};
}

#endif
