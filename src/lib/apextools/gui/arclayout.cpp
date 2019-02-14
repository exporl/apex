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

#include "arclayout.h"

#include <cmath>

#include <QWidget>

#define ARC_PI 3.14159265358979323846

class ArcLayoutPrivate
{
public:
    ArcLayoutPrivate(ArcLayout::ArcType arcType);

    int doLayout(const QRect &rect, bool testOnly) const;

    QList<QLayoutItem *> items;
    ArcLayout::ArcType arcType;
    ArcLayout::ArcLayoutFlags arcLayoutFlags;
};

// ArcLayoutPrivate ============================================================

ArcLayoutPrivate::ArcLayoutPrivate(ArcLayout::ArcType arcType)
    : arcType(arcType), arcLayoutFlags(0)
{
}

// activate == false for height for width calculation
int ArcLayoutPrivate::doLayout(const QRect &rect, bool activate) const
{
    int x = rect.x();
    int y = rect.y();

    if (!items.count())
        return 0;

    const double arc = items.count() < 3
                           ? ARC_PI
                           : arcType == ArcLayout::ARC_FULL
                                 ? 2 * ARC_PI / (items.count() - 1)
                                 : ARC_PI / (items.count() - 2);

    QSize minimumSize;
    Q_FOREACH (QLayoutItem *item, items)
        minimumSize = minimumSize.expandedTo(item->minimumSize());
    const double ratio =
        arcLayoutFlags & ArcLayout::ARC_RECTANGULAR
            ? double(minimumSize.width()) / minimumSize.height()
            : 1.0;

    const double radiusFromWidth = qMax(
        rect.width() /
            ((arcType & (ArcLayout::ARC_LEFT | ArcLayout::ARC_RIGHT) ? 1 : 2) +
             2 / sqrt(1 + 1 / (ratio * ratio)) * sin(arc / 2)),
        rect.width() /
            ((arcType & (ArcLayout::ARC_LEFT | ArcLayout::ARC_RIGHT) ? 1 : 2) +
             1 / sqrt(1 + 1 / (ratio * ratio))));

    const double radiusFromHeight = qMax(
        rect.height() /
            ((arcType & (ArcLayout::ARC_TOP | ArcLayout::ARC_BOTTOM) ? 1 : 2) +
             2 / sqrt(1 + (ratio * ratio)) * sin(arc / 2)),
        rect.height() /
            ((arcType & (ArcLayout::ARC_TOP | ArcLayout::ARC_BOTTOM) ? 1 : 2) +
             1 / sqrt(1 + (ratio * ratio))));

    unsigned radius;
    unsigned height;
    unsigned width;
    if (radiusFromHeight < radiusFromWidth && activate) {
        radius = unsigned(ceil(radiusFromHeight));
        height =
            rect.height() -
            (arcType & (ArcLayout::ARC_TOP | ArcLayout::ARC_BOTTOM) ? 1 : 2) *
                radius;
        width = unsigned(ratio * height);
    } else {
        radius = unsigned(ceil(radiusFromWidth));
        width =
            rect.width() -
            (arcType & (ArcLayout::ARC_LEFT | ArcLayout::ARC_RIGHT) ? 1 : 2) *
                radius;
        height = unsigned(width / ratio);
    }
    const unsigned centerWidth =
        unsigned(2 * radius / sqrt(1 + 1 / (ratio * ratio)) - width);
    const unsigned centerHeight =
        unsigned(2 * radius / sqrt(1 + (ratio * ratio)) - height);

    if (!activate)
        return radius * (arcType & (ArcLayout::ARC_TOP | ArcLayout::ARC_BOTTOM)
                             ? 1
                             : 2) +
               height;

    unsigned minimumArc = 0;
    switch (arcType) {
    case ArcLayout::ARC_RIGHT:
        ++minimumArc;
    /* Falls through. */
    case ArcLayout::ARC_TOP:
        ++minimumArc;
    /* Falls through. */
    case ArcLayout::ARC_FULL:
    case ArcLayout::ARC_LEFT:
        ++minimumArc;
    case ArcLayout::ARC_BOTTOM:
        break;
    }

    for (int i = 0; i < items.count(); ++i) {
        QLayoutItem *item = items[i];

        QRect r;

        if (i == 0 && arcLayoutFlags & ArcLayout::ARC_IRREGULAR_CENTER) {
            r = QRect((rect.width() - centerWidth) / 2,
                      (rect.height() - centerHeight) / 2, centerWidth,
                      centerHeight);
        } else {
            r = QRect((rect.width() - width) / 2, (rect.height() - height) / 2,
                      width, height);
        }
        r.translate(x, y);
        if (arcType != ArcLayout::ARC_FULL)
            r.translate(
                QPoint(qRound(radius / 2.0 * sin(minimumArc / 2.0 * ARC_PI)),
                       qRound(radius / 2.0 * -cos(minimumArc / 2.0 * ARC_PI))));
        if (i == 0) {
            // do nothing
        } else if (items.count() == 2) {
            r.translate(
                QPoint(qRound(radius *
                              cos(((arcType == ArcLayout::ARC_FULL ? 1 : 0.5) +
                                   minimumArc / 2.0) *
                                  ARC_PI)),
                       qRound(radius *
                              sin(((arcType == ArcLayout::ARC_FULL ? 1 : 0.5) +
                                   minimumArc / 2.0) *
                                  ARC_PI))));
        } else {
            r.translate(QPoint(
                qRound(radius *
                       cos(((i - 1) /
                                (items.count() -
                                 (arcType == ArcLayout::ARC_FULL ? 1.0 : 2.0)) +
                            minimumArc / 2.0) *
                           (arcType == ArcLayout::ARC_FULL ? 2 : 1) * ARC_PI)),
                qRound(radius *
                       sin(((i - 1) /
                                (items.count() -
                                 (arcType == ArcLayout::ARC_FULL ? 1.0 : 2.0)) +
                            minimumArc / 2.0) *
                           (arcType == ArcLayout::ARC_FULL ? 2 : 1) *
                           ARC_PI))));
        }
        item->setGeometry(r);
    }

    return -1;
}

