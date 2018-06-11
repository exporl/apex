/******************************************************************************
 * Copyright (C) 2018  Sanne Raymaekers <sanne.raymaekers@gmail.com>         *
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

#include "apexmain/study/studymanager.h"

#include "apextools/apextools.h"

#include "commongui/listpicker.h"

using namespace apex;
using namespace cmn;

class StudyRunnerCreator : public QObject, public PluginRunnerCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginRunnerCreator)
    Q_PLUGIN_METADATA(IID "apex.study")
public:
    virtual QStringList availablePlugins() const;
    virtual PluginRunnerInterface *createRunner(const QString &name) const;
};

class StudyRunner : public QObject, public PluginRunnerInterface
{
    Q_OBJECT
public:
    StudyRunner();
    ~StudyRunner();

    QString getFileName() Q_DECL_OVERRIDE;
    const QString getButtonText() Q_DECL_OVERRIDE;
};

/* StudyRunner=============================================================== */

StudyRunner::StudyRunner() : PluginRunnerInterface("Study")
{
}

StudyRunner::~StudyRunner()
{
}

QString StudyRunner::getFileName()
{
    StudyManager *manager = StudyManager::instance();
    if (!manager->activeStudy()) {
        manager->showConfigurationDialog();
        if (!manager->activeStudy())
            return QString();
    }

    manager->activeStudy()->checkoutExperiments(true);
    manager->activeStudy()->fetchExperiments();
    QString index = manager->activeStudy()->indexExperiment();
    if (!index.isEmpty())
        return index;

    ListPicker listPicker(QL1S("Selected an experiment from study ") +
                          manager->activeStudy()->name() + QL1S(":"));
    Q_FOREACH (const QString &experiment,
               manager->activeStudy()->experiments()) {
        /* PluginRunner will try to parse the file as an experiment */
        if (experiment.endsWith(QL1S(".apf")))
            continue;
        QString relative = experiment;
        relative.remove(manager->activeStudy()->experimentsPath());
        listPicker.addItem(relative, QVariant(experiment));
    }
    ApexTools::expandWidgetToWindow(&listPicker);

    if (listPicker.exec() == QDialog::Accepted)
        return listPicker.selectedData().toString();
    return QString();
}

const QString StudyRunner::getButtonText()
{
    return QSL("Study");
}

/* StudyRunnerCreator======================================================== */

QStringList StudyRunnerCreator::availablePlugins() const
{
    return QStringList() << "study";
}

PluginRunnerInterface *
StudyRunnerCreator::createRunner(const QString &name) const
{
    try {
        if (name == "study")
            return new StudyRunner;
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "studyrunner.moc"
