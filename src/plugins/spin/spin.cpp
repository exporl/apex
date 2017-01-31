/******************************************************************************
 * Copyright (C) 2007  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "gui/spindialog.h"
#include "pluginrunnerinterface.h"
#include "pathtools.h"
#include <QStringList>
#include <QDialog>
#include <QMessageBox>
#include <QDebug>

class SpinRunnerCreator :
            public QObject,
            public PluginRunnerCreator
{
        Q_OBJECT
        Q_INTERFACES(PluginRunnerCreator)

    public:
        virtual QStringList availablePlugins() const;

        virtual PluginRunnerInterface *createRunner(
            const QString &name,
            const QString &apexbasepath) const;
};

Q_EXPORT_PLUGIN2(spin, SpinRunnerCreator)

class SpinRunner:
            public QObject,
            public PluginRunnerInterface
{
        Q_OBJECT

    public:
        SpinRunner(QString apexbasepath);
        ~SpinRunner();

        /**
         * Return absolute path of the experiment to be ran
         * Can be a newly generated (written) experiment or an existing one
         * on disk
         */
        virtual QString getFileName();

    private:
        QString basepath;
        QString configFile;
        QString schemaFile;
};

// SpinRunner ==============================================================

SpinRunner::SpinRunner(QString apexbasepath)
        : PluginRunnerInterface("Spin"), basepath(apexbasepath),
                                configFile("spin.xml"), schemaFile("spin.xsd")
{
}

SpinRunner::~SpinRunner()
{
}


QString SpinRunner::getFileName()
{
    try
    {
        spin::gui::SpinDialog dlg(
                apex::PathTools::GetConfigFilePath(basepath, configFile),
                apex::PathTools::GetSchemaPath(basepath, schemaFile));

        dlg.exec();
        return dlg.getFileName();
    }
    catch (ApexStringException e)
    {
        QString error = e.what();
        qDebug() << error;
        QString title = tr("Error finding file");
        QString message = tr("Failed to find %1 file.\n"
                             "Please fix this problem and restart SPIN.");

        //either config or schema file was not found
        if (error.contains(configFile))
            QMessageBox::critical(0, title, message.arg(tr("configuration")));
        else if (error.contains(schemaFile))
            QMessageBox::critical(0, title, message.arg(tr("schema")));
        else
        {
            QMessageBox::critical(0, tr("Unknown error"),
                                  tr("An unknown error occurred:\n%1")
                                          .arg(error));
        }

        return "";
    }
}

// SpinRunnerCreator =======================================================

QStringList SpinRunnerCreator::availablePlugins() const
{
    return QStringList() << "spin";
}

PluginRunnerInterface *SpinRunnerCreator::createRunner
(const QString &name,
 const QString &apexbasepath) const
{
    try
    {
        if (name == "spin")
            return new SpinRunner(apexbasepath);
    }
    catch (...)
    {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "spin.moc"
