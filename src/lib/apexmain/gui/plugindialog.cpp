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

#include "filter/pluginfilterinterface.h"
#include "device/plugincontrollerinterface.h"
#include "runner/pluginrunnerinterface.h"
#include "calibration/soundlevelmeter.h"
#include "stimulus/l34/apexnresocketinterface.h"
#include "feedback/pluginfeedbackinterface.h"

#include "services/pluginloader.h"
#include "plugindialog.h"
#include "ui_plugindialog.h"

#include <QHeaderView>
#include "services/paths.h"
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QMap>
#include <QMessageBox>

namespace apex
{


class PluginDialogPrivate:
        public QObject
{
    Q_OBJECT
    //Q_DECLARE_TR_FUNCTIONS (PluginDialog)
public:
    void add (const QObject *object, const QString &what, const QString &text,
            const QString &tooltip = QString(),
            const QString &path = QString());
    void scanPlugins();

    QMap<QString, QTreeWidgetItem*> parents;
    Ui::PluginDialog ui;

public slots:
    void help();
};

// PluginDialogPrivate =========================================================

void PluginDialogPrivate::add (const QObject *object, const QString &what,
        const QString &text, const QString &tooltip, const QString &path)
{
    QTreeWidgetItem *parent = parents.value (what);
    if (!parent) {
        parent = new QTreeWidgetItem (ui.treeWidget,
                QStringList() << what);
        parent->setExpanded (true);
        parents.insert (what, parent);
    }

    QString className (tr ("Unknown"));
    QString filePath (path);

    if (object) {
        className = QString::fromAscii (object->metaObject()->className());
        if (className.endsWith ("Creator"))
            className.resize (className.size() - 7);
        filePath = PluginLoader::Get().pluginPath (object);
    }

    QTreeWidgetItem * const item = new QTreeWidgetItem (parent, QStringList()
            << className
            << text
            << QFileInfo (filePath).fileName());

    item->setToolTip (2, filePath);
    if (!tooltip.isEmpty()) {
        item->setToolTip (0, tooltip);
        item->setToolTip (1, tooltip);
    } else {
        item->setToolTip (0, className);
        item->setToolTip (1, text);
    }
}

void PluginDialogPrivate::scanPlugins()
{
    ui.treeWidget->clear();
    parents.clear();

    Q_FOREACH (const QObject * const object, PluginLoader::Get().allAvailablePlugins()) {
        bool added = false;

        if (const PluginFilterCreator * const creator =
            qobject_cast<PluginFilterCreator*> (object)) {
            add (object, tr ("Filters"), creator->availablePlugins().join(","),
                 "");
            added = true;
        }

        if (const PluginControllerCreator * const creator =
            qobject_cast<PluginControllerCreator*> (object)) {
            add (object, tr ("Controllers"),
                 creator->availablePlugins().join(","),
                 "");
            added = true;
        }
        
        if (const PluginRunnerCreator * const creator =
            qobject_cast<PluginRunnerCreator*> (object)) {
            add (object, tr ("Runners"),
                 creator->availablePlugins().join(","),
                 "");
            added = true;
        }

        if (const SoundLevelMeterPluginCreator * const creator =
            qobject_cast<SoundLevelMeterPluginCreator*> (object)) {
            add (object, tr ("Sound level meters"),
                 creator->availablePlugins().join(","),
                 "");
            added = true;
        }

        if (const PluginFeedbackCreator * const creator =
            qobject_cast<PluginFeedbackCreator*> (object)) {
            add (object, tr ("Feedback plugins"),
                            creator->availablePlugins().join(","),
                            "");
                            added = true;
        }

        if (const ApexNreSocketCreatorInterface * const creator =
            qobject_cast<ApexNreSocketCreatorInterface*> (object)) {
            add (object, tr ("L34 plugin"), "",
                 "");
            added = true;
        }


        
        /*if (const ModuleCreator * const creator =
                qobject_cast<ModuleCreator*> (object)) {
            add (object, tr ("Modules"), creator->fullName(),
                    creator->description());
            added = true;
        }
        if (const CiClientCreator * const creator =
                qobject_cast<CiClientCreator*> (object)) {
            add (object, tr ("CI Clients"), creator->ciDriverName());
            added = true;
        }
        if (const SoundcardCreator * const creator =
                qobject_cast<SoundcardCreator*> (object)) {
            add (object, tr ("Soundcard Drivers"),
                    creator->soundcardDriverName());
            added = true;
        } */
        if (!added) {
            add (object, tr ("Unknown plugins"), tr ("unknown"));
        }
    }

    //PluginLoader::Get().blaDiBla();
    //QMapIterator<QString, QString> i (PluginLoader::Get().blaDiBla());
    QMapIterator<QString, QString> i (PluginLoader::Get().pluginsWithErrors());
    while (i.hasNext()) {
        i.next();
        add (NULL, tr ("Invalid plugins"), tr (""), i.value(), i.key());
    }

    ui.treeWidget->resizeColumnToContents (0);
    ui.treeWidget->resizeColumnToContents (1);
    ui.treeWidget->resizeColumnToContents (2);
}


void PluginDialogPrivate::help()
{
    QMessageBox::information(0, tr("Help for plugin dialog"),
                             tr("This dialog lists all plugins that could "
                            "be found in the plugin directories: \n %1 \n"
                            "The plugin specified in the experiment file "
                            "should correspond to one of the names in the "
                            "second column. By holding the cursor above the "
                            "first column, eventual error messages during "
                            "loading of the plugin are shown.")
                            .arg(PluginLoader::Get().pluginDirectories()
                                     .join("\n") )
                            );
    // FIXME: pass correct parent window
}

// PluginDialog ================================================================

PluginDialog::PluginDialog (QWidget *parent) :
    QDialog (parent),
    d (new PluginDialogPrivate)
{
    d->ui.setupUi (this);
    d->ui.treeWidget->header()->hide();
    
    /*setAttribute (Qt::WA_DeleteOnClose);          // [Tom] causes segfault
    setAttribute (Qt::WA_QuitOnClose, false);*/

    connect(d->ui.buttonBox, SIGNAL(helpRequested()), d.get(), SLOT(help()));
    
    d->scanPlugins();
}

PluginDialog::~PluginDialog()
{
}

} // namespace apex

#include "plugindialog.moc"
