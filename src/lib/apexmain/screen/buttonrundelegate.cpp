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

#include "apexdata/screen/buttonelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "screen/screenrundelegate.h"

#include "buttonrundelegate.h"

#include <QFont>
#include <QFontMetrics>
#include <QStyleFactory>
#include <QVariant>

namespace apex
{
namespace rundelegates
{

const ScreenElement* ButtonRunDelegate::getScreenElement() const
{
    return element;
}

QWidget* ButtonRunDelegate::getWidget()
{
    return this;
}

bool ButtonRunDelegate::hasText() const
{
    return true;
}

bool ButtonRunDelegate::hasInterestingText() const
{
    return false;
}

const QString ButtonRunDelegate::getText() const
{
    return text();
}

void ButtonRunDelegate::resizeEvent( QResizeEvent* e )
{
    QPushButton::resizeEvent (e);
    setFont (initialFont);
    ApexTools::shrinkTillItFits (this, element->text(), QSize (2, 2));
}

void ButtonRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( this, SIGNAL( answered( ScreenElementRunDelegate* ) ),
             d, SIGNAL( answered( ScreenElementRunDelegate* ) ) );
}

}
}

void apex::rundelegates::ButtonRunDelegate::sendAnsweredSignal()
{
    Q_EMIT answered( this );
}

apex::rundelegates::ButtonRunDelegate::ButtonRunDelegate(
        ExperimentRunDelegate* p_rd,
    QWidget* parent, const ButtonElement* e, const QFont& defaultFont ) :
      QPushButton( parent ),
      ScreenElementRunDelegate(p_rd,e),
      element( e )
{
    setObjectName(element->getID());
    //setStyleSheet(element->getStyle());
    setCommonProperties(this);
    //    qCDebug(APEX_RS, "Creating button with parent %p", parent);

    #ifdef Q_WS_WIN
    // make sure we can change the button color (is not possible when using xp or vista engine)
    setStyle(QStyleFactory::create("windows"));
    #endif

    QPushButton::setText( e->text() );
    if ( !element->getShortCut().isEmpty() ) {
        QPushButton::setShortcut( e->getShortCut() );
        /*qCDebug(APEX_RS, "Setting shortcut of element %s to %s",
               qPrintable(e->getID()), qPrintable(e->getShortCut().toString()));*/
    }

    connect( this, SIGNAL( clicked() ), this, SLOT( sendAnsweredSignal() ) );

    if (! e->getFGColor().isEmpty()) {
        QPalette p=palette();
        p.setColor(QPalette::Active, QPalette::ButtonText, QColor(e->getFGColor()));
        setPalette(p);
    }
    if (! e->getBGColor().isEmpty()) {
        QPalette p=palette();
        p.setColor(QPalette::Active, QPalette::Button, QColor(e->getBGColor()));
        setPalette(p);
    }

    QFont font = defaultFont;
    if ( element->getFontSize() != -1 )
        font.setPointSize( element->getFontSize() );
    QPushButton::setFont( font );
    initialFont = font;

    // set maximum height to N*font height
    QFontMetrics fm( font );
    setMaximumHeight( fm.height()*5 );
    setMinimumHeight( fm.height());

    feedBack(ScreenElementRunDelegate::NoFeedback );
}


void apex::rundelegates::ButtonRunDelegate::feedBack
        (const FeedbackMode& mode)
{
    QColor newFGColor_enabled, newFGColor_disabled;
    QColor newBGColor_enabled, newBGColor_disabled;
    QPalette dp;        // default palette


    if ( mode == ScreenElementRunDelegate::NoFeedback )    {
        //setDown(false);
        setProperty("role", "none");
        setStyleSheet(styleSheet());
        if (! element->getFGColor().isEmpty()) {
            newFGColor_disabled=element->getFGColor();
            newFGColor_enabled=newFGColor_disabled;
        } else {
            newFGColor_disabled = dp.color(QPalette::Disabled,
                QPalette::ButtonText);
            newFGColor_enabled = dp.color(QPalette::Active,
                QPalette::ButtonText);
        }
        if (! element->getBGColor().isEmpty()) {
            newBGColor_disabled=element->getBGColor();
            newBGColor_enabled=newBGColor_disabled;
        } else {
            newBGColor_disabled = dp.color(QPalette::Disabled,
                                 QPalette::Button);
            newBGColor_enabled = dp.color(QPalette::Active,
                                           QPalette::Button);
        }

    } else if ( mode == ScreenElementRunDelegate::HighlightFeedback ) {
        setProperty("role", "highlight");
        setStyleSheet(styleSheet());
        newBGColor_disabled=QColor(0xff, 0xff, 0x90);      // light yellow
    } else if ( mode == ScreenElementRunDelegate::NegativeFeedback ) {
        setProperty("role", "negative");
        setStyleSheet(styleSheet());
        newBGColor_disabled=Qt::red;
    } else if ( mode == ScreenElementRunDelegate::PositiveFeedback ) {
        setProperty("role", "positive");
        setStyleSheet(styleSheet());
        newBGColor_disabled=Qt::darkGreen;
    }

    QPalette palette(this->palette());
    if (newBGColor_disabled.isValid())
        palette.setColor(QPalette::Disabled, QPalette::Button,
                         newBGColor_disabled);
    if (newBGColor_enabled.isValid())
        palette.setColor(QPalette::Active, QPalette::Button,
                         newBGColor_enabled);

    if (newFGColor_disabled.isValid())
        palette.setColor(QPalette::Disabled, QPalette::ButtonText,
                         newFGColor_disabled);
    if (newFGColor_enabled.isValid())
        palette.setColor(QPalette::Active, QPalette::ButtonText,
                         newFGColor_enabled);
    setPalette(palette);

}

