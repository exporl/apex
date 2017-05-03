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

#include "apexdata/interactive/parameterdialogresults.h"

#include "interactiveparametersdialog.h"
#include "ui_parameterdialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScroller>
#include <QScrollBar>
#include <QSpinBox>

namespace apex
{

// ParameterDialog =============================================================

InteractiveParametersDialog::InteractiveParametersDialog(InteractiveParameters* data, QWidget *parent) :
    QDialog(parent),
    InteractiveParameters::Callback(),
    ui(new Ui::ParameterDialog),
    data(data)
{
    buildWidgets();
}

InteractiveParametersDialog::~InteractiveParametersDialog()
{
    qDeleteAll(widgets);
    widgets.clear();
}

void InteractiveParametersDialog::warning(const QString &message)
{
    QMessageBox::warning(this, tr("Warning"), message);
}

void InteractiveParametersDialog::buildWidgets()
{
    ui->setupUi(this);

#ifdef Q_OS_ANDROID
    QScrollArea *area = qobject_cast<QScrollArea *>(ui->scrollArea);
    QScroller::grabGesture(area->viewport(), QScroller::LeftMouseButtonGesture);
    area->horizontalScrollBar()->setEnabled(false);
    area->verticalScrollBar()->hide();
    showMaximized();
#endif

    QGridLayout *gridLayout = new QGridLayout(ui->parameters);
    Q_FOREACH (const InteractiveParameters::Entry &entry, data->entries()) {
        const unsigned line = gridLayout->rowCount();
        const QString description = entry.description;
        const QString defaultValue = entry.defaultvalue;
        const int type = entry.type;

        QWidget* widget = Q_NULLPTR;
        switch (type) {
        case InteractiveParameters::IntValue: {
            QSpinBox * const spinbox = new QSpinBox(this);
            spinbox->setRange(-999999999, 999999999);
            spinbox->setSingleStep(1);
            spinbox->setValue(defaultValue.toInt());
            widget = spinbox;
            break; }
        case InteractiveParameters::DoubleValue: {
            QDoubleSpinBox * const spinbox = new QDoubleSpinBox(this);
            spinbox->setRange(-999999999, 999999999);
            spinbox->setSingleStep(0.1);
            spinbox->setValue(defaultValue.toDouble());
            widget = spinbox;
            break; }
        case InteractiveParameters::StringValue: {
            QLineEdit * const edit = new QLineEdit(this);
            edit->setText(defaultValue);
            widget = edit;
            break; }
        case InteractiveParameters::BoolValue: {
            QCheckBox * const checkbox = new QCheckBox(this);
            checkbox->setChecked(defaultValue.toLower() == QLatin1String("true"));
            widget = checkbox;
            break; }
        case InteractiveParameters::ComboValue: {
            QComboBox* const combobox = new QComboBox(this);
            combobox->addItems( defaultValue.split("|"));
            widget = combobox;
            break; }
        }
        gridLayout->addWidget(widget, line, 1);
        widget->setToolTip(description);
        widgets.append(widget);

        QLabel * const label = new QLabel(description, this);
        label->setBuddy(widget);
        widget->setToolTip(description);
        gridLayout->addWidget(label, line, 0);
    }

    gridLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                QSizePolicy::Expanding), gridLayout->rowCount(), 0);

    if (!widgets.isEmpty())
        widgets.value(0)->setFocus();
}

void InteractiveParametersDialog::apply() {
    QStringList results;
    for(int i = 0; i < widgets.size(); ++i) {
        const QWidget * const widget = widgets[i];
        const int type = data->entries().at(i).type;

        switch (type) {
        case InteractiveParameters::IntValue:
            results << QString::number(dynamic_cast<const QSpinBox*>(widget)->value());
            break;
        case InteractiveParameters::DoubleValue:
            results << QString::number(dynamic_cast<const QDoubleSpinBox*>(widget)->value());
            break;
        case InteractiveParameters::StringValue:
            results << dynamic_cast<const QLineEdit*>(widget)->text();
            break;
        case InteractiveParameters::BoolValue:
            results << (dynamic_cast<const QAbstractButton*>(widget)
                ->isChecked() ? QLatin1String("true") : QLatin1String("false"));
            break;
        case InteractiveParameters::ComboValue:
            results << dynamic_cast<const QComboBox*>(widget)->currentText();
            break;
        default:
            qFatal("invalid type: %u", type);
        }
    }
    data->apply(results, this);
}

} // namespace apex
