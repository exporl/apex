/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#include "apextools/apexpaths.h"
#include "apextools/apextools.h"

#include "calibration/soundlevelmeter.h"

#include "coh/cohclient.h"

#include "common/paths.h"
#include "common/pluginloader.h"
#include "common/utils.h"

#include "device/plugincontrollerinterface.h"

#include "feedback/pluginfeedbackinterface.h"

#include "filter/pluginfilterinterface.h"

#include "runner/pluginrunnerinterface.h"

#include "plugindialog.h"
#include "ui_plugindialog.h"

#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

using namespace cmn;
using namespace coh;

namespace apex
{

class PluginDialogPrivate : public QObject
{
    Q_OBJECT
public:
    void add(const QObject *object, const QString &what, const QString &text,
             const QString &tooltip = QString(),
             const QString &path = QString());
    void scanPlugins();

public Q_SLOTS:
    void on_refresh_clicked();
    void on_buttonBox_helpRequested();

public:
    QMap<QString, QTreeWidgetItem *> parents;
    Ui::PluginDialog ui;

private:
    DECLARE_PUBLIC(PluginDialog)
protected:
    DECLARE_PUBLIC_DATA(PluginDialog)
};

// PluginDialogPrivate =========================================================

void PluginDialogPrivate::add(const QObject *object, const QString &what,
                              const QString &text, const QString &tooltip,
                              const QString &path)
{
    QTreeWidgetItem *parent = parents.value(what);
    if (!parent) {
        parent = new QTreeWidgetItem(ui.treeWidget, QStringList() << what);
        parent->setExpanded(true);
        parents.insert(what, parent);
    }

    QString className(PluginDialog::tr("Unknown"));
    QString filePath(path);

    if (object) {
        className = QString::fromLatin1(object->metaObject()->className());
        if (className.endsWith("Creator"))
            className.resize(className.size() - 7);
        filePath = PluginLoader().pluginPath(object);
    }

    QTreeWidgetItem *const item = new QTreeWidgetItem(
        parent, QStringList() << className << text
                              << QFileInfo(filePath).fileName());

    item->setToolTip(2, filePath);
    if (!tooltip.isEmpty()) {
        item->setToolTip(0, tooltip);
        item->setToolTip(1, tooltip);
    } else {
        item->setToolTip(0, className);
        item->setToolTip(1, text);
    }
}

void PluginDialogPrivate::scanPlugins()
{
    ui.treeWidget->clear();
    parents.clear();

    Q_FOREACH (const QObject *const object,
               PluginLoader().allAvailablePlugins()) {
        bool added = false;
        if (const PluginFilterCreator *const creator =
                qobject_cast<PluginFilterCreator *>(object)) {
            add(object, PluginDialog::tr("Filters"),
                creator->availablePlugins().join(","));
            added = true;
        }

        if (const PluginControllerCreator *const creator =
                qobject_cast<PluginControllerCreator *>(object)) {
            add(object, PluginDialog::tr("Controllers"),
                creator->availablePlugins().join(","));
            added = true;
        }

        if (const PluginRunnerCreator *const creator =
                qobject_cast<PluginRunnerCreator *>(object)) {
            add(object, PluginDialog::tr("Runners"),
                creator->availablePlugins().join(","));
            added = true;
        }

        if (const SoundLevelMeterPluginCreator *const creator =
                qobject_cast<SoundLevelMeterPluginCreator *>(object)) {
            add(object, PluginDialog::tr("Sound level meters"),
                creator->availablePlugins().join(","));
            added = true;
        }

        if (const PluginFeedbackCreator *const creator =
                qobject_cast<PluginFeedbackCreator *>(object)) {
            add(object, PluginDialog::tr("Feedback plugins"),
                creator->availablePlugins().join(","));
            added = true;
        }

        if (const CohClientCreator *const creator =
                qobject_cast<CohClientCreator *>(object)) {
            add(object, PluginDialog::tr("CI Clients"),
                creator->cohDriverName());
            added = true;
        }
        if (!added) {
            add(object, PluginDialog::tr("Unknown plugins"),
                PluginDialog::tr("unknown"));
        }
    }

    QMapIterator<QString, QString> i(PluginLoader().pluginsWithErrors());
    while (i.hasNext()) {
        i.next();
        add(NULL, PluginDialog::tr("Invalid plugins"),
            PluginDialog::tr("unknown"), i.value(), i.key());
    }

    ui.treeWidget->resizeColumnToContents(0);
    ui.treeWidget->resizeColumnToContents(1);
    ui.treeWidget->resizeColumnToContents(2);
}

void PluginDialogPrivate::on_refresh_clicked()
{
    E_P(PluginDialog);

    int pluginCount = PluginLoader().allAvailablePlugins().size();
    PluginDialog::refreshPluginCache();
    scanPlugins();
    if (pluginCount == PluginLoader().allAvailablePlugins().size()) {
        QMessageBox::information(p, tr("Plugins"),
                                 tr("No new plugins have been found.\n"
                                    "Restarting RBA might help."));
    }
}

void PluginDialogPrivate::on_buttonBox_helpRequested()
{
    E_P(PluginDialog);

    QMessageBox::information(
        p, tr("Help for plugin dialog"),
        tr("This dialog lists all plugins that could "
           "be found in the plugin directories: \n %1 \n"
           "The plugin specified in the experiment file "
           "should correspond to one of the names in the "
           "second column. By holding the cursor above the "
           "first column, eventual error messages during "
           "loading of the plugin are shown.")
            .arg(Paths::pluginDirectories().join("\n")));
}

// PluginDialog ================================================================

PluginDialog::PluginDialog(QWidget *parent)
    : QDialog(parent), dataPtr(new PluginDialogPrivate)
{
    E_D(PluginDialog);

    d->pubPtr = this;

    d->ui.setupUi(this);

    d->ui.treeWidget->header()->hide();
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);

    QPushButton *button = d->ui.buttonBox->addButton(
        tr("Refresh plugins"), QDialogButtonBox::ActionRole);
    button->setObjectName(QL1S("refresh"));
    connectSlotsByNameToPrivate(this, d);

    ApexTools::expandWidgetToWindow(this);

    d->scanPlugins();
}

PluginDialog::~PluginDialog()
{
    delete dataPtr;
}

void PluginDialog::refreshPluginCache()
{
    QSettings settings(QL1S("Trolltech"));
    Q_FOREACH (const auto &group, settings.childGroups())
        if (group.startsWith(QL1S("Qt Plugin Cache ")))
            settings.remove(group);
    PluginLoader().reloadAllPlugins();
}

} // namespace apex

#include "plugindialog.moc"
