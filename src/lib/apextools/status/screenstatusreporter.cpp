/*****************************************************************************
* Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "screenstatusreporter.h"
#include "statusitem.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QShortcut>
#include <QTreeWidget>

using namespace apex;

static QColor sf_SetColor(StatusItem::Level level)
{
    switch (level) {
    case StatusItem::Fatal:
    case StatusItem::Critical:
        return Qt::red;
    case StatusItem::Warning:
        return QColor(0, 0x80, 0); // less flashy than Qt::green
    case StatusItem::Info:
    case StatusItem::Debug:
        return Qt::gray;
    default:
        return Qt::black;
    }
}

ScreenStatusReporter::ScreenStatusReporter() :
    reportLevels(StatusItem::AllButDebug),
    list(new QTreeWidget(this))
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_QuitOnClose, false);

    list->setColumnCount(3);
    list->setHeaderLabels(QStringList() << tr("Time") << tr("Source") << tr("Message"));
    list->setUniformRowHeights(false);
    list->setItemsExpandable(false);
    list->setIndentation(0);
    list->setRootIsDecorated(false);
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    list->header()->resizeSection(0, 70);
    list->header()->resizeSection(1, 250);
    list->header()->resizeSection(2, 470);

    QAction* copyAction = new QAction(tr("Copy all messages to clipboard"), this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyAllToClipboard()));
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
    connect(shortcut, SIGNAL( activated()), this, SLOT(copyAllToClipboard()));

    QAction* copyThisAction = new QAction(tr("Copy selected messages to clipboard"), this);
    connect(copyThisAction, SIGNAL(triggered()), this, SLOT(copySelectedToClipboard()));

    QAction* clearAction = new QAction(tr("Clear message window"), this);
    connect(clearAction, SIGNAL(triggered()), list, SLOT(clear()));

    QAction* hideWindowAction = new QAction(tr("Hide"), this);
    connect(hideWindowAction, SIGNAL(triggered()), this, SLOT(hide()));

    QMenu* levelsMenu = new QMenu("Reporting Levels", this);
    warningsAction = new QAction(tr("Warnings"), this);
    warningsAction->setCheckable(true);
    warningsAction->setChecked((reportLevels & StatusItem::Warning) > 0);
    levelsMenu->addAction(warningsAction);
    connect(warningsAction, SIGNAL(triggered(bool)), this, SLOT(updateLevels()));

    infoMessagesAction = new QAction(tr("Info messages"), this);
    infoMessagesAction->setCheckable(true);
    infoMessagesAction->setChecked((reportLevels & StatusItem::Info) > 0);
    levelsMenu->addAction(infoMessagesAction);
    connect(infoMessagesAction, SIGNAL(triggered(bool)), this, SLOT(updateLevels()));

    debugMessagesAction = new QAction(tr("Debug messages"), this);
    debugMessagesAction->setCheckable(true);
    debugMessagesAction->setChecked((reportLevels & StatusItem::Debug) > 0);
    levelsMenu->addAction(debugMessagesAction);
    connect(debugMessagesAction, SIGNAL(triggered(bool)), this, SLOT(updateLevels()));

    QMenu *fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addMenu(levelsMenu);
    fileMenu->addAction(copyAction);
    fileMenu->addAction(copyThisAction);
    fileMenu->addAction(clearAction);
    fileMenu->addSeparator();
    fileMenu->addAction(hideWindowAction);
    menuBar()->addMenu(fileMenu);

    setCentralWidget(list);
    resize(QSize(800, 400));
    setWindowTitle(tr("Messages - APEX"));
    show();
}

ScreenStatusReporter::~ScreenStatusReporter()
{
}

void ScreenStatusReporter::addItem(const StatusItem &item)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(QStringList()
            << item.dateTime().time().toString()
            << item.source()
            << item.message());
    for (unsigned i = 0; i < 3; ++i) {
        treeItem->setForeground(i, sf_SetColor(item.level()));
        treeItem->setTextAlignment(i, Qt::AlignTop);
    }
    treeItem->setData(0, Qt::UserRole, (int) item.level());
    treeItem->setData(0, Qt::UserRole + 1, item.dateTime().time().toString() +
            "\t" + item.source() + "\t" + item.message());

    list->addTopLevelItem(treeItem);
    treeItem->setHidden((reportLevels & item.level()) == 0);

    list->scrollToBottom();
    list->resizeColumnToContents(2);
    if (item.level() >= StatusItem::Critical)
        raise();
}

void ScreenStatusReporter::copyAllToClipboard()
{
    QStringList result;
    for (int i = 0; i< list->topLevelItemCount(); ++i)
        result << list->topLevelItem(i)->data(0, Qt::UserRole + 1).toString();
    QApplication::clipboard()->setText(result.join(QL1S("\n")));
}

void ScreenStatusReporter::copySelectedToClipboard()
{
    QStringList result;
    Q_FOREACH (const QTreeWidgetItem *item, list->selectedItems())
        result << item->data(0, Qt::UserRole + 1).toString();
    QApplication::clipboard()->setText(result.join(QL1S("\n")));
}

void ScreenStatusReporter::updateLevels()
{
    reportLevels = StatusItem::Fatal | StatusItem::Critical;

    if (warningsAction->isChecked())
        reportLevels |= StatusItem::Warning;
    if (infoMessagesAction->isChecked())
        reportLevels |= StatusItem::Info;
    if (debugMessagesAction->isChecked())
        reportLevels |= StatusItem::Debug;

    for (int i = 0; i< list->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = list->topLevelItem(i);
        StatusItem::Level level = (StatusItem::Level) item->data(0, Qt::UserRole).toInt();
        item->setHidden((reportLevels & level) == 0);
    }
}

void ScreenStatusReporter::hideEvent(QHideEvent *)
{
    Q_EMIT visibilityChanged(false);
}

void ScreenStatusReporter::showEvent(QShowEvent *)
{
    Q_EMIT visibilityChanged(true);
}
