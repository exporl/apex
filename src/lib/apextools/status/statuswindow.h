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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSWINDOW_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_STATUS_STATUSWINDOW_H_

#include <status/statusitem.h>

#include <QDateTime>
#include <QString>
#include <QFrame>

class QPainter;
class QHBoxLayout;
class QTreeView;

namespace apex
{
class StatusItem;
class StatusReporter;

/**
* ApexMessageWindow
*   displays list of messages
***************************** */
class APEXTOOLS_EXPORT StatusWindow : public QFrame
{
        Q_OBJECT

    public:

        StatusWindow(StatusItem::Levels reportLevels);
        ~StatusWindow();

        const QTreeView* treeView() const;

    public slots:

        void clearList();
        void addItem( const StatusItem* const a_cpItem );

        void copyAllToClipboard();
        void copySelectedToClipboard();

    signals:

        void levelsChanged(StatusItem::Levels);

    protected:
        /**
        * Override to control closing.
        */
        void closeEvent( QCloseEvent* );

    private slots:

        void updateLevels();

    private:

        class Model;

        Model*        m_Model;
        QTreeView*    m_List;
        QHBoxLayout*  m_Layout;
        QAction*      errorsAction;
        QAction*      warningsAction;
        QAction*      messagesAction;
};

}

#endif

