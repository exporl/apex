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

#include "apexdata/experimentdata.h"

#include "apextools/apexpaths.h"

#include "common/pluginloader.h"

#include "experimentparser.h"
#include "pluginrunner.h"
#include "pluginrunnerinterface.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QPluginLoader>

using namespace apex;
using namespace cmn;

void PluginRunner::selectFromDir(const QString & /*path*/)
{
}

bool PluginRunner::select(const QString &pname)
{
    qCDebug(APEX_RS, "select");
    QMap<QString, PluginRunnerCreator *> allPlugins;

    Q_FOREACH (PluginRunnerCreator *plugin,
               PluginLoader().availablePlugins<PluginRunnerCreator>())
        Q_FOREACH (const QString &name, plugin->availablePlugins())
            allPlugins.insert(name, plugin);

    if (!pname.isEmpty() || allPlugins.count() == 1) {
        qCDebug(APEX_RS, "stuff");
        PluginRunnerCreator *creator = pname.isEmpty()
                                           ? allPlugins.values().at(0)
                                           : allPlugins.value(pname);
        if (!creator) {
            Q_EMIT errorMessage("PluginRunner",
                                tr("Cannot find a plugin named %1").arg(pname));
            return false;
        }
        qCDebug(APEX_RS, "stuff");
        PluginRunnerInterface *interface = creator->createRunner(
            pname.isEmpty() ? allPlugins.keys().at(0) : pname);
        if (!interface) {
            qCDebug(APEX_RS, "blub");
            Q_EMIT errorMessage(
                "PluginRunner",
                tr("Cannot creater plugin interface %1").arg(pname));
            return false;
        }
        qCDebug(APEX_RS, "stuff");
        const QString file = interface->getFileName();
        qCDebug(APEX_RS, "stuff");
        if (!file.isEmpty()) {
            ExperimentParser parser(file);
            Q_EMIT selected(parser.parse(true, true));
        }
        qCDebug(APEX_RS, "stuff");
        return true;
    }

    // show button for each plugin on screen
    // FIXME memory leak but we cannot delete window because after it is fully
    // created, it will own the buttons but they will also be owned by the
    // plugin runner interfaces causing a "double free" of memory. we should:
    //- remove getButton() from the interface (seems unnecessary anyway) and
    //  create the button here with text provided by getButtonText(), or
    //- let the interface create the button with new and pass ownership to the
    //  widget created here, then the comment below can just be uncommented
    QWidget *window = new QWidget();
    // window->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *gl = new QGridLayout(window);
    QButtonGroup *bg = new QButtonGroup(window);
    QMapIterator<QString, PluginRunnerCreator *> i(allPlugins);
    while (i.hasNext()) {
        i.next();
        PluginRunnerInterface *interface = i.value()->createRunner(i.key());
        if (!interface)
            continue;
        interfaces.insert(i.key(), interface);
        QPushButton *btn = interface->getButton();
        bg->addButton(btn);
        gl->addWidget(btn);
    }

    connect(bg, SIGNAL(buttonClicked(QAbstractButton *)), window,
            SLOT(close()));
    connect(bg, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(pluginSelected(QAbstractButton *)));
    window->show();

    return true;
}

void PluginRunner::pluginSelected(QAbstractButton *btn)
{
    QString file;

    QList<PluginRunnerInterface *>::const_iterator it;
    Q_FOREACH (PluginRunnerInterface *i, interfaces) {
        if (i->getButton()->text() == btn->text()) {
            // i->GetButton()->parentWidget()->parentWidget()->hide();
            file = i->getFileName();
            break;
        }
    }

    qCDebug(APEX_RS, "Selecting File %s", qPrintable(file));
    if (!file.isEmpty()) {
        // NOTE [job] added this to load experiment file from spin into apex
        //@tom: please verify this is correct
        ExperimentParser parser(file);
        Q_EMIT selected(parser.parse(true, true));
    }
}

apex::PluginRunner::~PluginRunner()
{
}
