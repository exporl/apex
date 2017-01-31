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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_GUI_ARCLAYOUT_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_GUI_ARCLAYOUT_H_

#include "../global.h"
#include <QLayout>
#include <QList>
#include <memory>

class ArcLayoutPrivate;

/** QLayout that arranges its children in an arc.
 *
 * \todo
 * - take spacing and margins into account
 * - take maximum sizes and size policies into account
 */
class APEXTOOLS_EXPORT ArcLayout: public QLayout {
    Q_PROPERTY (ArcType arcType READ arcType WRITE setArcType)
    Q_PROPERTY (ArcLayoutFlags arcLayoutFlags READ arcLayoutFlags
            WRITE setArcLayoutFlags)
    Q_ENUMS (ArcType)
    Q_FLAGS (ArcLayoutFlags)
public:
    enum ArcType {
        ARC_TOP    = 0x0001,
        ARC_BOTTOM = 0x0002,
        ARC_LEFT   = 0x0004,
        ARC_RIGHT  = 0x0008,
        ARC_FULL   = 0x0010
    };

    enum ArcLayoutFlag {
        ARC_RECTANGULAR      = 0x0001,
        ARC_IRREGULAR_CENTER = 0x0002
    };
    Q_DECLARE_FLAGS (ArcLayoutFlags, ArcLayoutFlag)

    ArcLayout (ArcType arcType, QWidget *parent = NULL);
    ~ArcLayout();

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
    void setArcType (ArcType arcType);
    ArcType arcType() const;

    void setArcLayoutFlags (ArcLayoutFlags arcLayoutFlags);
    ArcLayoutFlags arcLayoutFlags() const;

private:
    QScopedPointer<ArcLayoutPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS (ArcLayout::ArcLayoutFlags)

#endif
