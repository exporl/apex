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
 
#ifndef PICTURERUNDELEGATE_H
#define PICTURERUNDELEGATE_H

#include "screen/pictureelement.h"

#include "screenelementrundelegate.h"
#include "parameterscontainerrundelegate.h"

#include <QLabel>
#include <QPushButton>

class PictureButtonWidget;

namespace apex
{
namespace data
{
class ScreenElement;
class PictureElement;
}

namespace rundelegates
{
using data::PictureElement;
using data::ScreenElement;

/**
 * The PictureRunDelegate class is an implementation of
 * ScreenElementRunDelegate representing a PictureElement.
 *
 * It is implemented as a QWidget containing a RatioLayout containing a QLabel
 */
class PictureRunDelegate :
             public QWidget,
             public ScreenElementRunDelegate
{
    Q_OBJECT

    const PictureElement* element;
public:
    PictureRunDelegate(ExperimentRunDelegate* p_exprd,
                       QWidget* parent, const PictureElement* e);

    virtual ~PictureRunDelegate();

    const ScreenElement* getScreenElement() const;

    QWidget* getWidget();
    void connectSlots(gui::ScreenRunDelegate* d);

    void feedBack(const FeedbackMode& mode);

    // get mouse clicks
    void mouseReleaseEvent ( QMouseEvent * event ) ;
    void setEnabled(const bool e);

signals:
    void answered(ScreenElementRunDelegate*);
    void released();

public slots:
     void newStimulus( stimulus::Stimulus* );
private slots:
    void sendAnsweredSignal( const QPointF& );
private:
    //QLabel* m_label;
    PictureButtonWidget* m_button;




    const QPixmap* m_pPixMap;
    const QPixmap* m_pDisabled;
    const QPixmap* m_pHighLight;
    const QPixmap* m_pPositive;
    const QPixmap* m_pNegative;



protected:
    void clicked();
};
}
}

#endif
