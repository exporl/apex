/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "noisestartpointdialog.h"

#include "startlevelwidget.h"

#include <QDoubleValidator>

using namespace spin::gui;

NoiseStartpointDialog::NoiseStartpointDialog(QWidget *parent,
                                             StartLevelWidget *slw,
                                             double value)
    : QDialog(parent)
{
    setupUi(value);
    setupConnections(slw);
}

void NoiseStartpointDialog::setupUi(double value)
{
    widgets.setupUi(this);

    if (value >= 0) {
        widgets.fixedRadio->setChecked(true);
        widgets.startpointLine->setText(QString::number(value));
    } else {
        widgets.randomRadio->setChecked(true);
        widgets.startpointLine->setVisible(false);
        widgets.sLbl->setVisible(false);
    }

    QDoubleValidator *v =
        new QDoubleValidator(0, 1000, 2, widgets.startpointLine);
    widgets.startpointLine->setValidator(v);
}

void NoiseStartpointDialog::setupConnections(StartLevelWidget *slw)
{
    connect(widgets.fixedRadio, SIGNAL(toggled(bool)), widgets.startpointLine,
            SLOT(setVisible(bool)));
    connect(widgets.fixedRadio, SIGNAL(toggled(bool)), widgets.startpointLine,
            SLOT(setFocus()));
    connect(widgets.fixedRadio, SIGNAL(toggled(bool)), widgets.sLbl,
            SLOT(setVisible(bool)));

    connect(widgets.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    connect(this, SIGNAL(startpointChosen(double)), slw,
            SLOT(updateNoiseStartPoint(double)));
}

void NoiseStartpointDialog::accept()
{
    if (widgets.randomRadio->isChecked())
        Q_EMIT startpointChosen(-1);
    else
        Q_EMIT startpointChosen(widgets.startpointLine->text().toDouble());

    QDialog::accept();
}
