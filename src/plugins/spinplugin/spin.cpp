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

#include "apexmain/runner/pluginrunnerinterface.h"

#include "apexspin/spindialog.h"

#include <QMessageBox>
#include <QStringList>

using namespace apex;

class SpinRunnerCreator :
    public QObject,
    public PluginRunnerCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginRunnerCreator)
    Q_PLUGIN_METADATA(IID "apex.spin")
public:
    virtual QStringList availablePlugins() const;
    virtual PluginRunnerInterface *createRunner(const QString &name) const;
};

class SpinRunner:
    public QObject,
    public PluginRunnerInterface
{
    Q_OBJECT
public:
    SpinRunner();
    ~SpinRunner();

    /**
     * Return absolute path of the experiment to be ran
     * Can be a newly generated (written) experiment or an existing one
     * on disk
     */
    virtual QString getFileName();
};

// SpinRunner ==================================================================

SpinRunner::SpinRunner() :
    PluginRunnerInterface("Spin")
{
}

SpinRunner::~SpinRunner()
{
}

QString SpinRunner::getFileName()
{
    try {
        spin::gui::SpinDialog dlg;
        dlg.exec();
        return dlg.getFileName();
    } catch (const std::exception &e) {
        QMessageBox::critical(NULL, tr("Error"),
                tr("Error occurred:\n%1").arg(e.what()));
    }
    return QString();
}

// SpinRunnerCreator ===========================================================

QStringList SpinRunnerCreator::availablePlugins() const
{
    return QStringList() << "spin";
}

PluginRunnerInterface *SpinRunnerCreator::createRunner
(const QString &name) const
{
    try {
        if (name == "spin")
            return new SpinRunner;
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "spin.moc"
