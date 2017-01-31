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

#include "apexdata/screen/screenelement.h"

#include "apextools/gui/arclayout.h"

#include "screenelementrundelegate.h"

#include <QGridLayout>
#include <QWidget>

#include <iostream>

using namespace std;

namespace apex
{
namespace rundelegates
{


bool ScreenElementRunDelegate::hasText() const
{
    return false;
}

bool ScreenElementRunDelegate::hasInterestingText() const
{
    return false;
}

const QString ScreenElementRunDelegate::getText() const
{
    return QString();
}

ScreenElementRunDelegate::ScreenElementRunDelegate(ExperimentRunDelegate* p_parent,const data::ScreenElement* const d) :
        feedBackMode(NoFeedback),
        m_rd(p_parent),
        lastClickPosition(-1, -1),
        element(d)
{


}

QString ScreenElementRunDelegate::getID() const
{
    return getScreenElement()->getID();
}

void ScreenElementRunDelegate::connectSlots(
    gui::ScreenRunDelegate* /*d*/)
{
    // default does nothing, a lot of elements don't have relevant
    // signals/slots
}



const QPointF ScreenElementRunDelegate::getClickPosition ( ) const
{
    return lastClickPosition;
}

ScreenElementRunDelegate::FeedbackMode ScreenElementRunDelegate::getFeedBackMode() const
{
    return feedBackMode;
}

void ScreenElementRunDelegate::clearText()
{
    // default does nothing, most elements don't have text that can be
    // cleared..
}

ScreenElementRunDelegate::~ScreenElementRunDelegate()
{
    //qCDebug(APEX_RS, "~ %s", qPrintable(element->getID()));
}

void ScreenElementRunDelegate::feedBack(const FeedbackMode& mode)
{
    feedBackMode = mode;
}

void ScreenElementRunDelegate::setEnabled(const bool e)
{
    bool value = e;
    if (element->getDisabled())     // never enable if disabled==true
        value=false;
    if (getWidget()) {
        getWidget()->setDisabled(!value);
        setCommonProperties(getWidget());
    }
}

void ScreenElementRunDelegate::addToGridLayout(QGridLayout* gl)
{
    QLayout* layout = getLayout();
    if (layout)
    {
//        qCDebug(APEX_RS, "Adding layout to grid layout at %d,%d", getScreenElement()->getY(),
//               getScreenElement()->getX());

        gl->addLayout(layout, getScreenElement()->getY(),
                      getScreenElement()->getX());


    }
    else
    {
        // a non-layout element..

//        qCDebug(APEX_RS, "Adding element %s to grid layout at %d,%d", qPrintable(element->getID()), getScreenElement()->getY(),
//               getScreenElement()->getX());

        gl->addWidget(getWidget(), getScreenElement()->getY(),
                      getScreenElement()->getX());

        getWidget()->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    }


}

QLayout* ScreenElementRunDelegate::getLayout()
{
    return 0;
}

void ScreenElementRunDelegate::addToArcLayout(ArcLayout* al)
{
    int x = getScreenElement()->getX()+1;

    delete al->takeAt(x);      // remove dummy widget

    QLayout* layout = getLayout();
    if (layout)
    {

        al->insertItem(x, layout);
    }
    else
    {
        //getWidget()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        al->insertWidget(x, getWidget());
    }
}


void ScreenElementRunDelegate::setBgColor(QWidget* target, const QString& color) {
//    qCDebug(APEX_RS, "ScreenElementRunDelegate::setBgColor: %s", qPrintable(color));
    if (color.isEmpty())
        return;
    else {
        // this is already done at parse time
       /* if (!isValidColor(color)) {
            ErrorHandler::Get().AddWarning("Setbackgroundcolor",
                       QString(tr("Cannot set the background color of element %1 to %2,\n"
                               "check whether it is a valid Qt color string"))
                               .arg(getID()).arg(color));
        }*/
    }


    QPalette pal(target->palette());
    pal.setColor( target->backgroundRole(), color );
    target->setPalette( pal );
}

void ScreenElementRunDelegate::setFgColor(QWidget* target, const QString& color) {
    //qCDebug(APEX_RS, "ScreenElementRunDelegate::setFgColor: %s", qPrintable(color));
    if (color.isEmpty())
        return;

    QPalette pal(target->palette());
    pal.setColor( target->foregroundRole(), color );
    target->setPalette( pal );
}

}

}

bool apex::rundelegates::ScreenElementRunDelegate::isValidColor(const QString & color)
{
    QColor c(color);
    return c.isValid();
}


void apex::rundelegates::ScreenElementRunDelegate::setCommonProperties(QWidget * target)
{
    target->setStyleSheet(element->getStyle());
    setBgColor(target, element->getBGColor());
    setFgColor(target, element->getFGColor());
    if (!element->getFont().isEmpty()) {
        QFont f( target->font());
        f.setFamily( element->getFont() );
        target->setFont(f);
    }
}

