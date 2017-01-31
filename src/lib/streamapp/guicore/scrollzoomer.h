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
 
#ifndef _SCROLLZOOMER_H
#define _SCROLLZOOMER_H

#include <qglobal.h>
#if QT_VERSION < 0x040000
#include <qscrollview.h>
#endif
#include <qwt_plot_zoomer.h>

class ScrollData;
class ScrollBar;

class ScrollZoomer: public QwtPlotZoomer
{
    Q_OBJECT
public:
    enum ScrollBarPosition
    {
        AttachedToScale,
        OppositeToScale
    };

    ScrollZoomer(QwtPlotCanvas *);
    virtual ~ScrollZoomer();

  signals:
    void ms_Rescaled();

public:
    ScrollBar *horizontalScrollBar() const;
    ScrollBar *verticalScrollBar() const;

#if QT_VERSION < 0x040000
    void setHScrollBarMode(QScrollView::ScrollBarMode);
    void setVScrollBarMode(QScrollView::ScrollBarMode);

    QScrollView::ScrollBarMode vScrollBarMode () const;
    QScrollView::ScrollBarMode hScrollBarMode () const;
#else
    void setHScrollBarMode(Qt::ScrollBarPolicy);
    void setVScrollBarMode(Qt::ScrollBarPolicy);

    Qt::ScrollBarPolicy vScrollBarMode () const;
    Qt::ScrollBarPolicy hScrollBarMode () const;
#endif

    void setHScrollBarPosition(ScrollBarPosition);
    void setVScrollBarPosition(ScrollBarPosition);

    ScrollBarPosition hScrollBarPosition() const;
    ScrollBarPosition vScrollBarPosition() const;

    QWidget* cornerWidget() const;
    virtual void setCornerWidget(QWidget *); 
    
    virtual bool eventFilter(QObject *, QEvent *);

    virtual void rescale();

    void mp_DoProperResize( QResizeEvent* e );
    QSize mf_CanvasSize() const;

protected:
    virtual ScrollBar *scrollBar(Qt::Orientation);
    virtual void updateScrollBars();
    virtual void layoutScrollBars(const QRect &);

private slots:
    void scrollBarMoved(Qt::Orientation o, double min, double max);

private:
    bool needScrollBar(Qt::Orientation) const;
    int oppositeAxis(int) const;

    QWidget *d_cornerWidget;

    ScrollData *d_hScrollData;
    ScrollData *d_vScrollData;

    bool d_inZoom;
};
            
#endif
