/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "calibration/calibrationdatabase.h"
#include "calibrationsetupsdialog.h"
#include "ui_calibrationsetupsdialog.h"

#include <QFileDialog>
#include <QMessageBox>

namespace apex
{

class CalibrationSetupsDialogPrivate : public QObject
{
    Q_OBJECT
public:
    CalibrationSetupsDialogPrivate (CalibrationSetupsDialog *pub);

private Q_SLOTS:
    void on_add_clicked();
    void on_remove_clicked();
    void on_rename_clicked();
    void on_list_currentItemChanged();
    void on_input_textChanged();
    void on_importSetup_clicked();
    void on_exportSetup_clicked();
    void accepted();
    void finished (int result);

private:
    void checkAddButton();
    void checkRenameButton();
    void checkDefaultButton();
    void addSetup (const QString &setup, bool imported = false);
    void renameSetup (QListWidgetItem *item, const QString &newName);
    bool hasSetup (const QString &setup);

private:
    CalibrationSetupsDialog * const p;
    const int setupNameRole;
    const int setupRenamedNameRole;
    const int setupImportedRole;
    Ui::CalibrationSetupsDialog ui;
};

// CalibrationSetupsDialogPrivate ==============================================

CalibrationSetupsDialogPrivate::CalibrationSetupsDialogPrivate
    (CalibrationSetupsDialog *pub) :
    p (pub),
    setupNameRole (Qt::UserRole),
    setupRenamedNameRole (setupNameRole + 1),
    setupImportedRole (setupRenamedNameRole + 1)
{
    ui.setupUi (p);

    ui.detailsList->setVisible (false);
    QPushButton *details = ui.buttonBox->addButton (tr ("Details..."),
            QDialogButtonBox::ActionRole);
    details->setCheckable (true);

    connect (ui.add, SIGNAL (clicked()), this, SLOT (on_add_clicked()));
    connect (ui.remove, SIGNAL (clicked()), this, SLOT (on_remove_clicked()));
    connect (ui.rename, SIGNAL (clicked()), this, SLOT (on_rename_clicked()));
    connect (ui.exportSetup, SIGNAL (clicked()),
            this, SLOT (on_exportSetup_clicked()));
    connect (ui.importSetup, SIGNAL (clicked()),
            this, SLOT (on_importSetup_clicked()));
    connect (ui.list, SIGNAL
            (currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT (on_list_currentItemChanged()));
    connect (ui.input, SIGNAL (textChanged(QString)),
            this, SLOT (on_input_textChanged()));
    connect (p, SIGNAL (accepted()), this, SLOT (accepted()));
    connect (p, SIGNAL (finished(int)), this, SLOT (finished(int)));
    connect (details, SIGNAL (toggled(bool)),
            ui.detailsList, SLOT (setVisible(bool)));

    Q_FOREACH (const QString &setup,
            CalibrationDatabase().hardwareSetups())
        addSetup (setup);

    on_list_currentItemChanged();
    ui.add->setEnabled (false);
}

void CalibrationSetupsDialogPrivate::finished (int result)
{
    if (result != QDialog::Accepted)
        for (int i = 0; i < ui.list->count(); ++i)
            if (ui.list->item (i)->data (setupImportedRole).toBool())
                CalibrationDatabase().removeHardwareSetup
                    (ui.list->item (i)->data (setupNameRole).toString());
}

void CalibrationSetupsDialogPrivate::accepted()
{
    const QSet<QString> before = QSet<QString>::fromList
        (CalibrationDatabase().hardwareSetups());
    QMap<QString, QString> after;
    for (int i = 0; i < ui.list->count(); ++i)
        after[ui.list->item (i)->data (setupNameRole).toString()] =
            ui.list->item (i)->data (setupRenamedNameRole).toString();

    const QSet<QString> afterKeys = QSet<QString>::fromList (after.keys());
    const QSet<QString> toRemove = QSet<QString> (before) - afterKeys;
    const QSet<QString> toAdd = QSet<QString> (afterKeys) - before;
    const QSet<QString> toModify = QSet<QString> (afterKeys) - toAdd;

    Q_FOREACH (const QString &setup, toRemove)
        CalibrationDatabase().removeHardwareSetup (setup);
    Q_FOREACH (const QString &setup, toAdd)
        CalibrationDatabase().addHardwareSetup (!after.value (setup).isEmpty() ?
                after.value (setup) : setup);
    Q_FOREACH (const QString &setup, toModify)
        if (!after.value (setup).isEmpty())
            CalibrationDatabase().renameHardwareSetup (setup, after.value
                    (setup));
}

void CalibrationSetupsDialogPrivate::on_add_clicked()
{
    const QString text = ui.input->text().simplified();
    if (!text.isEmpty() && !hasSetup (text)) {
        addSetup (text);
        ui.add->setEnabled (false);
    }
}

void CalibrationSetupsDialogPrivate::on_rename_clicked()
{
    QListWidgetItem * const item = ui.list->currentItem();
    Q_ASSERT (item);

    const QString text = ui.input->text().simplified();
    if (!text.isEmpty() && !hasSetup (text)) {
        renameSetup (item, text);
        ui.add->setEnabled (false);
        ui.rename->setEnabled (false);
    }
}

void CalibrationSetupsDialogPrivate::on_remove_clicked()
{
    Q_ASSERT (ui.list->currentItem());

    delete ui.list->currentItem();
    checkAddButton();
    checkRenameButton();
}

void CalibrationSetupsDialogPrivate::on_exportSetup_clicked()
{
    Q_ASSERT (ui.list->currentItem());

    const QListWidgetItem * const item = ui.list->currentItem();
    const QString setup = item->data (setupNameRole).toString();
    const QString fileName = QFileDialog::getSaveFileName (p,
            tr ("Export Calibration Data"), QString(),
            tr ("Calibration data (*.calib)"));
    if (fileName.isEmpty())
        return;

    try {
        CalibrationDatabase().exportHardwareSetup (setup, fileName);
    } catch (std::exception &e) {
        QMessageBox::critical (p, tr ("Calibration Database Error"),
                tr ("Unable to export setup:\n%1").arg (e.what()));
    }
}

void CalibrationSetupsDialogPrivate::on_importSetup_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName (p,
            tr ("Import Calibration Data"), QString(),
            tr ("Calibration data (*.calib)"));
    if (fileName.isEmpty())
        return;

    try {
        const QString setup = CalibrationDatabase().importHardwareSetup
            (fileName);
        addSetup (setup, true);
    } catch (std::exception &e) {
        QMessageBox::critical (p, tr ("Calibration Database Error"),
                tr ("Unable to import setup:\n%1").arg (e.what()));
    }
}

void CalibrationSetupsDialogPrivate::on_list_currentItemChanged()
{
    CalibrationDatabase database;
    const QListWidgetItem * const item = ui.list->currentItem();
    ui.remove->setEnabled (item != NULL);
    ui.exportSetup->setEnabled (item != NULL);
    checkRenameButton();
    ui.detailsList->clear();
    if (item) {
        const QString setup = item->data (setupNameRole).toString();
        Q_FOREACH (const QString &profile, database.calibrationProfiles (setup))
            Q_FOREACH (const QString &name, database.parameterNames
                    (setup, profile))
                ui.detailsList->addItem (QString ("%3 - %4 (%1:%2)")
                 .arg (database.targetAmplitude (setup, profile, name))
                 .arg (database.outputParameter (setup, profile, name))
                 .arg (profile, name));
    }
}

void CalibrationSetupsDialogPrivate::on_input_textChanged()
{
    checkAddButton();
    checkRenameButton();
    checkDefaultButton();
}

void CalibrationSetupsDialogPrivate::checkAddButton()
{
    const QString text = ui.input->text().simplified();
    ui.add->setEnabled (!text.isEmpty() && !text.startsWith ('<') &&
            !hasSetup (text));
}

void CalibrationSetupsDialogPrivate::checkRenameButton()
{
    const QString text = ui.input->text().simplified();
    ui.rename->setEnabled (ui.list->currentItem() && !text.isEmpty() &&
            !text.startsWith ('<') && !hasSetup (text));
}

void CalibrationSetupsDialogPrivate::checkDefaultButton()
{
    if (ui.input->text().simplified().isEmpty())
        ui.buttonBox->button (QDialogButtonBox::Ok)->setDefault (true);
    else
        ui.add->setDefault (true);
}

void CalibrationSetupsDialogPrivate::addSetup (const QString &setup,
        bool imported)
{
    QListWidgetItem *item = new QListWidgetItem (tr ("%2 - %1 calibration(s)")
            .arg (CalibrationDatabase().calibrationCount (setup)).arg (setup));
    ui.list->addItem (item);
    item->setData (setupNameRole, setup);
    if (imported)
        item->setData (setupImportedRole, true);
}

void CalibrationSetupsDialogPrivate::renameSetup (QListWidgetItem *item,
        const QString &newName)
{
    const QString setup = item->data (setupNameRole).toString();
    item->setText (tr ("%2 - %1 calibration(s)")
            .arg (CalibrationDatabase().calibrationCount (setup))
            .arg (newName));
    item->setData (setupRenamedNameRole, newName);
}

bool CalibrationSetupsDialogPrivate::hasSetup (const QString &setup)
{
    for (int i = 0; i < ui.list->count(); ++i) {
        QListWidgetItem *item = ui.list->item (i);
        if (item->data (setupNameRole) == setup ||
                item->data (setupRenamedNameRole) == setup)
            return true;
    }
    return false;
}

// CalibrationSetupsDialog =====================================================

CalibrationSetupsDialog::CalibrationSetupsDialog (QWidget *parent) :
    QDialog (parent),
    d (new CalibrationSetupsDialogPrivate (this))
{
}

CalibrationSetupsDialog::~CalibrationSetupsDialog()
{
}

} // namespace rba

#include "calibrationsetupsdialog.moc"
