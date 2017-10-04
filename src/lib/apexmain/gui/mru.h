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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_MRU_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_MRU_H_

#include <QAction>
#include <QObject>

class QString;
class QMenu;

namespace apex
{
/**
      * Derived QAction that emits Activated with it's text
      ***************************************************** */
class MRUAction : public QAction
{
    Q_OBJECT
public:
    MRUAction(QObject *a_pParent, const QString &ac_sText) : QAction(a_pParent)
    {
        QAction::setText(ac_sText);
        connect(this, SIGNAL(triggered()), this, SLOT(Activated()));
    }
    virtual ~MRUAction()
    {
    }

signals:
    void Activated(const QString &);

private slots:
    virtual void Activated()
    {
        Q_EMIT Activated(QAction::text());
    }
};

/**
      * Class with simple MRU functionality
      *   inserts seperator and strings added to menu
      *   emits text of item clicked
      *********************************************** */
class MRU : public QObject
{
    Q_OBJECT
public:
    MRU(QMenu *const menuToMRU);
    ~MRU();

    void loadFromFile();

    // get/set the open dir (doesn't affect the menu but is handy to store in
    // file)
    void setOpenDir(const QString &file);
    const QString &openDir() const;

    void enable(const bool enable);

    QStringList items(bool filterNonExistingFiles = true) const;

public slots:
    void addAndSave(const QString &item, const bool enable = true);

signals:
    void Released(const QString &fileName);

private:
    typedef QVector<MRUAction *> MRUvector;

    QMenu *const m_menu;
    MRUvector *const m_items;
    QString m_openDirName;

    void addItem(const QString &item, const bool enable = true);
    void saveToFile();
    void removeAllItems();
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_MRU_H_
