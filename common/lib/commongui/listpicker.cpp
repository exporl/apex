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

#include "listpicker.h"
#include "ui_listpicker.h"

#include <QScrollBar>
#include <QScroller>

namespace cmn
{
class ListPickerPrivate : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void itemClicked(QListWidgetItem *item)
    {
        result = item->text();
        resultData = item->data(Qt::UserRole).toString();
#ifdef Q_OS_ANDROID
        p->accept();
#endif
    }
    void itemDoubleClicked(QListWidgetItem *item)
    {
        Q_UNUSED(item);
        p->accept();
    }
    void selectButtonClicked()
    {
        p->accept();
    }

public:
    ListPicker *p;
    Ui::ListPicker ui;
    QString result;
    QVariant resultData;
};

ListPicker::ListPicker(const QString &title) : d(new ListPickerPrivate)
{
    d->p = this;
    d->ui.setupUi(this);
    setWindowTitle(title);
    QScroller::grabGesture(d->ui.listWidget->viewport(),
                           QScroller::LeftMouseButtonGesture);
    d->ui.listWidget->verticalScrollBar()->hide();

#ifdef Q_OS_ANDROID
    d->ui.selectButton->hide();
    d->ui.label->setText(title);
    QSize gridSize = d->ui.listWidget->gridSize();
    d->ui.listWidget->setGridSize(
        QSize(gridSize.width(), gridSize.height() * 2));
    d->ui.listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    d->ui.listWidget->setBackgroundRole(QPalette::Window);
#else
    d->ui.label->hide();
    connect(d->ui.selectButton, SIGNAL(clicked(bool)), d.data(),
            SLOT(selectButtonClicked()));
    connect(d->ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            d.data(), SLOT(itemDoubleClicked(QListWidgetItem *)));
#endif
    connect(d->ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *)), d.data(),
            SLOT(itemClicked(QListWidgetItem *)));
}

ListPicker::~ListPicker()
{
}

void ListPicker::addItem(const QString &item, const QVariant &data)
{
    QListWidgetItem *listWidgetItem = new QListWidgetItem(item);
    listWidgetItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter |
                                     Qt::AlignCenter);
    QFont font(QApplication::font());
#ifdef Q_OS_ANDROID
    font.setPixelSize(font.pixelSize() * 1.5);
#else
    font.setPointSize(font.pointSize() * 1.5);
#endif
    listWidgetItem->setFont(font);
    listWidgetItem->setSizeHint(QSize(listWidgetItem->sizeHint().width(),
                                      d->ui.listWidget->gridSize().height()));
    if (data.isValid())
        listWidgetItem->setData(Qt::UserRole, data);
    d->ui.listWidget->addItem(listWidgetItem);
}

const QString &ListPicker::selectedItem() const
{
    return d->result;
}

const QVariant &ListPicker::selectedData() const
{
    return d->resultData;
}
}

#include "listpicker.moc"
