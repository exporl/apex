/******************************************************************************
 * Copyright (C) 2010  Michael Hofmann <mh21@piware.de>                       *
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

#include "apextools/apextools.h"

#include "common/pluginloader.h"

#include "runner/pluginrunnerinterface.h"

#include "startupdialog.h"
#include "ui_startupdialog.h"

#include <QPushButton>

using namespace cmn;

namespace apex
{

class StartupDialogPrivate : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void on_recentFiles_itemClicked(QListWidgetItem *item);
    void on_recentFiles_itemActivated(QListWidgetItem *item);
    void on_open_clicked();

    void plugin_clicked();

public:
    StartupDialog *p;
    QMap<QObject *, PluginRunnerInterface *> interfaces;

    Ui::StartupDialog ui;

    QString selectedFile;
};

// StartupDialogPrivate ========================================================

void StartupDialogPrivate::on_recentFiles_itemClicked(QListWidgetItem *item)
{
    selectedFile = item->text();
    p->accept();
}

void StartupDialogPrivate::on_recentFiles_itemActivated(QListWidgetItem *item)
{
    selectedFile = item->text();
    p->accept();
}

void StartupDialogPrivate::on_open_clicked()
{
    p->accept();
}

void StartupDialogPrivate::plugin_clicked()
{
    p->hide();
    selectedFile = interfaces[sender()]->getFileName();
    if (!selectedFile.isEmpty())
        p->accept();
    else
        p->reject();
}

// StartupDialog ===============================================================

StartupDialog::StartupDialog(const QStringList &recent, QWidget *parent)
    : QDialog(parent), d(new StartupDialogPrivate)
{
    d->p = this;

    d->ui.setupUi(this);

    d->ui.buttonBox->addButton(tr("Open..."), QDialogButtonBox::ActionRole)
        ->setObjectName("open");

    Q_FOREACH (PluginRunnerCreator *plugin,
               PluginLoader().availablePlugins<PluginRunnerCreator>()) {
        Q_FOREACH (const QString &name, plugin->availablePlugins()) {
            PluginRunnerInterface *interface = plugin->createRunner(name);
            if (!interface)
                continue;
            QPushButton *button = d->ui.buttonBox->addButton(
                interface->getButtonText(), QDialogButtonBox::ActionRole);
            connect(button, SIGNAL(clicked()), d, SLOT(plugin_clicked()));
            d->interfaces.insert(button, interface);
        }
    }

    Q_FOREACH (const QString &fileName, recent)
        d->ui.recentFiles->insertItem(0, fileName);

    ApexTools::expandWidgetToWindow(this);

    ApexTools::connectSlotsByNameToPrivate(this, d);
}

StartupDialog::~StartupDialog()
{
    qDeleteAll(d->interfaces.values());
    delete d;
}

QString StartupDialog::selectedFile() const
{
    return d->selectedFile;
}

} // namespace apex

#include "startupdialog.moc"
