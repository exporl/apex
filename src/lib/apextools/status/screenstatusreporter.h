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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_STATUS_SCREENSTATUSREPORTER_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_STATUS_SCREENSTATUSREPORTER_H_

#include "../global.h"

#include "statusitem.h"

#include <QMainWindow>
#include <QTimer>

class QCloseEvent;
class QTreeWidget;

namespace apex
{

class APEXTOOLS_EXPORT ScreenStatusReporter : public QMainWindow
{
    Q_OBJECT
public:
    ScreenStatusReporter();
    virtual ~ScreenStatusReporter();

public Q_SLOTS:
    void addItem(const apex::StatusItem &item);

private Q_SLOTS:
    void updateLevels();
    void copyAllToClipboard();
    void copySelectedToClipboard();

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void visibilityChanged(bool visible);

private:
    StatusItem::Levels reportLevels;
    QTreeWidget *list;
    QAction *warningsAction;
    QAction *infoMessagesAction;
    QAction *debugMessagesAction;
    int longestStatusItem;
};
}

#endif
