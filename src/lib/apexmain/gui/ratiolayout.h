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

#ifndef _APEX_GUI_RatioLayout_H_
#define _APEX_GUI_RatioLayout_H_

#include <QLayout>
#include <QList>

#include <memory>

class RatioLayoutPrivate;

/** QLayout that has one child and makes sure it maintains its ratio
 *
 * \todo
 * - take spacing and margins into account
 * - take maximum sizes and size policies into account
 */
class RatioLayout: public QLayout {
public:

    /**
     * ratio = height/width
     */
    RatioLayout (double ratio, QWidget *parent = NULL);
    ~RatioLayout();

    // element 0 is the center element
    void addItem (QLayoutItem *item);
    void insertWidget (int index, QWidget *item);
    void insertItem (int index, QLayoutItem *item);
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth (int width) const;
    int count() const;
    QLayoutItem *itemAt (int index) const;
    QSize minimumSize() const;
    void setGeometry (const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt (int index);

public Q_SLOTS:
    void setRatio (double ratio);
    double ratio() const;

private:
    QScopedPointer<RatioLayoutPrivate> d;
};


#endif
