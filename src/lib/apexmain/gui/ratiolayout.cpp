/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "ratiolayout.h"

#include <cmath>

#include <QWidget>

class RatioLayoutPrivate
{
public:
    RatioLayoutPrivate(double ratio);

    int doLayout(const QRect &rect, bool testOnly) const;

    QList<QLayoutItem *> items;
    double ratio;
};

// RatioLayoutPrivate
// ============================================================

RatioLayoutPrivate::RatioLayoutPrivate(double ratio) : ratio(ratio)
{
}

// activate == false for height for width calculation
int RatioLayoutPrivate::doLayout(const QRect &rect, bool activate) const
{
    int x = rect.x();
    int y = rect.y();

    if (!items.count())
        return 0;
    QLayoutItem *const item = items.first();

    double rectRatio = double(rect.height()) / rect.width();

    int newHeight = rect.height();
    int newWidth = rect.width();

    if (rectRatio > ratio)
        newHeight = int(ratio * rect.width());
    else
        newWidth = int(double(rect.height()) / ratio);

    if (!activate)
        return newHeight;

    item->setGeometry(QRect(x + (rect.width() - newWidth) / 2,
                            y + (rect.height() - newHeight) / 2, newWidth,
                            newHeight));

    return -1;
}

// RatioLayout
// ===================================================================

RatioLayout::RatioLayout(double ratio, QWidget *parent)
    : QLayout(parent), d(new RatioLayoutPrivate(ratio))
{
}

RatioLayout::~RatioLayout()
{
    qDeleteAll(d->items);
}

void RatioLayout::addItem(QLayoutItem *item)
{
    d->items.append(item);
}

void RatioLayout::insertItem(int index, QLayoutItem *item)
{
    // append
    if (index < 0)
        index = d->items.count();

    d->items.insert(index, item);
}

void RatioLayout::insertWidget(int index, QWidget *widget)
{
    // append
    if (index < 0)
        index = d->items.count();

    d->items.insert(index, new QWidgetItem(widget));
}

int RatioLayout::count() const
{
    return d->items.count();
}

QLayoutItem *RatioLayout::itemAt(int index) const
{
    if (index < 0 || index >= d->items.count())
        return NULL;

    return d->items.at(index);
}

QLayoutItem *RatioLayout::takeAt(int index)
{
    if (index < 0 || index >= d->items.count())
        return NULL;

    return d->items.takeAt(index);
}

Qt::Orientations RatioLayout::expandingDirections() const
{
    return 0;
}

bool RatioLayout::hasHeightForWidth() const
{
    return true;
}

int RatioLayout::heightForWidth(int width) const
{
    return d->doLayout(QRect(0, 0, width, 0), false);
}

void RatioLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    d->doLayout(rect, true);
}

QSize RatioLayout::sizeHint() const
{
    return minimumSize();
}

QSize RatioLayout::minimumSize() const
{
    // remove the || true if you want to get a sane size
    if (!d->items.count() || true)
        return QSize(0, 0);

    QLayoutItem *const item = d->items.first();

    const int width = int(item->minimumSize().width());
    const int height = int(d->ratio * width);

    return QSize(width, height);
}

void RatioLayout::setRatio(double ratio)
{
    d->ratio = ratio;
    invalidate();
}

double RatioLayout::ratio() const
{
    return d->ratio;
}
