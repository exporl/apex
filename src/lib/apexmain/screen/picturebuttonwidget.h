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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_PICTUREBUTTONWIDGET_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_PICTUREBUTTONWIDGET_H_

#include "apextools/global.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QPointF>
#include <QPushButton>

class APEX_EXPORT PictureButtonWidget : public QPushButton
{
    Q_OBJECT

public:
    PictureButtonWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *ev);
    void setPixmap(QPixmap p);
    void setBorderSize(int s);

signals:
    // QPointF are the relative coordinates of the point where the user clicked
    void mousePressed(const QPointF &);

private:
    QPixmap pixmap;
    int bordersize;
};

#endif