// ArcLayout ===================================================================

ArcLayout::ArcLayout(ArcType arcType, QWidget *parent)
    : QLayout(parent), d(new ArcLayoutPrivate(arcType))
{
}

ArcLayout::~ArcLayout()
{
    qDeleteAll(d->items);
}

void ArcLayout::addItem(QLayoutItem *item)
{
    d->items.append(item);
}

void ArcLayout::insertItem(int index, QLayoutItem *item)
{
    // append
    if (index < 0)
        index = d->items.count();

    d->items.insert(index, item);
}

void ArcLayout::insertWidget(int index, QWidget *widget)
{
    // append
    if (index < 0)
        index = d->items.count();

    d->items.insert(index, new QWidgetItem(widget));
}

int ArcLayout::count() const
{
    return d->items.count();
}

QLayoutItem *ArcLayout::itemAt(int index) const
{
    if (index < 0 || index >= d->items.count())
        return NULL;

    return d->items.at(index);
}

QLayoutItem *ArcLayout::takeAt(int index)
{
    if (index < 0 || index >= d->items.count())
        return NULL;

    return d->items.takeAt(index);
}

Qt::Orientations ArcLayout::expandingDirections() const
{
    return 0;
}

bool ArcLayout::hasHeightForWidth() const
{
    return true;
}

int ArcLayout::heightForWidth(int width) const
{
    return d->doLayout(QRect(0, 0, width, 0), false);
}

void ArcLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    d->doLayout(rect, true);
}

QSize ArcLayout::sizeHint() const
{
    return minimumSize();
}

QSize ArcLayout::minimumSize() const
{
    if (!d->items.count())
        return QSize(0, 0);

    const double arc = d->items.count() < 3
                           ? ARC_PI
                           : d->arcType == ARC_FULL
                                 ? 2 * ARC_PI / (d->items.count() - 1)
                                 : ARC_PI / (d->items.count() - 2);

    QSize minimumSize;
    Q_FOREACH (QLayoutItem *item, d->items)
        minimumSize = minimumSize.expandedTo(item->minimumSize());
    if ((d->arcLayoutFlags & ArcLayout::ARC_RECTANGULAR) == 0)
        minimumSize = minimumSize.expandedTo(
            QSize(minimumSize.height(), minimumSize.width()));
    const double ratio = double(minimumSize.width()) / minimumSize.height();

    const double minRadiusFromWidth = qMax(
        minimumSize.width() * sqrt(1 + 1 / (ratio * ratio)) / sin(arc / 2) / 2,
        minimumSize.width() * sqrt(1 + 1 / (ratio * ratio)));
    const double minRadiusFromHeight = qMax(
        minimumSize.height() * sqrt(1 + (ratio * ratio)) / sin(arc / 2) / 2,
        minimumSize.height() * sqrt(1 + (ratio * ratio)));
    const unsigned minRadius =
        unsigned(ceil(qMax(minRadiusFromHeight, minRadiusFromWidth)));

    return QSize(minRadius * (d->arcType & (ARC_LEFT | ARC_RIGHT) ? 1 : 2) +
                     minimumSize.width(),
                 minRadius * (d->arcType & (ARC_TOP | ARC_BOTTOM) ? 1 : 2) +
                     minimumSize.height());
}

void ArcLayout::setArcType(ArcType arcType)
{
    d->arcType = arcType;
    invalidate();
}

ArcLayout::ArcType ArcLayout::arcType() const
{
    return d->arcType;
}

void ArcLayout::setArcLayoutFlags(ArcLayoutFlags arcLayoutFlags)
{
    d->arcLayoutFlags = arcLayoutFlags;
    invalidate();
}

ArcLayout::ArcLayoutFlags ArcLayout::arcLayoutFlags() const
{
    return d->arcLayoutFlags;
}
