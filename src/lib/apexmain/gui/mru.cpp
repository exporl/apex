/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "streamapp/appcore/deleter.h"

#include "mru.h"

#include "apextools/settingsfactory.h"

#include <QDebug>
#include <QFileInfo>
#include <QMenu>
#include <QSettings>
#include <QStringList>

#include <algorithm>

namespace
{
const QString recentFilesGroup("recentFiles");
const QString fileKey("file");
const QString recentDirGroup("recentDirs");
const QString dirKey("dir");
const size_t sc_nMaxItems = 10;
}

namespace apex
{
MRU::MRU(QMenu *const ac_pMenuToMru)
    : m_menu(ac_pMenuToMru), m_items(new MRUvector())
{
}

MRU::~MRU()
{
    removeAllItems();
    delete m_items;
}

void MRU::loadFromFile()
{
    QScopedPointer<QSettings> settings(SettingsFactory().createSettings());

    int size = settings->beginReadArray(recentFilesGroup);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        addItem(settings->value(fileKey).toString());
    }
    settings->endArray();

    settings->beginGroup(recentDirGroup);
    setOpenDir(settings->value(dirKey).toString());
    settings->endGroup();
}

void MRU::saveToFile()
{
    QScopedPointer<QSettings> settings(SettingsFactory().createSettings());

    settings->beginWriteArray(recentFilesGroup);
    for (int i = 0; i < m_items->size(); ++i) {
        settings->setArrayIndex(i);
        settings->setValue(fileKey, m_items->at(i)->text());
    }
    settings->endArray();

    settings->beginGroup(recentDirGroup);
    settings->setValue(dirKey, m_openDirName);
    settings->endGroup();

    settings->sync();
}

void MRU::setOpenDir(const QString &dirName)
{
    m_openDirName = dirName;
}

const QString &MRU::openDir() const
{
    //    qCDebug(APEX_RS, "MRU::mf_sGetOpenDir");
    return m_openDirName;
}

void MRU::addAndSave(const QString &item, const bool enable)
{
    addItem(item, enable);
    setOpenDir(QFileInfo(item).absolutePath());
    saveToFile();
}

void MRU::addItem(const QString &item, const bool enable)
{
    const size_t size = m_items->size();
    if (!size)
        m_menu->addSeparator();
    else {
        // check duplicate
        MRUvector::iterator it = m_items->begin();
        while (it != m_items->end()) {
            if ((*it)->text() == item) {
                std::iter_swap(it, (m_items->end() - 1));
                return;
            }
            ++it;
        }
    }
    if (!QFileInfo::exists(item))
        return;
    MRUAction *p = new MRUAction(m_menu, item);
    if (size + 1 >= sc_nMaxItems) {
        MRUvector::iterator it = m_items->begin();
        m_menu->removeAction(*it);
        m_items->erase(it);
    }
    m_items->push_back(p);
    m_menu->addAction(p);
    p->setEnabled(enable);
    connect(p, SIGNAL(Activated(const QString &)), this,
            SIGNAL(Released(const QString &)));
}

void MRU::removeAllItems()
{
    disconnect();
    std::for_each(m_items->begin(), m_items->end(), appcore::Deleter());
    m_items->clear();
}

void MRU::enable(const bool enable)
{
    MRUvector::const_iterator it = m_items->begin();
    while (it != m_items->end()) {
        (*it)->setEnabled(enable);
        ++it;
    }
}

QStringList MRU::items(bool filterNonExistingFiles) const
{
    QStringList result;
    Q_FOREACH (MRUAction *action, *m_items) {
        if (!filterNonExistingFiles || QFileInfo(action->text()).exists())
            result << action->text();
    }
    return result;
}
}
