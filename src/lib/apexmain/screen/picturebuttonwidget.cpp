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

#include "picturebuttonwidget.h"

#include <QPushButton>
#include <QPaintEvent>
#include <QPixmap>
#include <QPainter>
#include <QPoint>



void PictureButtonWidget::setPixmap(QPixmap p) {
        pixmap=p;
}

void PictureButtonWidget::setBorderSize(int s) {
        bordersize=s;
}


PictureButtonWidget::PictureButtonWidget(QWidget* parent):
        QPushButton(parent),
        bordersize(0)
{
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void PictureButtonWidget::paintEvent(QPaintEvent* e)
{
        QPushButton::paintEvent(e);
        QPainter painter(this);
        QRect rect(QPoint(), size());
        rect.adjust(bordersize,bordersize,-bordersize,-bordersize);
        painter.drawPixmap( rect, pixmap, pixmap.rect() );
}


void PictureButtonWidget::mousePressEvent( QMouseEvent* ev )
{
    QPushButton::mousePressEvent(ev);

    const QPoint pi = ev->pos();
    QPointF pf;
    pf.setX( ((qreal) pi.x()) / this->width() );
    pf.setY( ((qreal) pi.y()) / this->height() );
    emit mousePressed( pf );

    //qCDebug(APEX_RS, "Clicked PictureButtonWidget at (%f, %f)", pf.x(), pf.y());
}
